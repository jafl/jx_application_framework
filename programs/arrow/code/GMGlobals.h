/******************************************************************************
 GMGlobals.h

 *****************************************************************************/

#ifndef _H_GMGlobals
#define _H_GMGlobals

#include <jXGlobals.h>

class GMApp;
class GNetDebugDir;
class GAddressBookMgr;
class GPrefsMgr;
class GInboxMgr;
class JXPTPrinter;
class JXSpellChecker;
class GMChooseSaveFile;
class GMPTPrinter;
class GMDirectorManager;
class GMailboxChooseSaveFile;
class GAddressBookTreeDir;
class GMAccountManager;
class GMFilterManager;
class GMailboxTreeDir;
class JRegex;

GMApp*				GMGetApplication();

GNetDebugDir*		GMGetSMTPDebugDir();
GNetDebugDir*		GMGetPOPDebugDir();

GAddressBookMgr*		GGetAddressBookMgr();
GAddressBookTreeDir*	GMGetAddressBookDir();

GMailboxTreeDir*		GGetMailboxTreeDir();

GPrefsMgr*			GGetPrefsMgr();
GInboxMgr*			GGetInboxMgr();
GMAccountManager*	GGetAccountMgr();
GMFilterManager*	GGetFilterMgr();


void				GCheckPGP();
void				GGheckGPG();
JBoolean			GPGP_2_6_Enabled();
JBoolean			GGPG_1_0_Enabled();
JBoolean			GEncryptionEnabled();

void				GBlockUntilFileUnlocked(const JCharacter* filename);
JBoolean			GLockFile(const JCharacter* filename);
JBoolean			GFileLocked(const JCharacter* filename);
void				GUnlockFile(const JCharacter* filename);

JBoolean			GProgramIsDying();
void				GSetProgramDying();

void				GQuoteString(JString* text, const JBoolean quoteFirst);

void				GCheckMail();
JXPTPrinter*		GMGetPTPrinter();

GMChooseSaveFile*	GMGetAltChooseSaveFile();
GMPTPrinter*		GMGetAltPTPrinter();

GMailboxChooseSaveFile*	GMGetMailboxCSF();

GMDirectorManager*	GGetDirMgr();

JBoolean			GMReplaceFileWithTemp(const JCharacter* tempname,
										  const JCharacter* filename);

const JCharacter*	GMGetMailRegexStr();
const JRegex&		GMGetMailRegex();

const JCharacter*	GMGetVersionNumberStr();
JString				GMGetVersionStr();

	// called by TestApp

JBoolean	GMCreateGlobals(GMApp* app);
void		GMDeleteGlobals();

void		GMSuspend();
void		GMResume();

	// window manager resources

const JCharacter*	GMGetWMClassInstance();
const JCharacter*	GMGetTableWindowClass();
const JCharacter*	GMGetFindDirWindowClass();
const JCharacter*	GMGetViewWindowClass();
const JCharacter*	GMGetEditorWindowClass();
const JCharacter*	GMGetAddressBookWindowClass();
const JCharacter*	GMGetMailboxWindowClass();

#endif
