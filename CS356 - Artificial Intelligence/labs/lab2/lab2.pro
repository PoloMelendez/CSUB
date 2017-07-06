fact(a).
fact(b).
fact(c).
fact(d).
fact(e).

fact(z) :- fact(y), fact(d).
fact(y) :- fact(x), fact(b), fact(e).
fact(x) :- fact(a).
fact(l) :- fact(c).
fact(n) :- fact(l), fact(m).
