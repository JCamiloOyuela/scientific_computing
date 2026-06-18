#include "VSM_autodiff.hh"
#include "VSM_RK4.hh"
#include "VSM_small_signal.hh"

int
main() {
  using namespace VSM;

  VSM_Parameters p;

  state_type x0;
  x0 << 0.5, 0.95;

  real_type dual_newton_cpu_ms = 0;
  bool converged = false;

  state_type xstar_dual = newton_equilibrium_dual(
    x0,
    p,
    dual_newton_cpu_ms,
    converged
  );

  state_type xstar_exact = analytical_equilibrium(p);
  state_type error = xstar_dual - xstar_exact;
  state_type residual = vsm_model(xstar_dual, p);

  fmt::print("VSM equilibrium using dual.hh and professor Newton solver\n");
  fmt::print("========================================================\n\n");

  fmt::print("Initial guess\n");
  fmt::print("-------------\n");
  fmt::print("delta0 = {:.8f}\n", x0(0));
  fmt::print("omega0 = {:.8f}\n\n", x0(1));

  fmt::print("Dual Newton equilibrium\n");
  fmt::print("-----------------------\n");
  fmt::print("delta* = {:.12f} rad\n", xstar_dual(0));
  fmt::print("omega* = {:.12f} p.u.\n", xstar_dual(1));
  fmt::print("||f(x*)|| = {:.6e}\n", residual.norm());
  fmt::print("CPU time = {:.6e} ms\n\n", dual_newton_cpu_ms);
  fmt::print("Converged = {}\n\n", converged ? "yes" : "no");

  fmt::print("Validation against analytical equilibrium\n");
  fmt::print("-----------------------------------------\n");
  fmt::print("exact delta* = {:.12f} rad\n", xstar_exact(0));
  fmt::print("exact omega* = {:.12f} p.u.\n", xstar_exact(1));
  fmt::print("error delta  = {:.6e}\n", std::abs(error(0)));
  fmt::print("error omega  = {:.6e}\n\n", std::abs(error(1)));

  state_type x_initial;
  x_initial << xstar_dual(0) + 0.05, xstar_dual(1);

  real_type const h  = 0.001;
  real_type const tf = 10.0;

  real_type const rk4_cpu_ms = simulate_RK4(
    x_initial,
    xstar_dual,
    h,
    tf,
    p,
    "trajectory.csv"
  );

    tic();
  jac_type A_dual = jacobian_dual(xstar_dual, p);
  real_type const jacobian_dual_cpu_ms = toc();

  Eigen::VectorXcd eigvals;
  Eigen::MatrixXcd eigvecs;

  tic();
  eig_analysis(A_dual, eigvals, eigvecs);
  real_type const eig_cpu_ms = toc();

  save_small_signal_eigenvalues_csv("eigenvalues_dual.csv", eigvals);
  save_small_signal_eigenvectors_csv("eigenvectors_dual.csv", eigvecs);

  fmt::print("Small-signal analysis using dual.hh\n");
  fmt::print("-----------------------------------\n");
  fmt::print("Jacobian A = df/dx at equilibrium:\n");
  fmt::print("[{: .8e} {: .8e}]\n", A_dual(0,0), A_dual(0,1));
  fmt::print("[{: .8e} {: .8e}]\n", A_dual(1,0), A_dual(1,1));
  fmt::print("Jacobian CPU time = {:.6e} ms\n\n", jacobian_dual_cpu_ms);

  fmt::print("Eigenvalues:\n");
  for ( integer i = 0; i < eigvals.size(); ++i ) {
    fmt::print(
      "lambda{} = {: .8e} {:+.8e}j\n",
      i+1,
      eigvals(i).real(),
      eigvals(i).imag()
    );
  }

  fmt::print("\nEigenvectors columns:\n");
  for ( integer j = 0; j < eigvecs.cols(); ++j ) {
    fmt::print("v{} = [", j+1);
    for ( integer i = 0; i < eigvecs.rows(); ++i ) {
      fmt::print(
        "{: .6e}{:+.6e}j",
        eigvecs(i,j).real(),
        eigvecs(i,j).imag()
      );
      if ( i+1 < eigvecs.rows() ) fmt::print(", ");
    }
    fmt::print("]\n");
  }

  fmt::print("\nEigenvalue CPU time = {:.6e} ms\n", eig_cpu_ms);
  fmt::print("files saved = eigenvalues_dual.csv, eigenvectors_dual.csv\n\n");

  fmt::print("Nonlinear RK4 simulation\n");
  fmt::print("------------------------\n");
  fmt::print("initial delta = {:.12f} rad\n", x_initial(0));
  fmt::print("initial omega = {:.12f} p.u.\n", x_initial(1));
  fmt::print("time step     = {:.6e} s\n", h);
  fmt::print("final time    = {:.6f} s\n", tf);
  fmt::print("CPU time      = {:.6e} ms\n", rk4_cpu_ms);
  fmt::print("file saved    = trajectory.csv\n\n");

  std::ofstream csv("results_autodiff.csv");

  csv << "method,delta_star,omega_star,residual_norm,cpu_ms,converged\n";

  csv << "Newton autodiff,"
      << xstar_dual(0) << ","
      << xstar_dual(1) << ","
      << residual.norm() << ","
      << dual_newton_cpu_ms << ","
      << converged << "\n";

  csv.close();


  jac_type A_exact = analytical_jacobian(xstar_exact, p);

  real_type const equilibrium_error_norm =
    (xstar_dual - xstar_exact).norm();

  real_type const jacobian_error_norm =
    (A_dual - A_exact).norm();

  std::ofstream summary("summary_autodiff.csv");

  summary << "quantity,value\n";
  summary << "equilibrium_error_norm," << equilibrium_error_norm << "\n";
  summary << "jacobian_error_norm," << jacobian_error_norm << "\n";
  summary << "newton_autodiff_cpu_ms," << dual_newton_cpu_ms << "\n";
  summary << "jacobian_autodiff_cpu_ms," << jacobian_dual_cpu_ms << "\n";
  summary << "eigenanalysis_cpu_ms," << eig_cpu_ms << "\n";
  summary << "rk4_cpu_ms," << rk4_cpu_ms << "\n";
  summary << "final_residual_norm," << residual.norm() << "\n";

  summary.close();

  return 0;
}