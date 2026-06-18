close all; clear; clc;

set(groot, 'defaultTextInterpreter', 'latex');
set(groot, 'defaultAxesTickLabelInterpreter', 'latex');
set(groot, 'defaultLegendInterpreter', 'latex');

traj = readtable("trajectory.csv");
eigv = readtable("eigenvalues_dual.csv");

t = traj.t;
delta = traj.delta;
omega = traj.omega;
delta_dev = traj.delta_dev;
omega_dev = traj.omega_dev;

lambda = eigv.real + 1i*eigv.imag;

sigma = real(lambda);
wd = abs(imag(lambda));
wn = abs(lambda);
zeta = -sigma ./ wn;
freq_hz = wd / (2*pi);

%% Figure 1: absolute states
fig1 = figure;
set(fig1, 'Color', 'w');

subplot(2,1,1);
plot(t, delta, 'LineWidth', 1.5);
grid on;
xlabel('$t$ [s]');
ylabel('$\delta$ [rad]');
title('Angle response');

subplot(2,1,2);
plot(t, omega, 'LineWidth', 1.5);
grid on;
xlabel('$t$ [s]');
ylabel('$\omega$ [p.u.]');
title('Frequency response');

sgtitle('Nonlinear VSM response', 'Interpreter', 'latex');

exportgraphics(fig1, 'fig_states.pdf', 'ContentType', 'vector');

%% Figure 2: deviations from equilibrium
fig2 = figure;
set(fig2, 'Color', 'w');

subplot(2,1,1);
plot(t, delta_dev, 'LineWidth', 1.5);
grid on;
xlabel('$t$ [s]');
ylabel('$\Delta\delta$ [rad]');
title('Angle deviation');

subplot(2,1,2);
plot(t, omega_dev, 'LineWidth', 1.5);
grid on;
xlabel('$t$ [s]');
ylabel('$\Delta\omega$ [p.u.]');
title('Frequency deviation');

sgtitle('Deviation from the equilibrium point', 'Interpreter', 'latex');

exportgraphics(fig2, 'fig_deviations.pdf', 'ContentType', 'vector');

%% Figure 3: eigenvalues in complex plane
fig3 = figure;
set(fig3, 'Color', 'w');
hold on; grid on; box on;

plot(real(lambda), imag(lambda), 'x', ...
     'MarkerSize', 12, ...
     'LineWidth', 2);

xline(0, '--', 'LineWidth', 1.2);

xlabel('$\mathrm{Re}(\lambda)$');
ylabel('$\mathrm{Im}(\lambda)$');
title('Small-signal eigenvalues');

for k = 1:length(lambda)
    txt = sprintf(['$\\lambda_%d = %.3f %+.3fj$ \n' ...
                   '$f_d = %.3f\\,\\mathrm{Hz}$, $\\zeta = %.3f$'], ...
                   k, real(lambda(k)), imag(lambda(k)), freq_hz(k), zeta(k));

    text(real(lambda(k)) + 0.15, imag(lambda(k)), txt, ...
         'Interpreter', 'latex', ...
         'FontSize', 10);
end

xlim([-2 0.5]);
ylim([-4 4]);

exportgraphics(fig3, 'fig_eigenvalues.pdf', 'ContentType', 'vector');

%% Figure 4: combined deviations
fig4 = figure;
set(fig4, 'Color', 'w');

yyaxis left
plot(t, delta_dev, 'LineWidth', 1.5);
ylabel('$\Delta\delta$ [rad]');

yyaxis right
plot(t, omega_dev, 'LineWidth', 1.5);
ylabel('$\Delta\omega$ [p.u.]');

grid on;
xlabel('$t$ [s]');
title('Coupled nonlinear response around equilibrium');
legend('$\Delta\delta$', '$\Delta\omega$', 'Location', 'best');

exportgraphics(fig4, 'fig_combined_response.pdf', 'ContentType', 'vector');

disp("PDF figures saved:")
disp("fig_states.pdf")
disp("fig_deviations.pdf")
disp("fig_eigenvalues.pdf")
disp("fig_combined_response.pdf")

%%
%% Newton convergence
newton = readtable("newton_convergence.csv");

fig5 = figure;
set(fig5, 'Color', 'w');

semilogy(newton.iteration, newton.residual_norm, 'o-', 'LineWidth', 1.5);
grid on;
xlabel('Newton iteration $k$');
ylabel('$\|F(x_k)\|_2$');
title('Newton convergence with analytical Jacobian / AutoDiff comparison');

exportgraphics(fig5, 'fig_newton_convergence.pdf', 'ContentType', 'vector');

%% Newton comparison: analytical vs autodiff
res_analytic = readtable("results.csv");
res_autodiff = readtable("results_autodiff.csv");

methods = categorical({'Analytical Jacobian', 'AutoDiff Jacobian'});
methods = reordercats(methods, {'Analytical Jacobian', 'AutoDiff Jacobian'});

cpu_times = [
    res_analytic.newton_cpu_ms(1)
    res_autodiff.cpu_ms(1)
];

residuals = [
    res_analytic.residual_norm(1)
    res_autodiff.residual_norm(1)
];

fig6 = figure;
set(fig6, 'Color', 'w');

subplot(1,2,1);
bar(methods, cpu_times);
grid on;
ylabel('CPU time [ms]');
title('Newton computational time');

subplot(1,2,2);
bar(methods, residuals);
set(gca, 'YScale', 'log');
grid on;
ylabel('$\|F(x^\star)\|_2$');
title('Final residual');

sgtitle('Newton comparison: analytical vs AutoDiff Jacobian', ...
        'Interpreter', 'latex');

exportgraphics(fig6, 'fig_newton_comparison.pdf', 'ContentType', 'vector');

%% Computational performance summary
summary = readtable("summary_autodiff.csv");

names = summary.quantity;
values = summary.value;

newton_time = values(strcmp(names, "newton_autodiff_cpu_ms"));
jac_time    = values(strcmp(names, "jacobian_autodiff_cpu_ms"));
eig_time    = values(strcmp(names, "eigenanalysis_cpu_ms"));
rk4_time    = values(strcmp(names, "rk4_cpu_ms"));

times = [newton_time; jac_time; eig_time; rk4_time];

labels = categorical({
    'Newton + AD'
    'Jacobian AD'
    'Eigenanalysis'
    'RK4'
});

labels = reordercats(labels, {
    'Newton + AD'
    'Jacobian AD'
    'Eigenanalysis'
    'RK4'
});

fig7 = figure;
set(fig7, 'Color', 'w');
bar(labels, times);
grid on;
ylabel('CPU time [ms]');
title('Computational cost by stage');
set(gca, 'YScale', 'log');

exportgraphics(fig7, 'fig_computational_cost.pdf', 'ContentType', 'vector');

%% Accuracy summary
eq_error  = values(strcmp(names, "equilibrium_error_norm"));
jac_error = values(strcmp(names, "jacobian_error_norm"));
res_error = values(strcmp(names, "final_residual_norm"));

errors = [eq_error; jac_error; res_error];

err_labels = categorical({
    'Equilibrium error'
    'Jacobian error'
    'Final residual'
});

err_labels = reordercats(err_labels, {
    'Equilibrium error'
    'Jacobian error'
    'Final residual'
});

fig8 = figure;
set(fig8, 'Color', 'w');
bar(err_labels, errors);
grid on;
ylabel('Error norm');
title('Accuracy of AutoDiff-based computation');
set(gca, 'YScale', 'log');

exportgraphics(fig8, 'fig_accuracy_summary.pdf', 'ContentType', 'vector');