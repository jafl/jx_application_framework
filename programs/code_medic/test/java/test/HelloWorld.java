package test;

public final class HelloWorld
{
	public static void main
		(
		String[] args
		)
		throws Exception
	{
		System.out.println("Hello " + args[0] + "!");

		System.out.print("> ");

		byte[] data = new byte[1000];
		int count   = System.in.read(data);
		if (data[count-1] == '\n')
			{
			count--;
			}
		System.out.println("Thank you for " + (new String(data, 0, count)));
	}
}
