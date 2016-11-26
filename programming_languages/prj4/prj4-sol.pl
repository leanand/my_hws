/** Exercise 1 Requirements: 
 *  quadratic_roots(A, B, C, Roots) should succeed if Roots matches 
 *  a 2-element list containing the two roots of the quadratic 
 *  equation A*x^2 + B*x + C.  The first  element of Roots corresponds 
 *  to the positive square-root of the discriminant; the second element
 *  to the negative square-root of the discriminant.  It is assumed 
 *  that the discriminant is non-negative.  
 */
%Hints: 
%Arithmetic is done in Prolog using operator is/2.  Specifically,
%Var is Expr unifies Var with the result of evaluating Expr which can
%involve usual infix arith operators and function sqrt(...).
%Edited Log:
%?- quadratic_roots(2, 5, 2, Roots).
%Roots = [-0.5, -2.0].
%?- quadratic_roots(5, 6, 1, Roots).
%Roots = [-0.2, -1.0].

quadratic_roots(X, Y, Z, Roots):- 
  Den is 2 * X,
  Num is (Y * Y - 4 * X * Z),
  Fir is (- Y + Num )/ Den ,
  Sec is (- Y - Num )/ Den ,
  Roots = [Fir, Sec].

/** The next few problems involve problems over trees where a Tree is
 *  either leaf(V) or tree(Tree, V, Tree) where value V is any
 *  non-variable prolog term.
 */

/** Exercise 2 Requirements: 
 *  sum_tree(Tree, Sum) should succeed if Tree is a tree containing 
 *  numeric values and Sum matches the sum of all the values in Tree.
 */
%Edited Log:
%?- sum_tree(leaf(5), Sum).
%Sum = 5.
%?- sum_tree(tree(leaf(5), 3, tree(leaf(3), 2, leaf(4))), Sum).
%Sum = 17.
sum_tree(leaf(X), Sum):-
  Sum = X.
sum_tree(tree(X, Y, Z), Sum):-
  sum_tree(X, Sum1),
  sum_tree(Z, Sum2),
  Sum is Y + Sum1 + Sum2.  


/** Exercise 3 Requirements: 
 *  naive_flatten_tree(Tree, Flattened) should succeed if Tree is a tree and
 *  Flattened is a list containing all the values in Tree obtained 
 *  using an in-order traversal.  The solution may not use any
 *  auxiliary procedures.
 */
%Hints: Use built-in append/3.
%Edited Log:
%?- naive_flatten_tree(leaf(5), Flattened).
%Flattened = [5].
%?- naive_flatten_tree(tree(leaf(5), 3, tree(leaf(3), 2, leaf(4))), Flattened).
%Flattened = [5, 3, 3, 2, 4].
%?- naive_flatten_tree(tree(leaf(x), y, tree(leaf([a, b]), 2, leaf([c, z]))),
%                      Flattened).
%Flattened = [x, y, [a, b], 2, [c, z]].

naive_flatten_tree(leaf(X), Flattened):-
  Flattened = [X].
naive_flatten_tree(tree(X, Y, Z), Flattened):-
  naive_flatten_tree(X, Flattened1),
  naive_flatten_tree(Z, Flattened2),
  append(Flattened1 , [ Y | Flattened2], Flattened).


/** Exercise 4 Requirements:
 *  flatten_tree/2 has the same requirements as naive_flatten_tree/2.
 *  However, it may use auxiliary procedures and must run in time 
 *  linear in the size of Tree.
 */
%Hints: Use an auxiliary procedure with an accumulator.
%Use prolog built-in reverse/2.
%Edited Log:
%?- flatten_tree(tree(leaf(5), 3, tree(leaf(3), 2, leaf(4))), Flattened).
%Flattened = [5, 3, 3, 2, 4].
%?- flatten_tree(tree(leaf(x), y, tree(leaf([a, b]), 2, leaf([c, z]))),
%                Flattened).
%Flattened = [x, y, [a, b], 2, [c, z]].

accumulator(leaf(X) , List, Flattened):-
  Flattened = [ X | List ].
accumulator(tree(X, Y, Z) , List, Flattened):-
  accumulator(Z, List, TempList),
  accumulator(X, [ Y | TempList], Flattened).

flatten_tree(X, Flattened):-
  accumulator(X, [], Flattened).



/** Exercise 5 Requirements:
 *  A traditional notation regex can be represented as a Prolog term 
 *  as follows:
 *
 *  A prolog string "abcd" represents the character class [abcd].
 *  (Recall that a Prolog string is a list of integers corresponding to
 *  the character codes of the characters in the string).
 *
 *  The prolog term range(C1, C2) represents the character class [C1-C2].
 *
 *  The prolog term A + B represents the regex A|B.
 *
 *  The prolog term A * B represents the regex AB.
 *
 *  The prolog term optional(A) represents the regex A?.
 *
 *  The prolog term closure0(A) represents A*.
 *
 */
%Sample regex equivalent to [-+]?([0-9] | [0-9][0-9_]*[0-9])
sample_regex(optional(Sign) * 
             (Digits + 
	      Digits * closure0(XDigits) * Digits)):-
  Sign = "-+",
  Digits = range(0'0, 0'9),
  XDigits = Digits + "_".


/** Exercise 5 Requirements Continued:
 *  regex_match(Regex, String) succeeds iff prolog string String matched
 *  regex Regex.
 */
%Hints:
%Use Prolog backtracking to help with the matching.
%The infix binary operators =< and >= can be used for comparing numbers.
%Specifically A =< B, succeeds iff number A is less-than-or-equal-to
%number B; similarly, >= for greater-than-or-equal-to.
%
%Edited Log:
%?- sample_regex(A), regex_match(A, "12_3").
%A = ...
%?- sample_regex(A), regex_match(A, "+12_3").
%A = ...
%?- sample_regex(A), regex_match(A, "-12_3").
%A = ...
%?- sample_regex(A), regex_match(A, "-12_3_").
%false.

as -->  [].
as -->  [a], as.

regex_match_aux(_, []).

regex_match_aux(range(X, Y), [H | T]) :-
  X =< H,
  Y >= H,
  regex_match_aux([], T).

regex_match_aux(optional(String), [H | T]) :-
  string_to_list(String, List),
  ( member(H , List) 
  -> regex_match_aux([], T)
  ; regex_match_aux([], [H | T])
  ).

regex_match_aux(closure0(X), [H | T]) :-
  X =:= H,
  regex_match_aux([], T).

regex_match(List, String):-
  List = String.
/** Exercise 6 Requirements:
 *  A NFA is represented as nfa(Initial, Transitions, Finals).
 *
 *  Initial is the initial state (any non-var Prolog term) of the NFA.
 *
 *  Transitions is a list of transition(State1, Input, State2)
 *  representing a transition from nfa state State1 to nfa state State2 
 *  under input symbol Input. The reserved input  symbol 'epsilon' 
 *  is used to indicate a transition which does not require any input symbol.  
 *  Note that states and input symbols can be any ground Prolog term 
 *  (without any Prolog vars).
 *
 *  Finals is a list of accepting nfa states.  
 *
 */
sample_nfa(nfa(s0,
	       [transition(s0, 1, s1), transition(s0, 1, s2),
                transition(s2, epsilon, s3),
                transition(s3, 0, s3), transition(s3, 1, s3),
		transition(s3, 1, s4)],
	       [s4])).

/** Exercise 6 Requirements Continued:
 *  nfa_simulate(NFA, Inputs, States, Z) should succeed when
 *  NFA is a nfa, Inputs is a list of input symbols, States is the list
 *  of states which can be transited in NFA under Inputs and Z is
 *  either the atom 'accept' or 'reject'.  Z is 'accept' if NFA
 *  is in a state in Finals after consuming all input symbols
 *  in Inputs; it is 'reject' if the NFA is in a state not in
 *  Finals after consuming all input symbols or if it gets
 *  into a state where no transitions are possible.
 */
%Hints: 
%Use Prolog backtracking to simulate the non-determinism
%inherent in a NFA.  
%Use built-in member/2 to lookup transitions.  Specifically,
%member(transition(S1, I, S2), Transitions) can
%be used to lookup any applicable transition from S1 to S2 under
%I.
%The prefix unary operator \+ can be used to negate a Prolog goal;
%i.e. \+ P succeeds (fails) iff P fails (succeeds).
%Edited Log: (your log need not be identical but should reflect
%essentially the same behavior);
%?- sample_nfa(N), nfa_simulate(N, [1, 0, 1], States, Z).
%N = ...
%States = [s0, s1],
%Z = reject ;
%N = ...
%States = [s0, s2, s3, s3],
%Z = reject ;
%N = ,,,
%States = [s0, s2, s3, s3, s4],
%Z = accept ;
%false.
%?- sample_nfa(N), nfa_simulate(N, [0, 0, 1], States, Z).
%N = ...
%States = [s0],
%Z = reject.


check_transition(Transitions, InputState, Input, NextState):-
  member(transition(InputState, Input, NextState), Transitions);
  member(transition(InputState, epsilon, NextState), Transitions).

simulate(nfa(InitialState, _, FinalState), [], StatesList, States, Z):-
  member(InitialState, FinalState),
  reverse([InitialState | StatesList], States), Z = accept.

simulate(nfa(InitialState, _, FinalState), [], StatesList, States, Z):-
  \+ member(InitialState, FinalState),
  reverse([InitialState | StatesList], States), Z = reject.

simulate(nfa(InitialState, Transitions, FinalState), [ Input | Tail ], StatesList, States, Z):-
  check_transition(Transitions, InitialState, Input, NextState),
  simulate(nfa(NextState, Transitions, FinalState), Tail, [InitialState | StatesList], States, Z).

simulate(nfa(InitialState, Transitions, _), [ Input | _ ], StatesList, States, Z):-
  \+ check_transition(Transitions, InitialState, Input, _),
  reverse([InitialState | StatesList], States), Z = reject.

nfa_simulate(NFA, Inputs, States, Z):-
  simulate(NFA, Inputs, [], States, Z).

