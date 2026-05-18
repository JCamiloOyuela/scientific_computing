close all; clear, clc;

data = readtable("top10.txt", ...
                 "FileType", "text", ...
                 "ReadVariableNames", false);

rank = data.Var1;
freq = data.Var2;
word = data.Var3;

% Zipf model: freq ~ c/rank
numerator = sum(freq ./ rank);
denominator = sum(1 ./ (rank.^2));
c = numerator / denominator;
model_zipf = c ./ rank;

disp("Best c:")
disp(c)

% Zipf-Mandelbrot fit: freq ~ C / (rank + b)^a
zipf_mandelbrot = @(params, r) params(1) ./ ((r + params(2)).^params(3));

% params = [C, b, a]
initial_guess = [freq(1), 2.7, 1];

params = fminsearch( ...
    @(params) sum((freq - zipf_mandelbrot(params, rank)).^2), ...
    initial_guess);

C = params(1);
b = params(2);
a_mandelbrot = params(3);

model_mandelbrot = zipf_mandelbrot(params, rank);

disp("Zipf-Mandelbrot C:")
disp(C)

disp("Zipf-Mandelbrot b:")
disp(b)

disp("Zipf-Mandelbrot a:")
disp(a_mandelbrot)

% Plot
loglog(rank, freq, 'o')
hold on
loglog(rank, model_zipf, '-')
loglog(rank, model_mandelbrot, '-')

grid on

xlabel("Rank")
ylabel("Frequency")

legend("data", "c/rank", "Zipf-Mandelbrot")

% save figure as pdf
exportgraphics(gcf, "zipf_plot.pdf", "ContentType", "vector");