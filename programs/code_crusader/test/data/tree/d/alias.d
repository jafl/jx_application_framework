template Foo2(T) { alias t = T; }
alias t1 = Foo2!(int);
alias t2 = Foo2!(int).t;
alias t3 = t1.t;
alias t4 = t2;

template Foo3(T) : Foo2(T) {}

class Foo4 : t1 {}

class Foo5 : t3 {}
