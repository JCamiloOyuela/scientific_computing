#include "VSM_autodiff.hh"

namespace VSM {

  VSM_Problem_AD::VSM_Problem_AD(
    VSM_Parameters const & p,
    state_type const & x0
  )
  : _p(p)
  {
    _x0.resize(2);
    _x0(0) = x0(0);
    _x0(1) = x0(1);
  }

  VSM_Problem_AD::d_dual_vec
  VSM_Problem_AD::F(
    d_dual_vec const & X
  ) const {
    d_dual_vec R;
    R.resize(2);

    auto const delta = X(0);
    auto const omega = X(1);

    R(0) = _p.omegaN * (omega - _p.omega_ref);

    R(1) = (
      -_p.zeta * (omega - _p.omega_ref)
      + _p.Pm
      - _p.Pmax * AD::sin(delta)
    ) / _p.M;

    return R;
  }

  state_type
  newton_equilibrium_dual(
    state_type const & x0,
    VSM_Parameters const & p,
    real_type & elapsed_ms,
    bool & converged
  ) {
    VSM_Problem_AD problem(p, x0);

    AD::NewtonSolver<real_type> solver(&problem);

    tic();
    solver.solve();
    elapsed_ms = toc();

    converged = solver.converged();

    auto Xsol = solver.solution();

    state_type xstar;
    xstar(0) = Xsol(0);
    xstar(1) = Xsol(1);

    return xstar;
  }

}