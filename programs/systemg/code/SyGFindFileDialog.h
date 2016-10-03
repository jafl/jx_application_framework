/******************************************************************************
 SyGFindFileDialog.h

	Copyright (C) 1996-2009 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGFindFileDialog
#define _H_SyGFindFileDialog

#include <JXWindowDirector.h>
#include <JPrefObject.h>
#include <JString.h>

class JXTextButton;
class JXTextCheckbox;
class JXRadioGroup;
class JXInputField;
class JXPathInput;

class SyGFindFileDialog : public JXWindowDirector, public JPrefObject
{
public:

	SyGFindFileDialog(JXDirector* supervisor);

	virtual ~SyGFindFileDialog();

	virtual void	Activate();

	void	Search(const JCharacter* path);
	void	SearchFileName(const JCharacter* path, const JCharacter* pattern);
	void	SearchExpr(const JCharacter* path, const JCharacter* expr);

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

	JXRadioGroup*   itsActionRG;
	JXTextButton*   itsCloseButton;
	JXTextButton*   itsSearchButton;
	JXInputField*   itsFileInput;
	JXTextButton*   itsHelpButton;
	JXTextCheckbox* itsStayOpenCB;
	JXInputField*   itsExprInput;
	JXPathInput*    itsPathInput;
	JXTextButton*   itsChoosePathButton;

// end JXLayout

private:

	void		BuildWindow();
	void		UpdateDisplay();
	void		UpdateButtons();
	JBoolean	Search();

	// not allowed

	SyGFindFileDialog(const SyGFindFileDialog& source);
	const SyGFindFileDialog& operator=(const SyGFindFileDialog& source);
};

#endif
