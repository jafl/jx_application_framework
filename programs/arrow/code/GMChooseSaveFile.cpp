/******************************************************************************
 GMChooseSaveFile.cc

	Class to test overriding of JXChooseSaveFile dialog windows.

	BASE CLASS = JXChooseSaveFile

	Written by John Lindal.

 ******************************************************************************/

#include "GMChooseSaveFile.h"
#include "GMSaveFileDialog.h"
#include "GMChooseFileDialog.h"

#include <jStreamUtil.h>
#include <jAssert.h>

const JFileVersion kCurrentPrefsVersion	= 1;

/******************************************************************************
 Constructor

 ******************************************************************************/

GMChooseSaveFile::GMChooseSaveFile()
	:
	JXChooseSaveFile(),
	itsSaveHeaders(kJFalse)
{
	itsSaveDialog	= NULL;
	itsChooseDialog	= NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMChooseSaveFile::~GMChooseSaveFile()
{
}

/******************************************************************************
 CreateSaveFileDialog (virtual protected)

 ******************************************************************************/

JXSaveFileDialog*
GMChooseSaveFile::CreateSaveFileDialog
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
		new GMSaveFileDialog(supervisor, dirInfo, fileFilter, origName, prompt, message);
	assert( itsSaveDialog != NULL );
	itsSaveDialog->ShouldSaveHeaders(itsSaveHeaders);
	return itsSaveDialog;
}

/******************************************************************************
 CreateChooseFileDialog (virtual protected)

 ******************************************************************************/

JXChooseFileDialog*
GMChooseSaveFile::CreateChooseFileDialog
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
		new GMChooseFileDialog(supervisor, dirInfo, fileFilter, allowSelectMultiple);
	assert( itsChooseDialog != NULL );
	itsChooseDialog->BuildWindow(origName, message);
	itsChooseDialog->ShouldSaveHeaders(itsSaveHeaders);
	return itsChooseDialog;
}

/******************************************************************************
 SaveHeaders

 ******************************************************************************/

JBoolean
GMChooseSaveFile::IsSavingHeaders()
	const
{
	return itsSaveHeaders;
}

void
GMChooseSaveFile::ShouldSaveHeaders
	(
	const JBoolean save
	)
{
	itsSaveHeaders = save;
}


/******************************************************************************
 Receive (protected)

	Listen for response from itsSaveDialog.

 ******************************************************************************/

void
GMChooseSaveFile::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSaveDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsSaveHeaders = itsSaveDialog->IsSavingHeaders();
			}
		itsSaveDialog = NULL;
		}

	else if (sender == itsChooseDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast(const JXDialogDirector::Deactivated*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsSaveHeaders = itsChooseDialog->IsSavingHeaders();
			}
		itsChooseDialog = NULL;
		}

	JXChooseSaveFile::Receive(sender, message);
}

/******************************************************************************
 Read/WritePrefs

 ******************************************************************************/

void
GMChooseSaveFile::ReadGMSetup
	(
	istream& input
	)
{
	JFileVersion version;
	input >> version;
	if (version == 1)
		{
		input >> itsSaveHeaders;
		ReadSetup(input);
		}
}

void
GMChooseSaveFile::WriteGMSetup
	(
	ostream& output
	)
	const
{
	output << kCurrentPrefsVersion << ' ';
	output << itsSaveHeaders << ' ';
	WriteSetup(output);
}
