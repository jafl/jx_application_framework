/******************************************************************************
 JXProgressDisplay.h

  Interface for the JXProgressDisplay class.

  Copyright (C) 1995 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXProgressDisplay
#define _H_JXProgressDisplay

#include <jx-af/jcore/JProgressDisplay.h>
#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/JString.h>

class JXTextButton;
class JXTEBase;
class JXStaticText;
class JXProgressIndicator;
class JXPGMessageDirector;

class JXProgressDisplay : public JProgressDisplay, virtual public JBroadcaster
{
public:

	JXProgressDisplay();

	~JXProgressDisplay() override;

	void	SetItems(JXTextButton* cancelButton, JXStaticText* counter,
					 JXProgressIndicator* indicator,
					 JXTEBase* label = nullptr);

	bool	IncrementProgress(const JString& message = JString::empty) override;
	bool	IncrementProgress(const JSize delta) override;
	bool	IncrementProgress(const JString& message,
										  const JSize delta) override;
	bool	ProcessContinuing() override;
	void	ProcessFinished() override;
	void	DisplayBusyCursor() override;

protected:

	void	ProcessBeginning(const ProcessType processType,
									 const JSize stepCount,
									 const JString& message, 
									 const bool allowCancel,
									 const bool allowBackground) override;

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

private:

	bool	IncrementProgress1(const JString& message);

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
