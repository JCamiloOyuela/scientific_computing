data_original  = readtable("results.csv");
data_optimized = readtable("results_optimized.csv");

%% Convergence plot: error vs h

figure

loglog(data_original.h, data_original.max_error, ...
    'o-', 'LineWidth', 2, 'MarkerSize', 8)

hold on

loglog(data_optimized.h, data_optimized.max_error, ...
    's-', 'LineWidth', 2, 'MarkerSize', 8)

grid on
grid minor

xlabel('Grid size h', 'FontSize', 12)
ylabel('Maximum error ||e||_\infty', 'FontSize', 12)

title('Convergence of the Block Tridiagonal Thomas-LDU Solver', ...
    'FontSize', 14)

p_original  = polyfit(log(data_original.h),  log(data_original.max_error),  1);
p_optimized = polyfit(log(data_optimized.h), log(data_optimized.max_error), 1);

slope_original  = p_original(1);
slope_optimized = p_optimized(1);

legend( ...
    sprintf('Original, slope = %.4f', slope_original), ...
    sprintf('Optimized, slope = %.4f', slope_optimized), ...
    'Location', 'southEast' )

fprintf('Original convergence order  = %.6f\n', slope_original);
fprintf('Optimized convergence order = %.6f\n', slope_optimized);

exportgraphics(gcf, 'comparison_convergence.pdf', ...
    'ContentType', 'vector');


%% Timing plot: CPU time vs N

figure

plot(data_original.N, data_original.cpu_ms, ...
    'o-', 'LineWidth', 2, 'MarkerSize', 8)

hold on

plot(data_optimized.N, data_optimized.cpu_ms, ...
    's-', 'LineWidth', 2, 'MarkerSize', 8)

grid on
grid minor

xlabel('Number of interior nodes N', 'FontSize', 12)
ylabel('CPU time [ms]', 'FontSize', 12)

title('Performance Comparison of the Block Thomas-LDU Solver', ...
    'FontSize', 14)

legend('Original', 'Optimized', 'Location', 'northWest')

exportgraphics(gcf, 'comparison_timing.pdf', ...
    'ContentType', 'vector');


%% Log-log timing plot: estimated complexity

figure

loglog(data_original.N, data_original.cpu_ms, ...
    'o-', 'LineWidth', 2, 'MarkerSize', 8)

hold on

loglog(data_optimized.N, data_optimized.cpu_ms, ...
    's-', 'LineWidth', 2, 'MarkerSize', 8)

grid on
grid minor

xlabel('Number of interior nodes N', 'FontSize', 12)
ylabel('CPU time [ms]', 'FontSize', 12)

title('Empirical Time Complexity of the Block Thomas-LDU Solver', ...
    'FontSize', 14)

p_time_original  = polyfit(log(data_original.N),  log(data_original.cpu_ms),  1);
p_time_optimized = polyfit(log(data_optimized.N), log(data_optimized.cpu_ms), 1);

time_slope_original  = p_time_original(1);
time_slope_optimized = p_time_optimized(1);

legend( ...
    sprintf('Original, slope = %.4f', time_slope_original), ...
    sprintf('Optimized, slope = %.4f', time_slope_optimized), ...
    'Location', 'northWest' )

fprintf('Original time exponent  = %.6f\n', time_slope_original);
fprintf('Optimized time exponent = %.6f\n', time_slope_optimized);

exportgraphics(gcf, 'comparison_timing_loglog.pdf', ...
    'ContentType', 'vector');