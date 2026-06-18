#pragma once

#ifndef VSM_SMALL_SIGNAL_HH
#define VSM_SMALL_SIGNAL_HH

#include "VSM.hh"
#include "VSM_autodiff.hh"

/**
 * @file VSM_small_signal.hh
 * @brief Small-signal analysis of the VSM around its equilibrium point.
 *
 * After computing the equilibrium using Newton's method and automatic
 * differentiation, the nonlinear model is linearized around the operating
 * point. The resulting Jacobian matrix is used to study local stability,
 * oscillatory modes, damping ratios and modal properties of the system.
 */
namespace VSM {

  /**
   * @brief Compute the Jacobian matrix using automatic differentiation.
   *
   * The Jacobian is evaluated at the equilibrium point:
   *
   * \f[
   * A =
   * \left.
   * \frac{\partial f}{\partial x}
   * \right|_{x=x^\star}.
   * \f]
   *
   * This matrix corresponds to the linearized model:
   *
   * \f[
   * \Delta\dot{x} = A\,\Delta x.
   * \f]
   *
   * @param x equilibrium point.
   * @param p VSM model parameters.
   * @return Jacobian matrix evaluated at the equilibrium.
   */
  jac_type
  jacobian_dual(
    state_type const & x,
    VSM_Parameters const & p
  );

  /**
   * @brief Compute eigenvalues and eigenvectors of the linearized model.
   *
   * The eigenvalues characterize the local dynamic behavior around the
   * equilibrium point. Their real parts determine stability, while their
   * imaginary parts determine oscillation frequencies.
   *
   * The associated eigenvectors describe the modal participation of the
   * system states.
   *
   * @param A linearized state matrix.
   * @param eigvals computed eigenvalues.
   * @param eigvecs computed eigenvectors.
   */
  void
  eig_analysis(
    jac_type const & A,
    Eigen::VectorXcd & eigvals,
    Eigen::MatrixXcd & eigvecs
  );

  /**
   * @brief Save eigenvalues to a CSV file.
   *
   * The file is intended for post-processing and visualization in MATLAB.
   *
   * @param filename output CSV file.
   * @param eigvals eigenvalues of the linearized model.
   */
  void
  save_small_signal_eigenvalues_csv(
    std::string const & filename,
    Eigen::VectorXcd const & eigvals
  );

  /**
   * @brief Save eigenvectors to a CSV file.
   *
   * The file is intended for post-processing and visualization in MATLAB.
   *
   * @param filename output CSV file.
   * @param eigvecs eigenvectors of the linearized model.
   */
  void
  save_small_signal_eigenvectors_csv(
    std::string const & filename,
    Eigen::MatrixXcd const & eigvecs
  );

}

#endif