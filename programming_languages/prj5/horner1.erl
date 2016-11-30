-module(horner1).
-export([poly_eval/1]).

%Given Coeffs list [C[N], C[N-1], ..., C[1], C[0]], specifying 
%the polynomial
%
%   C[N]*X^(N) + C[N-1]]*X^(N-1) + ... + C[1]*X^(1) + C[0]
%
%return a function of one variable X which when called returns the
%value of the polynomial at X.
poly_eval(Coeffs) ->
  fun(InputX) ->
        lists:foldl(fun(Initial, Acc)-> (Acc * InputX) + Initial end, 0, Coeffs)
  end.
  
