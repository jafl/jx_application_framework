/******************************************************************************
 TestChooseSaveFile.cpp

	Class to test overriding of JXChooseSaveFile dialog windows.

	BASE CLASS = JXChooseSaveFile

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestChooseSaveFile.h"
#include "TestSaveFileDialog.h"
#include <JDirInfo.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestChooseSaveFile::TestChooseSaveFile()
	:
	JXChooseSaveFile()
{
	itsTestSaveDialog = NULL;
	itsSaveFormat     = kGIFFormat;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestChooseSaveFile::~TestChooseSaveFile()
{
}

/******************************************************************************
 CreateSaveFileDialog (virtual protected)

 ******************************************************************************/

JXSaveFileDialog*
TestChooseSaveFile::CreateSaveFileDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JCharacter*	origName,
	const JCharacter*	prompt,
	const JCharacter*	message
	)
{
	itsTestSaveDialog =
		TestSaveFileDialog::Create(supervisor, dirInfo, fileFilter, itsSaveFormat,
								   origName, prompt, message);
	return itsTestSaveDialog;
}

/******************************************************************************
 GetSaveFormat

 ******************************************************************************/

TestChooseSaveFile::SaveFormat
TestChooseSaveFile::GetSaveFormat()
	const
{
	return itsSaveFormat;
}

/******************************************************************************
 Receive (virtual protected)

	Listen for response from itsTestSaveDialog.

 ******************************************************************************/

void
TestChooseSaveFile::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTestSaveDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsSaveFormat = itsTestSaveDialog->GetSaveFormat();
			}
		itsTestSaveDialog = NULL;
		}

	JXChooseSaveFile::Receive(sender, message);
}

/******************************************************************************
 SetChooseFileFilters (virtual protected)

 ******************************************************************************/

void
TestChooseSaveFile::SetChooseFileFilters
	(
	JDirInfo* dirInfo
	)
{
	dirInfo->SetContentFilter("(^[[:space:]]*$)|(^[[:space:]]*From )");
}
