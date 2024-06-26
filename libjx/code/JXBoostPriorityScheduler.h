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
		itsPriority(0),
		itsName(nullptr)
	{
	}

	~JXBoostPriorityProps()
	{
		jdelete itsName;
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

	const JString* GetName() const
	{
		return itsName;
	}

	void SetName(JString* name)
	{
		itsName = name;
	}

private:

	int			itsPriority;
	JString*	itsName;
};

class JXBoostPriorityScheduler :
	public boost::fibers::algo::algorithm_with_properties<JXBoostPriorityProps>
{
	using rqueue_t = boost::fibers::scheduler::ready_queue_type;

public:

	JXBoostPriorityScheduler()
		:
		itsNotifyFlag(false)
	{
	}

	// fiber is ready to run

	void awakened
		(
		boost::fibers::context*	ctx,
		JXBoostPriorityProps&	props
		)
		noexcept
		override
	{
		const int p = props.GetPriority();

		// With this scheduler, fibers with higher priority values are
		// preferred over fibers with lower priority values. But fibers with
		// equal priority values are processed in round-robin fashion. So when
		// we're handed a new context*, put it at the end of the fibers
		// with that same priority. In other words: search for the first fiber
		// in the queue with LOWER priority, and insert before that one.

		auto i(std::ranges::find_if(itsReadyQ.begin(), itsReadyQ.end(),
				[this, p](boost::fibers::context& c)
				{
					return properties(&c).GetPriority() < p;
				}));

		// Now, whether or not we found a fiber with lower priority,
		// insert this new fiber here.

		itsReadyQ.insert( i, *ctx);
	}

	// tell the manager which fiber to run

	boost::fibers::context* pick_next() noexcept override
	{
		if (itsReadyQ.empty())
		{
			return nullptr;
		}

		boost::fibers::context* ctx(&itsReadyQ.front());
		itsReadyQ.pop_front();

		if (debug_fiber > 0)
		{
			const JString* name = dynamic_cast<JXBoostPriorityProps*>(ctx->get_properties())->GetName();
			if (name != nullptr)
			{
				std::cout << "activate: " << ctx << " " << *name << std::endl;
			}
		}
		return ctx;
	}

	bool has_ready_fibers() const noexcept override
	{
		return ! itsReadyQ.empty();
	}

	// reshuffle the queue based on the new priority

	void property_change
		(
		boost::fibers::context*	ctx,
		JXBoostPriorityProps&	props
		)
		noexcept
		override
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
		override
	{
		std::unique_lock lock(itsMutex);
		auto f = [this](){ return itsNotifyFlag; };

		if (std::chrono::steady_clock::time_point::max() == time_point)
		{
			itsCondition.wait(lock, f);
		}
		else
		{
			itsCondition.wait_until(lock, time_point, f);
		}

		itsNotifyFlag = false;
	}

	void notify() noexcept override
	{
		std::unique_lock lock(itsMutex);
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
