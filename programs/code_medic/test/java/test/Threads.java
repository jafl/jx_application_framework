package test;

import java.util.*;

public final class Threads
{
	public static void main
		(
		String[] args
		)
		throws Exception
	{
		final Random r = new Random();

		List<ThreadGroup> groups     = new ArrayList<ThreadGroup>();
		List<ThreadGroup> deadGroups = new ArrayList<ThreadGroup>();
		while (true)
		{
			ThreadGroup g;
			if (r.nextFloat() < 0.2 || groups.isEmpty())
			{
				ThreadGroup p = Thread.currentThread().getThreadGroup();
				if (!groups.isEmpty() && r.nextFloat() < 0.4)
				{
					p = groups.get(r.nextInt(groups.size()));
				}

				g = new ThreadGroup(p, "ThreadGroup-" + r.nextInt());
				System.out.println("new " + g.getName() + (p != null ? "; parent " + p.getName() : ""));
				groups.add(g);
			}
			if (r.nextFloat() < 0.1 && groups.size() > 1)
			{
				g = groups.remove(r.nextInt(groups.size()));
				System.out.println("removing " + g.getName());
				deadGroups.add(g);
			}

			g = null;
			if (r.nextFloat() < 0.8)
			{
				g = groups.get(r.nextInt(groups.size()));
			}

			Thread t = new Thread(g, new TestThread(r));
			System.out.println("starting " + t.getName() + (g != null ? " -- " + g.getName() : ""));

			t.start();

			Thread.currentThread().sleep(100*r.nextInt(20));

			for (int i=deadGroups.size()-1; i>=0; i--)
			{
				try
				{
					g = deadGroups.get(i);
					g.destroy();
					deadGroups.remove(i);
				}
				catch (Exception ex)
				{
					System.err.println(ex.getClass().getName());
				}
			}
		}
	}
}
