/******************************************************************************
 GMGlobals.cc

	Access to testjx global objects and factories.

	Written by Glenn W. Bach.

 ******************************************************************************/

#include "GMGlobals.h"
#include "GMApp.h"
#include "GNetDebugDir.h"
#include "GAddressBookMgr.h"
#include "GPrefsMgr.h"
#include "GInboxMgr.h"
#include "GHelpText.h"
#include "GMChooseSaveFile.h"
#include "GMChooseSavePrefObject.h"
#include "GMPTPrinter.h"
#include "GMDirectorManager.h"
#include "GMailboxChooseSaveFile.h"
#include "GMailboxTreeDir.h"
#include "GAddressBookTreeDir.h"
#include "GMAccountManager.h"
#include "GMFilterManager.h"

#include <JXFSBindingManager.h>
#include <JXWindow.h>
#include <JXPTPrinter.h>

#include <JUserNotification.h>
#include <JDirEntry.h>
#include <JRegex.h>

#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jSysUtil.h>
#include <jFileUtil.h>
#include <jTime.h>

#include <unistd.h>
#include <stdlib.h>
#include <jAssert.h>

static GMApp*				theApplication			= NULL;		// owns itself
static JXDirector*			thePermWindowOwner	= NULL;
static GNetDebugDir*		itsSMTPDebugDir			= NULL;
static GNetDebugDir*		itsPOPDebugDir			= NULL;
static JBoolean			itsPGPEnabled;
static JBoolean			itsGPGEnabled;
static GAddressBookMgr*	itsAddressBookMgr		= NULL;
static GPrefsMgr*			itsPrefsMgr				= NULL;
static GInboxMgr*			itsInboxMgr				= NULL;
static JBoolean			itsProgramIsDying		= kJFalse;
static JXPTPrinter*			itsPrinter				= NULL;
static GMChooseSaveFile*	itsAltChooseSaveFile	= NULL;
static GMPTPrinter*			itsAltPTPrinter			= NULL;
static GMDirectorManager*	itsDirMgr				= NULL;
static GMailboxTreeDir*		itsMailboxTreeDir		= NULL;
static GAddressBookTreeDir*	itsAddressBookTreeDir	= NULL;
static GMAccountManager*	itsAccountMgr			= NULL;
static GMFilterManager*		itsFilterMgr			= NULL;

static GMailboxChooseSaveFile*	itsMailboxCSF	= NULL;
static GMChooseSavePrefObject*	itsCSFPref		= NULL;

static JBoolean				itsLockFilePrgExists = kJFalse;

const JCharacter* smtpDebugTitle					= "SMTP Debug Window";
const JCharacter* popDebugTitle						= "POP Debug Window";

static const JCharacter* kGMWMInstance			= "Arrow";
static const JCharacter* kTableWindowClass		= "Arrow_Table";
static const JCharacter* kFindDirWindowClass	= "Arrow_Find_Dir";
static const JCharacter* kViewWindowClass		= "Arrow_View";
static const JCharacter* kEditorWindowClass		= "Arrow_Editor";
static const JCharacter* kAddressBookWindowClass	= "Arrow_Address_Book";
static const JCharacter* kMailboxWindowClass		= "Arrow_Mailbox";

static const JCharacter* kMailRegexStr	= "^From[[:space:]]+.*.{3}[[:space:]]+.{3}[[:space:]]+[[:digit:]]+[[:space:]]+([[:digit:]]+:)+[[:digit:]]+[[:space:]]+(...[[:space:]])?[[:digit:]]{4}";
static const JRegex itsMailRegex(kMailRegexStr);
//static const JCharacter* kMailRegex				= "^From[[:space:]]+.*.{3}[[:space:]]+.{3}[[:space:]]+[[:digit:]]+[[:space:]]+([[:digit:]]+:)+[[:digit:]]+[[:space:]]+(...[[:space:]])?[[:digit:]]{4}";

static const JCharacter* kLockfileArgs				= "lockfile -r 0 ";

// string ID's

static const JCharacter* kDescriptionID = "Description::gmGlobals";

/******************************************************************************
 GMCreateGlobals

	server can be NULL

 ******************************************************************************/

JBoolean
GMCreateGlobals
	(
	GMApp*		app
	)
{
	theApplication = app;

	JString oldPrefsFile, newPrefsFile;
	if (JGetHomeDirectory(&oldPrefsFile))
		{
		oldPrefsFile = JCombinePathAndName(oldPrefsFile, ".gArrow.pref");
		if (JFileExists(oldPrefsFile) &&
			(JPrefsFile::GetFullName(app->GetSignature(), &newPrefsFile)).OK() &&
			!JFileExists(newPrefsFile))
			{
			JRenameFile(oldPrefsFile, newPrefsFile);
			}
		}

	itsDirMgr = new GMDirectorManager();
	assert(itsDirMgr != NULL);

	itsAddressBookMgr = new GAddressBookMgr();
	assert(itsAddressBookMgr != NULL);

	itsPGPEnabled = kJTrue;

	JXInitHelp(kGTOCHelpName, kHelpSectionCount, kHelpSectionName);

	JBoolean isNew;
	itsPrefsMgr = new GPrefsMgr(&isNew);
	assert(itsPrefsMgr != NULL);

	itsInboxMgr = new GInboxMgr();
	assert(itsInboxMgr != NULL);

	GCheckPGP();
	GGheckGPG();

	thePermWindowOwner = new JXDirector(NULL);
	assert( thePermWindowOwner != NULL );

	itsAccountMgr	= new GMAccountManager();
	assert(itsAccountMgr != NULL);

	itsFilterMgr	= new GMFilterManager();
	assert(itsFilterMgr != NULL);

	itsMailboxTreeDir = new GMailboxTreeDir(app);
	assert(itsMailboxTreeDir != NULL);

	itsAddressBookTreeDir = new GAddressBookTreeDir(thePermWindowOwner);
	assert(itsAddressBookTreeDir != NULL);

	itsSMTPDebugDir = new GNetDebugDir(thePermWindowOwner, smtpDebugTitle);
	assert(itsSMTPDebugDir != NULL);

	itsPOPDebugDir = new GNetDebugDir(thePermWindowOwner, popDebugTitle);
	assert(itsPOPDebugDir != NULL);

	itsPrefsMgr->ReadChooseSaveDialogPrefs();

	itsPrinter = new JXPTPrinter();
	assert(itsPrinter != NULL);

	itsPrefsMgr->ReadPrinterSetup(itsPrinter);

	itsAltChooseSaveFile = new GMChooseSaveFile();
	assert(itsAltChooseSaveFile != NULL);

	itsCSFPref = new GMChooseSavePrefObject(itsPrefsMgr, kGAltChooseSaveSetupID, itsAltChooseSaveFile);
	assert(itsCSFPref != NULL);

	itsAltPTPrinter = new GMPTPrinter(itsPrefsMgr, kGSpecialPTPrintSetupID);
	assert(itsAltPTPrinter != NULL);

	itsMailboxCSF	= new GMailboxChooseSaveFile();
	assert(itsMailboxCSF != NULL);

	if (JProgramAvailable("lockfile"))
		{
		itsLockFilePrgExists = kJTrue;
		}

	JXFSBindingManager::Initialize();

	return isNew;
}

/******************************************************************************
 GMDeleteGlobals

 ******************************************************************************/

void
GMDeleteGlobals()
{
	delete itsCSFPref;
	delete itsAltChooseSaveFile;
	delete itsAltPTPrinter;
	delete itsMailboxCSF;
	itsPrefsMgr->WritePrinterSetup(itsPrinter);
	delete itsPrinter;
	itsPrefsMgr->WriteChooseSaveDialogPrefs();
	theApplication = NULL;
	delete itsInboxMgr;
	delete itsAccountMgr;
	delete itsFilterMgr;

	const JBoolean ok = thePermWindowOwner->Close();
	assert( ok );

	thePermWindowOwner   = NULL;

	delete itsPrefsMgr;
	delete itsAddressBookMgr;
	delete itsDirMgr;

	delete JXFSBindingManager::Instance();
}

/******************************************************************************
 GMGetVersionNumberStr

 ******************************************************************************/

const JCharacter*
GMGetVersionNumberStr()
{
	return JGetString("VERSION");
}

/******************************************************************************
 GMGetVersionStr

 ******************************************************************************/

JString
GMGetVersionStr()
{
	const JCharacter* map[] =
		{
		"version",   JGetString("VERSION"),
		"copyright", JGetString("COPYRIGHT")
		};
	return JGetString(kDescriptionID, map, sizeof(map));
}

/******************************************************************************
 GMSuspend

 ******************************************************************************/

void
GMSuspend()
{
	thePermWindowOwner->Suspend();
}

/******************************************************************************
 GMResume

 ******************************************************************************/

void
GMResume()
{
	thePermWindowOwner->Resume();
}

/******************************************************************************
 GMGetApplication

 ******************************************************************************/

GMApp*
GMGetApplication()
{
	assert( theApplication != NULL );
	return theApplication;
}

/******************************************************************************
 GGetPrefsMgr

 ******************************************************************************/

GPrefsMgr*
GGetPrefsMgr()
{
	assert(itsPrefsMgr != NULL);
	return itsPrefsMgr;
}

/******************************************************************************
 GGetInboxMgr

 ******************************************************************************/

GInboxMgr*
GGetInboxMgr()
{
	assert(itsInboxMgr != NULL);
	return itsInboxMgr;
}

/******************************************************************************
 GGetAccountMgr

 ******************************************************************************/

GMAccountManager*
GGetAccountMgr()
{
	assert(itsAccountMgr != NULL);
	return itsAccountMgr;
}

/******************************************************************************
 GGetFilterMgr

 ******************************************************************************/

GMFilterManager*
GGetFilterMgr()
{
	assert(itsFilterMgr != NULL);
	return itsFilterMgr;
}

/******************************************************************************
 GMGetSMTPDebugDir

 ******************************************************************************/

GNetDebugDir*
GMGetSMTPDebugDir()
{
	assert( itsSMTPDebugDir != NULL );
	return itsSMTPDebugDir;
}

/******************************************************************************
 GMGetPOPDebugDir

 ******************************************************************************/

GNetDebugDir*
GMGetPOPDebugDir()
{
	assert( itsPOPDebugDir != NULL );
	return itsPOPDebugDir;
}

/******************************************************************************
 GGetAddressBookMgr

 ******************************************************************************/

GAddressBookMgr*
GGetAddressBookMgr()
{
	assert(itsAddressBookMgr != NULL);
	return itsAddressBookMgr;
}

/******************************************************************************
 GPGP_2_6_Enabled

 ******************************************************************************/

JBoolean
GPGP_2_6_Enabled()
{
	return itsPGPEnabled;
}

/******************************************************************************
 GGPG_1_0_Enabled

 ******************************************************************************/

JBoolean
GGPG_1_0_Enabled()
{
	return itsGPGEnabled;
}

/******************************************************************************
 GEncryptionEnabled

 ******************************************************************************/

JBoolean
GEncryptionEnabled()
{
	if (itsPrefsMgr->GetEncryptionType() == GPrefsMgr::kPGP2_6)
		{
		return itsPGPEnabled;
		}
	else
		{
		return itsGPGEnabled;
		}
}

/******************************************************************************
 GCheckPGP

 ******************************************************************************/

void
GCheckPGP()
{
	if (!JProgramAvailable("pgp"))
		{
		itsPGPEnabled = kJFalse;
		return;
		}
	JString sysCmd = "pgp -kv";
	int inFD;
	JError err =
		JExecute(sysCmd, NULL,
			kJIgnoreConnection, NULL,
			kJCreatePipe, &inFD,
			kJTossOutput, NULL);
	if (err.OK())
		{
		JString buffer;
		JReadAll(inFD, &buffer);
		JRegex regex;
		JBoolean matched;
		JArray<JIndexRange> subList;
		err = regex.SetPattern("([0-9]+) matching key[s]* found");
		assert(err.OK());
		matched = regex.Match(buffer, &subList);
		if (matched)
			{
			JIndexRange sRange = subList.GetElement(2);
			JString number = buffer.GetSubstring(sRange);
			if (number == "0")
				{
				itsPGPEnabled = kJTrue;
				}
			}
		else
			{
			itsPGPEnabled = kJFalse;
			}
		}
	else
		{
		itsPGPEnabled = kJFalse;
		}
}

/******************************************************************************
 GGheckGPG

 ******************************************************************************/

void
GGheckGPG()
{
	if (!JProgramAvailable("gpg"))
		{
		itsGPGEnabled = kJFalse;
		return;
		}
	JString sysCmd = "gpg --list-key --no-secmem-warning";
	int fromFD;
	JError err =
		JExecute(sysCmd,		NULL,
			kJIgnoreConnection,	NULL,
			kJCreatePipe,		&fromFD,
			kJTossOutput,		NULL);
	if (err.OK())
		{
		JString buffer;
		JReadAll(fromFD, &buffer);
		buffer.TrimWhitespace();
		if (buffer.IsEmpty())
			{
			itsGPGEnabled = kJFalse;
			}
		}
	else
		{
		itsGPGEnabled = kJFalse;
		}
	itsGPGEnabled = kJTrue;
}

/******************************************************************************
 GBlockUntilFileUnlocked

 ******************************************************************************/

void
GBlockUntilFileUnlocked
	(
	const JCharacter* filename
	)
{
	GMGetApplication()->DisplayBusyCursor();
	JString lockfile = JString(filename) + ".lock";
	if (!JFileExists(lockfile))
		{
		return;
		}
	for (JSize count = 1; count <= 6; count++)
		{
		JWait(0.5);
		if (!JFileExists(lockfile))
			{
			return;
			}
		}
	JString report = "The file \"" + JString(filename) + "\" is locked. Waiting for the file to be unlocked.";
	if (!itsProgramIsDying)
		{
		JGetUserNotification()->ReportError(report);
		}
	for (JSize count = 1; count <= 60; count++)
		{
		JWait(0.5);
		if (!JFileExists(lockfile))
			{
			return;
			}
		}
	GUnlockFile(filename);
}

/******************************************************************************
 GLockFile

 ******************************************************************************/

JBoolean
GLockFile
	(
	const JCharacter* filename
	)
{
	JString lockfile = JString(filename) + ".lock";
	if (JFileExists(lockfile))
		{
		return kJFalse;
		}
	JString username = JGetUserName();

	JString inbox = "/var/spool/mail/" + username;
	if ((inbox == filename) && itsLockFilePrgExists)
		{
		JString cmd = "lockfile -ml";
		JString errStr;
		const JError err = JRunProgram(cmd, &errStr);
		if (!err.OK())
			{
			if (JFileExists(lockfile))
				{
				return kJFalse;
				}
			err.ReportIfError();
			JString report = "I was unable to lock the mailbox \"" +
							 JString(filename) +
							 "\", so I won't be able to save your changes.";
			JGetUserNotification()->ReportError(report);
			return kJFalse;
			}
		return kJTrue;
		}
	else
		{
		JString cmd	= kLockfileArgs + JString("\"") + lockfile + JString("\"");
		JString errStr;
		const JError err = JRunProgram(cmd, &errStr);
		if (!err.OK())
			{
			return kJFalse;
			}
		return kJTrue;
		}

	return kJTrue;
}

/******************************************************************************
 GFileLocked


 ******************************************************************************/

JBoolean
GFileLocked
	(
	const JCharacter* filename
	)
{
	JString lockfile = JString(filename) + ".lock";
	if (JFileExists(lockfile))
		{
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 GUnlockFile

 ******************************************************************************/

void
GUnlockFile
	(
	const JCharacter* filename
	)
{
	JString lockfile = JString(filename) + ".lock";
	if (!JFileExists(lockfile))
		{
		return;
		}
	JString cmd = "rm -f \"" + lockfile + "\"";
	JString errors;
	const JError err	= JRunProgram(cmd, &errors);
	if (err.OK())
		{
		return;
		}
	if (!JFileExists(lockfile))
		{
		return;
		}

	JString username = JGetUserName();

	JString inbox = "/var/spool/mail/" + username;
	if ((inbox == filename) && itsLockFilePrgExists)
		{
		JString cmd = "lockfile -mu";
		JString errStr;
		const JError err = JRunProgram(cmd, &errStr);
		if (!err.OK())
			{
			JGetUserNotification()->ReportError(errStr);
			}
		}
}

/******************************************************************************
 GProgramIsDying

 ******************************************************************************/

JBoolean
GProgramIsDying()
{
	return itsProgramIsDying;
}

/******************************************************************************
 GSetProgramDying

 ******************************************************************************/

void
GSetProgramDying()
{
	itsProgramIsDying = kJTrue;
}

/******************************************************************************
 GQuoteString

 ******************************************************************************/

void
GQuoteString
	(
	JString*		text,
	const JBoolean	quoteFirst
	)
{
	if (quoteFirst)
		{
		if (text->IndexValid(1))
			{
			if (text->GetCharacter(1) == '>')
				{
				text->InsertSubstring(">", 1);
				}
			else
				{
				text->InsertSubstring("> ", 1);
				}
			}
		}
	JIndex index = 1;
	while (text->IndexValid(index) && text->LocateNextSubstring("\n", &index))
		{
		if (text->IndexValid(index + 1))
			{
			if (text->GetCharacter(index+1) == '>')
				{
				text->InsertSubstring(">", index+1);
				}
			else if (text->GetCharacter(index+1) == '\n')
				{
				text->InsertSubstring(">", index+1);
				}
			else
				{
				text->InsertSubstring("> ", index+1);
				}
			}
		index ++;
		}

}

/******************************************************************************
 GCheckMail

 ******************************************************************************/

void
GCheckMail()
{
	theApplication->CheckMail();
}

/******************************************************************************
 GDBGetPTPrinter

 ******************************************************************************/

JXPTPrinter*
GMGetPTPrinter()
{
	return itsPrinter;
}

/******************************************************************************
 GMReplaceFileWithTemp

 ******************************************************************************/

JBoolean
GMReplaceFileWithTemp
	(
	const JCharacter* tempname,
	const JCharacter* filename
	)
{
	const JString tempStr(tempname);
	const JString fileStr(filename);
	JString cmd = "mv -f \"" + tempStr + "\" \"" + fileStr + "\"";
	JString errStr;
	JError err = JRunProgram(cmd, &errStr);
	if (err.OK())
		{
		return kJTrue;
		}
	cmd = "cp -f \"" + tempStr + "\" \"" + fileStr + "\"";
	err = JRunProgram(cmd, &errStr);
	if (err.OK())
		{
		JRemoveFile(tempStr);
		return kJTrue;
		}
	// do it by hand...
	JSize size;
	err = JGetFileLength(tempname, &size);
	if (!err.OK())
		{
		JRemoveFile(tempStr);
		return kJFalse;
		}
	ifstream is(tempname);
	if (!is.good())
		{
		JRemoveFile(tempStr);
		return kJFalse;
		}
	ofstream os(filename);
	if (!os.good())
		{
		JRemoveFile(tempStr);
		return kJFalse;
		}
	JCopyBinaryData(is, os, size);
	JRemoveFile(tempStr);
	return kJTrue;
}

/******************************************************************************
 GMGetAltChooseSaveFile

 ******************************************************************************/

GMChooseSaveFile*
GMGetAltChooseSaveFile()
{
	return itsAltChooseSaveFile;
}

/******************************************************************************
 GMGetAltPTPrinter

 ******************************************************************************/

GMPTPrinter*
GMGetAltPTPrinter()
{
	return itsAltPTPrinter;
}

/******************************************************************************
 GGetDirMgr

 ******************************************************************************/

GMDirectorManager*
GGetDirMgr()
{
	return itsDirMgr;
}

/******************************************************************************
 GMGetMailboxCSF

 ******************************************************************************/

GMailboxChooseSaveFile*
GMGetMailboxCSF()
{
	return itsMailboxCSF;
}

/******************************************************************************
 GMGetAddressBookDir

 ******************************************************************************/

GAddressBookTreeDir*
GMGetAddressBookDir()
{
	assert(itsAddressBookTreeDir != NULL);
	return itsAddressBookTreeDir;
}

/******************************************************************************
 GMGetWMClassInstance

 ******************************************************************************/

const JCharacter*
GMGetWMClassInstance()
{
	return kGMWMInstance;
}

/******************************************************************************
 GMGetTableWindowClass

 ******************************************************************************/

const JCharacter*
GMGetTableWindowClass()
{
	return kTableWindowClass;
}

/******************************************************************************
 GMGetFindDirWindowClass

 ******************************************************************************/

const JCharacter*
GMGetFindDirWindowClass()
{
	return kFindDirWindowClass;
}

/******************************************************************************
 GMGetViewWindowClass

 ******************************************************************************/

const JCharacter*
GMGetViewWindowClass()
{
	return kViewWindowClass;
}

/******************************************************************************
 GMGetEditorWindowClass

 ******************************************************************************/

const JCharacter*
GMGetEditorWindowClass()
{
	return kEditorWindowClass;
}

/******************************************************************************
 GMGetAddressBookWindowClass

 ******************************************************************************/

const JCharacter*
GMGetAddressBookWindowClass()
{
	return kAddressBookWindowClass;
}

/******************************************************************************
 GMGetMailboxWindowClass

 ******************************************************************************/

const JCharacter*
GMGetMailboxWindowClass()
{
	return kMailboxWindowClass;
}

/******************************************************************************
 GGetMailboxTreeDir

 ******************************************************************************/

GMailboxTreeDir*
GGetMailboxTreeDir()
{
	assert(itsMailboxTreeDir != NULL);
	return itsMailboxTreeDir;
}

/******************************************************************************
 GMGetMailRegexStr

 ******************************************************************************/

const JCharacter*
GMGetMailRegexStr()
{
	return kMailRegexStr;
}

/******************************************************************************
 GMGetMailboxWindowClass

 ******************************************************************************/

const JRegex&
GMGetMailRegex()
{
	return itsMailRegex;
}
