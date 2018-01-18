/******************************************************************************
 CBFindFileDialog.h

	Interface for the CBFindFileDialog class

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBFindFileDialog
#define _H_CBFindFileDialog

#include <JXWindowDirector.h>
#include <JPrefObject.h>

class JXTextButton;
class JXTextCheckbox;
class JXInputField;
class JXStringHistoryMenu;

class CBFindFileDialog : public JXWindowDirector, public JPrefObject
{
public:

	CBFindFileDialog(JXDirector* supervisor);

	virtual ~CBFindFileDialog();

	virtual void	Activate() override;

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

// begin JXLayout

	JXTextButton*        itsCloseButton;
	JXTextButton*        itsFindButton;
	JXInputField*        itsFileName;
	JXStringHistoryMenu* itsFileHistoryMenu;
	JXTextCheckbox*      itsIgnoreCaseCB;
	JXTextCheckbox*      itsStayOpenCB;

// end JXLayout

private:

	void		BuildWindow();
	void		UpdateDisplay();
	JBoolean	FindFile();

	// not allowed

	CBFindFileDialog(const CBFindFileDialog& source);
	const CBFindFileDialog& operator=(const CBFindFileDialog& source);
};

#endif
