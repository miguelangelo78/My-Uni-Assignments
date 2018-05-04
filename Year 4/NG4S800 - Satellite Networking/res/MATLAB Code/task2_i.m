erlangs = 120;  % How many calls/hour on the telephone system  %
grade   = 10.0; % The grade of service desired for this system %

% Calculate and plot the blocking probability for every N and rho values %
[N, PB] = plot_N_vs_PB(...
    1,       ... % Create figure 1                                     %
    250,     ... % N will be plotted from 1 to 250 on the x-axis       %
    erlangs, ... % The desired telephone exchange load (Erlangs)       %
    40,      ... % The erlang increment used to plot different loads   %
    grade    ... % The desired grade of service / blocking probability %
);

% Show results %
fprintf('** Answer for task 2.i: N = %d, Rho = %d, PB = %.4f %% **\n', N, erlangs, PB(N));