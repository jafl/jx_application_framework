/******************************************************************************
 CBFindFileDialog.h

	Interface for the CBFindFileDialog class

	Copyright © 1996-98 by John Lindal.

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

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

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
	bool	FindFile();

	// not allowed

	CBFindFileDialog(const CBFindFileDialog& source);
	const CBFindFileDialog& operator=(const CBFindFileDialog& source);
};

#endif
