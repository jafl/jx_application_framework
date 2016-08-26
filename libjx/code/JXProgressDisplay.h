/******************************************************************************
 JXProgressDisplay.h

  Interface for the JXProgressDisplay class.

  Copyright © 1995 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXProgressDisplay
#define _H_JXProgressDisplay

#include <JProgressDisplay.h>
#include <JBroadcaster.h>

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

	virtual JBoolean	IncrementProgress(const JCharacter* message = NULL);
	virtual JBoolean	IncrementProgress(const JSize delta);
	virtual JBoolean	IncrementProgress(const JCharacter* message,
										  const JSize delta);
	virtual JBoolean	ProcessContinuing();
	virtual void		ProcessFinished();
	virtual void		DisplayBusyCursor();

protected:

	virtual void	ProcessBeginning(const ProcessType processType,
									 const JSize stepCount,
									 const JCharacter* message, 
									 const JBoolean allowCancel,
									 const JBoolean allowBackground);

	virtual void		AppendToMessageWindow(const JCharacter* message);
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

	JBoolean	IncrementProgress1(const JCharacter* message);

	// not allowed

	JXProgressDisplay(const JXProgressDisplay& source);
	const JXProgressDisplay& operator=(const JXProgressDisplay& source);

public:

	// JBroadcaster messages

	static const JCharacter* kCancelRequested;

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
