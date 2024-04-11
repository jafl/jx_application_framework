/******************************************************************************
 JXProgressDisplay.h

	Copyright (C) 1995 by Glenn W. Bach.
	Copyright (C) 1997-2024 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXProgressDisplay
#define _H_JXProgressDisplay

#include <jx-af/jcore/JProgressDisplay.h>
#include <jx-af/jcore/JBroadcaster.h>
#include <boost/fiber/condition_variable.hpp>

class JString;
class JXTextButton;
class JXTEBase;
class JXStaticText;
class JXProgressIndicator;
class JXPGMessageDirector;
class JXFunctionTask;

class JXProgressDisplay : public JProgressDisplay, virtual public JBroadcaster
{
public:

	JXProgressDisplay();

	~JXProgressDisplay() override;

	void	SetItems(JXTextButton* cancelButton, JXStaticText* counter,
					 JXProgressIndicator* indicator,
					 JXTEBase* label = nullptr);

	using JProgressDisplay::IncrementProgress;

	bool	IncrementProgress(const JString& message, const JSize delta) override;
	bool	ProcessContinuing() override;
	void	ProcessFinished() override;
	void	DisplayBusyCursor() override;

protected:

	void	ProcessBeginning(const ProcessType processType,
							 const JSize stepCount,
							 const JString& message,
							 const bool allowCancel,
							 const bool modal) override;

	virtual void	AppendToMessageWindow(const JString& message);
	bool			CheckForCancel() override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsCancelFlag;

	JXTextButton*			itsCancelButton;		// can be nullptr unless cancellable
	JXStaticText*			itsCounter;				// can be nullptr unless var length
	JXProgressIndicator*	itsIndicator;			// can be nullptr unless fixed length
	JXTEBase*				itsLabel;				// can be nullptr

	JXPGMessageDirector*	itsMessageDirector;		// nullptr unless messages during process

	JXFunctionTask*						itsContinueTask;
	boost::fibers::condition_variable	itsCondition;
	boost::fibers::mutex				itsMutex;
	bool								itsContinueFlag;

private:

	bool	CalledByIncrementProgress(const JString& message);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kCancelRequested;

	class CancelRequested : public JBroadcaster::Message
		{
		public:

			CancelRequested()
				:
				JBroadcaster::Message(kCancelRequested)
				{ };
		};
};

#endif
