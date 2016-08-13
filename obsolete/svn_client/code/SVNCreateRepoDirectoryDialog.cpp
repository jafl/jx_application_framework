/******************************************************************************
 SVNCreateRepoDirectoryDialog.cpp

	BASE CLASS = JXGetStringDialog

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "SVNCreateRepoDirectoryDialog.h"
#include "SVNRepoTreeNode.h"
#include <JXInputField.h>
#include <JXChooseSaveFile.h>
#include <jXGlobals.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNCreateRepoDirectoryDialog::SVNCreateRepoDirectoryDialog
	(
	JXDirector*			supervisor,
	const JCharacter*	windowTitle,
	const JCharacter*	prompt,
	const JCharacter*	initialName,
	SVNRepoTreeNode*	parentNode
	)
	:
	JXGetStringDialog(supervisor, windowTitle, prompt, initialName),
	itsParentNode(parentNode)
{
	assert( itsParentNode != NULL );

	GetInputField()->SetCharacterInWordFunction(JXChooseSaveFile::IsCharacterInWord);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNCreateRepoDirectoryDialog::~SVNCreateRepoDirectoryDialog()
{
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
SVNCreateRepoDirectoryDialog::OKToDeactivate()
{
	if (!JXGetStringDialog::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	const JString& name = GetString();
	JNamedTreeNode* node;
	if (itsParentNode->FindNamedChild(name, &node))
		{
		(JGetUserNotification())->ReportError(JGetString("NameUsed::SVNCreateRepoDirectoryDialog"));
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}
