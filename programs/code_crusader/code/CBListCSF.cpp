/******************************************************************************
 CBListCSF.cpp

	BASE CLASS = JXChooseSaveFile

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "CBListCSF.h"
#include "CBListChooseFileDialog.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBListCSF::CBListCSF
	(
	const JCharacter* replaceListStr,
	const JCharacter* appendToListStr
	)
	:
	JXChooseSaveFile()
{
	itsChooseDialog        = nullptr;
	itsReplaceExistingFlag = kJFalse;
	itsReplaceListStr      = replaceListStr;
	itsAppendToListStr     = appendToListStr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBListCSF::~CBListCSF()
{
}

/******************************************************************************
 CreateChooseFileDialog (virtual protected)

 ******************************************************************************/

JXChooseFileDialog*
CBListCSF::CreateChooseFileDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		allowSelectMultiple,
	const JCharacter*	origName,
	const JCharacter*	message
	)
{
	itsChooseDialog =
		CBListChooseFileDialog::Create(supervisor, dirInfo, fileFilter,
									   allowSelectMultiple,
									   itsReplaceListStr, itsAppendToListStr,
									   origName, message);
	return itsChooseDialog;
}

/******************************************************************************
 Receive (protected)

	Listen for response from itsChooseDialog.

 ******************************************************************************/

void
CBListCSF::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChooseDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			itsReplaceExistingFlag = itsChooseDialog->ReplaceExisting();
			}
		itsChooseDialog = nullptr;
		}

	JXChooseSaveFile::Receive(sender, message);
}
