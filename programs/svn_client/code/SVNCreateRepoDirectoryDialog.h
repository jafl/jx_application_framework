/******************************************************************************
 SVNCreateRepoDirectoryDialog.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNCreateRepoDirectoryDialog
#define _H_SVNCreateRepoDirectoryDialog

#include <JXGetStringDialog.h>

class SVNRepoTreeNode;

class SVNCreateRepoDirectoryDialog : public JXGetStringDialog
{
public:

	SVNCreateRepoDirectoryDialog(JXDirector* supervisor, const JCharacter* windowTitle,
							const JCharacter* prompt, const JCharacter* initialName,
							SVNRepoTreeNode* parentNode);

	virtual ~SVNCreateRepoDirectoryDialog();

	SVNRepoTreeNode*	GetParentNode();

protected:

	virtual JBoolean	OKToDeactivate();

private:

	SVNRepoTreeNode*	itsParentNode;

private:

	// not allowed

	SVNCreateRepoDirectoryDialog(const SVNCreateRepoDirectoryDialog& source);
	const SVNCreateRepoDirectoryDialog& operator=(const SVNCreateRepoDirectoryDialog& source);
};


/******************************************************************************
 GetParentNode

 ******************************************************************************/

inline SVNRepoTreeNode*
SVNCreateRepoDirectoryDialog::GetParentNode()
{
	return itsParentNode;
}

#endif
