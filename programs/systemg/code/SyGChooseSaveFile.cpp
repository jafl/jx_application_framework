/******************************************************************************
 SyGChooseSaveFile.cpp

	Class to test overriding of JXChooseSaveFile dialog windows.

	BASE CLASS = JXChooseSaveFile

	Copyright (C) 1999 by Glenn W. Bach.

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
	itsPathDialog(nullptr),
	itsOpenInNewWindow(true)
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
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const JString&	origName,
	const JString&	prompt,
	const JString&	message
	)
{
	itsSaveDialog =
		jnew GMSaveFileDialog(supervisor, dirInfo, fileFilter, origName, prompt, message);
	assert( itsSaveDialog != nullptr );
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
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const JString&	origName,
	const bool	allowSelectMultiple,
	const JString&	message
	)
{
	itsChooseDialog =
		jnew GMChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple);
	assert( itsChooseDialog != nullptr );
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
	JXDirector*		supervisor,
	JDirInfo*		dirInfo,
	const JString&	fileFilter,
	const bool	selectOnlyWritable,
	const JString&	message
	)
{
	itsPathDialog =
		jnew SyGChoosePathDialog(supervisor, dirInfo, fileFilter, selectOnlyWritable);
	assert( itsPathDialog != nullptr );
	itsPathDialog->BuildWindow(itsOpenInNewWindow, message);
	return itsPathDialog;
}

/******************************************************************************
 New window

 ******************************************************************************/

bool
SyGChooseSaveFile::IsOpeningInNewWindow()
	const
{
	return itsOpenInNewWindow;
}

void
SyGChooseSaveFile::ShouldOpenInNewWindow
	(
	const bool newWindow
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
		const auto* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			itsOpenInNewWindow = itsPathDialog->OpenInNewWindow();
			}
		itsPathDialog = nullptr;
		}

	JXChooseSaveFile::Receive(sender, message);
}

/******************************************************************************
 Read/WriteSetup

 ******************************************************************************/

void
SyGChooseSaveFile::SGReadSetup
	(
	std::istream& input
	)
{
	JFileVersion version;
	input >> version;
	if (version > kCurrentPrefsVersion)
		{
		return;
		}

	input >> JBoolFromString(itsOpenInNewWindow);
	JXChooseSaveFile::ReadSetup(input);
}

void
SyGChooseSaveFile::SGWriteSetup
	(
	std::ostream& output
	)
	const
{
	output << kCurrentPrefsVersion << ' ';
	output << JBoolToString(itsOpenInNewWindow) << ' ';
	JXChooseSaveFile::WriteSetup(output);
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
SyGChooseSaveFile::ReadPrefs
	(
	std::istream& input
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
	std::ostream& output
	)
	const
{
	SGWriteSetup(output);
}
