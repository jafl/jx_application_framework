/******************************************************************************
 SVNCreateRepoDirectoryDialog.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNCreateRepoDirectoryDialog
#define _H_SVNCreateRepoDirectoryDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
