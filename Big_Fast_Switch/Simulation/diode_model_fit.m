function diode_model_fit ()
  % Boltzmann constant (J/K)
  k = 1.38064852e-23;
  % Temperature for equation fit (K)
  T = 273.15 + 25;
  % Charge of an electron (C)
  q = 1.602176634e-19;
  
  % Column 1 is diode external forward voltage in Volts
  % Column 2 is diode forward current in milliAmps
  data = dlmread('APT2012SURCK_i-V_298K.csv', ',', 1, 0);
  % Convert column 2 to Amps
  data(:,2) = 0.001 .* data(:,2);

  % number of data points
  nData = size(data, 1);

  % parameter vector
  % Row 1 is Is (A), saturation current
  % Row 2 is N, emission coefficient
  % Row 3 is Rs (?), series resistance
  params_prev = zeros(3,1);
  
  % parameter vector initial guess
  fprintf('Brute force evaluting initial parameter guess...\n');
  Is_min = 1e-18;
  Is_multiplier = 2;
  Is_max = 1e-9;
  N_min = 0.5;
  N_step = 0.1;
  N_max = 4;
  Rs_min = 1e-4;
  Rs_multiplier = 2;
  Rs_max = 10;
  lowest_residual = realmax('double');
  Is_best = NaN('double');
  N_best = NaN('double');
  Rs_best = NaN('double');
  
  Is_guess = Is_min;
  while Is_guess < Is_max,
    N_guess = N_min;
    while N_guess < N_max,
      Rs_guess = Rs_min;
      while Rs_guess < Rs_max,
        this_err_vec = error_vector([Is_guess; N_guess; Rs_guess]);
        this_residual = sum(this_err_vec.^2);
        if this_residual < lowest_residual,
          lowest_residual = this_residual;
          Is_best = Is_guess;
          N_best = N_guess;
          Rs_best = Rs_guess;
        end
        Rs_guess = Rs_guess * Rs_multiplier;
      end
      N_guess = N_guess + N_step;
    end
    Is_guess = Is_guess * Is_multiplier;
  end
  
  params_prev = [Is_best; N_best; Rs_best];
  fprintf('Best initial guess:\n');
  fprintf('Is (A) = %.6e\n', Is_best);
  fprintf('N = %.6e\n', N_best);
  fprintf('Rs (?) = %.6e\n', Rs_best);
  fprintf('Residual: %.6e\n', lowest_residual);
  
  % Levenberg-Marquardt lambda parameter initial value and limits
  lambda = 0.01;
  min_lambda = 1e-9;
  max_lambda = 1e5;
  
  % Iteration count & limit
  jIteration = 0;
  max_iterations = 5e4;
  
  % Slow iteration threshold and count limit
  slow_iteration_threshold = 1e-9;
  slow_iteration_count = 0;
  slow_iteration_count_limit = 1000;

  err_prev = error_vector(params_prev);
  res_prev = sum(err_prev.^2);
  mean_abs_err_prev = mean(abs(err_prev));
  
  while true,
    J = jacobian(params_prev);
    params_update = -[J; sqrt(lambda)*eye(3)] \ [err_prev; zeros(3,1)];
    
    params_next = params_prev + params_update;
    err_next = error_vector(params_next);
    res_next = sum(err_next.^2);
    
    if params_next(1,1) > 0 && params_next(2,1) > 0 && params_next(3,1) >= 0 && res_next < res_prev,
      % Exit if we're not making progress
      if res_prev - res_next < slow_iteration_threshold,
        slow_iteration_count = slow_iteration_count + 1;
        if slow_iteration_count > slow_iteration_count_limit,
          fprintf('Routine is making slow progress. Calling it finished.\n');
          break;
        end
      else
        slow_iteration_count = 0;
      end
      
      % Adjust lambda
      if res_next < 0.9 * res_prev,
        lambda = 0.5 * lambda;
        if lambda < min_lambda,
          lambda = min_lambda;
        end
      end
      
      % Perform update
      params_prev = params_next;
      err_prev = err_next;
      res_prev = res_next;
      jIteration = jIteration + 1;
      
      fprintf('Iteration %i, residual: %.6e, lambda: %.3e\n', jIteration, res_prev, lambda);
    elseif lambda < max_lambda,
      lambda = 2 * lambda;
      if lambda > max_lambda,
        lambda = max_lambda;
      end
      fprintf('Missed step, residual: %.6e, lambda: %.3e\n', res_next, lambda);
    else
      fprintf('Error: hit max lambda. Exiting.\n');
      break;
    end
    
    if jIteration > max_iterations,
      fprintf('Error: hit max iteration limit. Exiting.\n');
      break;
    end
  end
  
  fprintf('\nResults:\n');
  fprintf('Iterations: %i\n', jIteration);
  fprintf('Max Absolute Error (V): %.6e\n', max(abs(err_prev)));
  fprintf('Mean Absolute Error (V): %.6e\n', mean(abs(err_prev)));
  fprintf('Is (A): %.6e\n', params_prev(1,1));
  fprintf('N: %.6e\n', params_prev(2,1));
  fprintf('Rs (?): %.6e\n\n', params_prev(3,1));
  
  function err_vec = error_vector (params_err)
    err_vec = zeros(nData, 1);
    for jDatum = 1:nData,
      % Equation for VAK (diode external forward voltage) minus data VAK
      err_vec(jDatum,1) = (params_err(2,1)*k*T/q)*log(data(jDatum,2)/params_err(1,1)+1) + params_err(3,1)*data(jDatum,2) - data(jDatum, 1);
    end
  end
  
  function J = jacobian (params_J)
    J = zeros(nData, 3);
    for jDatum = 1:nData,
      % Partial derivative of equation for VAK (diode external forward voltage) with respect to parameter Is
      J(jDatum, 1) = -data(jDatum,2) * params_J(2,1) * T * k / (params_J(1,1)^2*q*(data(jDatum,2)/params_J(1,1)+1));
      % Partial derivative of equation for VAK (diode external forward voltage) with respect to parameter N
      J(jDatum, 2) = (k*T/q) * log(data(jDatum,2)/params_J(1,1) + 1);
      % Partial derivative of equation for VAK (diode external forward voltage) with respect to parameter Rs
      J(jDatum, 3) = data(jDatum,2);
    end
  end
  
end
