#pragma once

// old way to do pragma once
#ifndef VSM_HH
#define VSM_HH

// {fmt}
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>

// Eigen
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <Eigen/LU>

// Standard
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <stdexcept>
#include <fstream>

/**
 * @file VSM.hh
 * @brief Data structures and interfaces for a Virtual Synchronous Machine model.
 *
 * This file contains:
 * - Eigen-based numeric aliases;
 * - VSM model parameters in per-unit;
 * - nonlinear vector field declaration;
 * - equilibrium computation using Newton's method;
 * - analytical linearization and small-signal analysis tools;
 * - timing utilities `tic()` and `toc()`.
 *
 * The model used in this project is:
 *
 * \f[
 * \dot{\delta} = \omega_N(\omega-\omega_{ref})
 * \f]
 *
 * \f[
 * M\dot{\omega}
 * =
 * -\zeta(\omega-\omega_{ref}) + P_m - P_{max}\sin(\delta)
 * \f]
 */
namespace VSM {

  /// Floating point type used throughout the project.
  using real_type = double;

  /// Integer type used for indices and sizes.
  using integer = int;

  /// Dynamic dense matrix of real scalars.
  using mat_type = Eigen::Matrix<real_type,Eigen::Dynamic,Eigen::Dynamic>;

  /// Dynamic column vector of real scalars.
  using vec_type = Eigen::Matrix<real_type,Eigen::Dynamic,1>;

  /// Fixed-size state vector `[delta, omega]`.
  using state_type = Eigen::Matrix<real_type,2,1>;

  /// Fixed-size 2x2 matrix used for Jacobians.
  using jac_type = Eigen::Matrix<real_type,2,2>;

  /// π stored in double precision.
  static constexpr real_type pi = 3.14159265358979323846;

  /**
   * @brief Start a lightweight global timer.
   *
   * It is meant for simple local timing measurements:
   * `tic(); ...; toc();`
   */
  void tic();

  /**
   * @brief Return the time elapsed since the last call to `tic`.
   * @return elapsed time in milliseconds.
   */
  real_type toc();

  /**
   * @brief Parameters of the reduced-order Virtual Synchronous Machine model.
   *
   * All electrical quantities are expressed in per-unit, except:
   * - `Sbase`, given in VA;
   * - `Vbase`, given in V;
   * - `fN`, given in Hz;
   * - `omegaN`, given in rad/s;
   * - `taup`, given in s.
   */
  struct VSM_Parameters {

    /// Nominal apparent power [VA].
    real_type Sbase = 1.0e6;

    /// Nominal line-to-line RMS voltage [V].
    real_type Vbase = 563.0;

    /// Nominal frequency [Hz].
    real_type fN = 50.0;

    /// Nominal angular frequency [rad/s].
    real_type omegaN = 2.0 * pi * fN;

    /// Reference frequency [p.u.].
    real_type omega_ref = 1.0;

    /// Active power reference/input power [p.u.].
    real_type Pm = 0.8;

    /// Maximum transferable active power [p.u.].
    real_type Pmax = 1.2;

    /// Active power-frequency droop coefficient [p.u./p.u.].
    real_type kpw = 0.02;

    /// Active power filter time constant [s].
    real_type taup = 0.5;

    /// Virtual inertia coefficient.
    real_type M = taup / kpw;

    /// Damping coefficient.
    real_type zeta = 1.0 / kpw;
  };

  /**
   * @brief Summary of the quantities obtained during one numerical experiment.
   *
   * This structure stores equilibrium information, Newton convergence
   * statistics and the dominant eigenvalues of the linearized model.
   * The data can be exported to CSV files for later visualization and
   * post-processing.
   */
  struct Experiment_Result {

    /// Equilibrium angle [rad].
    real_type delta_star = 0;

    /// Equilibrium frequency [p.u.].
    real_type omega_star = 0;

    /// Residual norm at the computed equilibrium.
    real_type residual_norm = 0;

    /// Number of Newton iterations.
    integer newton_iter = 0;

    /// Time spent in Newton's method [ms].
    real_type newton_cpu_ms = 0;

    /// First eigenvalue real part.
    real_type lambda1_real = 0;

    /// First eigenvalue imaginary part.
    real_type lambda1_imag = 0;

    /// Second eigenvalue real part.
    real_type lambda2_real = 0;

    /// Second eigenvalue imaginary part.
    real_type lambda2_imag = 0;

    /// Time spent computing eigenvalues [ms].
    real_type eig_cpu_ms = 0;
  };

  /**
   * @brief Computes the nonlinear VSM vector field.
   *
   * State vector:
   * - `x(0) = delta`, power angle [rad];
   * - `x(1) = omega`, frequency [p.u.].
   *
   * @param x state vector `[delta, omega]`.
   * @param p model parameters.
   * @return time derivative `f(x)`.
   */
  state_type vsm_model(
    state_type const & x,
    VSM_Parameters const & p
  );

  /**
   * @brief Computes the analytical equilibrium of the VSM model.
   *
   * At equilibrium:
   *
   * \f[
   * \omega^\star = \omega_{ref}
   * \f]
   *
   * \f[
   * \delta^\star =
   * \arcsin\left(\frac{P_m}{P_{max}}\right)
   * \f]
   *
   * @param p model parameters.
   * @return equilibrium vector `[delta_star, omega_star]`.
   */
  state_type analytical_equilibrium(
    VSM_Parameters const & p
  );

  /**
   * @brief Compute the analytical Jacobian of the nonlinear VSM model.
   *
   * The Jacobian corresponds to the first-order linearization around a given
   * operating point. When evaluated at the equilibrium point, it defines the
   * small-signal model:
   *
   * \f[
   * \Delta\dot{x}=A\,\Delta x.
   * \f]
   *
   * The eigenvalues of this matrix determine the local stability properties
   * of the equilibrium.
   * @param x state vector `[delta, omega]`.
   * @param p model parameters.
   * @return Jacobian matrix.
   */
  jac_type analytical_jacobian(
    state_type const & x,
    VSM_Parameters const & p
  );

    /**
   * @brief Computes the equilibrium point using Newton's method.
   *
   * Newton solves:
   *
   * \f[
   * J(x_k)\Delta x_k = -f(x_k)
   * \f]
   *
   * and updates:
   *
   * \f[
   * x_{k+1}=x_k+\Delta x_k
   * \f]
   *
   * The convergence history is stored in a CSV file.
   *
   * @param x0 initial guess.
   * @param p model parameters.
   * @param tol convergence tolerance.
   * @param max_iter maximum number of Newton iterations.
   * @param iterations number of performed iterations.
   * @return computed equilibrium vector.
   */
  state_type newton_equilibrium(
    state_type const & x0,
    VSM_Parameters const & p,
    real_type tol,
    integer max_iter,
    integer & iterations
  );

   /**
   * @brief Save the main numerical results to a CSV file.
   *
   * @param filename output CSV file name.
   * @param res structure containing the experiment results.
   */
  void save_results_csv(
    std::string const & filename,
    Experiment_Result const & res
  );

  /**
   * @brief Save the eigenvalues to a CSV file.
   *
   * @param filename output CSV file name.
   * @param eigvals vector of complex eigenvalues.
   */
  void save_eigenvalues_csv(
    std::string const & filename,
    Eigen::VectorXcd const & eigvals
  );

}

#endif