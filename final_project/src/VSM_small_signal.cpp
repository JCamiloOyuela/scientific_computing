#include "VSM_small_signal.hh"

namespace VSM {

  jac_type
  jacobian_dual(
    state_type const & x,
    VSM_Parameters const & p
  ) {
    VSM_Problem_AD problem(p, x);

    AD::NewtonProblemAD<real_type>::d_vec X(2);
    X(0) = x(0);
    X(1) = x(1);

    return problem.JF(X);
  }

  void
  eig_analysis(
    jac_type const & A,
    Eigen::VectorXcd & eigvals,
    Eigen::MatrixXcd & eigvecs
  ) {
    Eigen::EigenSolver<jac_type> solver(A);

    eigvals = solver.eigenvalues();
    eigvecs = solver.eigenvectors();
  }

  void
  save_small_signal_eigenvalues_csv(
    std::string const & filename,
    Eigen::VectorXcd const & eigvals
  ) {
    std::ofstream csv(filename);

    csv << "index,real,imag\n";

    for ( integer i = 0; i < eigvals.size(); ++i ) {
      csv << i << ","
          << eigvals(i).real() << ","
          << eigvals(i).imag() << "\n";
    }

    csv.close();
  }

  void
  save_small_signal_eigenvectors_csv(
    std::string const & filename,
    Eigen::MatrixXcd const & eigvecs
  ) {
    std::ofstream csv(filename);

    csv << "mode,state,real,imag\n";

    for ( integer j = 0; j < eigvecs.cols(); ++j ) {
      for ( integer i = 0; i < eigvecs.rows(); ++i ) {
        csv << j << ","
            << i << ","
            << eigvecs(i,j).real() << ","
            << eigvecs(i,j).imag() << "\n";
      }
    }

    csv.close();
  }

}