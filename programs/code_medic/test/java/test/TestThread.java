package test;

import java.util.*;

public final class TestThread
	implements Runnable
{
	Random r;

	public TestThread
		(
		Random _r
		)
	{
		r = _r;
	}

	public void run()
	{
		Thread t1 = Thread.currentThread();
		try
		{
			t1.sleep(100*r.nextInt(25));
			t1.sleep(100*r.nextInt(25));
			t1.sleep(100*r.nextInt(25));
			t1.sleep(100*r.nextInt(25));
		}
		catch (InterruptedException ex)
		{
			System.out.println("interrupted");
		}
		System.out.println("finished " + t1.getName());
	}
}
