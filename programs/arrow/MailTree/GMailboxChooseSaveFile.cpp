/******************************************************************************
 GMailboxChooseSaveFile.cc

	Class to test overriding of JXChooseSaveFile dialog windows.

	BASE CLASS = JXChooseSaveFile

	Written by John Lindal.

 ******************************************************************************/

#include "GMailboxChooseSaveFile.h"
#include "GMailboxFileDialog.h"
#include "GMailboxPathDialog.h"
#include "GMGlobals.h"
#include <JDirInfo.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GMailboxChooseSaveFile::GMailboxChooseSaveFile()
	:
	JXChooseSaveFile()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMailboxChooseSaveFile::~GMailboxChooseSaveFile()
{
}

/******************************************************************************
 SetChooseFileContentFilter (virtual protected)


 ******************************************************************************/

void
GMailboxChooseSaveFile::SetChooseFileFilters
	(
	JDirInfo* dirInfo
	)
{
	JString regex("^$|");
	regex += GMGetMailRegexStr();
	JError err = dirInfo->SetContentFilter(regex);
	if (!err.OK())
		{
		err.ReportIfError();
		}
}
