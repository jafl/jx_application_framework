/******************************************************************************
 JXTipOfTheDayDialog.h

	Copyright © 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTipOfTheDayDialog
#define _H_JXTipOfTheDayDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXStaticText;
class JXTextButton;
class JXTextCheckbox;

class JXTipOfTheDayDialog : public JXDialogDirector
{
public:

	JXTipOfTheDayDialog(const JBoolean showStartupCB = kJFalse,
						const JBoolean showAtStartup = kJFalse);

	virtual ~JXTipOfTheDayDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JPtrArray<JString>*	itsTipList;
	JIndex				itsTipIndex;

	JXStaticText*	itsText;

// begin JXLayout

    JXTextButton*   itsCloseButton;
    JXTextButton*   itsNextTipButton;
    JXTextCheckbox* itsShowAtStartupCB;

// end JXLayout

private:

	void	BuildWindow(const JBoolean showStartupCB,
						const JBoolean showAtStartup);

	void	ParseTips();
	void	AddTip(JString tip);
	void	DisplayTip();

	// not allowed

	JXTipOfTheDayDialog(const JXTipOfTheDayDialog& source);
	const JXTipOfTheDayDialog& operator=(const JXTipOfTheDayDialog& source);

public:

	// JBroadcaster messages

	static const JCharacter* kShowAtStartup;

	class ShowAtStartup : public JBroadcaster::Message
		{
		public:

			ShowAtStartup(const JBoolean showAtStartup)
				:
				JBroadcaster::Message(kShowAtStartup),
				itsShowAtStartupFlag(showAtStartup)
				{ };

			JBoolean
			ShouldShowAtStartup()
				const
			{
				return itsShowAtStartupFlag;
			};

		private:

			JBoolean	itsShowAtStartupFlag;
		};
};

#endif
