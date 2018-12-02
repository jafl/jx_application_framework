/******************************************************************************
 GLGetDelimiterDialog.h

	Interface for the GLGetDelimiterDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLGetDelimiterDialog
#define _H_GLGetDelimiterDialog

#include <JXDialogDirector.h>
#include <JPrefObject.h>

class JXTextMenu;
class JXRadioGroup;
class JXTextButton;
class JXStaticText;
class JString;
class JXInputField;
class JXIntegerInput;
class JXTextCheckbox;

class GLGetDelimiterDialog : public JXDialogDirector, public JPrefObject
{
public:

	enum DelimiterType
	{
		kWhiteSpace = 1,
		kSpace,
		kTab,
		kChar
	};

public:

	GLGetDelimiterDialog(JXWindowDirector* supervisor, const JString& text);

	virtual ~GLGetDelimiterDialog();

	DelimiterType	GetDelimiterType();
	JUtf8Byte		GetCharacter();
	JBoolean		IsSkippingLines();
	JSize			GetSkipLineCount();
	JBoolean		HasComments();
	const JString&	GetCommentString();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

private:

	JIndex 					itsFilterIndex;
	JXStaticText*			itsFileText;

// begin JXLayout

	JXRadioGroup*   itsRG;
	JXInputField*   itsCharInput;
	JXTextCheckbox* itsSkipCB;
	JXTextCheckbox* itsCommentCB;
	JXIntegerInput* itsSkipCountInput;
	JXInputField*   itsCommentInput;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	GLGetDelimiterDialog(const GLGetDelimiterDialog& source);
	const GLGetDelimiterDialog& operator=(const GLGetDelimiterDialog& source);
};

#endif
