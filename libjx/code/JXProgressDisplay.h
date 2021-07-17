/******************************************************************************
 JXProgressDisplay.h

  Interface for the JXProgressDisplay class.

  Copyright (C) 1995 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXProgressDisplay
#define _H_JXProgressDisplay

#include <JProgressDisplay.h>
#include <JBroadcaster.h>
#include <JString.h>

class JXTextButton;
class JXTEBase;
class JXStaticText;
class JXProgressIndicator;
class JXPGMessageDirector;

class JXProgressDisplay : public JProgressDisplay, virtual public JBroadcaster
{
public:

	JXProgressDisplay();

	virtual ~JXProgressDisplay();

	void	SetItems(JXTextButton* cancelButton, JXStaticText* counter,
					 JXProgressIndicator* indicator,
					 JXTEBase* label = nullptr);

	virtual bool	IncrementProgress(const JString& message = JString::empty) override;
	virtual bool	IncrementProgress(const JSize delta) override;
	virtual bool	IncrementProgress(const JString& message,
										  const JSize delta) override;
	virtual bool	ProcessContinuing() override;
	virtual void		ProcessFinished() override;
	virtual void		DisplayBusyCursor() override;

protected:

	virtual void	ProcessBeginning(const ProcessType processType,
									 const JSize stepCount,
									 const JString& message, 
									 const bool allowCancel,
									 const bool allowBackground) override;

	virtual void		AppendToMessageWindow(const JString& message);
	virtual bool	CheckForCancel() override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsCancelFlag;

	JXTextButton*			itsCancelButton;		// can be nullptr unless cancellable
	JXStaticText*			itsCounter;				// can be nullptr unless var length
	JXProgressIndicator*	itsIndicator;			// can be nullptr unless fixed length
	JXTEBase*				itsLabel;				// can be nullptr

	JXPGMessageDirector*	itsMessageDirector;		// nullptr unless messages during process

private:

	bool	IncrementProgress1(const JString& message);

	// not allowed

	JXProgressDisplay(const JXProgressDisplay& source);
	const JXProgressDisplay& operator=(const JXProgressDisplay& source);

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
