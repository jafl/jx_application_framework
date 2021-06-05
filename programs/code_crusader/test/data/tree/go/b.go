package b

import "a"

type B1 struct /* comment */ { /* comment */
	a.A1
	x, y int
	_ float32  // padding
	F func()
}

type b2 struct {
	a.A1
}

type B3 interface {
}

type b4 interface {
}
