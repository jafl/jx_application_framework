/******************************************************************************
 SVNDuplicateRepoItemDialog.h

	Copyright (C) 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNDuplicateRepoItemDialog
#define _H_SVNDuplicateRepoItemDialog

#include <JXGetStringDialog.h>

class SVNRepoTreeNode;

class SVNDuplicateRepoItemDialog : public JXGetStringDialog
{
public:

	SVNDuplicateRepoItemDialog(JXDirector* supervisor, const JString& windowTitle,
							   const JString& prompt, const JString& initialName,
							   SVNRepoTreeNode* srcNode);

	virtual ~SVNDuplicateRepoItemDialog();

	SVNRepoTreeNode*	GetSrcNode();

protected:

	virtual bool	OKToDeactivate();

private:

	SVNRepoTreeNode*	itsSrcNode;

private:

	// not allowed

	SVNDuplicateRepoItemDialog(const SVNDuplicateRepoItemDialog& source);
	const SVNDuplicateRepoItemDialog& operator=(const SVNDuplicateRepoItemDialog& source);
};


/******************************************************************************
 GetSrcNode

 ******************************************************************************/

inline SVNRepoTreeNode*
SVNDuplicateRepoItemDialog::GetSrcNode()
{
	return itsSrcNode;
}

#endif
