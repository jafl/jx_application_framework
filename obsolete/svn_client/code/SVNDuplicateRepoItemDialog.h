/******************************************************************************
 SVNDuplicateRepoItemDialog.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNDuplicateRepoItemDialog
#define _H_SVNDuplicateRepoItemDialog

#include <JXGetStringDialog.h>

class SVNRepoTreeNode;

class SVNDuplicateRepoItemDialog : public JXGetStringDialog
{
public:

	SVNDuplicateRepoItemDialog(JXDirector* supervisor, const JCharacter* windowTitle,
							   const JCharacter* prompt, const JCharacter* initialName,
							   SVNRepoTreeNode* srcNode);

	virtual ~SVNDuplicateRepoItemDialog();

	SVNRepoTreeNode*	GetSrcNode();

protected:

	virtual JBoolean	OKToDeactivate();

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
