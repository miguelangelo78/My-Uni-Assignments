function PB = ErlangB(N, rho)
    r = (1:N)';
    x = 10.^(N * log10(rho) - sum(log10(r))); % First, calculate numerator separately %
    
    y = 1; % And the denominator as well %
    for n = 1:N, r = (1:n)';
        y = y + 10.^(n * log10(rho) - sum(log10(r)));
    end
    
    PB = x / y;  % Finally, divide the two to get the intended result %
end  % -- END FUNCTION - ErlangB() --