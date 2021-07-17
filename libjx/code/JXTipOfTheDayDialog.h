/******************************************************************************
 JXTipOfTheDayDialog.h

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTipOfTheDayDialog
#define _H_JXTipOfTheDayDialog

#include "JXDialogDirector.h"

class JXStaticText;
class JXTextButton;
class JXTextCheckbox;

class JXTipOfTheDayDialog : public JXDialogDirector
{
public:

	JXTipOfTheDayDialog(const bool showStartupCB = false,
						const bool showAtStartup = false);

	virtual ~JXTipOfTheDayDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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

	void	BuildWindow(const bool showStartupCB,
						const bool showAtStartup);

	void	ParseTips();
	void	AddTip(JString tip);
	void	DisplayTip();

	// not allowed

	JXTipOfTheDayDialog(const JXTipOfTheDayDialog& source);
	const JXTipOfTheDayDialog& operator=(const JXTipOfTheDayDialog& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kShowAtStartup;

	class ShowAtStartup : public JBroadcaster::Message
		{
		public:

			ShowAtStartup(const bool showAtStartup)
				:
				JBroadcaster::Message(kShowAtStartup),
				itsShowAtStartupFlag(showAtStartup)
				{ };

			bool
			ShouldShowAtStartup()
				const
			{
				return itsShowAtStartupFlag;
			};

		private:

			bool	itsShowAtStartupFlag;
		};
};

#endif
