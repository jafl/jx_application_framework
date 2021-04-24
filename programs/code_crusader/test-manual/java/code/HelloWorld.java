package com.nps;

public final class HelloWorld
{
	public static void main(String[] args)
		{
		System.out.println("Hello world!");

		char x = 'y';
		long i = 0x35;
		long j = 0x0FFFFFFFF;
		long k = 0x1FFFFFFFF;
		char c = '\x35';

		String s = "h" + "y" + "z";

		/* comment */

		/**<pre>@see</tag> comment @see junk */
		}

	private void foo() { }

	void foodyhoo() { };

	protected void g() { };
}
