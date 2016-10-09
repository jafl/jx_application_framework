/******************************************************************************
 SyGChooseSaveFile.cpp

	Class to test overriding of JXChooseSaveFile dialog windows.

	BASE CLASS = JXChooseSaveFile

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "SyGChooseSaveFile.h"
#include "SyGChoosePathDialog.h"
#include <jAssert.h>

const JFileVersion kCurrentPrefsVersion	= 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGChooseSaveFile::SyGChooseSaveFile
	(
	JPrefsManager*	prefsMgr,
	const JPrefID&	id
	)
	:
	JXChooseSaveFile(prefsMgr, id),
	itsPathDialog(NULL),
	itsOpenInNewWindow(kJTrue)
{
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGChooseSaveFile::~SyGChooseSaveFile()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 CreateSaveFileDialog (virtual protected)

 ******************************************************************************/
/*
JXSaveFileDialog*
SyGChooseSaveFile::CreateSaveFileDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JCharacter*	origName,
	const JCharacter*	prompt,
	const JCharacter*	message
	)
{
	itsSaveDialog =
		jnew GMSaveFileDialog(supervisor, dirInfo, fileFilter, origName, prompt, message);
	assert( itsSaveDialog != NULL );
	itsSaveDialog->ShouldSaveHeaders(itsSaveHeaders);
	return itsSaveDialog;
}
*/
/******************************************************************************
 CreateChooseFileDialog (virtual protected)

 ******************************************************************************/
/*
JXChooseFileDialog*
SyGChooseSaveFile::CreateChooseFileDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JCharacter*	origName,
	const JBoolean		allowSelectMultiple,
	const JCharacter*	message
	)
{
	itsChooseDialog =
		jnew GMChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple);
	assert( itsChooseDialog != NULL );
	itsChooseDialog->BuildWindow(origName, message);
	itsChooseDialog->ShouldSaveHeaders(itsSaveHeaders);
	return itsChooseDialog;
}
*/
/******************************************************************************
 CreateChoosePathDialog (virtual protected)

 ******************************************************************************/

JXChoosePathDialog*
SyGChooseSaveFile::CreateChoosePathDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		selectOnlyWritable,
	const JCharacter*	message
	)
{
	itsPathDialog =
		jnew SyGChoosePathDialog(supervisor, dirInfo, fileFilter, selectOnlyWritable);
	assert( itsPathDialog != NULL );
	itsPathDialog->BuildWindow(itsOpenInNewWindow, message);
	return itsPathDialog;
}

/******************************************************************************
 New window

 ******************************************************************************/

JBoolean
SyGChooseSaveFile::IsOpeningInNewWindow()
	const
{
	return itsOpenInNewWindow;
}

void
SyGChooseSaveFile::ShouldOpenInNewWindow
	(
	const JBoolean newWindow
	)
{
	itsOpenInNewWindow = newWindow;
}

/******************************************************************************
 Receive (virtual protected)

	Listen for response from itsSaveDialog.

 ******************************************************************************/

void
SyGChooseSaveFile::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsPathDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsOpenInNewWindow = itsPathDialog->OpenInNewWindow();
			}
		itsPathDialog = NULL;
		}

	JXChooseSaveFile::Receive(sender, message);
}

/******************************************************************************
 Read/WriteSetup

 ******************************************************************************/

void
SyGChooseSaveFile::SGReadSetup
	(
	istream& input
	)
{
	JFileVersion version;
	input >> version;
	if (version > kCurrentPrefsVersion)
		{
		return;
		}

	input >> itsOpenInNewWindow;
	JXChooseSaveFile::ReadSetup(input);
}

void
SyGChooseSaveFile::SGWriteSetup
	(
	ostream& output
	)
	const
{
	output << kCurrentPrefsVersion << ' ';
	output << itsOpenInNewWindow << ' ';
	JXChooseSaveFile::WriteSetup(output);
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
SyGChooseSaveFile::ReadPrefs
	(
	istream& input
	)
{
	SGReadSetup(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
SyGChooseSaveFile::WritePrefs
	(
	ostream& output
	)
	const
{
	SGWriteSetup(output);
}
