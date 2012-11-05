// no parents -- ignored

class A {};

// 1 parent -- ignored

class A1 {};
class A2 : public A1 {};

// 2 parents -- simplest case

class B1 {};
class B2 {};
class B3 : public B1, public B2 {};

// 3 parents -- 1 on each side

class C1 {};
class C2 {};
class C3 {};
class C4 : public C1, public C2, public C3 {};

// 4 parents -- 1 above, 2 below

class D1 {};
class D2 {};
class D3 {};
class D4 {};
class D5 : public D1, public D2, public D3, public D4 {};

// 5 parents -- 2 on each side

class E1 {};
class E2 {};
class E3 {};
class E4 {};
class E5 {};
class E6 : public E1, public E2, public E3, public E4, public E5 {};

/*

F1\
F2-F4-F5
F3---/

G1\
G2-G5-G4
G3---/

H1-H4
  /
H2-H5
  \
H3-H6

all children of all roots of first MI class are checked

*/

class F1 {};
class F2 {};
class F3 {};
class F4 : public F2, public F1 {};
class F5 : public F4, public F3 {};

class G1 {};
class G2 {};
class G3 {};
class G4 : public G5, public G3 {};
class G5 : public G2, public G1 {};

class H1 {};
class H2 {};
class H3 {};
class H4 : public H1, public H2 {};
class H5 : public H2 {};
class H6 : public H3, public H2 {};

// MI within a single tree -- nothing to do

class J1 {};
class J2 : public J1 {};
class J3 : public J1 {};
class J4 : public J2, protected J3 {};
