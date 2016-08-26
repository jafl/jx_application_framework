/******************************************************************************
 CMEditCommandsDialog.h

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMEditCommandsDialog
#define _H_CMEditCommandsDialog

#include <JXDialogDirector.h>
#include <JPrefObject.h>
#include <JPtrArray-JString.h>

class JXTextButton;
class CMEditCommandsTable;

class CMEditCommandsDialog : public JXDialogDirector, public JPrefObject
{
public:

	CMEditCommandsDialog();
	virtual ~CMEditCommandsDialog();

	JSize						GetStringCount() const;
	const JPtrArray<JString>&	GetStringList() const;

	const JString&	GetString(const JIndex index) const;
	void			SetString(const JIndex index, const JString& str);
	void			AppendString(const JString& str);
	void			DeleteString(const JIndex index);

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message);
	virtual JBoolean	OKToDeactivate();

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

private:

	JPtrArray<JString>*		itsCommands;
	CMEditCommandsTable*	itsWidget;

// begin JXLayout

	JXTextButton* itsNewButton;
	JXTextButton* itsRemoveButton;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	CMEditCommandsDialog(const CMEditCommandsDialog& source);
	const CMEditCommandsDialog& operator=(const CMEditCommandsDialog& source);
};


/******************************************************************************
 GetStringCount

 ******************************************************************************/

inline JSize
CMEditCommandsDialog::GetStringCount()
	const
{
	return itsCommands->GetElementCount();
}

/******************************************************************************
 GetString

 ******************************************************************************/

inline const JString&
CMEditCommandsDialog::GetString
	(
	const JIndex index
	)
	const
{
	return *(itsCommands->NthElement(index));
}

/******************************************************************************
 GetStringList

 ******************************************************************************/

inline const JPtrArray<JString>&
CMEditCommandsDialog::GetStringList()
	const
{
	return *itsCommands;
}

#endif
