/******************************************************************************
 GMailPrefsDialog.h

	Interface for the GMailPrefsDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GMailPrefsDialog
#define _H_GMailPrefsDialog

#include <JXDialogDirector.h>

class JString;

class JXChooseMonoFont;
class JXFileInput;
class JXInputField;
class JXIntegerInput;
class JXRadioGroup;
class JXStaticText;
class JXTextButton;
class JXTextCheckbox;

class GMailPrefsDialog : public JXDialogDirector
{
public:

	GMailPrefsDialog(JXDirector* supervisor,
						const JBoolean showstate,
						const JBoolean autoQuote,
						const JBoolean showSender,
						const JString& senderString,
						const JString& fontname,
						const JSize	   fontsize,
						const JSize	   maxQuotedReply,
						const JBoolean reportSCErrors,
						const JSize	   spacesPerTab,
						const JBoolean beepOnNewMail,
						const JIndex   encType,
						const JString& filterFile,
						const JString& defInbox,
						const JBoolean openMailboxWindow);

	virtual ~GMailPrefsDialog();

	void	GetValues(JBoolean*	showstate,
						JBoolean*	autoQuote,
						JBoolean*	showSender,
						JString*	senderString,
						JString*	fontname,
						JSize*		fontsize,
						JSize*		maxQuotedReply,
						JBoolean*	reportSCErrors,
						JSize*		spacesPerTab,
						JBoolean*	beepOnNewMail,
						JIndex*		encType,
						JString*	filterFile,
						JString*	defInbox,
						JBoolean*	openMailboxWindow);

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXIntegerInput*   itsMaxReplyInput;
	JXInputField*     itsSenderStringInput;
	JXIntegerInput*   itsSpacesPerTabInput;
	JXTextCheckbox*   itsShowStateInTitle;
	JXTextCheckbox*   itsAutoQuote;
	JXTextCheckbox*   itsShowSenderString;
	JXStaticText*     itsSenderText;
	JXTextButton*     itsHelpButton;
	JXChooseMonoFont* itsFontMenu;
	JXTextCheckbox*   itsReportSCErrorsCB;
	JXTextCheckbox*   itsBeepOnNewMailCB;
	JXRadioGroup*     itsEncryptionType;
	JXTextButton*     itsFilterButton;
	JXFileInput*      itsDefaultInbox;
	JXTextButton*     itsInboxButton;
	JXFileInput*      itsFilterFileInput;
	JXTextCheckbox*   itsCopySelectedCB;
	JXTextCheckbox*   itsHomeEndCB;
	JXTextCheckbox*   itsOpenMailTreeCB;

// end JXLayout

private:

	void	BuildWindow(const JBoolean showstate,
						const JBoolean autoQuote,
						const JBoolean showSender,
						const JString& senderString,
						const JString& fontname,
						const JSize	   fontsize,
						const JSize	   maxQuotedReply,
						const JBoolean reportSCErrors,
						const JSize	   spacesPerTab,
						const JBoolean beepOnNewMail,
						const JIndex   encType,
						const JString& filterFile,
						const JString& defInbox,
						const JBoolean openMailboxWindow);

	void	AdjustForSenderString(const JBoolean showSender);
	void	AdjustForQuotedReply(const JBoolean quote);

	// not allowed

	GMailPrefsDialog(const GMailPrefsDialog& source);
	const GMailPrefsDialog& operator=(const GMailPrefsDialog& source);
};

#endif
