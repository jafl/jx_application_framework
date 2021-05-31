<?php

namespace A;

abstract class A1
{
	function __construct()
	{
	}

	public function foo() { }
	protected function bar($b, $c) { }
	private function baz($a) { }
	abstract function shug() {}
	abstract protected static function whee() { }
}
