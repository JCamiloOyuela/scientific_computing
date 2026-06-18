#pragma once

#ifndef VSM_RK4_HH
#define VSM_RK4_HH

#include "VSM.hh"

/**
 * @file VSM_RK4.hh
 * @brief RK4 solver for the nonlinear VSM dynamic simulation.
 *
 * This file contains the functions used to simulate the nonlinear time-domain
 * response of the Virtual Synchronous Machine after the equilibrium point has
 * been computed. The integration method is the classical fourth-order
 * Runge-Kutta method.
 */
namespace VSM {

  /**
   * @brief Perform one classical fourth-order Runge-Kutta step.
   *
   * Starting from the current state \f$x_k\f$, this function computes an
   * approximation of the next state \f$x_{k+1}\f$ by applying RK4 to:
   *
   * \f[
   * \dot{x} = f(x).
   * \f]
   *
   * In this project the state is:
   *
   * \f[
   * x =
   * \begin{bmatrix}
   * \delta \\
   * \omega
   * \end{bmatrix}.
   * \f]
   *
   * @param x current state vector `[delta, omega]`.
   * @param h integration time step [s].
   * @param p VSM model parameters.
   * @return state vector after one RK4 step.
   */
  state_type
  rk4_step(
    state_type const & x,
    real_type h,
    VSM_Parameters const & p
  );

  /**
   * @brief Simulate the nonlinear VSM model using a fixed-step RK4 method.
   *
   * The simulation starts from a perturbed initial condition around the
   * equilibrium point. The computed trajectory is saved to a CSV file so that
   * the results can be plotted later in MATLAB.
   *
   * The output file contains:
   *
   * - `t`: simulation time [s];
   * - `delta`: absolute angle state [rad];
   * - `omega`: frequency state [p.u.];
   * - `delta_dev`: deviation from the equilibrium angle [rad];
   * - `omega_dev`: deviation from the equilibrium frequency [p.u.].
   *
   * @param x0 initial condition for the nonlinear simulation.
   * @param xstar equilibrium point used to compute state deviations.
   * @param h fixed integration time step [s].
   * @param tf final simulation time [s].
   * @param p VSM model parameters.
   * @param filename name of the CSV output file.
   * @return elapsed simulation time in milliseconds.
   */
  real_type
  simulate_RK4(
    state_type const & x0,
    state_type const & xstar,
    real_type h,
    real_type tf,
    VSM_Parameters const & p,
    std::string const & filename
  );

}

#endif