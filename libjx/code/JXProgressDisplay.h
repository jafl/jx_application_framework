/******************************************************************************
 JXProgressDisplay.h

  Interface for the JXProgressDisplay class.

  Copyright (C) 1995 by Glenn W. Bach. All rights reserved.

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
					 JXTEBase* label = NULL);

	virtual JBoolean	IncrementProgress(const JString& message = JString::empty);
	virtual JBoolean	IncrementProgress(const JSize delta);
	virtual JBoolean	IncrementProgress(const JString& message,
										  const JSize delta);
	virtual JBoolean	ProcessContinuing();
	virtual void		ProcessFinished();
	virtual void		DisplayBusyCursor();

protected:

	virtual void	ProcessBeginning(const ProcessType processType,
									 const JSize stepCount,
									 const JString& message, 
									 const JBoolean allowCancel,
									 const JBoolean allowBackground);

	virtual void		AppendToMessageWindow(const JString& message);
	virtual JBoolean	CheckForCancel();

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean	itsCancelFlag;

	JXTextButton*			itsCancelButton;		// can be NULL unless cancellable
	JXStaticText*			itsCounter;				// can be NULL unless var length
	JXProgressIndicator*	itsIndicator;			// can be NULL unless fixed length
	JXTEBase*				itsLabel;				// can be NULL

	JXPGMessageDirector*	itsMessageDirector;		// NULL unless messages during process

private:

	JBoolean	IncrementProgress1(const JString& message);

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
