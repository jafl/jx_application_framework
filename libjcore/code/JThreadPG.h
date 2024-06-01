/******************************************************************************
 JThreadPG.h

	Copyright © 2007-2024 by John Lindal.

 ******************************************************************************/

#ifndef _H_JThreadPG
#define _H_JThreadPG

#include "JProgressDisplay.h"
#include "JBroadcaster.h"		// need Message
#include <boost/fiber/buffered_channel.hpp>

class JThreadPG : public JProgressDisplay
{
public:

	JThreadPG(boost::fibers::buffered_channel<JBroadcaster::Message*>* channel,
			  std::atomic_bool* cancelFlag = nullptr, const JSize scaleFactor = 1);

	~JThreadPG() override;

	using JProgressDisplay::IncrementProgress;

	bool	IncrementProgress(const JString& message, const JSize delta) override;
	void	ProcessFinished() override;
	void	DisplayBusyCursor() override;

	void	WaitForProcessFinished(JProgressDisplay* pg);

protected:

	void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
							 const JString& message, const bool allowCancel,
							 const bool modal) override;

	bool	CheckForCancel() override;

private:

	boost::fibers::buffered_channel<JBroadcaster::Message*>*	itsChannel;	// not owned

	std::atomic_bool*	itsCancelFlag;	// not owned
	const JSize			itsScaleFactor;
	JSize				itsCounter;

public:

	// JBroadcaster messages

	static const JUtf8Byte* kProcessBeginning;
	static const JUtf8Byte* kIncrementProgress;
	static const JUtf8Byte* kProcessFinished;

	class ProcessBeginning : public JBroadcaster::Message
	{
	public:

		ProcessBeginning(const ProcessType processType, const JSize stepCount,
						 const JString& message, const bool allowCancel);

		~ProcessBeginning() override;

		ProcessType
		GetProcessType()
			const
		{
			return itsProcessType;
		};

		JSize
		GetStepCount()
			const
		{
			return itsStepCount;
		};

		bool
		GetMessage(const JString** m)
			const
		{
			*m = itsMessage;
			return itsMessage != nullptr;
		};

		bool
		AllowCancel()
			const
		{
			return itsAllowCancelFlag;
		};

	private:

		const ProcessType	itsProcessType;
		const JSize			itsStepCount;
		JString*			itsMessage;
		const bool			itsAllowCancelFlag;
	};

	class IncrementProgress : public JBroadcaster::Message
	{
	public:

		IncrementProgress(const JString& message, const JSize delta);

		~IncrementProgress() override;

		bool
		GetMessage(const JString** m)
			const
		{
			*m = itsMessage;
			return itsMessage != nullptr;
		};

		JSize
		GetDelta()
			const
		{
			return itsDelta;
		};

	private:

		JString*	itsMessage;
		const JSize	itsDelta;
	};

	class ProcessFinished : public JBroadcaster::Message
	{
	public:

		ProcessFinished()
			:
			JBroadcaster::Message(kProcessFinished)
		{ };
	};
};

#endif
