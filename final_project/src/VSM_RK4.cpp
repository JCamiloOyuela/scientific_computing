#include "VSM_RK4.hh"

namespace VSM {

  state_type
  rk4_step(
    state_type const & x,
    real_type h,
    VSM_Parameters const & p
  ) {
    state_type k1 = vsm_model(x, p);
    state_type k2 = vsm_model(x + 0.5 * h * k1, p);
    state_type k3 = vsm_model(x + 0.5 * h * k2, p);
    state_type k4 = vsm_model(x + h * k3, p);

    return x + (h / 6.0) * (k1 + 2.0*k2 + 2.0*k3 + k4);
  }

  real_type
  simulate_RK4(
    state_type const & x0,
    state_type const & xstar,
    real_type h,
    real_type tf,
    VSM_Parameters const & p,
    std::string const & filename
  ) {
    std::ofstream csv(filename);

    csv << "t,delta,omega,delta_dev,omega_dev\n";

    state_type x = x0;

    integer const nsteps = static_cast<integer>(tf / h);

    tic();

    for ( integer k = 0; k <= nsteps; ++k ) {
      real_type const t = k * h;

      csv << t << ","
          << x(0) << ","
          << x(1) << ","
          << x(0) - xstar(0) << ","
          << x(1) - xstar(1) << "\n";

      x = rk4_step(x, h, p);
    }

    real_type const elapsed_ms = toc();

    csv.close();

    return elapsed_ms;
  }

}