#include "VSM.hh"

int
main() {
  using namespace VSM;

  /// Parameters of the VSM model.
  VSM_Parameters p;

  /// Analytical equilibrium used as a first verification.
  tic();
  state_type xstar = analytical_equilibrium(p);
  real_type const equilibrium_cpu_ms = toc();

  /// Newton equilibrium computation.
  state_type x0;
  x0 << 0.5, 0.95;

  integer newton_iterations = 0;

  tic();
  state_type xstar_newton =
    newton_equilibrium(x0, p, 1e-12, 20, newton_iterations);
  real_type const newton_cpu_ms = toc();

  state_type newton_error = xstar_newton - xstar;

  /// Residual at equilibrium.
  state_type fstar = vsm_model(xstar, p);

  /// Analytical Jacobian and small-signal eigenvalues.
  tic();
  jac_type A = analytical_jacobian(xstar, p);
  Eigen::EigenSolver<jac_type> eig_solver(A);
  Eigen::VectorXcd eigvals = eig_solver.eigenvalues();
  real_type const eig_cpu_ms = toc();

  fmt::print("Virtual Synchronous Machine final project\n");
  fmt::print("=========================================\n\n");

  fmt::print("Nominal data\n");
  fmt::print("------------\n");
  fmt::print("Sbase  = {:.3f} MW\n", p.Sbase / 1.0e6);
  fmt::print("Vbase  = {:.3f} V\n", p.Vbase);
  fmt::print("fN     = {:.3f} Hz\n", p.fN);
  fmt::print("omegaN = {:.6f} rad/s\n\n", p.omegaN);

  fmt::print("Controller and VSM parameters\n");
  fmt::print("-----------------------------\n");
  fmt::print("kpw  = {:.6f}\n", p.kpw);
  fmt::print("taup = {:.6f} s\n", p.taup);
  fmt::print("M    = {:.6f}\n", p.M);
  fmt::print("zeta = {:.6f}\n\n", p.zeta);

  fmt::print("Analytical equilibrium\n");
  fmt::print("----------------------\n");
  fmt::print("delta* = {:.12f} rad\n", xstar(0));
  fmt::print("omega* = {:.12f} p.u.\n", xstar(1));
  fmt::print("||f(x*)|| = {:.6e}\n", fstar.norm());
  fmt::print("equilibrium time = {:.6e} ms\n\n", equilibrium_cpu_ms);

  fmt::print("Newton equilibrium\n");
  fmt::print("------------------\n");
  fmt::print("initial guess = [{:.6f}, {:.6f}]\n", x0(0), x0(1));
  fmt::print("delta* Newton = {:.12f} rad\n", xstar_newton(0));
  fmt::print("omega* Newton = {:.12f} p.u.\n", xstar_newton(1));
  fmt::print("iterations    = {}\n", newton_iterations);
  fmt::print("Newton time   = {:.6e} ms\n", newton_cpu_ms);
  fmt::print("error delta   = {:.6e}\n", std::abs(newton_error(0)));
  fmt::print("error omega   = {:.6e}\n\n", std::abs(newton_error(1)));

  fmt::print("Analytical Jacobian at equilibrium\n");
  fmt::print("----------------------------------\n");
  fmt::print("[{: .8e} {: .8e}]\n", A(0,0), A(0,1));
  fmt::print("[{: .8e} {: .8e}]\n\n", A(1,0), A(1,1));

  fmt::print("Small-signal eigenvalues\n");
  fmt::print("------------------------\n");
  for ( integer i = 0; i < eigvals.size(); ++i ) {
    fmt::print(
      "lambda{} = {: .8e} {:+.8e}j\n",
      i+1,
      eigvals(i).real(),
      eigvals(i).imag()
    );
  }
  fmt::print("eigenvalue time = {:.6e} ms\n\n", eig_cpu_ms);

  Experiment_Result res;

  res.delta_star    = xstar_newton(0);
  res.omega_star    = xstar_newton(1);
  res.residual_norm = vsm_model(xstar_newton, p).norm();
  res.newton_iter   = newton_iterations;
  res.newton_cpu_ms = newton_cpu_ms;

  res.lambda1_real = eigvals(0).real();
  res.lambda1_imag = eigvals(0).imag();
  res.lambda2_real = eigvals(1).real();
  res.lambda2_imag = eigvals(1).imag();
  res.eig_cpu_ms   = eig_cpu_ms;

  save_results_csv("results.csv", res);
  save_eigenvalues_csv("eigenvalues.csv", eigvals);

  fmt::print("CSV files saved:\n");
  fmt::print("- results.csv\n");
  fmt::print("- eigenvalues.csv\n");
  fmt::print("- newton_convergence.csv\n\n");

  return 0;
}