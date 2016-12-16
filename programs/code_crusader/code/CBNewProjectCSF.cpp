/******************************************************************************
 CBNewProjectCSF.cpp

	BASE CLASS = JXChooseSaveFile, JPrefObject

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBNewProjectCSF.h"
#include "CBNewProjectSaveFileDialog.h"
#include "cbGlobals.h"
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 1;

	// version  1 stores itsProjectTemplate

/******************************************************************************
 Constructor

 ******************************************************************************/

CBNewProjectCSF::CBNewProjectCSF()
	:
	JXChooseSaveFile()
{
	itsSaveDialog     = NULL;
	itsMakefileMethod = CBBuildManager::kMakemake;

	SetPrefInfo(CBGetPrefsManager(), kCBNewProjectCSFID);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBNewProjectCSF::~CBNewProjectCSF()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 CreateSaveFileDialog (virtual protected)

 ******************************************************************************/

JXSaveFileDialog*
CBNewProjectCSF::CreateSaveFileDialog
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
		CBNewProjectSaveFileDialog::Create(supervisor, dirInfo, fileFilter,
										   itsProjectTemplate, itsMakefileMethod,
										   origName, prompt, message);
	return itsSaveDialog;
}

/******************************************************************************
 Receive (protected)

	Listen for response from itsChooseDialog.

 ******************************************************************************/

void
CBNewProjectCSF::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsSaveDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != NULL );
		if (info->Successful())
			{
			itsSaveDialog->GetProjectTemplate(&itsProjectTemplate);
			itsMakefileMethod = itsSaveDialog->GetMakefileMethod();
			}
		itsSaveDialog = NULL;
		}

	JXChooseSaveFile::Receive(sender, message);
}

/******************************************************************************
 ReadPrefs (virtual)

	Intentional override of JXChooseSaveFile::ReadPrefs().

 ******************************************************************************/

void
CBNewProjectCSF::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentSetupVersion)
		{
		if (vers >= 1)
			{
			input >> itsProjectTemplate;
			}

		input >> itsMakefileMethod;
		}
}

/******************************************************************************
 WritePrefs (virtual)

	Intentional override of JXChooseSaveFile::WritePrefs().

 ******************************************************************************/

void
CBNewProjectCSF::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << itsProjectTemplate;
	output << ' ' << itsMakefileMethod;
	output << ' ';
}
