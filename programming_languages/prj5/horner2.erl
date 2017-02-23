-module(horner2).
-export([server/1, client/2, do_server/1, child_coeff/1]).

%Given Coeffs list [C[N], C[N-1], ..., C[1], C[0]], set up a server
%to evaluate the polynomial
%
%   C[N]*X^(N) + C[N-1]]*X^(N-1) + ... + C[1]*X^(1) + C[0]
%
%at some (yet unspecified) X.
%
%The server must consist of multiple Erlang processes with one Erlang 
%process per polynomial coefficient (there may be additional processes too).
%The processes for the coefficients should store their respective
%coefficients and communicate among themselves to evaluate the
%polynomial at a specified X using Horner's rule.
%
%The return value of this function should be the PID of the server.
%
%The details of the messages exchanged between the processes for
%evaluating the polynomial can be chosen by the implementation.
%However, the server must respond to a 'stop' message by stopping
%all the processes as well as itself.
%
%When a process for a coefficient is assisting in evaluating the
%polynomial, it must log to standard error it's stored coeffient
%and the value accumulated so far in the Horner evaluation.
%When a process for a coefficient is being stopped, it must
%log to standard error it's stored coefficient and the 'stop'
%message.  

do_server(ListPids) ->
  [HeadPid | TailPids ] = ListPids,
  receive
    {ClientPid, calculate, X} ->
      HeadPid ! {self(), ClientPid, TailPids, X, 0},
      do_server(ListPids);
    {calculated, ClientPid, Output} ->
      ClientPid ! {result, Output},
      do_server(ListPids);
    stop ->
      HeadPid ! {TailPids, stop},
      true
  end.


server(Coeffs) ->
  ListPids = lists:map(fun(Coeff) -> spawn(horner2, child_coeff, [Coeff]) end, Coeffs),
  spawn(horner2, do_server, [ListPids]).
  

%Return the value at X of the polynomial stored within the polynomial server
%specified by PID.
client(Pid, X) ->
  Pid ! {self(), calculate, X},
  receive
    {result, Output} ->
      Output
  end.


do_log(Coeff, Msg) ->
    io:format(standard_error, "coeff = ~w; ~w~n", [Coeff, Msg]).


child_coeff(Coeff)->
  receive
    {ServerPid, ClientPid, [NextPid|RemainingPid], X, Acc} ->
      TempAcc = (X * Acc) + Coeff,
      do_log(Coeff, TempAcc),
      NextPid ! {ServerPid, ClientPid, RemainingPid, X, TempAcc},
      child_coeff(Coeff);
    {ServerPid, ClientPid, [], X, Acc} ->
      TempAcc = (X * Acc) + Coeff,
      do_log(Coeff, TempAcc),
      ServerPid ! {calculated, ClientPid, TempAcc},
      child_coeff(Coeff);
    {[NextPid|RemainingPid] , stop} ->
      do_log(Coeff, stop),
      NextPid ! {RemainingPid, stop},
      true;
    { [], stop} ->
      do_log(Coeff, stop),
      true
  end.
