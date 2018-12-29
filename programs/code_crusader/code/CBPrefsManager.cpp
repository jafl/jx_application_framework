/******************************************************************************
 CBPrefsManager.cpp

	BASE CLASS = JXPrefsManager

	Copyright (C) 1997-2001 John Lindal.

 ******************************************************************************/

#include "CBPrefsManager.h"
#include "CBEditFileTypesDialog.h"
#include "CBEditMacroDialog.h"
#include "CBEditCRMDialog.h"
#include "CBProjectDocument.h"
#include "CBTextDocument.h"
#include "CBTextEditor.h"
#include "CBCharActionManager.h"
#include "CBMacroManager.h"
#include "cbmUtil.h"
#include "cbGlobals.h"
#include "cbFileVersions.h"
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXChooseSaveFile.h>
#include <JXHelpManager.h>
#include <JXFontManager.h>
#include <JXColorManager.h>
#include <JXSharedPrefsManager.h>
#include <JRegex.h>
#include <JSubstitute.h>
#include <JDirInfo.h>
#include <jFileUtil.h>
#include <jAssert.h>

static const JString kScriptDir("scripts", kJFalse);
static const JUtf8Character kCBNameRegexMarker("*");

const JFileVersion kCurrentTextColorVers = 1;

// version  1 appends kRightMarginColorIndex

// JBroadcaster message types

const JUtf8Byte* CBPrefsManager::kFileTypesChanged = "FileTypesChanged::CBPrefsManager";
const JUtf8Byte* CBPrefsManager::kTextColorChanged = "TextColorChanged::CBPrefsManager";

/******************************************************************************
 Constructor

 ******************************************************************************/

CBPrefsManager::CBPrefsManager
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue)
{
	itsFileTypeList = nullptr;
	itsMacroList    = nullptr;
	itsCRMList      = nullptr;

	itsExecOutputWordWrapFlag  = kJTrue;
	itsUnknownTypeWordWrapFlag = kJTrue;

	itsFileTypesDialog = nullptr;
	itsMacroDialog     = nullptr;
	itsCRMDialog       = nullptr;

	*isNew = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kCBgCSFSetupID);
	csf->JPrefObject::ReadPrefs();

	ReadColors();
	ReadStaticGlobalPrefs(kCurrentPrefsFileVersion);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPrefsManager::~CBPrefsManager()
{
	SaveAllBeforeDestruct();

	JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsFileTypeList->GetElement(i)).Free();
		}

	jdelete itsFileTypeList;

	count = itsMacroList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsMacroList->GetElement(i)).Free();
		}

	jdelete itsMacroList;

	count = itsCRMList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(itsCRMList->GetElement(i)).Free();
		}

	jdelete itsCRMList;
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
CBPrefsManager::SaveAllBeforeDestruct()
{
	WriteStaticGlobalPrefs();

	WriteData(*itsMacroList);
	WriteData(*itsCRMList);
	WriteFileTypeInfo();
	WriteColors();

	SetData(kCBProgramVersionID, CBGetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 GetJCCVersionStr

 ******************************************************************************/

JString
CBPrefsManager::GetJCCVersionStr()
	const
{
	std::string data;
	if (GetData(kCBProgramVersionID, &data))
		{
		return JString(data);
		}
	else
		{
		return JString("< 0.13.0");		// didn't exist before this version
		}
}

/******************************************************************************
 GetExpirationTimeStamp

 ******************************************************************************/

JBoolean
CBPrefsManager::GetExpirationTimeStamp
	(
	time_t* t
	)
	const
{
	std::string data;
	if (GetData(kCBExpireTimeStampID, &data))
		{
		std::istringstream input(data);
		input >> *t;
		return JI2B(!input.fail());
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetExpirationTimeStamp

 ******************************************************************************/

void
CBPrefsManager::SetExpirationTimeStamp
	(
	const time_t t
	)
{
	std::ostringstream data;
	data << t;

	SetData(kCBExpireTimeStampID, data);
}

/******************************************************************************
 EditFileTypes

 ******************************************************************************/

void
CBPrefsManager::EditFileTypes()
{
	assert( itsFileTypesDialog == nullptr );

	itsFileTypesDialog =
		jnew CBEditFileTypesDialog(CBGetApplication(), *itsFileTypeList,
								  *itsMacroList, *itsCRMList,
								  itsExecOutputWordWrapFlag,
								  itsUnknownTypeWordWrapFlag);
	assert( itsFileTypesDialog != nullptr );
	itsFileTypesDialog->BeginDialog();
	ListenTo(itsFileTypesDialog);
}

/******************************************************************************
 UpdateFileTypes (private)

 ******************************************************************************/

void
CBPrefsManager::UpdateFileTypes
	(
	const CBEditFileTypesDialog& dlog
	)
{
	JPtrArray< JPtrArray<JString> > origSuffixList(JPtrArrayT::kDeleteAll);
	for (JUnsignedOffset i=0; i<kCBFTCount; i++)
		{
		JPtrArray<JString>* list = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert( list != nullptr );
		origSuffixList.Append(list);

		GetFileSuffixes((CBTextFileType) i, list);
		}

	dlog.GetFileTypeInfo(itsFileTypeList, &itsExecOutputWordWrapFlag,
						 &itsUnknownTypeWordWrapFlag);

	itsFileTypeList->SetCompareFunction(CompareFileTypeSpecAndLength);
	itsFileTypeList->Sort();

	FileTypesChanged msg;
	JPtrArray<JString> suffixes(JPtrArrayT::kDeleteAll);
	for (JUnsignedOffset i=0; i<kCBFTCount; i++)
		{
		GetFileSuffixes((CBTextFileType) i, &suffixes);

		msg.SetChanged((CBTextFileType) i,
			!JSameStrings(*(origSuffixList.GetElement(i+1)), suffixes, kJTrue));
		}

	Broadcast(msg);

	if (msg.AnyChanged())
		{
		CBMWriteSharedPrefs(kJTrue);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBPrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileTypesDialog &&
		message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			UpdateFileTypes(*itsFileTypesDialog);
			}
		itsFileTypesDialog = nullptr;
		}

	else if (sender == itsMacroDialog &&
		message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			UpdateMacros(*itsMacroDialog);
			}
		itsMacroDialog = nullptr;
		}

	else if (sender == itsCRMDialog &&
		message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			UpdateCRMRuleLists(*itsCRMDialog);
			}
		itsCRMDialog = nullptr;
		}

	else
		{
		JXPrefsManager::Receive(sender, message);
		}
}

/******************************************************************************
 cbAddNewSuffixes (local)

 ******************************************************************************/

struct CBNewSuffixInfo
{
	const JUtf8Byte*	suffix;
	JBoolean			found;
};

void
cbAddNewSuffixes
	(
	const JUtf8Byte*		macroName,		// can be nullptr
	const JUtf8Byte*		origCRMName,	// can be nullptr -- uses macroName
	const CBTextFileType	type,
	CBNewSuffixInfo*		newInfo,
	const JSize				newSize,

	JArray<CBPrefsManager::FileTypeInfo>*			fileTypeList,
	JArray<CBPrefsManager::MacroSetInfo>*			macroList,
	const JArray<CBPrefsManager::CRMRuleListInfo>&	crmList
	)
{
	const JSize newCount = newSize / sizeof(CBNewSuffixInfo);

	// if suffix already exists, don't change it

	const JSize ftCount = fileTypeList->GetElementCount();
	for (JUnsignedOffset i=0; i<newCount; i++)
		{
		for (JIndex j=1; j<=ftCount; j++)
			{
			const CBPrefsManager::FileTypeInfo ftInfo = fileTypeList->GetElement(j);
			if (*(ftInfo.suffix) == newInfo[i].suffix)
				{
				newInfo[i].found = kJTrue;
				break;
				}
			}
		}

	// install new suffixes

	const JIndex macroID = CBPrefsManager::FindMacroName(macroName, macroList, kJTrue);

	const JUtf8Byte* crmName =
		(JString::IsEmpty(origCRMName) ? macroName : origCRMName);

	const JIndex crmID = CBPrefsManager::FindCRMRuleListName(crmName, crmList);

	for (JUnsignedOffset i=0; i<newCount; i++)
		{
		if (!newInfo[i].found)
			{
			JString* suffix = jnew JString(newInfo[i].suffix);
			assert( suffix != nullptr );

			JString* complSuffix = jnew JString;
			assert( complSuffix != nullptr );

			fileTypeList->InsertSorted(
				CBPrefsManager::FileTypeInfo(
					suffix, nullptr, nullptr, type, macroID, crmID,
					kJTrue, nullptr, kJFalse, complSuffix, nullptr));
			}
		}
}

/******************************************************************************
 cbAddNewExternalSuffixes (local)

 ******************************************************************************/

struct CBNewExternalSuffixInfo
{
	const JUtf8Byte*	suffix;
	const JUtf8Byte*	cmd;
	JBoolean			found;
};

void
cbAddNewExternalSuffixes
	(
	CBNewExternalSuffixInfo*				newInfo,
	const JSize								newCount,
	JArray<CBPrefsManager::FileTypeInfo>*	fileTypeList
	)
{
	const JSize ftCount = fileTypeList->GetElementCount();
	for (JUnsignedOffset i=0; i<newCount; i++)
		{
		for (JIndex j=1; j<=ftCount; j++)
			{
			const CBPrefsManager::FileTypeInfo ftInfo = fileTypeList->GetElement(j);
			if (*(ftInfo.suffix) == newInfo[i].suffix)
				{
				newInfo[i].found = kJTrue;
				break;
				}
			}
		}

	for (JUnsignedOffset i=0; i<newCount; i++)
		{
		if (!newInfo[i].found)
			{
			JString* suffix = jnew JString(newInfo[i].suffix);
			assert( suffix != nullptr );

			JString* complSuffix = jnew JString;
			assert( complSuffix != nullptr );

			JString* editCmd = jnew JString(newInfo[i].cmd);
			assert( editCmd != nullptr );

			CBPrefsManager::FileTypeInfo info(
				suffix, nullptr, nullptr, kCBExternalFT,
				kCBEmptyMacroID, kCBEmptyCRMRuleListID,
				kJTrue, nullptr, kJFalse, complSuffix, editCmd);
			info.CreateRegex();
			fileTypeList->InsertSorted(info);
			}
		}
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
CBPrefsManager::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	std::string data;

	//
	// upgrades that must be done in order
	//

	// initialize suffixes

	if (isNew)
		{
		std::ostringstream sourceSuffixData;
		sourceSuffixData << 7;
		sourceSuffixData << ' ' << JString(".c")   << ' ' << JString(".cc");
		sourceSuffixData << ' ' << JString(".cpp") << ' ' << JString(".cxx");
		sourceSuffixData << ' ' << JString(".c++") << ' ' << JString(".C");
		sourceSuffixData << ' ' << JString(".tmpl");
		SetData(10000, sourceSuffixData);

		std::ostringstream headerSuffixData;
		headerSuffixData << 6;
		headerSuffixData << ' ' << JString(".h")   << ' ' << JString(".hh");
		headerSuffixData << ' ' << JString(".hpp") << ' ' << JString(".hxx");
		headerSuffixData << ' ' << JString(".h++") << ' ' << JString(".H");
		SetData(10001, headerSuffixData);
		}

	// add default font

	if (currentVersion < 5)
		{
		SetDefaultFont(JFontManager::GetDefaultMonospaceFontName(),
					   JFontManager::GetDefaultMonospaceFontSize());
		}

	// collect file suffixes

	if (!isNew && currentVersion < 6)
		{
		const JIndex origID[] = { 2, 3, 16, 17 };
		for (JUnsignedOffset i=0; i<4; i++)
			{
			GetData(origID[i], &data);
			RemoveData(origID[i]);
			SetData(10000 + i, data);
			}
		}

	// cmds for using external editor

	if (!isNew && currentVersion == 0)
		{
		std::string oldData;
		GetData(kCBDocMgrID, &oldData);
		std::ostringstream newData;
		newData << 0 << ' ' << kJTrue << ' ' << oldData.c_str();
		SetData(kCBDocMgrID, newData);
		}
	else if (4 <= currentVersion && currentVersion <= 6)
		{
		if (GetData(4, &data))
			{
			RemoveData(4);
			SetData(kCBPrintPlainTextID, data);
			}
		}

	// remove PS,EPS print setup for tree

	if (!isNew && currentVersion < 8)
		{
		RemoveData(1);
		RemoveData(8);
		}

	// add in misc file type info

	if (currentVersion < 9)
		{
		if (GetData(10003, &data))
			{
			SetData(10004, data);
			}
		if (GetData(10002, &data))
			{
			SetData(10003, data);
			}
		if (GetData(20003, &data))
			{
			SetData(20004, data);
			}

		std::ostringstream otherSourceSuffixData;
		otherSourceSuffixData << 0;
		SetData(10002, otherSourceSuffixData);

		JBoolean codeWrap = kJFalse;
		if (GetData(kCBTextDocID, &data))
			{
			std::istringstream dataStream(data);
			JFileVersion vers = 0;
			dataStream >> vers;
			if (!dataStream.eof() && !dataStream.fail() && vers == 1)
				{
				JBoolean breakCodeCROnly;
				dataStream >> JBoolFromString(breakCodeCROnly);
				codeWrap = !breakCodeCROnly;
				}
			}

		std::ostringstream codeData;
		codeData << codeWrap;
		SetData(30000, codeData);
		SetData(30001, codeData);
		SetData(30002, codeData);

		std::ostringstream otherData;
		otherData << kJTrue;
		SetData(30003, otherData);
		SetData(30004, otherData);
		SetData(39998, otherData);
		SetData(39999, otherData);
		}

	if (!IDValid(10003))
		{
		std::ostringstream docSuffixData;
		docSuffixData << 1 << ' ' << JString(".doc");
		SetData(10003, docSuffixData);
		}

	if (!IDValid(10004))
		{
		std::ostringstream htmlSuffixData;
		htmlSuffixData << 5;
		htmlSuffixData << ' ' << JString(".html") << ' ' << JString(".HTML");
		htmlSuffixData << ' ' << JString(".htm")  << ' ' << JString(".HTM");
		htmlSuffixData << ' ' << JString(".jsp");
		SetData(10004, htmlSuffixData);
		}

	if (currentVersion < 12)
		{
		// move styler data into main block of ID's

		if (GetData(20000, &data))
			{
			RemoveData(20000);
			SetData(kCBCStyleID, data);
			}

		if (GetData(20004, &data))
			{
			RemoveData(20004);
			SetData(kCBHTMLStyleID, data);
			}

		ConvertFromSuffixLists();
		}
	else
		{
		CreateAndReadData(&itsMacroList, currentVersion);
		ReadFileTypeInfo(currentVersion);
		}

	if (currentVersion < 13)
		{
		CreateCRMRuleLists();
		}
	else
		{
		CreateAndReadData(&itsCRMList, currentVersion);
		}

	if (currentVersion < 14)
		{
		itsFileTypeList->Sort();
		}

	if (currentVersion < 18)
		{
		CBNewSuffixInfo kEiffelSuffix[] =
		{
			{ ".e", kJFalse }
		};
		cbAddNewSuffixes("Eiffel",  nullptr, kCBEiffelFT, kEiffelSuffix, sizeof(kEiffelSuffix),  itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kFortranSuffix[] =
		{
			{ ".f",   kJFalse },
			{ ".for", kJFalse },
			{ ".ftn", kJFalse },
			{ ".f77", kJFalse },
			{ ".f90", kJFalse },
			{ ".f95", kJFalse },
			{ ".F",   kJFalse },
			{ ".FOR", kJFalse },
			{ ".FTN", kJFalse },
			{ ".F77", kJFalse },
			{ ".F90", kJFalse },
			{ ".F95", kJFalse }
		};
		cbAddNewSuffixes("FORTRAN", nullptr, kCBFortranFT, kFortranSuffix, sizeof(kFortranSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kJavaSuffix[] =
		{
			{ ".java", kJFalse },
			{ ".jws",  kJFalse }
		};
		cbAddNewSuffixes("Java", nullptr, kCBJavaSourceFT, kJavaSuffix, sizeof(kJavaSuffix),    itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kStaticLibrarySuffix[] =
		{
			{ ".a", kJFalse }
		};
		cbAddNewSuffixes(nullptr, nullptr, kCBStaticLibraryFT, kStaticLibrarySuffix, sizeof(kStaticLibrarySuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kSharedLibrarySuffix[] =
		{
			{ ".so",    kJFalse },
			{ ".dylib", kJFalse }
		};
		cbAddNewSuffixes(nullptr, nullptr, kCBSharedLibraryFT, kSharedLibrarySuffix, sizeof(kSharedLibrarySuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 23)
		{
		RemoveData(47);
		}

	if (currentVersion < 24)
		{
		CBNewSuffixInfo kAssemblySuffix[] =
		{
			{ ".asm", kJFalse },
			{ ".s",   kJFalse },
			{ ".S",   kJFalse }
		};
		cbAddNewSuffixes("Assembly", nullptr, kCBAssemblyFT, kAssemblySuffix, sizeof(kAssemblySuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kPascalSuffix[] =
		{
			{ ".p",   kJFalse },
			{ ".pas", kJFalse }
		};
		cbAddNewSuffixes("Pascal", nullptr, kCBPascalFT, kPascalSuffix, sizeof(kPascalSuffix),   itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kRatforSuffix[] =
		{
			{ ".r", kJFalse }
		};
		cbAddNewSuffixes("FORTRAN", nullptr, kCBRatforFT, kRatforSuffix, sizeof(kRatforSuffix),   itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 27)
		{
		RemoveData(18);
		RemoveData(20);

		if (GetData(19, &data))
			{
			RemoveData(19);
			ConvertHTMLSuffixesToFileTypes(data);
			}
		else
			{
#ifdef _J_OSX
			CBNewExternalSuffixInfo kImageSuffix[] =
			{
				{ ".gif", "open $f", kJFalse },
				{ ".jpg", "open $f", kJFalse }
			};
#else
			CBNewExternalSuffixInfo kImageSuffix[] =
			{
				{ ".gif", "eog $f", kJFalse },
				{ ".jpg", "eog $f", kJFalse }
			};
#endif
			cbAddNewExternalSuffixes(kImageSuffix, sizeof(kImageSuffix)/sizeof(CBNewExternalSuffixInfo), itsFileTypeList);
			}
		}

	if (currentVersion < 28)
		{
		CBNewExternalSuffixInfo kFDesignSuffix[] =
		{
			{ ".fd", "jfdesign $f", kJFalse }
		};
		cbAddNewExternalSuffixes(kFDesignSuffix, sizeof(kFDesignSuffix)/sizeof(CBNewExternalSuffixInfo), itsFileTypeList);
		}

	if (currentVersion < 29)
		{
		if (GetData(0, &data))
			{
			RemoveData(0);
			SetData(kCBCStyleID, data);
			}
		}

	if (currentVersion < 30)
		{
		CBNewSuffixInfo kModula2ModuleSuffix[] =
		{
			{ ".md", kJFalse }
		};
		cbAddNewSuffixes("Modula-2", nullptr, kCBModula2ModuleFT, kModula2ModuleSuffix, sizeof(kModula2ModuleSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kModula2InterfaceSuffix[] =
		{
			{ ".mi", kJFalse }
		};
		cbAddNewSuffixes("Modula-2", nullptr, kCBModula2InterfaceFT, kModula2InterfaceSuffix, sizeof(kModula2InterfaceSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kModula3ModuleSuffix[] =
		{
			{ ".m3", kJFalse }
		};
		cbAddNewSuffixes("Modula-3", nullptr, kCBModula3ModuleFT, kModula3ModuleSuffix, sizeof(kModula3ModuleSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kModula3InterfaceSuffix[] =
		{
			{ ".i3", kJFalse }
		};
		cbAddNewSuffixes("Modula-3", nullptr, kCBModula3InterfaceFT, kModula3InterfaceSuffix, sizeof(kModula3InterfaceSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 31)
		{
		CBNewSuffixInfo kAWKSuffix[] =
		{
			{ ".awk", kJFalse }
		};
		cbAddNewSuffixes("AWK", "unix script", kCBAWKFT, kAWKSuffix, sizeof(kAWKSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kCobolSuffix[] =
		{
			{ ".cob", kJFalse },
			{ ".COB", kJFalse }
		};
		cbAddNewSuffixes("Cobol", nullptr, kCBCobolFT, kCobolSuffix, sizeof(kCobolSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kLispSuffix[] =
		{
			{ ".cl",    kJFalse },
			{ ".clisp", kJFalse },
			{ ".el",    kJFalse },
			{ ".lisp",  kJFalse },
			{ ".lsp",   kJFalse }
		};
		cbAddNewSuffixes("Lisp", nullptr, kCBLispFT, kLispSuffix, sizeof(kLispSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kPerlSuffix[] =
		{
			{ ".pl",   kJFalse },
			{ ".pm",   kJFalse },
			{ ".perl", kJFalse },
			{ ".plx",  kJFalse }
		};
		cbAddNewSuffixes("Perl", "unix script", kCBPerlFT, kPerlSuffix, sizeof(kPerlSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kPythonSuffix[] =
		{
			{ ".py",     kJFalse },
			{ ".python", kJFalse }
		};
		cbAddNewSuffixes("Python", "unix script", kCBPythonFT, kPythonSuffix, sizeof(kPythonSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kSchemeSuffix[] =
		{
			{ ".sch",    kJFalse },
			{ ".scheme", kJFalse },
			{ ".scm",    kJFalse },
			{ ".sm",     kJFalse },
			{ ".SCM",    kJFalse },
			{ ".SM",     kJFalse }
		};
		cbAddNewSuffixes("Scheme", nullptr, kCBSchemeFT, kSchemeSuffix, sizeof(kSchemeSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kBourneShellSuffix[] =
		{
			{ ".sh",   kJFalse },
			{ ".SH",   kJFalse },
			{ ".bsh",  kJFalse },
			{ ".bash", kJFalse },
			{ ".ksh",  kJFalse }
		};
		cbAddNewSuffixes("Bourne Shell", "unix script", kCBBourneShellFT, kBourneShellSuffix, sizeof(kBourneShellSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kTCLSuffix[] =
		{
			{ ".tcl",  kJFalse },
			{ ".tk",   kJFalse },
			{ ".wish", kJFalse }
		};
		cbAddNewSuffixes("TCL", "unix script", kCBTCLFT, kTCLSuffix, sizeof(kTCLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kVIMSuffix[] =
		{
			{ ".vim", kJFalse }
		};
		cbAddNewSuffixes("Vim", "unix script", kCBVimFT, kVIMSuffix, sizeof(kVIMSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 32)
		{
		CBNewSuffixInfo kJavaArchiveSuffix[] =
		{
			{ ".jar", kJFalse }
		};
		cbAddNewSuffixes(nullptr, nullptr, kCBJavaArchiveFT, kJavaArchiveSuffix, sizeof(kJavaArchiveSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 33)
		{
		CBNewSuffixInfo kPHPSuffix[] =
		{
			{ ".php",   kJFalse },
			{ ".php3",  kJFalse },
			{ ".phtml", kJFalse }
		};
		cbAddNewSuffixes("HTML", "PHP", kCBPHPFT, kPHPSuffix, sizeof(kPHPSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 34)
		{
		CBNewSuffixInfo kASPSuffix[] =
		{
			{ ".asp", kJFalse },
			{ ".asa", kJFalse }
		};
		cbAddNewSuffixes("ASP", nullptr, kCBASPFT, kASPSuffix, sizeof(kASPSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 35)
		{
		CBNewSuffixInfo kMakeSuffix[] =
		{
			{ ".mak",         kJFalse },
			{ "Makefile",     kJFalse },
			{ "makefile",     kJFalse },
			{ "Make.header",  kJFalse }
		};
		cbAddNewSuffixes("Make", "unix script", kCBMakeFT, kMakeSuffix, sizeof(kMakeSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 36)
		{
		CBNewSuffixInfo kREXXSuffix[] =
		{
			{ ".cmd",  kJFalse },
			{ ".rexx", kJFalse },
			{ ".rx",   kJFalse }
		};
		cbAddNewSuffixes("REXX", "C", kCBREXXFT, kREXXSuffix, sizeof(kREXXSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kRubySuffix[] =
		{
			{ ".rb", kJFalse }
		};
		cbAddNewSuffixes("Ruby", "unix script", kCBRubyFT, kRubySuffix, sizeof(kRubySuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 38)
		{
		CBNewSuffixInfo kLexSuffix[] =
		{
			{ ".l", kJFalse }
		};
		cbAddNewSuffixes("Lex", "C", kCBLexFT, kLexSuffix, sizeof(kLexSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 39)
		{
		CBNewSuffixInfo kCShellSuffix[] =
		{
			{ ".csh",   kJFalse },
			{ ".CSH",   kJFalse },
			{ ".tcsh",  kJFalse }
		};
		cbAddNewSuffixes("C shell", "unix script", kCBCShellFT, kCShellSuffix, sizeof(kCShellSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 41)
		{
		CBNewSuffixInfo kBisonSuffix[] =
		{
			{ ".y", kJFalse }
		};
		cbAddNewSuffixes("Bison", "C", kCBBisonFT, kBisonSuffix, sizeof(kBisonSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 44)
		{
		CBNewSuffixInfo kBetaSuffix[] =
		{
			{ ".bet", kJFalse }
		};
		cbAddNewSuffixes("Beta", "Beta", kCBBetaFT, kBetaSuffix, sizeof(kBetaSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kLuaSuffix[] =
		{
			{ ".lua", kJFalse }
		};
		cbAddNewSuffixes("Lua", "Lua",  kCBLuaFT, kLuaSuffix, sizeof(kLuaSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kSLangSuffix[] =
		{
			{ ".sl", kJFalse }
		};
		cbAddNewSuffixes("SLang", nullptr, kCBSLangFT, kSLangSuffix, sizeof(kSLangSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kSQLSuffix[] =
		{
			{ ".sql",  kJFalse },
			{ ".ddl",  kJFalse }
		};
		cbAddNewSuffixes("SQL", "SQL", kCBSQLFT, kSQLSuffix, sizeof(kSQLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kVeraSourceSuffix[] =
		{
			{ ".vr",  kJFalse },
			{ ".vri", kJFalse }
		};
		cbAddNewSuffixes("Vera", "Vera", kCBVeraSourceFT, kVeraSourceSuffix, sizeof(kVeraSourceSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kVeraHeaderSuffix[] =
		{
			{ ".vrh", kJFalse }
		};
		cbAddNewSuffixes("Vera", "Vera", kCBVeraHeaderFT, kVeraHeaderSuffix, sizeof(kVeraHeaderSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kVerilogSuffix[] =
		{
			{ ".v", kJFalse }
		};
		cbAddNewSuffixes("Verilog", nullptr, kCBVerilogFT, kVerilogSuffix, sizeof(kVerilogSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 45)
		{
		RemoveData(14);
		RemoveData(15);
		}

	if (currentVersion < 49)
		{
		JIndex macroID = FindMacroName("C#", itsMacroList, kJTrue);
		JIndex macroIndex;
		if (FindMacroID(*itsMacroList, macroID, &macroIndex))
			{
			const MacroSetInfo macroInfo = itsMacroList->GetElement(macroIndex);
			if (((macroInfo.macro)->GetMacroList()).GetElementCount() == 0)
				{
				AddDefaultCSharpMacros(macroInfo.macro);
				}
			}

		CBNewSuffixInfo kCSharpSuffix[] =
		{
			{ ".cs", kJFalse }
		};
		cbAddNewSuffixes("C#", nullptr, kCBCSharpFT, kCSharpSuffix, sizeof(kCSharpSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kErlangSuffix[] =
		{
			{ ".erl", kJFalse }
		};
		cbAddNewSuffixes("Erlang", nullptr, kCBErlangFT, kErlangSuffix, sizeof(kErlangSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kSMLSuffix[] =
		{
			{ ".sml", kJFalse }
		};
		cbAddNewSuffixes("SML", nullptr, kCBSMLFT, kSMLSuffix, sizeof(kSMLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 50)
		{
		CBNewSuffixInfo kJavaScriptSuffix[] =
		{
			{ ".js",   kJFalse },
			{ ".json", kJFalse }
		};
		cbAddNewSuffixes("JavaScript", "Java", kCBJavaScriptFT, kJavaScriptSuffix, sizeof(kJavaScriptSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 52)
		{
		CBNewSuffixInfo kAntSuffix[] =
		{
			{ "build.xml", kJFalse }
		};
		cbAddNewSuffixes("Ant", "XML", kCBAntFT, kAntSuffix, sizeof(kAntSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 53)
		{
		itsFileTypeList->Sort();
		}

	if (currentVersion < 56)
		{
		CBNewSuffixInfo kJSPSuffix[] =
		{
			{ ".jsp",   kJFalse },
			{ ".jspf",  kJFalse }
		};
		cbAddNewSuffixes("JSP", "Java", kCBJSPFT, kJSPSuffix, sizeof(kJSPSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 57)
		{
		JIndex macroID = FindMacroName("XML", itsMacroList, kJTrue);
		JIndex macroIndex;
		if (FindMacroID(*itsMacroList, macroID, &macroIndex))
			{
			const MacroSetInfo macroInfo = itsMacroList->GetElement(macroIndex);
			if (((macroInfo.macro)->GetMacroList()).GetElementCount() == 0)
				{
				AddDefaultXMLMacros(macroInfo.macro);
				}
			if (((macroInfo.action)->GetActionList()).GetElementCount() == 0)
				{
				AddDefaultXMLActions(macroInfo.action);
				}
			}
		}

	if (currentVersion < 58)
		{
		CBNewSuffixInfo kXMLSuffix[] =
		{
			{ ".xml",  kJFalse },
			{ ".xsd",  kJFalse },
			{ ".wsdl", kJFalse },
			{ ".dtd",  kJFalse }
		};
		cbAddNewSuffixes("XML", nullptr, kCBXMLFT, kXMLSuffix, sizeof(kXMLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 59)
		{
		CBNewSuffixInfo kBasicSuffix[] =
		{
			{ ".bas", kJFalse },
			{ ".bi",  kJFalse },
			{ ".bb",  kJFalse },
			{ ".pb",  kJFalse }
		};
		cbAddNewSuffixes("Basic", nullptr, kCBBasicFT, kBasicSuffix, sizeof(kBasicSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 62)
		{
#ifdef _J_OSX
		CBNewExternalSuffixInfo kImageSuffix[] =
		{
			{ ".png", "open $f", kJFalse }
		};
#else
		CBNewExternalSuffixInfo kImageSuffix[] =
		{
			{ ".png", "eog $f", kJFalse }
		};
#endif
		cbAddNewExternalSuffixes(kImageSuffix, sizeof(kImageSuffix)/sizeof(CBNewExternalSuffixInfo), itsFileTypeList);
		}

	if (currentVersion < 63)
		{
		CBNewSuffixInfo kMatlabSuffix[] =
		{
			{ ".m", kJFalse }
		};
		cbAddNewSuffixes("Matlab", "C", kCBMatlabFT, kMatlabSuffix, sizeof(kMatlabSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kAdobeFlexSuffix[] =
		{
			{ ".as",   kJFalse },
			{ ".mxml", kJFalse }
		};
		cbAddNewSuffixes("Adobe Flash", "C", kCBAdobeFlexFT, kAdobeFlexSuffix, sizeof(kAdobeFlexSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 64)
		{
		JString name;
		JSize size;
		GetDefaultFont(&name, &size);
		if (name != "Courier")
			{
			SetDefaultFont(JFontManager::GetDefaultMonospaceFontName(),
						   JFontManager::GetDefaultMonospaceFontSize());
			}
		}

	if (currentVersion < 65)
		{
		itsFileTypeList->Sort();

		CBNewExternalSuffixInfo kCoreSuffix[] =
		{
			{ "core.*", "medic -c $f", kJFalse }
		};
		cbAddNewExternalSuffixes(kCoreSuffix, sizeof(kCoreSuffix)/sizeof(CBNewExternalSuffixInfo), itsFileTypeList);
		}

	if (currentVersion < 66)
		{
		CBNewSuffixInfo kINISuffix[] =
		{
			{ ".ini", kJFalse }
		};
		cbAddNewSuffixes("INI", nullptr, kCBINIFT, kINISuffix, sizeof(kINISuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 67)
		{
		CBNewSuffixInfo kPropertiesSuffix[] =
		{
			{ ".properties", kJFalse }
		};
		cbAddNewSuffixes("Properties", "UNIX script", kCBPropertiesFT, kPropertiesSuffix, sizeof(kPropertiesSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	//
	// upgrades that can be done in any order
	//

	ReadStaticGlobalPrefs(currentVersion);
	WriteStaticGlobalPrefs();

	// Copy Source Window geometry to Header Window, Other Text Window, and
	// Exec Output Window if they don't already exist.

	if (IDValid(kCBSourceWindSizeID))
		{
		GetData(kCBSourceWindSizeID, &data);
		if (!IDValid(kCBHeaderWindSizeID))
			{
			SetData(kCBHeaderWindSizeID, data);
			}
		if (!IDValid(kCBOtherTextWindSizeID))
			{
			SetData(kCBOtherTextWindSizeID, data);
			}
		if (!IDValid(kCBExecOutputWindSizeID))
			{
			SetData(kCBExecOutputWindSizeID, data);
			}
		if (!IDValid(kCBSearchOutputWindSizeID))
			{
			SetData(kCBSearchOutputWindSizeID, data);
			}
		}
}

/******************************************************************************
 ConvertFromSuffixLists (private)

	[10000,10004], [30000,30004], 39998, 39999 -> kCBFileTypeListID

	40000, 40004, 50000, 50004 read in and deleted.

 ******************************************************************************/

static const JUtf8Byte* kOrigMacroName[] =
{
	"C/C++", "Eiffel", "FORTRAN", "HTML", "Java"
};

static const JUnsignedOffset kCBOrigMacroID[] =
{
	0, 100, 100, 4, 100
};

const JSize kOrigMacroCount = sizeof(kOrigMacroName)/sizeof(JUtf8Byte*);

static const CBTextFileType kOrigFileType[] =
{
	kCBCSourceFT, kCBCHeaderFT, kCBOtherSourceFT, kCBDocumentationFT, kCBHTMLFT
};

static const JUnsignedOffset kOrigFTMacroMap[] =
{
	kCBFirstMacroID, kCBFirstMacroID, kCBEmptyMacroID, kCBEmptyMacroID, kCBFirstMacroID+3
};

const JSize kOrigFileTypeCount = sizeof(kOrigFileType)/sizeof(CBTextFileType);

void
CBPrefsManager::ConvertFromSuffixLists()
{
std::string data;

	assert( itsMacroList == nullptr && itsFileTypeList == nullptr );

	itsMacroList = CreateMacroList();

	for (JUnsignedOffset i=0; i<kOrigMacroCount; i++)
		{
		CBCharActionManager* actionMgr = jnew CBCharActionManager;
		assert( actionMgr != nullptr );
		if (GetData(50000 + kCBOrigMacroID[i], &data))
			{
			RemoveData(50000 + kCBOrigMacroID[i]);
			std::istringstream dataStream(data);
			actionMgr->ReadSetup(dataStream);
			}

		CBMacroManager* macroMgr = jnew CBMacroManager;
		assert( macroMgr != nullptr );
		if (GetData(40000 + kCBOrigMacroID[i], &data))
			{
			RemoveData(40000 + kCBOrigMacroID[i]);
			std::istringstream dataStream(data);
			macroMgr->ReadSetup(dataStream);
			}
		else if (i == 0)
			{
			AddDefaultCMacros(macroMgr);
			}
		else if (i == 1)
			{
			AddDefaultEiffelMacros(macroMgr);
			}
		else if (i == 2)
			{
			AddDefaultFortranMacros(macroMgr);
			}
		else if (i == 3)
			{
			AddDefaultHTMLMacros(macroMgr);
			}
		else
			{
			assert( i == 4 );
			AddDefaultJavaMacros(macroMgr);
			}

		JString* name = jnew JString(kOrigMacroName[i]);
		assert( name != nullptr );

		itsMacroList->AppendElement(
			MacroSetInfo(kCBFirstMacroID + i, name, actionMgr, macroMgr));
		}

	itsFileTypeList = CreateFileTypeList();

	JPtrArray<JString> suffixList(JPtrArrayT::kForgetAll);
	for (JUnsignedOffset i=0; i<kOrigFileTypeCount; i++)
		{
		GetStringList(10000+i, &suffixList);
		RemoveData(10000+i);

		const JBoolean ok = GetData(30000+i, &data);
		assert( ok );
		RemoveData(30000+i);
		std::istringstream dataStream(data);
		JBoolean wordWrap;
		dataStream >> JBoolFromString(wordWrap);

		const JSize count = suffixList.GetElementCount();
		for (JIndex j=1; j<=count; j++)
			{
			JString* complSuffix = jnew JString;
			assert( complSuffix != nullptr );
			InitComplementSuffix(*(suffixList.GetElement(j)), complSuffix);

			itsFileTypeList->AppendElement(
				FileTypeInfo(suffixList.GetElement(j), nullptr, nullptr, kOrigFileType[i],
							 kOrigFTMacroMap[i], kCBEmptyCRMRuleListID,
							 kJTrue, nullptr, wordWrap, complSuffix, nullptr));
			}

		suffixList.RemoveAll();		// avoid DeleteAll() in GetStringList()
		}

	itsFileTypeList->Sort();

	{
	const JBoolean ok = GetData(39998, &data);
	assert( ok );
	RemoveData(39998);
	std::istringstream dataStream(data);
	dataStream >> JBoolFromString(itsExecOutputWordWrapFlag);
	}

	{
	const JBoolean ok = GetData(39999, &data);
	assert( ok );
	RemoveData(39999);
	std::istringstream dataStream(data);
	dataStream >> JBoolFromString(itsUnknownTypeWordWrapFlag);
	}
}

/******************************************************************************
 Default macros (private)

 ******************************************************************************/

void
CBPrefsManager::AddDefaultCMacros
	(
	CBMacroManager* mgr
	)
	const
{
	mgr->AddMacro("/*",  "  */\\l\\l\\l");
	mgr->AddMacro("#\"", "\\binclude \"\"\\l");
	mgr->AddMacro("#<",  "\\binclude <>\\l");

	mgr->AddMacro("if",   " ()\\n\\t{\\n}\\u\\u\\l");
	mgr->AddMacro("ei",   "\\blse if ()\\n\\t{\\n}\\u\\u\\r\\r\\r\\r");
	mgr->AddMacro("el",   "se\\n\\t{\\n}\\u\\n");

	mgr->AddMacro("for",    " (;;)\\n\\t{\\n}\\u\\u");
	mgr->AddMacro("while",  " ()\\n\\t{\\n}\\u\\u\\r\\r");
	mgr->AddMacro("do",     "\\n\\t{\\n}\\nwhile ();\\l\\l");
	mgr->AddMacro("switch", " ()\\n\\t{\\ndefault:\\n\\tbreak;\\n\\b}\\u\\u\\u\\u\\r\\r\\r");
	mgr->AddMacro("try",    "\\n\\t{\\n}\\n\\b"
							"catch (...)\\n\\t{\\n}\\u\\u\\u\\u\\n");

	mgr->AddMacro("&c", "\\b\\b(C)");
	mgr->AddMacro("&r", "\\b\\b(R)");
}

void
CBPrefsManager::AddDefaultEiffelMacros
	(
	CBMacroManager* mgr
	)
	const
{
}

void
CBPrefsManager::AddDefaultFortranMacros
	(
	CBMacroManager* mgr
	)
	const
{
}

void
CBPrefsManager::AddDefaultHTMLMacros
	(
	CBMacroManager* mgr
	)
	const
{
	mgr->AddMacro("<a",   " href=\"\"></a>\\l\\l\\l\\l\\l\\l");
	mgr->AddMacro("<img", " src=\"\" />\\l\\l\\l\\l");

	mgr->AddMacro("<ul",  ">\\n<li></li>\\n</ul>\\u\\l");
	mgr->AddMacro("<ol",  ">\\n<li></li>\\n</ol>\\u\\l");
	mgr->AddMacro("<dl",  ">\\n<dt></dt>\\n<dd></dd>\\n</dl>\\u\\u\\l");

	mgr->AddMacro("<!--", "  -->\\l\\l\\l\\l");

	// PHP

	mgr->AddMacro("<?",   "  ?>\\l\\l\\l");
	mgr->AddMacro("<?=",  "  ?>\\l\\l\\l");

	// JSP

	mgr->AddMacro("<%",   "  %>\\l\\l\\l");
	mgr->AddMacro("<%=",  "  %>\\l\\l\\l");
	mgr->AddMacro("<%@",  "  %>\\l\\l\\l");
	mgr->AddMacro("<%!",  "  %>\\l\\l\\l");
	mgr->AddMacro("<%--", "  --%>\\l\\l\\l\\l\\l");
}

void
CBPrefsManager::AddDefaultJavaMacros
	(
	CBMacroManager* mgr
	)
	const
{
	mgr->AddMacro("/*",   "  */\\l\\l\\l");
	mgr->AddMacro("/**",  "  */\\l\\l\\l");
	mgr->AddMacro("#\"",  "\\binclude \"\"\\l");
	mgr->AddMacro("#<",   "\\binclude <>\\l");

	mgr->AddMacro("if",   " ()\\n\\t{\\n}\\u\\u\\l");
	mgr->AddMacro("ei",   "\\blse if ()\\n\\t{\\n}\\u\\u\\r\\r\\r\\r");
	mgr->AddMacro("el",   "se\\n\\t{\\n}\\u\\n");

	mgr->AddMacro("for",    " (;;)\\n\\t{\\n}\\u\\u");
	mgr->AddMacro("while",  " ()\\n\\t{\\n}\\u\\u\\r\\r");
	mgr->AddMacro("do",     "\\n\\t{\\n}\\nwhile ();\\l\\l");
	mgr->AddMacro("switch", " ()\\n\\t{\\ndefault:\\n\\tbreak;\\n\\b}\\u\\u\\u\\u\\r\\r\\r");
	mgr->AddMacro("try",    "\\n\\t{\\n}\\n\\b"
							"catch (Exception e)\\n\\t{\\n}\\n\\b"
							"final\\n\\t{\\n}\\u\\u\\u\\u\\u\\u\\u\\n");

	mgr->AddMacro("&c", "\\b\\b(C)");
	mgr->AddMacro("&r", "\\b\\b(R)");
}

void
CBPrefsManager::AddDefaultCSharpMacros
	(
	CBMacroManager* mgr
	)
	const
{
	mgr->AddMacro("/*",  "  */\\l\\l\\l");
	mgr->AddMacro("/**", "  */\\l\\l\\l");
	mgr->AddMacro("#\"", "\\binclude \"\"\\l");
	mgr->AddMacro("#<",  "\\binclude <>\\l");

	mgr->AddMacro("if",   " ()\\n\\t{\\n}\\u\\u\\l");
	mgr->AddMacro("ei",   "\\blse if ()\\n\\t{\\n}\\u\\u\\r\\r\\r\\r");
	mgr->AddMacro("el",   "se\\n\\t{\\n}\\u\\n");

	mgr->AddMacro("for",    " (;;)\\n\\t{\\n}\\u\\u");
	mgr->AddMacro("while",  " ()\\n\\t{\\n}\\u\\u\\r\\r");
	mgr->AddMacro("do",     "\\n\\t{\\n}\\nwhile ();\\l\\l");
	mgr->AddMacro("foreach"," ( in )\\n\\t{\\n}\\u\\u\\r\\r\\r\\r");
	mgr->AddMacro("switch", " ()\\n\\t{\\ndefault:\\n\\tbreak;\\n\\b}\\u\\u\\u\\u\\r\\r\\r");
	mgr->AddMacro("try",    "\\n\\t{\\n}\\n\\b"
							"catch (...)\\n\\t{\\n}\\u\\u\\u\\u\\n");

	mgr->AddMacro("&c", "\\b\\b(C)");
	mgr->AddMacro("&r", "\\b\\b(R)");
}

void
CBPrefsManager::AddDefaultXMLMacros
	(
	CBMacroManager* mgr
	)
	const
{
	mgr->AddMacro("<!--", "  -->\\l\\l\\l\\l");
}

void
CBPrefsManager::AddDefaultXMLActions
	(
	CBCharActionManager* mgr
	)
	const
{
	mgr->SetAction('>',  JString("$(xml-auto-close $t)", kJFalse));
}

/******************************************************************************
 InitComplementSuffix (private)

 ******************************************************************************/

static const JUtf8Byte* kInitCSourceSuffix[] =
{
	".c", ".cc", ".cpp", ".cxx", ".C", ".tmpl"
};

const JSize kInitCSourceSuffixCount = sizeof(kInitCSourceSuffix) / sizeof(JUtf8Byte*);

static const JUtf8Byte* kInitCHeaderSuffix[] =
{
	".h", ".hh", ".hpp", ".hxx", ".H", ".h"
};

const JSize kInitCHeaderSuffixCount = sizeof(kInitCHeaderSuffix) / sizeof(JUtf8Byte*);

void
CBPrefsManager::InitComplementSuffix
	(
	const JString&	suffix,
	JString*		complSuffix
	)
	const
{
	assert( kInitCSourceSuffixCount == kInitCHeaderSuffixCount );

	for (JUnsignedOffset i=0; i<kInitCSourceSuffixCount; i++)
		{
		if (suffix == kInitCSourceSuffix[i])
			{
			*complSuffix = kInitCHeaderSuffix[i];
			break;
			}
		if (suffix == kInitCHeaderSuffix[i])
			{
			*complSuffix = kInitCSourceSuffix[i];
			break;
			}
		}
}

/******************************************************************************
 FindMacroName (static private)

	Returns the id of the macro set with the given name.  Can create a new
	one if the name doesn't exist.

 ******************************************************************************/

JIndex
CBPrefsManager::FindMacroName
	(
	const JUtf8Byte*		macroName,
	JArray<MacroSetInfo>*	macroList,
	const JBoolean			create
	)
{
	if (JString::IsEmpty(macroName))
		{
		return kCBEmptyMacroID;
		}

	// search for macroName

	JIndex maxID = kCBEmptyMacroID;

	const JSize macroCount = macroList->GetElementCount();
	for (JIndex i=1; i<=macroCount; i++)
		{
		const MacroSetInfo macroInfo = macroList->GetElement(i);
		maxID = JMax(maxID, macroInfo.id);
		if (JString::Compare(macroName, *(macroInfo.name), kJFalse) == 0)
			{
			return macroInfo.id;
			}
		}

	if (!create)
		{
		return kCBEmptyMacroID;
		}

	// create a new macro set

	const JIndex macroID = maxID+1;

	JString* name = jnew JString(macroName);
	assert( name != nullptr );

	CBCharActionManager* actionMgr = jnew CBCharActionManager;
	assert( actionMgr != nullptr );

	CBMacroManager* macroMgr = jnew CBMacroManager;
	assert( macroMgr != nullptr );

	macroList->InsertSorted(
		MacroSetInfo(macroID, name, actionMgr, macroMgr));

	return macroID;
}

/******************************************************************************
 FindCRMRuleListName (static private)

	Returns the id of the CRM rule list with the given name.

 ******************************************************************************/

JIndex
CBPrefsManager::FindCRMRuleListName
	(
	const JUtf8Byte*				crmName,
	const JArray<CRMRuleListInfo>&	crmList
	)
{
	if (JString::IsEmpty(crmName))
		{
		return kCBEmptyCRMRuleListID;
		}

	const JSize crmCount = crmList.GetElementCount();
	for (JIndex i=1; i<=crmCount; i++)
		{
		const CRMRuleListInfo crmInfo = crmList.GetElement(i);
		if (JString::Compare(crmName, *(crmInfo.name), kJFalse) == 0)
			{
			return crmInfo.id;
			}
		}

	return kCBEmptyCRMRuleListID;
}

/******************************************************************************
 ConvertHTMLSuffixesToFileTypes (private)

 ******************************************************************************/

void
CBPrefsManager::ConvertHTMLSuffixesToFileTypes
	(
	const std::string& data
	)
{
	std::istringstream dataStream(data);
	JSize count;
	dataStream >> count;
	JString suffix;
	for (JIndex i=1; i<=count; i++)
		{
		JBoolean found = kJFalse;
		dataStream >> suffix;

		const JSize ftCount = itsFileTypeList->GetElementCount();
		for (JIndex j=1; j<=ftCount; j++)
			{
			const FileTypeInfo info = itsFileTypeList->GetElement(j);
			if (*(info.suffix) == suffix)
				{
				found = kJTrue;
				break;
				}
			}

		if (!found)
			{
			#ifdef _J_OSX
			const JUtf8Byte* cmd = "open $f";
			#else
			const JUtf8Byte* cmd = "eog $f";
			#endif

			FileTypeInfo info(jnew JString(suffix), nullptr, nullptr, kCBExternalFT,
							  kCBEmptyMacroID, kCBEmptyCRMRuleListID,
							  kJTrue, nullptr, kJTrue, jnew JString, jnew JString(cmd));
			assert( info.suffix != nullptr && info.complSuffix != nullptr && info.editCmd != nullptr );
			itsFileTypeList->InsertSorted(info);
			}
		}
}

/******************************************************************************
 CreateFileTypeList (private)

 ******************************************************************************/

JArray<CBPrefsManager::FileTypeInfo>*
CBPrefsManager::CreateFileTypeList()
{
	JArray<FileTypeInfo>* list = jnew JArray<FileTypeInfo>;
	assert( list != nullptr );
	list->SetSortOrder(JListT::kSortAscending);
	list->SetCompareFunction(CompareFileTypeSpecAndLength);
	return list;
}

/******************************************************************************
 ReadFileTypeInfo (private)

 ******************************************************************************/

void
CBPrefsManager::ReadFileTypeInfo
	(
	const JFileVersion vers
	)
{
	assert( itsFileTypeList == nullptr );

	itsFileTypeList = CreateFileTypeList();

	std::string listData;
	JBoolean ok = GetData(kCBFileTypeListID, &listData);
	assert( ok );
	std::istringstream listStream(listData);

	JSize count;
	listStream >> count;

	for (JIndex i=1; i<=count; i++)
		{
		FileTypeInfo info;

		info.suffix = jnew JString;
		assert( info.suffix != nullptr );

		info.complSuffix = jnew JString;
		assert( info.complSuffix != nullptr );

		listStream >> *(info.suffix) >> info.type;
		listStream >> info.macroID >> JBoolFromString(info.wordWrap);

		if (vers >= 13)
			{
			listStream >> info.crmID >> *(info.complSuffix);
			}
		else
			{
			info.crmID = kCBEmptyCRMRuleListID;	// set by CreateCRMRuleLists()
			InitComplementSuffix(*(info.suffix), info.complSuffix);
			}

		if (vers >= 26)
			{
			JBoolean hasEditCmd;
			listStream >> JBoolFromString(hasEditCmd);
			if (hasEditCmd)
				{
				info.editCmd = jnew JString;
				assert( info.editCmd != nullptr );
				listStream >> *(info.editCmd);
				}
			}

		if (vers >= 40)
			{
			JBoolean hasScriptPath;
			listStream >> JBoolFromString(hasScriptPath);
			if (hasScriptPath)
				{
				info.scriptPath = jnew JString;
				assert( info.scriptPath != nullptr );
				listStream >> *(info.scriptPath);
				}
			}

		info.CreateRegex();
		itsFileTypeList->AppendElement(info);
		}

	std::string wrapData;
	ok = GetData(kCBMiscWordWrapID, &wrapData);
	assert( ok );
	std::istringstream wrapStream(wrapData);
	wrapStream >> JBoolFromString(itsExecOutputWordWrapFlag)
			   >> JBoolFromString(itsUnknownTypeWordWrapFlag);
}

/******************************************************************************
 WriteFileTypeInfo (private)

	Writes out the suffix info.

 ******************************************************************************/

void
CBPrefsManager::WriteFileTypeInfo()
{
	std::ostringstream listStream;

	const JSize count = itsFileTypeList->GetElementCount();
	listStream << count;

	for (JIndex i=1; i<=count; i++)
		{
		FileTypeInfo info = itsFileTypeList->GetElement(i);
		listStream << ' ' << *(info.suffix) << ' ' << info.type;
		listStream << ' ' << info.macroID << ' ' << JBoolToString(info.wordWrap);
		listStream << ' ' << info.crmID << ' ' << *(info.complSuffix);

		if (info.editCmd != nullptr)
			{
			listStream << ' ' << JBoolToString(kJTrue) << ' ' << *(info.editCmd);
			}
		else
			{
			listStream << ' ' << JBoolToString(kJFalse);
			}

		if (info.scriptPath != nullptr)
			{
			listStream << ' ' << JBoolToString(kJTrue) << ' ' << *(info.scriptPath);
			}
		else
			{
			listStream << ' ' << JBoolToString(kJFalse);
			}
		}

	SetData(kCBFileTypeListID, listStream);

	std::ostringstream wrapStream;
	wrapStream << JBoolToString(itsExecOutputWordWrapFlag)
			   << JBoolToString(itsUnknownTypeWordWrapFlag);
	SetData(kCBMiscWordWrapID, wrapStream);
}

/******************************************************************************
 FileTypeInfo::CreateRegex

 ******************************************************************************/

void
CBPrefsManager::FileTypeInfo::CreateRegex()
{
	jdelete nameRegex;
	nameRegex = nullptr;

	jdelete contentRegex;
	contentRegex = nullptr;

	if (suffix->GetFirstCharacter() == kCBContentRegexMarker)
		{
		contentRegex = jnew JRegex(*suffix);
		assert( contentRegex != nullptr );
		contentRegex->SetSingleLine(kJTrue);
		}
	else if (suffix->Contains(kCBNameRegexMarker))
		{
		JString s;
		const JBoolean ok = JDirInfo::BuildRegexFromWildcardFilter(*suffix, &s);
		assert( ok );

		nameRegex = jnew JRegex(s);
		assert( nameRegex != nullptr );
		}
}

/******************************************************************************
 FileTypeInfo::IsPlainSuffix

 ******************************************************************************/

JBoolean
CBPrefsManager::FileTypeInfo::IsPlainSuffix()
	const
{
	return JI2B(nameRegex == nullptr && contentRegex == nullptr);
}

/******************************************************************************
 FileTypeInfo::Free

 ******************************************************************************/

void
CBPrefsManager::FileTypeInfo::Free()
{
	jdelete suffix;
	suffix = nullptr;

	jdelete nameRegex;
	nameRegex = nullptr;

	jdelete contentRegex;
	contentRegex = nullptr;

	jdelete scriptPath;
	scriptPath = nullptr;

	jdelete complSuffix;
	complSuffix = nullptr;

	jdelete editCmd;
	editCmd = nullptr;
}

/******************************************************************************
 CompareFileTypeSpec (static)

 ******************************************************************************/

JListT::CompareResult
CBPrefsManager::CompareFileTypeSpec
	(
	const FileTypeInfo& i1,
	const FileTypeInfo& i2
	)
{
	const JUtf8Character c1 = (i1.suffix)->GetFirstCharacter();
	const JUtf8Character c2 = (i2.suffix)->GetFirstCharacter();

	if (c1 == kCBContentRegexMarker && c2 != kCBContentRegexMarker)
		{
		return JListT::kFirstLessSecond;
		}
	else if (c1 != kCBContentRegexMarker && c2 == kCBContentRegexMarker)
		{
		return JListT::kFirstGreaterSecond;
		}
	else
		{
		return JCompareStringsCaseInsensitive(i1.suffix, i2.suffix);
		}
}

/******************************************************************************
 CompareFileTypeSpecAndLength (static private)

	We check the regexes first because we assume the content or a full name
	match is more reliable than the suffix.

 ******************************************************************************/

JListT::CompareResult
CBPrefsManager::CompareFileTypeSpecAndLength
	(
	const FileTypeInfo& i1,
	const FileTypeInfo& i2
	)
{
	const JUtf8Character c1 = i1.suffix->GetFirstCharacter();
	const JUtf8Character c2 = i2.suffix->GetFirstCharacter();

	if (c1 == kCBContentRegexMarker && c2 != kCBContentRegexMarker)
		{
		return JListT::kFirstLessSecond;
		}
	else if (c1 != kCBContentRegexMarker && c2 == kCBContentRegexMarker)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (i1.suffix->Contains(kCBNameRegexMarker) &&
			 !i2.suffix->Contains(kCBNameRegexMarker))
		{
		return JListT::kFirstLessSecond;
		}
	else if (!i1.suffix->Contains(kCBNameRegexMarker) &&
			 i2.suffix->Contains(kCBNameRegexMarker))
		{
		return JListT::kFirstGreaterSecond;
		}
	else
		{
		return JCompareStringsCaseInsensitive(i1.suffix, i2.suffix);
		}
}

/******************************************************************************
 Read/Write/Edit macro sets

 ******************************************************************************/

#define DataType      MacroSetInfo
#define ListVar       itsMacroList
#define CreateListFn  CreateMacroList
#define CompareNameFn CompareMacroNames
#define MainDataID    kCBMacroSetListID
#define EmptyDataID   kCBEmptyMacroID
#define FirstDataID   kCBFirstMacroID
#define FindIDFn      FindMacroID
#define FTStructIDVar macroID
#define EditDataFn    EditMacros
#define EditDataArg   CBMacroManager
#define EditDataSel   macro
#define UpdateDataFn  UpdateMacros
#define DialogClass   CBEditMacroDialog
#define DialogVar     itsMacroDialog
#define ExtractDataFn GetMacroList
#define CopyConstr    jnew CBCharActionManager(*(origInfo.action)), \
					  jnew CBMacroManager(*(origInfo.macro))
#define PtrCheck      newInfo.action != nullptr && \
					  newInfo.macro  != nullptr
#define Destr         jdelete info.action; \
					  jdelete info.macro
#include "CBPrefsManagerData.th"
#undef DataType
#undef ListVar
#undef CreateListFn
#undef CompareNameFn
#undef MainDataID
#undef EmptyDataID
#undef FirstDataID
#undef FindIDFn
#undef FTStructIDVar
#undef EditDataFn
#undef EditDataArg
#undef EditDataSel
#undef UpdateDataFn
#undef DialogClass
#undef DialogVar
#undef ExtractDataFn
#undef CopyConstr
#undef PtrCheck
#undef Destr

/******************************************************************************
 MacroSetInfo::CreateAndRead

 ******************************************************************************/

void
CBPrefsManager::MacroSetInfo::CreateAndRead
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	action = jnew CBCharActionManager;
	assert( action != nullptr );
	action->ReadSetup(input);

	macro = jnew CBMacroManager;
	assert( macro != nullptr );
	macro->ReadSetup(input);
}

/******************************************************************************
 MacroSetInfo::Write

 ******************************************************************************/

void
CBPrefsManager::MacroSetInfo::Write
	(
	std::ostream& output
	)
{
	action->WriteSetup(output);
	output << ' ';
	macro->WriteSetup(output);
}

/******************************************************************************
 MacroSetInfo::Free

 ******************************************************************************/

void
CBPrefsManager::MacroSetInfo::Free()
{
	jdelete name;
	jdelete action;
	jdelete macro;
}

/******************************************************************************
 CreateCRMRuleLists (private)

	Create default CRM rule lists and assign to file types.  Also create
	default content file types.

 ******************************************************************************/

// CRM rule lists

const JSize kMaxInitCRMRuleCount = 4;

struct InitCRMInfo
{
	const JUtf8Byte*	name;
	JSize				count;
	const JUtf8Byte*	first   [ kMaxInitCRMRuleCount ];
	const JUtf8Byte*	rest    [ kMaxInitCRMRuleCount ];
	const JUtf8Byte*	replace [ kMaxInitCRMRuleCount ];
};

static const InitCRMInfo kInitCRM[] =
{
	{ "ASP", 1,
	  { "[[:space:]]*('+[[:space:]]*)+" },
	  { "[[:space:]]*('+[[:space:]]*)+" },
	  { "$0"                            }
	},

	{ "Beta", 2,
	  { "([[:space:]]*)\\(((\\*+[[:space:]]*)+)", "[[:space:]]*(\\*+[[:space:]]*)+",    },
	  { "[[:space:]]*(\\*+\\)?[[:space:]]*)+",    "[[:space:]]*(\\*+\\)?[[:space:]]*)+" },
	  { "$1 $2",                                  "$0",                                 }
	},

	{ "C", 2,
	  { "([[:space:]]*)/((\\*+[[:space:]]*)+)", "[[:space:]]*(\\*+[[:space:]]*)+",  },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",    "[[:space:]]*(\\*+/?[[:space:]]*)+" },
	  { "$1 *",                                 "$0",                               }
	},

	{ "C++", 3,
	  { "([[:space:]]*)/((\\*+[[:space:]]*)+)", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",    "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*" },
	  { "$1 *",                                 "$0",                                "$0"                         }
	},

	{ "C#", 3,
	  { "([[:space:]]*)/((\\*+[[:space:]]*)+)", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*///?[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",    "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*///?[[:space:]]*" },
	  { "$1 *",                                 "$0",                                "$0"                           }
	},

	{ "Eiffel", 1,
	  { "[[:space:]]*--[[:space:]]*" },
	  { "[[:space:]]*--[[:space:]]*" },
	  { "$0"                         }
	},

	{ "E-mail", 1,
	  { "[[:space:]]*(>+[[:space:]]*)+" },
	  { "[[:space:]]*(>+[[:space:]]*)+" },
	  { "$0"                            }
	},

	{ "FORTRAN", 1,
	  { "[Cc][[:space:]]*" },
	  { "[Cc][[:space:]]*" },
	  { "$0"               }
	},

	{ "INI", 1,
	  { "[[:space:]]*(;+[[:space:]]*)+" },
	  { "[[:space:]]*(;+[[:space:]]*)+" },
	  { "$0"                            }
	},

	{ "Java", 3,
	  { "([[:space:]]*)/((\\*+[[:space:]]*)+)", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",    "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*" },
	  { "$1 *",                                 "$0",                                "$0"                         }
	},

	{ "JSP", 3,
	  { "([[:space:]]*)/((\\*+[[:space:]]*)+)", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",    "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*" },
	  { "$1 *",                                 "$0",                                "$0"                         }
	},

	{ "Lisp", 1,
	  { "[[:space:]]*;(;+[[:space:]]*)+" },
	  { "[[:space:]]*;(;+[[:space:]]*)+" },
	  { "$0"                             }
	},

	{ "Lua", 1,
	  { "[[:space:]]*--[[:space:]]*" },
	  { "[[:space:]]*--[[:space:]]*" },
	  { "$0"                         }
	},

	{ "Outline", 2,
	  { "(\\t*)\\*(\\t*)", "([[:space:]]*)\\*([[:space:]]*)" },
	  { "[[:space:]]*",    "[[:space:]]*"                    },
	  { "$1$2",            "$1 $2"                           }
	},

	{ "PHP", 4,
	  { "([[:space:]]*)/((\\*+[[:space:]]*)+)", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",    "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "$1 *",                                 "$0",                                "$0"                        , "$0"                            }
	},

	{ "SQL", 4,
	  { "([[:space:]]*)/((\\*+[[:space:]]*)+)", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*--[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",    "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*--[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "$1 *",                                 "$0",                                "$0",                         "$0"                            }
	},

	{ "UNIX script", 1,
	  { "[[:space:]]*(#+[[:space:]]*)+" },
	  { "[[:space:]]*(#+[[:space:]]*)+" },
	  { "$0"                            }
	},

	{ "Vera", 1,
	  { "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*//[[:space:]]*" },
	  { "$0"                         }
	},
};

const JSize kInitCRMCount = sizeof(kInitCRM)/sizeof(InitCRMInfo);

// regex file types

struct FTRegexInfo
{
	const JUtf8Byte*	pattern;
	CBTextFileType		type;
	const JUtf8Byte*	macroName;
	const JUtf8Byte*	crmName;
	JBoolean			wrap;
};

static const FTRegexInfo kFTRegexInfo[] =
{
	// AWK
	{ "^#![[:space:]]*/[^[:space:]]+/awk([[:space:]]|$)",
	  kCBAWKFT, "AWK", "UNIX script", kJFalse },

	// Perl
	{ "^#![[:space:]]*/[^[:space:]]+/perl[-_]?([0-9.]*)([[:space:]]|$)",
	  kCBPerlFT, "Perl", "UNIX script", kJFalse },

	// Python
	{ "^#![[:space:]]*/[^[:space:]]+/python([[:space:]]|$)",
	  kCBPythonFT, "Python", "UNIX script", kJFalse },

	// Ruby
	{ "^#![[:space:]]*/[^[:space:]]+/ruby([[:space:]]|$)",
	  kCBRubyFT, "Ruby", "UNIX script", kJFalse },

	// Shell
	{ "^#![[:space:]]*/[^[:space:]]+/(ba|k|z)?sh([[:space:]]|$)",
	  kCBBourneShellFT, "Bourne shell", "UNIX script", kJFalse },

	// C shell
	{ "^#![[:space:]]*/[^[:space:]]+/t?csh([[:space:]]|$)",
	  kCBCShellFT, "C shell", "UNIX script", kJFalse },

	// TCL
	{ "^#![[:space:]]*/[^[:space:]]+/tcl([[:space:]]|$)",
	  kCBTCLFT, "TCL", "UNIX script", kJFalse },

	// TCL/Tk
	{ "^#![[:space:]]*/[^[:space:]]+/wish([[:space:]]|$)",
	  kCBTCLFT, "TCL", "UNIX script", kJFalse },

	// XPM source
	{ "^/\\* XPM \\*/(\\n|$)",
	  kCBCHeaderFT, "C/C++", "C", kJFalse },

	// email
	{ "^From[[:space:]]+.*.{3}[[:space:]]+.{3}[[:space:]]+[[:digit:]]+"
	  "[[:space:]]+([[:digit:]]+:)+[[:digit:]]+[[:space:]]+(...[[:space:]])?"
	  "[[:digit:]]{4}",
	  kCBUnknownFT, nullptr, "E-mail", kJTrue }
};

const JSize kFTRegexCount = sizeof(kFTRegexInfo)/sizeof(FTRegexInfo);

void
CBPrefsManager::CreateCRMRuleLists()
{
	assert( itsCRMList == nullptr );

	// create CRM rule lists

	itsCRMList = CreateCRMList();

	for (JUnsignedOffset i=0; i<kInitCRMCount; i++)
		{
		JString* name = jnew JString(kInitCRM[i].name);
		assert( name != nullptr );

		JStyledText::CRMRuleList* ruleList = jnew JStyledText::CRMRuleList;
		assert( ruleList != nullptr );

		for (JUnsignedOffset j=0; j<kInitCRM[i].count; j++)
			{
			ruleList->AppendElement(JStyledText::CRMRule(
				JString(kInitCRM[i].first[j],   kJFalse),
				JString(kInitCRM[i].rest[j],    kJFalse),
				JString(kInitCRM[i].replace[j], kJFalse)));
			}

		itsCRMList->AppendElement(
			CRMRuleListInfo(kCBFirstCRMRuleListID+i, name, ruleList));
		}

	// set CRM rule list for each original suffix

	const JSize ftCount = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=ftCount; i++)
		{
		FileTypeInfo info = itsFileTypeList->GetElement(i);
		if (*(info.suffix) == ".c" ||
			*(info.suffix) == ".l" ||
			*(info.suffix) == ".y")
			{
			info.crmID = FindCRMRuleListName("C", *itsCRMList);
			}
		else if (info.type == kCBCSourceFT || info.type == kCBCHeaderFT)
			{
			info.crmID = FindCRMRuleListName("C++", *itsCRMList);
			}
		itsFileTypeList->SetElement(i, info);
		}

	// add content regex file types

	for (JUnsignedOffset i=0; i<kFTRegexCount; i++)
		{
		JString* suffix = jnew JString(kFTRegexInfo[i].pattern);
		assert( suffix != nullptr );

		JString* complSuffix = jnew JString;
		assert( complSuffix != nullptr );

		const JIndex macroID = FindMacroName(kFTRegexInfo[i].macroName, itsMacroList, kJTrue);
		const JIndex crmID   = FindCRMRuleListName(kFTRegexInfo[i].crmName, *itsCRMList);

		FileTypeInfo info(suffix, nullptr, nullptr, kFTRegexInfo[i].type, macroID, crmID,
						  kJTrue, nullptr, kFTRegexInfo[i].wrap, complSuffix, nullptr);
		info.CreateRegex();
		assert( info.contentRegex != nullptr );
		itsFileTypeList->InsertSorted(info);
		}
}

/******************************************************************************
 Read/Write/Edit CRM rule lists

 ******************************************************************************/

#define DataType      CRMRuleListInfo
#define ListVar       itsCRMList
#define CreateListFn  CreateCRMList
#define CompareNameFn CompareCRMNames
#define MainDataID    kCBCRMRuleSetListID
#define EmptyDataID   kCBEmptyCRMRuleListID
#define FirstDataID   kCBFirstCRMRuleListID
#define FindIDFn      FindCRMRuleListID
#define FTStructIDVar crmID
#define EditDataFn    EditCRMRuleLists
#define EditDataArg   JStyledText::CRMRuleList
#define EditDataSel   list
#define UpdateDataFn  UpdateCRMRuleLists
#define DialogClass   CBEditCRMDialog
#define DialogVar     itsCRMDialog
#define ExtractDataFn GetCRMRuleLists
#define CopyConstr    jnew JStyledText::CRMRuleList(*(origInfo.list))
#define PtrCheck      newInfo.list != nullptr
#define Destr         (info.list)->DeleteAll(); \
					  jdelete info.list
#include "CBPrefsManagerData.th"
#undef DataType
#undef ListVar
#undef CreateListFn
#undef CompareNameFn
#undef MainDataID
#undef EmptyDataID
#undef FirstDataID
#undef FindIDFn
#undef FTStructIDVar
#undef EditDataFn
#undef EditDataArg
#undef EditDataSel
#undef UpdateDataFn
#undef DialogClass
#undef DialogVar
#undef ExtractDataFn
#undef CopyConstr
#undef PtrCheck
#undef Destr

/******************************************************************************
 CRMRuleListInfo::CreateAndRead

 ******************************************************************************/

void
CBPrefsManager::CRMRuleListInfo::CreateAndRead
	(
	std::istream&			input,
	const JFileVersion	vers
	)
{
	list = jnew JStyledText::CRMRuleList;
	assert( list != nullptr );

	JSize ruleCount;
	input >> ruleCount;

	JString first, rest, replace;
	for (JIndex j=1; j<=ruleCount; j++)
		{
		input >> first >> rest >> replace;
		list->AppendElement(JStyledText::CRMRule(first, rest, replace));
		}
}

/******************************************************************************
 CRMRuleListInfo::Write

 ******************************************************************************/

void
CBPrefsManager::CRMRuleListInfo::Write
	(
	std::ostream& output
	)
{
	const JSize ruleCount = list->GetElementCount();
	output << ruleCount;

	for (JIndex i=1; i<=ruleCount; i++)
		{
		JStyledText::CRMRule r = list->GetElement(i);
		output << ' ' << (r.first)->GetPattern();
		output << ' ' << (r.rest)->GetPattern();
		output << ' ' << (r.first)->GetReplacePattern();
		}
}

/******************************************************************************
 CRMRuleListInfo::Free

 ******************************************************************************/

void
CBPrefsManager::CRMRuleListInfo::Free()
{
	jdelete name;

	list->DeleteAll();
	jdelete list;
}

/******************************************************************************
 Colors

 ******************************************************************************/

JColorID
CBPrefsManager::GetColor
	(
	const JIndex index
	)
	const
{
	assert( ColorIndexValid(index) );
	return itsColor [ index-1 ];
}

void
CBPrefsManager::SetColor
	(
	const JIndex		index,
	const JColorID	color
	)
{
	assert( ColorIndexValid(index) );

	if (color != itsColor [ index-1 ])
		{
		itsColor [ index-1 ] = color;

		if (index == kTextColorIndex)
			{
			Broadcast(TextColorChanged());
			}
		}
}

// private

void
CBPrefsManager::ReadColors()
{
	JBoolean ok[ kColorCount ];

	std::string data;
	if (GetData(kCBTextColorID, &data))
		{
		std::istringstream dataStream(data);

		JFileVersion vers;
		dataStream >> vers;
		assert( vers <= kCurrentTextColorVers );

		JRGB color[ kColorCount ];
		for (JUnsignedOffset i=0; i<kColorCount; i++)
			{
			if (vers == 0 && i == kRightMarginColorIndex-1)
				{
				ok[i] = kJFalse;
				}
			else
				{
				dataStream >> color[i];
				itsColor[i] = JColorManager::GetColorID(color[i]);
				ok[i]       = kJTrue;
				}
			}
		}
	else
		{
		for (JUnsignedOffset i=0; i<kColorCount; i++)
			{
			ok[i] = kJFalse;
			}
		}

	// set unallocated colors to the default values

	const JColorID defaultColor[] =
		{
		JColorManager::GetBlackColor(),
		JColorManager::GetWhiteColor(),
		JColorManager::GetRedColor(),
		JColorManager::GetDefaultSelectionColor(),
		JColorManager::GetBlueColor(),
		JColorManager::GetGrayColor(70)
		};
	assert( sizeof(defaultColor)/sizeof(JColorID) == kColorCount );

	for (JUnsignedOffset i=0; i<kColorCount; i++)
		{
		if (!ok[i])
			{
			itsColor[i] = defaultColor[i];
			}
		}
}

void
CBPrefsManager::WriteColors()
{
	std::ostringstream data;
	data << kCurrentTextColorVers;

	for (JUnsignedOffset i=0; i<kColorCount; i++)
		{
		data << ' ' << JColorManager::GetRGB(itsColor[i]);
		}

	SetData(kCBTextColorID, data);
}

/******************************************************************************
 Default font

 ******************************************************************************/

void
CBPrefsManager::GetDefaultFont
	(
	JString*	name,
	JSize*		size
	)
	const
{
	std::string data;
	const JBoolean ok = GetData(kCBDefFontID, &data);
	assert( ok );

	std::istringstream dataStream(data);
	dataStream >> *name >> *size;
}

void
CBPrefsManager::SetDefaultFont
	(
	const JString&	name,
	const JSize		size
	)
{
	std::ostringstream data;
	data << JString(name) << ' ' << size;
	SetData(kCBDefFontID, data);
}

/******************************************************************************
 Emulator

 ******************************************************************************/

CBEmulator
CBPrefsManager::GetEmulator()
	const
{
	std::string data;
	if (!GetData(kCBEmulatorID, &data))
		{
		return kCBNoEmulator;
		}

	std::istringstream dataStream(data);

	CBEmulator type;
	dataStream >> type;
	return type;
}

void
CBPrefsManager::SetEmulator
	(
	const CBEmulator type
	)
{
	std::ostringstream data;
	data << type;
	SetData(kCBEmulatorID, data);
}

/******************************************************************************
 Global text settings

 ******************************************************************************/

void
CBPrefsManager::ReadStaticGlobalPrefs
	(
	const JFileVersion vers
	)
	const
{
	std::string data;
	if (GetData(kCBStaticGlobalID, &data))
		{
		std::istringstream dataStream(data);
		CBTextDocument::ReadStaticGlobalPrefs(dataStream, vers);
		CBProjectDocument::ReadStaticGlobalPrefs(dataStream, vers);
		}
}

void
CBPrefsManager::WriteStaticGlobalPrefs()
{
	std::ostringstream data;
	CBTextDocument::WriteStaticGlobalPrefs(data);
	data << ' ';
	CBProjectDocument::WriteStaticGlobalPrefs(data);

	SetData(kCBStaticGlobalID, data);
}

/******************************************************************************
 Program state

	RestoreProgramState() returns kJTrue if there was any state to restore.

	ForgetProgramState() is required because we have to save state before
	anything is closed.  If the close fails, we shouldn't save the state.

 ******************************************************************************/

JBoolean
CBPrefsManager::RestoreProgramState()
{
	std::string data;
	if (GetData(kCBDocMgrStateID, &data))
		{
		const JBoolean saveAutoDock = JXWindow::WillAutoDockNewWindows();
		JXWindow::ShouldAutoDockNewWindows(kJFalse);

		std::istringstream dataStream(data);
		const JBoolean restored =
			(CBGetDocumentManager())->RestoreState(dataStream);
		RemoveData(kCBDocMgrStateID);

		JXWindow::ShouldAutoDockNewWindows(saveAutoDock);
		return restored;
		}
	else
		{
		return kJFalse;
		}
}

void
CBPrefsManager::SaveProgramState()
{
	CBMDIServer* mdi;
	if (!CBGetMDIServer(&mdi))
		{
		return;
		}

	std::ostringstream data;
	if ((CBGetDocumentManager())->SaveState(data))
		{
		SetData(kCBDocMgrStateID, data);
		}
	else
		{
		RemoveData(kCBDocMgrStateID);
		}
}

void
CBPrefsManager::ForgetProgramState()
{
	RemoveData(kCBDocMgrStateID);
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

JBoolean
CBPrefsManager::GetWindowSize
	(
	const JPrefID&	id,
	JPoint*			desktopLoc,
	JCoordinate*	width,
	JCoordinate*	height
	)
	const
{
	std::string data;
	if (GetData(id, &data))
		{
		std::istringstream dataStream(data);
		dataStream >> *desktopLoc >> *width >> *height;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SaveWindowSize

 ******************************************************************************/

void
CBPrefsManager::SaveWindowSize
	(
	const JPrefID&	id,
	JXWindow*		window
	)
{
	std::ostringstream data;
	data << window->GetDesktopLocation();
	data << ' ' << window->GetFrameWidth();
	data << ' ' << window->GetFrameHeight();

	SetData(id, data);

	// When the first editor window size is saved, set it for all of them.

	const JIndex i = id.GetID();
	if (i == kCBSourceWindSizeID ||
		i == kCBHeaderWindSizeID ||
		i == kCBOtherTextWindSizeID)
		{
		if (!IDValid(kCBSourceWindSizeID))
			{
			SetData(kCBSourceWindSizeID, data);
			}
		if (!IDValid(kCBHeaderWindSizeID))
			{
			SetData(kCBHeaderWindSizeID, data);
			}
		if (!IDValid(kCBOtherTextWindSizeID))
			{
			SetData(kCBOtherTextWindSizeID, data);
			}
		if (!IDValid(kCBExecOutputWindSizeID))
			{
			SetData(kCBExecOutputWindSizeID, data);
			}
		if (!IDValid(kCBSearchOutputWindSizeID))
			{
			SetData(kCBSearchOutputWindSizeID, data);
			}
		}
}

/******************************************************************************
 EditWithOtherProgram

	Returns kJTrue if the file should be opened via another program.
	This only checks the file suffix types since the file isn't open.

 ******************************************************************************/

JBoolean
CBPrefsManager::EditWithOtherProgram
	(
	const JString&	fileName,
	JString*		cmd
	)
	const
{
	JIndex i;
	if (GetFileType(fileName, &i) == kCBExternalFT)
		{
		const FileTypeInfo info = itsFileTypeList->GetElement(i);
		assert( info.editCmd != nullptr );
		*cmd = *(info.editCmd);
		if (cmd->IsEmpty())
			{
			return kJFalse;
			}

		const JUtf8Byte* map[] =
			{
			"f", fileName.GetBytes()
			};
		(JGetStringManager())->Replace(cmd, map, sizeof(map));
		return kJTrue;
		}
	else
		{
		cmd->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 GetFileType

	This only checks the file suffix types since the file isn't open.

 ******************************************************************************/

CBTextFileType
CBPrefsManager::GetFileType
	(
	const JString& fileName
	)
	const
{
	JIndex i;
	return GetFileType(fileName, &i);
}

// private

CBTextFileType
CBPrefsManager::GetFileType
	(
	const JString&	fileName,
	JIndex*			index
	)
	const
{
	const JString name = CleanFileName(fileName);

	const JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		FileTypeInfo info = itsFileTypeList->GetElement(i);
		if ((info.nameRegex != nullptr && info.nameRegex->Match(name)) ||
			(info.IsPlainSuffix() && name.EndsWith(*(info.suffix))))
			{
			*index = i;
			return info.type;
			}
		}

	*index = 0;
	return kCBUnknownFT;
}

/******************************************************************************
 GetFileType

	Any or all of action, macro, crm can come back nullptr.

 ******************************************************************************/

CBTextFileType
CBPrefsManager::GetFileType
	(
	const CBTextDocument&		doc,
	CBCharActionManager**		actionMgr,
	CBMacroManager**			macroMgr,
	JStyledText::CRMRuleList**	crmRuleList,
	JString*					scriptPath,
	JBoolean*					wordWrap
	)
	const
{
	*actionMgr   = nullptr;
	*macroMgr    = nullptr;
	*crmRuleList = nullptr;

	scriptPath->Clear();

	const CBTextFileType origType = doc.GetFileType();
	if (origType == kCBExecOutputFT || origType == kCBShellOutputFT)
		{
		*wordWrap = itsExecOutputWordWrapFlag;
		return origType;
		}
	else if (origType == kCBSearchOutputFT)
		{
		*wordWrap = kJTrue;
		return origType;
		}
	else if (origType == kCBManPageFT)
		{
		*wordWrap = kJFalse;
		return origType;
		}
	else if (origType == kCBDiffOutputFT)
		{
		*wordWrap = itsUnknownTypeWordWrapFlag;
		return origType;
		}

	JIndex index;
	if (CalcFileType(doc, &index))
		{
		const FileTypeInfo info = itsFileTypeList->GetElement(index);

		JIndex index;
		if (FindMacroID(*itsMacroList, info.macroID, &index))
			{
			const MacroSetInfo macroInfo = itsMacroList->GetElement(index);
			*actionMgr = macroInfo.action;
			*macroMgr  = macroInfo.macro;
			}

		if (FindCRMRuleListID(*itsCRMList, info.crmID, &index))
			{
			const CRMRuleListInfo crmInfo = itsCRMList->GetElement(index);
			*crmRuleList = crmInfo.list;
			}

		JString sysDir, userDir;
		if (info.scriptPath != nullptr && GetScriptPaths(&sysDir, &userDir))
			{
			*scriptPath =
				JCombinePathAndName(info.isUserScript ? userDir : sysDir,
									*(info.scriptPath));
			}

		*wordWrap = info.wordWrap;
		return info.type;
		}
	else
		{
		*wordWrap = itsUnknownTypeWordWrapFlag;
		return kCBUnknownFT;
		}
}

/******************************************************************************
 CalcFileType (private)

	Returns an index into itsFileTypeList.

 ******************************************************************************/

JBoolean
CBPrefsManager::CalcFileType
	(
	const CBTextDocument&	doc,
	JIndex*					index
	)
	const
{
	const JString fileName       = CleanFileName(doc.GetFileName());
	const JBoolean checkSuffixes = doc.ExistsOnDisk();

	const JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		FileTypeInfo info   = itsFileTypeList->GetElement(i);
		const JString& text = doc.GetTextEditor()->GetText()->GetText();
		if (info.contentRegex != nullptr)
			{
			// JRegex is so slow on a large file (even with the ^ anchor!)
			// that we must avoid running it at all costs.

			if ((info.literalRange).IsNothing())
				{
				info.literalRange = GetLiteralPrefixRange(*(info.suffix));
				itsFileTypeList->SetElement(i, info);
				}

			const JString prefix = info.suffix->GetSubstring(info.literalRange);
			if (text.BeginsWith(prefix) && (info.contentRegex)->Match(text))
				{
				*index = i;
				return kJTrue;
				}
			}
		else if ((info.nameRegex != nullptr && info.nameRegex->Match(fileName)) ||
				 (info.IsPlainSuffix() &&
				  checkSuffixes && fileName.EndsWith(*(info.suffix))))
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 GetLiteralPrefixRange (static)

 ******************************************************************************/

JIndexRange
CBPrefsManager::GetLiteralPrefixRange
	(
	const JString& regexStr
	)
{
	assert( regexStr.GetFirstCharacter() == kCBContentRegexMarker );

	JIndex i = 1;
	while (regexStr[i] != '\0' && regexStr[i] != '\\' &&
		   !JRegex::NeedsBackslashToBeLiteral(regexStr[i]))
		{
		i++;
		}

	if (regexStr[i] == '?')
		{
		i--;
		}

	return JIndexRange(2, i);
}

/******************************************************************************
 GetScriptPaths (static)

 ******************************************************************************/

JBoolean
CBPrefsManager::GetScriptPaths
	(
	JString* sysDir,
	JString* userDir
	)
{
	return JXGetProgramDataDirectories(kScriptDir, sysDir, userDir);
}

/******************************************************************************
 FindMacroID (static)

 ******************************************************************************/

JBoolean
CBPrefsManager::FindMacroID
	(
	const JArray<MacroSetInfo>&	list,
	const JIndex				id,
	JIndex*						index
	)
{
	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const MacroSetInfo info = list.GetElement(i);
		if (info.id == id)
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 FindCRMRuleListID (static)

 ******************************************************************************/

JBoolean
CBPrefsManager::FindCRMRuleListID
	(
	const JArray<CRMRuleListInfo>&	list,
	const JIndex					id,
	JIndex*							index
	)
{
	const JSize count = list.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const CRMRuleListInfo info = list.GetElement(i);
		if (info.id == id)
			{
			*index = i;
			return kJTrue;
			}
		}

	*index = 0;
	return kJFalse;
}

/******************************************************************************
 GetFileSuffixes

 ******************************************************************************/

void
CBPrefsManager::GetFileSuffixes
	(
	const CBTextFileType	type,
	JPtrArray<JString>*		list
	)
	const
{
	list->DeleteAll();

	const JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const FileTypeInfo info = itsFileTypeList->GetElement(i);

		if (info.type == type && info.IsPlainSuffix())
			{
			list->Append(*(info.suffix));
			}
		}
}

/******************************************************************************
 GetAllFileSuffixes

 ******************************************************************************/

void
CBPrefsManager::GetAllFileSuffixes
	(
	JPtrArray<JString>* list
	)
	const
{
	list->DeleteAll();

	const JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const FileTypeInfo info = itsFileTypeList->GetElement(i);

		if (info.IsPlainSuffix())
			{
			list->Append(*(info.suffix));
			}
		}
}

/******************************************************************************
 GetDefaultComplementSuffix

	If the given name ends with a suffix for the given file type, it
	is removed.  Then the default complement file's suffix is appended
	and the result is returned.  *index contains the value that must be
	passed to SetDefaultComplementSuffix().

 ******************************************************************************/

static const CBTextFileType kDefComplSuffixType[] =
{
	kCBCSourceFT,
	kCBCHeaderFT, 
	kCBModula2ModuleFT,
	kCBModula2InterfaceFT,
	kCBModula3ModuleFT,
	kCBModula3InterfaceFT,
	kCBVeraSourceFT,
	kCBVeraHeaderFT
};

static const JUtf8Byte* kDefComplSuffixStr[] =
{
	".h",
	".c", 
	".mi",
	".md",
	".i3",
	".m3",
	".vrh",
	".vr"
};

const JSize kDefComplSuffixCount = sizeof(kDefComplSuffixType) / sizeof(CBTextFileType);

JString
CBPrefsManager::GetDefaultComplementSuffix
	(
	const JString&			name,
	const CBTextFileType	type,
	JIndex*					index
	)
	const
{
	const JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		FileTypeInfo info = itsFileTypeList->GetElement(i);

		if (info.type == type && info.IsPlainSuffix() &&
			name.EndsWith(*info.suffix) &&
			name.GetCharacterCount() > info.suffix->GetCharacterCount())
			{
			for (JUnsignedOffset j=0; j<kDefComplSuffixCount; j++)
				{
				if (info.type == kDefComplSuffixType[j] && info.complSuffix->IsEmpty())
					{
					*info.complSuffix = kDefComplSuffixStr[j];
					itsFileTypeList->SetElement(i, info);
					break;
					}
				}

			JString complName = name.GetSubstring(1, name.GetLength() -
													 (info.suffix)->GetLength());
			complName += *(info.complSuffix);
			*index     = i;
			return complName;
			}
		}

	*index = 0;
	return name;
}

/******************************************************************************
 SetDefaultComplementSuffix

 ******************************************************************************/

void
CBPrefsManager::SetDefaultComplementSuffix
	(
	const JIndex	index,
	const JString&	name
	)
{
	FileTypeInfo info = itsFileTypeList->GetElement(index);

	JString root;
	if (JSplitRootAndSuffix(name, &root, info.complSuffix))
		{
		info.complSuffix->Prepend(JUtf8Character('.'));
		*info.complSuffix = CleanFileName(*info.complSuffix);
		}

	itsFileTypeList->SetElement(index, info);
}

/******************************************************************************
 FileMatchesSuffix (static)

 ******************************************************************************/

JBoolean
CBPrefsManager::FileMatchesSuffix
	(
	const JString&				fileName,
	const JPtrArray<JString>&	suffixList
	)
{
	const JSize count = suffixList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString& suffix = *(suffixList.GetElement(i));
		if (fileName.EndsWith(suffix))
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 CleanFileName (private)

	Removes the path and strips trailing ~ and #

 ******************************************************************************/

JString
CBPrefsManager::CleanFileName
	(
	const JString& name
	)
	const
{
	JString p, n;
	if (!name.IsEmpty())	// might be untitled document
		{
		JSplitPathAndName(name, &p, &n);

		while (n.GetLastCharacter() == '~' ||
			   n.GetLastCharacter() == '#')
			{
			n.RemoveSubstring(n.GetLength(), n.GetLength());
			}
		}

	return n;
}

/******************************************************************************
 Get/SetStringList (private)

 ******************************************************************************/

void
CBPrefsManager::GetStringList
	(
	const JPrefID&		id,
	JPtrArray<JString>*	list
	)
	const
{
	std::string data;
	const JBoolean ok = GetData(id, &data);
	assert( ok );

	std::istringstream dataStream(data);
	dataStream >> *list;
}

void
CBPrefsManager::SetStringList
	(
	const JPrefID&				id,
	const JPtrArray<JString>&	list
	)
{
	std::ostringstream data;
	data << list;
	SetData(id, data);
}
