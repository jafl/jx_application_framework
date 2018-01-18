/******************************************************************************
 GXGetDelimiterDialog.h

	Interface for the GXGetDelimiterDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GXGetDelimiterDialog
#define _H_GXGetDelimiterDialog

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

class GXGetDelimiterDialog : public JXDialogDirector, public JPrefObject
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

	GXGetDelimiterDialog(JXWindowDirector* supervisor, const JString& text);

	virtual ~GXGetDelimiterDialog();
	
	DelimiterType	GetDelimiterType();
	JCharacter		GetCharacter();
	JBoolean		IsSkippingLines();
	JSize			GetSkipLineCount();
	JBoolean		HasComments();
	const JString&	GetCommentString();
	
protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;
	
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

	GXGetDelimiterDialog(const GXGetDelimiterDialog& source);
	const GXGetDelimiterDialog& operator=(const GXGetDelimiterDialog& source);
};

#endif
