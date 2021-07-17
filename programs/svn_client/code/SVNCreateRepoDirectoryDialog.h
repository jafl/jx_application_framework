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

	SVNCreateRepoDirectoryDialog(JXDirector* supervisor, const JString& windowTitle,
							const JString& prompt, const JString& initialName,
							SVNRepoTreeNode* parentNode);

	virtual ~SVNCreateRepoDirectoryDialog();

	SVNRepoTreeNode*	GetParentNode();

protected:

	virtual bool	OKToDeactivate();

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
