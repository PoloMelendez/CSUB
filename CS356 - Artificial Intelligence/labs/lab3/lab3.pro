exist(a, 1.0).
exist(b, 0.7).
exist(c, 0.75).
exist(d, 0.8).
exist(e, 0.5).
exist(m, -1.0).

exist(z, CF) :- exist(y, CFY), exist(d, CFD),
                my_min([CFY, CFD], X),
                CF is X * 0.7.
exist(y, CF) :- exist(x, CFX), exist(b, CFB), exist(e, CFE),
                my_min([CFX, CFB, CFE], X),
                CF is X * 0.95.
exist(x, CF) :- exist(a, CFA),
                CF is CFA * 1.0.
exist(l, CF) :- exist(c, CFC),
                CF is CFC * 0.85.
exist(n, CF) :- exist(l, CFL), exist(m, CFM),
                my_min([CFL, CFM],X),
                CF is X * 1.0.

my_min([First], First) :- !.            % found min so cut
my_min([First,Second|Tail], M) :-
    ( First > Second ->                 % if First > Second
        my_min([Second|Tail], M)        % min rest of list, excluding First
        ;                               % else
        my_min([First|Tail], M)         % min rest of list, excluding Second
    ).
