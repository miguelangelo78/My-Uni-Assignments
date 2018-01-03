function [N_output, PB_output] = plot_N_vs_PB(figure_index, N_max, desired_rho, increment_rho_by, desired_grade)

fprintf('** Calculating N value for Erlangs = %d and Grade of Service = %.2f %% ...\n', desired_rho, desired_grade);

fig = figure(figure_index);
clf(fig);

PB = zeros(1, N_max);
N_vector = 1:N_max;

plot_index = 1;
legend_x_offset = 0;

if figure_index == 2
   max_rho = desired_rho;
   legend_x_offset = 8;
else
   max_rho = desired_rho * 2 - increment_rho_by;
end

for rho = increment_rho_by:increment_rho_by:max_rho
    % Calculate the Blocking Probability from values N = 1 up to N_max
    % for the current Rho value
    for N = N_vector
        PB(N) = ErlangB(N, rho) * 100;
    end
    
    % Plot PB (Block Probability) in percentages (0 -> 100%)
    % in a semi-logarithmic scale
    graph_handle = semilogy(1:N_max, PB, 'LineWidth', 2);

    % Set the title for all subplots and the labels for the xy axis (only once)
    if plot_index == 1
        title('N (# of trunks) vs PB (Blocking Probability (%))');
        xlabel('N (# of trunks)');
        ylabel('PB (Blocking Probability (%))');

        grid on;
        hold on;

        xlim([1 N_max]);
        ylim([10^-2 100]);
    end
    
    % Place the rho value on the current subplot at position y = 10^-1
    N = find(PB <= 10^-1);
    line_legend_text = text(N(1) + 4 + legend_x_offset, 10^-1, ['\rho = ' num2str(rho)], 'Rotation', -80 - legend_x_offset / 1.2 + plot_index * 1.5, 'FontSize', 12);

    % Process the rho value in question in a special way
    if rho == desired_rho
        line_legend_text.FontSize = 14;
        line_legend_text.FontWeight = 'bold';
        set(graph_handle, 'LineWidth', 4, 'color', [1 0.85 0]);
        
        % Look for a percentage value that fits the 
        % desired grade of service
        N_output = 0;
        PB_output = 0;
        N_ = find(PB <= desired_grade);
        
        if ~isempty(N_)
            N = N_(1);
            % Set the output values of this function
            N_output  = N;
            PB_output = PB;

            % Draw an arrow pointing to the result (no. of trunks)
            text(N - 2.5 - legend_x_offset / 1.85, PB(N) * 1.5,'\downarrow', 'FontSize', 25, 'FontWeight', 'bold', 'Color', [1 0 0]);
            text(N - 5, PB(N) * 2.25, ['N = ' num2str(N)], 'FontWeight', 'bold', 'FontSize', 12, 'Color', [0 0 1]);

            % A reference line as well
            ref_y = PB(N) * ones(N_max);
            plot(1:N_max, ref_y, 'color', [0 0 1]);
            text(N_max + 2, PB(N), ['PB = ' num2str(round(PB(N), 2)) ' %'], 'FontWeight', 'bold');

            % Finally, draw an interception point    
            plot(N, PB(N), 'rx', 'MarkerSize', 8, 'LineWidth', 1.5, 'Color', [0 0 0]);
        end
    end
    
    plot_index = plot_index + 1;
end

set(gca, 'XTick', sort([1, get(gca, 'XTick')])); % Add XTick at x=1 %
set(gca, 'YTickLabel', cellstr(['10^{-2}'; '10^{-1}'; '      1'; '     10'; '    100']));
refreshdata;

end % -- END FUNCTION - plot_N_vs_PB() --