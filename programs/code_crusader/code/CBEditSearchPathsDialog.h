/******************************************************************************
 CBEditSearchPathsDialog.h

	Interface for the CBEditSearchPathsDialog class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBEditSearchPathsDialog
#define _H_CBEditSearchPathsDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>
#include <JPrefObject.h>

class JString;
class JXTextButton;
class CBPathTable;
class CBDirList;
class CBRelPathCSF;

class CBEditSearchPathsDialog : public JXDialogDirector, public JPrefObject
{
public:

	CBEditSearchPathsDialog(JXDirector* supervisor,
							const CBDirList& dirList,
							CBRelPathCSF* csf);

	virtual ~CBEditSearchPathsDialog();

	void	AddDirectories(const JPtrArray<JString>& list);
	void	GetPathList(CBDirList* pathList) const;

protected:

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

private:

	CBPathTable*	itsTable;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const CBDirList& dirList, CBRelPathCSF* csf);

	// not allowed

	CBEditSearchPathsDialog(const CBEditSearchPathsDialog& source);
	const CBEditSearchPathsDialog& operator=(const CBEditSearchPathsDialog& source);
};

#endif
