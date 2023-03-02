#include "../../../libjx/code/JXBoostPriorityScheduler.h"
#include <thread>
#include <chrono>

void f(int i)
{
	std::cout << "before exit " << i << std::endl;
//	exit(0);
	std::cout << "after exit " << i << std::endl;
}

void y()
{
	while (1)
	{
		std::cout << "start y" << std::endl;
		boost::this_fiber::sleep_for(std::chrono::duration<long, std::deci>(1));
		std::cout << "end y" << std::endl;
	}
}

int
main
	(
	int argc,
	char** argv
	)
{
//	std::thread t1(f, -1);

	boost::fibers::use_scheduling_algorithm<JXBoostPriorityScheduler>();

	auto fiber = boost::fibers::fiber(std::bind(f,1));
	fiber.properties<JXBoostPriorityProps>().SetPriority(1);
	fiber.detach();

	fiber = boost::fibers::fiber(std::bind(f,2));
	fiber.properties<JXBoostPriorityProps>().SetPriority(2);
	fiber.detach();

//	std::thread t2(f, -2);

	fiber = boost::fibers::fiber(std::bind(f,3));
	fiber.properties<JXBoostPriorityProps>().SetPriority(1);
	fiber.detach();

	fiber = boost::fibers::fiber(std::bind(f,4));
	fiber.properties<JXBoostPriorityProps>().SetPriority(2);
	fiber.detach();

	fiber = boost::fibers::fiber(std::bind(f,5));
	fiber.properties<JXBoostPriorityProps>().SetPriority(2);
	fiber.detach();

	fiber = boost::fibers::fiber(y);
	fiber.properties<JXBoostPriorityProps>().SetPriority(10);
	fiber.detach();

	boost::this_fiber::yield();
	std::cout << "end of main" << std::endl;
	return 0;
}
