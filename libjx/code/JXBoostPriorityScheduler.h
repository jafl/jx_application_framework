//          Copyright Nat Goodspeed 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See http://www.boost.org/LICENSE_1_0.txt)

#include <boost/fiber/fiber.hpp>
#include <boost/fiber/scheduler.hpp>
#include <boost/fiber/operations.hpp>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <algorithm>	// std::find_if()

class JXBoostPriorityProps : public boost::fibers::fiber_properties
{
public:

	JXBoostPriorityProps
		(
		boost::fibers::context* ctx
		)
		:
		fiber_properties(ctx),
		itsPriority(0)
	{
	}

	int GetPriority() const
	{
		return itsPriority;
	}

	void SetPriority
		(
		const int p
		)
	{
		if ( p != itsPriority)
		{
			itsPriority = p;
			notify();
		}
	}

private:

	int itsPriority;
};

class JXBoostPriorityScheduler :
	public boost::fibers::algo::algorithm_with_properties<JXBoostPriorityProps>
{
	typedef boost::fibers::scheduler::ready_queue_type rqueue_t;

public:

	JXBoostPriorityScheduler()
		:
		itsNotifyFlag(false)
	{
	}

	// fiber is ready to run

	virtual void awakened
		(
		boost::fibers::context*	ctx,
		JXBoostPriorityProps&	props
		)
		noexcept
	{
		const int p = props.GetPriority();

		// With this scheduler, fibers with higher priority values are
		// preferred over fibers with lower priority values. But fibers with
		// equal priority values are processed in round-robin fashion. So when
		// we're handed a new context*, put it at the end of the fibers
		// with that same priority. In other words: search for the first fiber
		// in the queue with LOWER priority, and insert before that one.

		rqueue_t::iterator i(
			std::find_if(itsReadyQ.begin(), itsReadyQ.end(),
				[this, p](boost::fibers::context& c)
				{
					return properties(&c).GetPriority() < p;
				}));

		// Now, whether or not we found a fiber with lower priority,
		// insert this new fiber here.

		itsReadyQ.insert( i, *ctx);
	}

	// tell the manager which fiber to run

	virtual boost::fibers::context* pick_next() noexcept
	{
		if (itsReadyQ.empty())
		{
			return nullptr;
		}
/*
		std::cout << "ready fibers:";
		std::for_each(itsReadyQ.begin(), itsReadyQ.end(), [this](boost::fibers::context& c)
		{
			std::cout << ' ' << properties(&c).GetPriority();
		});
		std::cout << std::endl << std::endl;
*/
		boost::fibers::context* ctx(&itsReadyQ.front());
		itsReadyQ.pop_front();
		return ctx;
	}

	virtual bool has_ready_fibers() const noexcept
	{
		return ! itsReadyQ.empty();
	}

	// reshuffle the queue based on the new priority

	virtual void property_change
		(
		boost::fibers::context*	ctx,
		JXBoostPriorityProps&	props
		)
		noexcept
	{
		if (ctx->ready_is_linked())
		{
			ctx->ready_unlink();
			awakened(ctx, props);
		}
	}

	void suspend_until
		(
		std::chrono::steady_clock::time_point const& time_point
		)
		noexcept
	{
		std::unique_lock<std::mutex> lock(itsMutex);
		auto f = [this](){ return itsNotifyFlag; };

		if ((std::chrono::steady_clock::time_point::max)() == time_point)
		{
			itsCondition.wait(lock, f);
		}
		else
		{
			itsCondition.wait_until(lock, time_point, f);
		}

		itsNotifyFlag = false;
	}

	void notify() noexcept
	{
		std::unique_lock<std::mutex> lock(itsMutex);
		itsNotifyFlag = true;
		lock.unlock();
		itsCondition.notify_all();
	}

private:

	rqueue_t                    itsReadyQ;
	std::mutex                  itsMutex;
	std::condition_variable     itsCondition;
	bool                        itsNotifyFlag;
};
