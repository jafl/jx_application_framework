/******************************************************************************
 CBFindFileDialog.h

	Interface for the CBFindFileDialog class

	Copyright © 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBFindFileDialog
#define _H_CBFindFileDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	virtual void	Activate();

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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
