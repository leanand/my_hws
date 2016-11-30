-module(roots2).
-export([server/0, client/2, listener/0]).

-import(roots1, [roots/3]).


listener() ->
  receive
    {Pid, {A, B, C}} ->
      Output = roots1:roots(A, B, C),
      Pid ! Output,
      listener();
    stop ->
      true
  end.
%Return the PID of a quadratic-solver Erlang process which when given
%a message containing the coefficients A, B, C responds with a message
%giving the roots of the quadratic equation A*X^2 + B*X + C.  It is
%assumed that the return'd roots are not complex.
server() ->
  Pid = spawn(roots2, listener, []),
  Pid.

%Given the PID of a quadratic-solver and Coeffs a 3-tuple {A, B, C}, 
%this function uses that quadratic-solver to return the roots of the
%quadratic equation A*X^2 + B*X + C.  The roots are return'd as a
%2-tuple, with the first element of the return'd tuple using the
%positive square-root of the discriminant and the second element of
%the return'd tuple using the negative square-root of the
%discriminant.  It is assumed that the return'd roots are not complex.
client(PID, Coeffs) ->
  PID ! {self(), Coeffs},
  receive
    Output->
      Output
  end.

