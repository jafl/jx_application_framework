package c

import "a"
import z "b"
import . "d"
import _ "x"

type C1 struct {
	x, y int
	_ float32  // padding
}

type C2 struct {
	C1
	a.A1
	z.B1
	x, y int
	_ float32  // padding
	F func()
	*a.A2
}

type c3 interface {
	a.A2
	Read([]byte) (int, error)
	Write([]byte) (int, error)
	Close() error
	z.B3
	D1
}
