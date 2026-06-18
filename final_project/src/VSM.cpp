#include "VSM.hh"

namespace VSM {

  /// Internal time marker used by `tic()` and `toc()`.
  static std::chrono::high_resolution_clock::time_point tic_time;

  void
  tic() {
    tic_time = std::chrono::high_resolution_clock::now();
  }

  real_type
  toc() {
    auto toc_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<real_type, std::milli> elapsed =
      toc_time - tic_time;

    return elapsed.count();
  }

  state_type
  vsm_model(
    state_type const & x,
    VSM_Parameters const & p
  ) {
    state_type f;

    real_type const delta = x(0);
    real_type const omega = x(1);

    f(0) = p.omegaN * (omega - p.omega_ref);

    f(1) = (
      -p.zeta * (omega - p.omega_ref)
      + p.Pm
      - p.Pmax * std::sin(delta)
    ) / p.M;

    return f;
  }

  state_type
  analytical_equilibrium(
    VSM_Parameters const & p
  ) {
    if ( std::abs(p.Pm / p.Pmax) > 1.0 ) {
      throw std::runtime_error(
        "analytical_equilibrium: Pm/Pmax must be between -1 and 1"
      );
    }

    state_type xstar;

    xstar(0) = std::asin(p.Pm / p.Pmax);
    xstar(1) = p.omega_ref;

    return xstar;
  }

  jac_type
  analytical_jacobian(
    state_type const & x,
    VSM_Parameters const & p
  ) {
    jac_type J;

    real_type const delta = x(0);

    J(0,0) = 0.0;
    J(0,1) = p.omegaN;

    J(1,0) = -p.Pmax * std::cos(delta) / p.M;
    J(1,1) = -p.zeta / p.M;

    return J;
  }

  state_type
  newton_equilibrium(
    state_type const & x0,
    VSM_Parameters const & p,
    real_type tol,
    integer max_iter,
    integer & iterations
  ) {
    state_type x = x0;

    std::ofstream csv("newton_convergence.csv");
    csv << "iteration,residual_norm,dx_norm,cpu_ms\n";

    iterations = 0;

    for ( integer k = 0; k < max_iter; ++k ) {

      tic();

      state_type F = vsm_model(x, p);
      jac_type J = analytical_jacobian(x, p);

      state_type dx = J.fullPivLu().solve(-F);

      real_type const cpu_ms = toc();

      real_type const residual_norm = F.norm();
      real_type const dx_norm       = dx.norm();

      csv << k << ","
          << residual_norm << ","
          << dx_norm << ","
          << cpu_ms << "\n";

      x = x + dx;
      iterations = k + 1;

      if ( residual_norm < tol || dx_norm < tol ) {
        break;
      }
    }

    csv.close();

    return x;
  }

  void
  save_results_csv(
    std::string const & filename,
    Experiment_Result const & res
  ) {
    std::ofstream csv(filename);

    csv << "delta_star,omega_star,residual_norm,newton_iter,"
        << "newton_cpu_ms,lambda1_real,lambda1_imag,"
        << "lambda2_real,lambda2_imag,eig_cpu_ms\n";

    csv << res.delta_star << ","
        << res.omega_star << ","
        << res.residual_norm << ","
        << res.newton_iter << ","
        << res.newton_cpu_ms << ","
        << res.lambda1_real << ","
        << res.lambda1_imag << ","
        << res.lambda2_real << ","
        << res.lambda2_imag << ","
        << res.eig_cpu_ms << "\n";

    csv.close();
  }

  void
  save_eigenvalues_csv(
    std::string const & filename,
    Eigen::VectorXcd const & eigvals
  ) {
    std::ofstream csv(filename);

    csv << "real,imag\n";

    for ( integer i = 0; i < eigvals.size(); ++i ) {
      csv << eigvals(i).real() << ","
          << eigvals(i).imag() << "\n";
    }

    csv.close();
  }

}