#pragma once

#ifndef VSM_AUTODIFF_HH
#define VSM_AUTODIFF_HH

#include "VSM.hh"
#include "dual.hh"
#include "Newton.hh"

/**
 * @file VSM_autodiff.hh
 * @brief Automatic differentiation tools for the VSM equilibrium computation.
 *
 * This file defines the nonlinear VSM equilibrium problem in a form compatible
 * with the Newton solver provided in the course. The Jacobian required by
 * Newton's method is computed automatically using dual numbers from `dual.hh`.
 */
namespace VSM {

  /**
   * @brief Nonlinear VSM problem solved with Newton and automatic differentiation.
   *
   * The equilibrium is obtained by solving:
   *
   * \f[
   * f(x^\star)=0
   * \f]
   *
   * where:
   *
   * \f[
   * x =
   * \begin{bmatrix}
   * \delta \\
   * \omega
   * \end{bmatrix}.
   * \f]
   *
   * This class follows the structure used in the course examples: it inherits
   * from `AD::NewtonProblemAD<real_type>` and implements the residual function
   * using dual numbers. The Jacobian is then generated automatically by the
   * base class.
   */
  class VSM_Problem_AD : public AD::NewtonProblemAD<real_type> {
    /// VSM model parameters.
    VSM_Parameters _p;

  public:

    /**
     * @brief Constructor.
     *
     * The initial guess is stored in the base Newton problem and later used by
     * the professor's Newton solver.
     *
     * @param p VSM model parameters.
     * @param x0 initial guess for Newton's method, `[delta0, omega0]`.
     */
    VSM_Problem_AD(
      VSM_Parameters const & p,
      state_type const & x0
    );

    /**
     * @brief Evaluate the nonlinear VSM residual using dual numbers.
     *
     * The residual corresponds to:
     *
     * \f[
     * f_1(x) = \omega_N(\omega-\omega_{ref})
     * \f]
     *
     * \f[
     * f_2(x) =
     * \frac{
     * -\zeta(\omega-\omega_{ref}) + P_m - P_{max}\sin(\delta)
     * }{M}.
     * \f]
     *
     * @param X state vector using dual numbers.
     * @return residual vector using dual numbers.
     */
    d_dual_vec
    F( d_dual_vec const & X ) const override;
  };

  /**
   * @brief Compute the VSM equilibrium using Newton's method and dual numbers.
   *
   * This function builds a `VSM_Problem_AD`, passes it to the Newton solver
   * from the course, and measures the CPU time required by the solve.
   *
   * @param x0 initial guess `[delta0, omega0]`.
   * @param p VSM model parameters.
   * @param elapsed_ms elapsed CPU time in milliseconds.
   * @param converged true if Newton's method converged.
   * @return computed equilibrium `[delta_star, omega_star]`.
   */
  state_type
  newton_equilibrium_dual(
    state_type const & x0,
    VSM_Parameters const & p,
    real_type & elapsed_ms,
    bool & converged
  );

}

#endif