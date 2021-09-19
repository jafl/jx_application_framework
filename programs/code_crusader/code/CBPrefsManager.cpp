/******************************************************************************
 CBPrefsManager.cpp

	BASE CLASS = JXPrefsManager

	Copyright © 1997-2001 John Lindal.

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
#include <JStringIterator.h>
#include <JRegex.h>
#include <JSubstitute.h>
#include <JDirInfo.h>
#include <jFileUtil.h>
#include <jAssert.h>

static const JUtf8Byte* kScriptDir = "scripts";
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
	bool* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, true)
{
	itsFileTypeList = nullptr;
	itsMacroList    = nullptr;
	itsCRMList      = nullptr;

	itsExecOutputWordWrapFlag  = true;
	itsUnknownTypeWordWrapFlag = true;

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
		itsFileTypeList->GetElement(i).Free();
		}

	jdelete itsFileTypeList;

	count = itsMacroList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsMacroList->GetElement(i).Free();
		}

	jdelete itsMacroList;

	count = itsCRMList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		itsCRMList->GetElement(i).Free();
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

bool
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
		return !input.fail();
		}
	else
		{
		return false;
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
		auto* list = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
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
			!JSameStrings(*origSuffixList.GetElement(i+1), suffixes, JString::kCompareCase));
		}

	Broadcast(msg);

	if (msg.AnyChanged())
		{
		CBMWriteSharedPrefs(true);
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
		const auto* info =
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
		const auto* info =
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
		const auto* info =
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
	bool			found;
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
				newInfo[i].found = true;
				break;
				}
			}
		}

	// install new suffixes

	const JIndex macroID = CBPrefsManager::FindMacroName(macroName, macroList, true);

	const JUtf8Byte* crmName =
		(JString::IsEmpty(origCRMName) ? macroName : origCRMName);

	const JIndex crmID = CBPrefsManager::FindCRMRuleListName(crmName, crmList);

	for (JUnsignedOffset i=0; i<newCount; i++)
		{
		if (!newInfo[i].found)
			{
			auto* suffix = jnew JString(newInfo[i].suffix);
			assert( suffix != nullptr );

			auto* complSuffix = jnew JString;
			assert( complSuffix != nullptr );

			fileTypeList->InsertSorted(
				CBPrefsManager::FileTypeInfo(
					suffix, nullptr, nullptr, type, macroID, crmID,
					true, nullptr, false, complSuffix, nullptr));
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
	bool			found;
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
				newInfo[i].found = true;
				break;
				}
			}
		}

	for (JUnsignedOffset i=0; i<newCount; i++)
		{
		if (!newInfo[i].found)
			{
			auto* suffix = jnew JString(newInfo[i].suffix);
			assert( suffix != nullptr );

			auto* complSuffix = jnew JString;
			assert( complSuffix != nullptr );

			auto* editCmd = jnew JString(newInfo[i].cmd);
			assert( editCmd != nullptr );

			CBPrefsManager::FileTypeInfo info(
				suffix, nullptr, nullptr, kCBExternalFT,
				kCBEmptyMacroID, kCBEmptyCRMRuleListID,
				true, nullptr, false, complSuffix, editCmd);
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
	const bool		isNew,
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
		newData << 0 << ' ' << JBoolToString(true) << ' ' << oldData.c_str();
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

		bool codeWrap = false;
		if (GetData(kCBTextDocID, &data))
			{
			std::istringstream dataStream(data);
			JFileVersion vers = 0;
			dataStream >> vers;
			if (!dataStream.eof() && !dataStream.fail() && vers == 1)
				{
				bool breakCodeCROnly;
				dataStream >> JBoolFromString(breakCodeCROnly);
				codeWrap = !breakCodeCROnly;
				}
			}

		std::ostringstream codeData;
		codeData << JBoolToString(codeWrap);
		SetData(30000, codeData);
		SetData(30001, codeData);
		SetData(30002, codeData);

		std::ostringstream otherData;
		otherData << JBoolToString(true);
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
			{ ".e", false }
		};
		cbAddNewSuffixes("Eiffel",  nullptr, kCBEiffelFT, kEiffelSuffix, sizeof(kEiffelSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kFortranSuffix[] =
		{
			{ ".f",   false },
			{ ".for", false },
			{ ".ftn", false },
			{ ".f77", false },
			{ ".f90", false },
			{ ".f95", false },
			{ ".F",   false },
			{ ".FOR", false },
			{ ".FTN", false },
			{ ".F77", false },
			{ ".F90", false },
			{ ".F95", false }
		};
		cbAddNewSuffixes("FORTRAN", nullptr, kCBFortranFT, kFortranSuffix, sizeof(kFortranSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kJavaSuffix[] =
		{
			{ ".java", false },
			{ ".jws",  false }
		};
		cbAddNewSuffixes("Java", nullptr, kCBJavaSourceFT, kJavaSuffix, sizeof(kJavaSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kStaticLibrarySuffix[] =
		{
			{ ".a", false }
		};
		cbAddNewSuffixes(nullptr, nullptr, kCBStaticLibraryFT, kStaticLibrarySuffix, sizeof(kStaticLibrarySuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kSharedLibrarySuffix[] =
		{
			{ ".so",    false },
			{ ".dylib", false }
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
			{ ".asm", false },
			{ ".s",   false },
			{ ".S",   false }
		};
		cbAddNewSuffixes("Assembly", nullptr, kCBAssemblyFT, kAssemblySuffix, sizeof(kAssemblySuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kPascalSuffix[] =
		{
			{ ".p",   false },
			{ ".pas", false }
		};
		cbAddNewSuffixes("Pascal", nullptr, kCBPascalFT, kPascalSuffix, sizeof(kPascalSuffix),   itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kRatforSuffix[] =
		{
			{ ".r", false }
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
				{ ".gif", "open $f", false },
				{ ".jpg", "open $f", false }
			};
#else
			CBNewExternalSuffixInfo kImageSuffix[] =
			{
				{ ".gif", "eog $f", false },
				{ ".jpg", "eog $f", false }
			};
#endif
			cbAddNewExternalSuffixes(kImageSuffix, sizeof(kImageSuffix)/sizeof(CBNewExternalSuffixInfo), itsFileTypeList);
			}
		}

	if (currentVersion < 28)
		{
		CBNewExternalSuffixInfo kFDesignSuffix[] =
		{
			{ ".fd", "jfdesign $f", false }
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
			{ ".md", false }
		};
		cbAddNewSuffixes("Modula-2", nullptr, kCBModula2ModuleFT, kModula2ModuleSuffix, sizeof(kModula2ModuleSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kModula2InterfaceSuffix[] =
		{
			{ ".mi", false }
		};
		cbAddNewSuffixes("Modula-2", nullptr, kCBModula2InterfaceFT, kModula2InterfaceSuffix, sizeof(kModula2InterfaceSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kModula3ModuleSuffix[] =
		{
			{ ".m3", false }
		};
		cbAddNewSuffixes("Modula-3", nullptr, kCBModula3ModuleFT, kModula3ModuleSuffix, sizeof(kModula3ModuleSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kModula3InterfaceSuffix[] =
		{
			{ ".i3", false }
		};
		cbAddNewSuffixes("Modula-3", nullptr, kCBModula3InterfaceFT, kModula3InterfaceSuffix, sizeof(kModula3InterfaceSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 31)
		{
		CBNewSuffixInfo kAWKSuffix[] =
		{
			{ ".awk", false }
		};
		cbAddNewSuffixes("AWK", "unix script", kCBAWKFT, kAWKSuffix, sizeof(kAWKSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kCobolSuffix[] =
		{
			{ ".cob", false },
			{ ".COB", false }
		};
		cbAddNewSuffixes("Cobol", nullptr, kCBCobolFT, kCobolSuffix, sizeof(kCobolSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kLispSuffix[] =
		{
			{ ".cl",    false },
			{ ".clisp", false },
			{ ".el",    false },
			{ ".lisp",  false },
			{ ".lsp",   false }
		};
		cbAddNewSuffixes("Lisp", nullptr, kCBLispFT, kLispSuffix, sizeof(kLispSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kPerlSuffix[] =
		{
			{ ".pl",   false },
			{ ".pm",   false },
			{ ".perl", false },
			{ ".plx",  false }
		};
		cbAddNewSuffixes("Perl", "unix script", kCBPerlFT, kPerlSuffix, sizeof(kPerlSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kPythonSuffix[] =
		{
			{ ".py",     false },
			{ ".python", false }
		};
		cbAddNewSuffixes("Python", "unix script", kCBPythonFT, kPythonSuffix, sizeof(kPythonSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kSchemeSuffix[] =
		{
			{ ".sch",    false },
			{ ".scheme", false },
			{ ".scm",    false },
			{ ".sm",     false },
			{ ".SCM",    false },
			{ ".SM",     false }
		};
		cbAddNewSuffixes("Scheme", nullptr, kCBSchemeFT, kSchemeSuffix, sizeof(kSchemeSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kBourneShellSuffix[] =
		{
			{ ".sh",   false },
			{ ".SH",   false },
			{ ".bsh",  false },
			{ ".bash", false },
			{ ".ksh",  false }
		};
		cbAddNewSuffixes("Bourne Shell", "unix script", kCBBourneShellFT, kBourneShellSuffix, sizeof(kBourneShellSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kTCLSuffix[] =
		{
			{ ".tcl",  false },
			{ ".tk",   false },
			{ ".wish", false }
		};
		cbAddNewSuffixes("TCL", "unix script", kCBTCLFT, kTCLSuffix, sizeof(kTCLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kVIMSuffix[] =
		{
			{ ".vim", false }
		};
		cbAddNewSuffixes("Vim", "unix script", kCBVimFT, kVIMSuffix, sizeof(kVIMSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 32)
		{
		CBNewSuffixInfo kJavaArchiveSuffix[] =
		{
			{ ".jar", false }
		};
		cbAddNewSuffixes(nullptr, nullptr, kCBJavaArchiveFT, kJavaArchiveSuffix, sizeof(kJavaArchiveSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 33)
		{
		CBNewSuffixInfo kPHPSuffix[] =
		{
			{ ".php",   false },
			{ ".php3",  false },
			{ ".phtml", false }
		};
		cbAddNewSuffixes("HTML", "PHP", kCBPHPFT, kPHPSuffix, sizeof(kPHPSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 34)
		{
		CBNewSuffixInfo kASPSuffix[] =
		{
			{ ".asp", false },
			{ ".asa", false }
		};
		cbAddNewSuffixes("ASP", nullptr, kCBASPFT, kASPSuffix, sizeof(kASPSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 35)
		{
		CBNewSuffixInfo kMakeSuffix[] =
		{
			{ ".mak",         false },
			{ "Makefile",     false },
			{ "makefile",     false },
			{ "Make.header",  false }
		};
		cbAddNewSuffixes("Make", "unix script", kCBMakeFT, kMakeSuffix, sizeof(kMakeSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 36)
		{
		CBNewSuffixInfo kREXXSuffix[] =
		{
			{ ".cmd",  false },
			{ ".rexx", false },
			{ ".rx",   false }
		};
		cbAddNewSuffixes("REXX", "C", kCBREXXFT, kREXXSuffix, sizeof(kREXXSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kRubySuffix[] =
		{
			{ ".rb", false }
		};
		cbAddNewSuffixes("Ruby", "unix script", kCBRubyFT, kRubySuffix, sizeof(kRubySuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 38)
		{
		CBNewSuffixInfo kLexSuffix[] =
		{
			{ ".l", false }
		};
		cbAddNewSuffixes("Lex", "C", kCBLexFT, kLexSuffix, sizeof(kLexSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 39)
		{
		CBNewSuffixInfo kCShellSuffix[] =
		{
			{ ".csh",   false },
			{ ".CSH",   false },
			{ ".tcsh",  false }
		};
		cbAddNewSuffixes("C shell", "unix script", kCBCShellFT, kCShellSuffix, sizeof(kCShellSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 41)
		{
		CBNewSuffixInfo kBisonSuffix[] =
		{
			{ ".y", false }
		};
		cbAddNewSuffixes("Bison", "C", kCBBisonFT, kBisonSuffix, sizeof(kBisonSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 44)
		{
		CBNewSuffixInfo kBetaSuffix[] =
		{
			{ ".bet", false }
		};
		cbAddNewSuffixes("Beta", "Beta", kCBBetaFT, kBetaSuffix, sizeof(kBetaSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kLuaSuffix[] =
		{
			{ ".lua", false }
		};
		cbAddNewSuffixes("Lua", "Lua",  kCBLuaFT, kLuaSuffix, sizeof(kLuaSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kSLangSuffix[] =
		{
			{ ".sl", false }
		};
		cbAddNewSuffixes("SLang", nullptr, kCBSLangFT, kSLangSuffix, sizeof(kSLangSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kSQLSuffix[] =
		{
			{ ".sql",  false },
			{ ".ddl",  false }
		};
		cbAddNewSuffixes("SQL", "SQL", kCBSQLFT, kSQLSuffix, sizeof(kSQLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kVeraSourceSuffix[] =
		{
			{ ".vr",  false },
			{ ".vri", false }
		};
		cbAddNewSuffixes("Vera", "Vera", kCBVeraSourceFT, kVeraSourceSuffix, sizeof(kVeraSourceSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kVeraHeaderSuffix[] =
		{
			{ ".vrh", false }
		};
		cbAddNewSuffixes("Vera", "Vera", kCBVeraHeaderFT, kVeraHeaderSuffix, sizeof(kVeraHeaderSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kVerilogSuffix[] =
		{
			{ ".v", false }
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
		JIndex macroID = FindMacroName("C#", itsMacroList, true);
		JIndex macroIndex;
		if (FindMacroID(*itsMacroList, macroID, &macroIndex))
			{
			const MacroSetInfo macroInfo = itsMacroList->GetElement(macroIndex);
			if (macroInfo.macro->GetMacroList().IsEmpty())
				{
				AddDefaultCSharpMacros(macroInfo.macro);
				}
			}

		CBNewSuffixInfo kCSharpSuffix[] =
		{
			{ ".cs", false }
		};
		cbAddNewSuffixes("C#", nullptr, kCBCSharpFT, kCSharpSuffix, sizeof(kCSharpSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kErlangSuffix[] =
		{
			{ ".erl", false }
		};
		cbAddNewSuffixes("Erlang", nullptr, kCBErlangFT, kErlangSuffix, sizeof(kErlangSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kSMLSuffix[] =
		{
			{ ".sml", false }
		};
		cbAddNewSuffixes("SML", nullptr, kCBSMLFT, kSMLSuffix, sizeof(kSMLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 50)
		{
		CBNewSuffixInfo kJavaScriptSuffix[] =
		{
			{ ".js",   false },
			{ ".json", false }
		};
		cbAddNewSuffixes("JavaScript", "Java", kCBJavaScriptFT, kJavaScriptSuffix, sizeof(kJavaScriptSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 52)
		{
		CBNewSuffixInfo kAntSuffix[] =
		{
			{ "build.xml", false }
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
			{ ".jsp",   false },
			{ ".jspf",  false }
		};
		cbAddNewSuffixes("JSP", "Java", kCBJSPFT, kJSPSuffix, sizeof(kJSPSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 57)
		{
		JIndex macroID = FindMacroName("XML", itsMacroList, true);
		JIndex macroIndex;
		if (FindMacroID(*itsMacroList, macroID, &macroIndex))
			{
			const MacroSetInfo macroInfo = itsMacroList->GetElement(macroIndex);
			if (macroInfo.macro->GetMacroList().IsEmpty())
				{
				AddDefaultXMLMacros(macroInfo.macro);
				}
			if (macroInfo.action->GetActionMap().IsEmpty())
				{
				AddDefaultXMLActions(macroInfo.action);
				}
			}
		}

	if (currentVersion < 58)
		{
		CBNewSuffixInfo kXMLSuffix[] =
		{
			{ ".xml",  false },
			{ ".xsd",  false },
			{ ".wsdl", false },
			{ ".dtd",  false }
		};
		cbAddNewSuffixes("XML", nullptr, kCBXMLFT, kXMLSuffix, sizeof(kXMLSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 59)
		{
		CBNewSuffixInfo kBasicSuffix[] =
		{
			{ ".bas", false },
			{ ".bi",  false },
			{ ".bb",  false },
			{ ".pb",  false }
		};
		cbAddNewSuffixes("Basic", nullptr, kCBBasicFT, kBasicSuffix, sizeof(kBasicSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 62)
		{
#ifdef _J_OSX
		CBNewExternalSuffixInfo kImageSuffix[] =
		{
			{ ".png", "open $f", false }
		};
#else
		CBNewExternalSuffixInfo kImageSuffix[] =
		{
			{ ".png", "eog $f", false }
		};
#endif
		cbAddNewExternalSuffixes(kImageSuffix, sizeof(kImageSuffix)/sizeof(CBNewExternalSuffixInfo), itsFileTypeList);
		}

	if (currentVersion < 63)
		{
		CBNewSuffixInfo kMatlabSuffix[] =
		{
			{ ".m", false }
		};
		cbAddNewSuffixes("Matlab", "C", kCBMatlabFT, kMatlabSuffix, sizeof(kMatlabSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kAdobeFlexSuffix[] =
		{
			{ ".as",   false },
			{ ".mxml", false }
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
			{ "core.*", "medic -c $f", false }
		};
		cbAddNewExternalSuffixes(kCoreSuffix, sizeof(kCoreSuffix)/sizeof(CBNewExternalSuffixInfo), itsFileTypeList);
		}

	if (currentVersion < 66)
		{
		CBNewSuffixInfo kINISuffix[] =
		{
			{ ".ini", false }
		};
		cbAddNewSuffixes("INI", nullptr, kCBINIFT, kINISuffix, sizeof(kINISuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 67)
		{
		CBNewSuffixInfo kPropertiesSuffix[] =
		{
			{ ".properties", false }
		};
		cbAddNewSuffixes("Properties", "UNIX script", kCBPropertiesFT, kPropertiesSuffix, sizeof(kPropertiesSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
		}

	if (currentVersion < 68)
		{
		CreateDCRMRuleList();

		CBNewSuffixInfo kDSuffix[] =
		{
			{ ".d", false }
		};
		cbAddNewSuffixes("D", nullptr, kCBDFT, kDSuffix, sizeof(kDSuffix), itsFileTypeList, itsMacroList, *itsCRMList);

		CBNewSuffixInfo kGoSuffix[] =
		{
			{ ".go", false }
		};
		cbAddNewSuffixes("Go", nullptr, kCBGoFT, kGoSuffix, sizeof(kGoSuffix), itsFileTypeList, itsMacroList, *itsCRMList);
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
		auto* actionMgr = jnew CBCharActionManager;
		assert( actionMgr != nullptr );
		if (GetData(50000 + kCBOrigMacroID[i], &data))
			{
			RemoveData(50000 + kCBOrigMacroID[i]);
			std::istringstream dataStream(data);
			actionMgr->ReadSetup(dataStream);
			}

		auto* macroMgr = jnew CBMacroManager;
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

		auto* name = jnew JString(kOrigMacroName[i]);
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

		const bool ok = GetData(30000+i, &data);
		assert( ok );
		RemoveData(30000+i);
		std::istringstream dataStream(data);
		bool wordWrap;
		dataStream >> JBoolFromString(wordWrap);

		const JSize count = suffixList.GetElementCount();
		for (JIndex j=1; j<=count; j++)
			{
			auto* complSuffix = jnew JString;
			assert( complSuffix != nullptr );
			InitComplementSuffix(*(suffixList.GetElement(j)), complSuffix);

			itsFileTypeList->AppendElement(
				FileTypeInfo(suffixList.GetElement(j), nullptr, nullptr, kOrigFileType[i],
							 kOrigFTMacroMap[i], kCBEmptyCRMRuleListID,
							 true, nullptr, wordWrap, complSuffix, nullptr));
			}

		suffixList.RemoveAll();		// avoid DeleteAll() in GetStringList()
		}

	itsFileTypeList->Sort();

	{
	const bool ok = GetData(39998, &data);
	assert( ok );
	RemoveData(39998);
	std::istringstream dataStream(data);
	dataStream >> JBoolFromString(itsExecOutputWordWrapFlag);
	}

	{
	const bool ok = GetData(39999, &data);
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
	mgr->SetAction(JUtf8Character('>'),  JString("$(xml-auto-close $t)", JString::kNoCopy));
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
	const bool			create
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
		if (JString::Compare(macroName, *(macroInfo.name), JString::kIgnoreCase) == 0)
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

	auto* name = jnew JString(macroName);
	assert( name != nullptr );

	auto* actionMgr = jnew CBCharActionManager;
	assert( actionMgr != nullptr );

	auto* macroMgr = jnew CBMacroManager;
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
		if (JString::Compare(crmName, *(crmInfo.name), JString::kIgnoreCase) == 0)
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
		bool found = false;
		dataStream >> suffix;

		const JSize ftCount = itsFileTypeList->GetElementCount();
		for (JIndex j=1; j<=ftCount; j++)
			{
			const FileTypeInfo info = itsFileTypeList->GetElement(j);
			if (*(info.suffix) == suffix)
				{
				found = true;
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
							  true, nullptr, true, jnew JString, jnew JString(cmd));
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
	JArray<FileTypeInfo>* list = jnew JArray<FileTypeInfo>(256);
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
	bool ok = GetData(kCBFileTypeListID, &listData);
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
			bool hasEditCmd;
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
			bool hasScriptPath;
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
			listStream << ' ' << JBoolToString(true) << ' ' << *(info.editCmd);
			}
		else
			{
			listStream << ' ' << JBoolToString(false);
			}

		if (info.scriptPath != nullptr)
			{
			listStream << ' ' << JBoolToString(true) << ' ' << *(info.scriptPath);
			}
		else
			{
			listStream << ' ' << JBoolToString(false);
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
		contentRegex->SetSingleLine(true);
		}
	else if (suffix->Contains(kCBNameRegexMarker))
		{
		JString s;
		const bool ok = JDirInfo::BuildRegexFromWildcardFilter(*suffix, &s);
		assert( ok );

		nameRegex = jnew JRegex(s);
		assert( nameRegex != nullptr );
		}
}

/******************************************************************************
 FileTypeInfo::IsPlainSuffix

 ******************************************************************************/

bool
CBPrefsManager::FileTypeInfo::IsPlainSuffix()
	const
{
	return nameRegex == nullptr && contentRegex == nullptr;
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
	std::istream&		input,
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

const JSize kMaxInitCRMRuleCount = 5;	// D

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
	  { "([[:space:]]*)\\((\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",    },
	  { "[[:space:]]*(\\*+\\)?[[:space:]]*)+",  "[[:space:]]*(\\*+\\)?[[:space:]]*)+" },
	  { "$1 $2",                                "$0",                                 }
	},

	{ "C", 2,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",  },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+" },
	  { "$1 *",                               "$0",                               }
	},

	{ "C++", 3,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*" },
	  { "$1 *",                               "$0",                                "$0"                         }
	},

	{ "C#", 3,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*///?[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*///?[[:space:]]*" },
	  { "$1 *",                               "$0",                                "$0"                           }
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
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*" },
	  { "$1 *",                               "$0",                                "$0"                         }
	},

	{ "JSP", 3,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*" },
	  { "$1 *",                               "$0",                                "$0"                         }
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
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "$1 *",                               "$0",                                "$0"                        , "$0"                            }
	},

	{ "SQL", 4,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*--[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*--[[:space:]]*", "[[:space:]]*(#+[[:space:]]*)+" },
	  { "$1 *",                               "$0",                                "$0",                         "$0"                            }
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

void
cbCreateCRMRuleList
	(
	JArray<CBPrefsManager::CRMRuleListInfo>*	list,
	const InitCRMInfo&							info
	)
{
	auto* name = jnew JString(info.name);
	assert( name != nullptr );

	auto* ruleList = jnew JStyledText::CRMRuleList;
	assert( ruleList != nullptr );

	for (JUnsignedOffset j=0; j<info.count; j++)
		{
		ruleList->AppendElement(JStyledText::CRMRule(
			JString(info.first[j],   JString::kNoCopy),
			JString(info.rest[j],    JString::kNoCopy),
			JString(info.replace[j], JString::kNoCopy)));
		}

	list->AppendElement(
		CBPrefsManager::CRMRuleListInfo(
			kCBFirstCRMRuleListID + list->GetElementCount(),
			name, ruleList));
}

// regex file types

struct FTRegexInfo
{
	const JUtf8Byte*	pattern;
	CBTextFileType		type;
	const JUtf8Byte*	macroName;
	const JUtf8Byte*	crmName;
	bool				wrap;
};

static const FTRegexInfo kFTRegexInfo[] =
{
	// AWK
	{ "^#![[:space:]]*/[^[:space:]]+/awk([[:space:]]|$)",
	  kCBAWKFT, "AWK", "UNIX script", false },

	// Perl
	{ "^#![[:space:]]*/[^[:space:]]+/perl[-_]?([0-9.]*)([[:space:]]|$)",
	  kCBPerlFT, "Perl", "UNIX script", false },

	// Python
	{ "^#![[:space:]]*/[^[:space:]]+/python([[:space:]]|$)",
	  kCBPythonFT, "Python", "UNIX script", false },

	// Ruby
	{ "^#![[:space:]]*/[^[:space:]]+/ruby([[:space:]]|$)",
	  kCBRubyFT, "Ruby", "UNIX script", false },

	// Shell
	{ "^#![[:space:]]*/[^[:space:]]+/(ba|k|z)?sh([[:space:]]|$)",
	  kCBBourneShellFT, "Bourne shell", "UNIX script", false },

	// C shell
	{ "^#![[:space:]]*/[^[:space:]]+/t?csh([[:space:]]|$)",
	  kCBCShellFT, "C shell", "UNIX script", false },

	// TCL
	{ "^#![[:space:]]*/[^[:space:]]+/tcl([[:space:]]|$)",
	  kCBTCLFT, "TCL", "UNIX script", false },

	// TCL/Tk
	{ "^#![[:space:]]*/[^[:space:]]+/wish([[:space:]]|$)",
	  kCBTCLFT, "TCL", "UNIX script", false },

	// XPM source
	{ "^/\\* XPM \\*/(\\n|$)",
	  kCBCHeaderFT, "C/C++", "C", false },

	// email
	{ "^From[[:space:]]+.*.{3}[[:space:]]+.{3}[[:space:]]+[[:digit:]]+"
	  "[[:space:]]+([[:digit:]]+:)+[[:digit:]]+[[:space:]]+(...[[:space:]])?"
	  "[[:digit:]]{4}",
	  kCBUnknownFT, nullptr, "E-mail", true }
};

void
CBPrefsManager::CreateCRMRuleLists()
{
	assert( itsCRMList == nullptr );

	// create CRM rule lists

	itsCRMList = CreateCRMList();

	for (auto& info : kInitCRM)
		{
		cbCreateCRMRuleList(itsCRMList, info);
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

	for (auto& ftrInfo : kFTRegexInfo)
		{
		auto* suffix = jnew JString(ftrInfo.pattern);
		assert( suffix != nullptr );

		auto* complSuffix = jnew JString;
		assert( complSuffix != nullptr );

		const JIndex macroID = FindMacroName(ftrInfo.macroName, itsMacroList, true);
		const JIndex crmID   = FindCRMRuleListName(ftrInfo.crmName, *itsCRMList);

		FileTypeInfo info(suffix, nullptr, nullptr, ftrInfo.type, macroID, crmID,
						  true, nullptr, ftrInfo.wrap, complSuffix, nullptr);
		info.CreateRegex();
		assert( info.contentRegex != nullptr );
		itsFileTypeList->InsertSorted(info);
		}
}

/******************************************************************************
 CreateDCRMRuleList (private)

 ******************************************************************************/

static const InitCRMInfo kInitDCRM =
{
	"D", 5,
	  { "([[:space:]]*)/(\\*+[[:space:]]*)+", "[[:space:]]*(\\*+[[:space:]]*)+",   "[[:space:]]*//[[:space:]]*", "([[:space:]]*)/(\\++[[:space:]]*)+", "[[:space:]]*(\\++[[:space:]]*)+"   },
	  { "[[:space:]]*(\\*+/?[[:space:]]*)+",  "[[:space:]]*(\\*+/?[[:space:]]*)+", "[[:space:]]*//[[:space:]]*", "[[:space:]]*(\\++/?[[:space:]]*)+",  "[[:space:]]*(\\++/?[[:space:]]*)+" },
	  { "$1 *",                               "$0",                                "$0"                        , "$1 +",                               "$0"                                }
};

void
CBPrefsManager::CreateDCRMRuleList()
{
	cbCreateCRMRuleList(itsCRMList, kInitDCRM);
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
	std::istream&		input,
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

	for (const auto& r : *list)
		{
		output << ' ' << r.first->GetPattern();
		output << ' ' << r.rest->GetPattern();
		output << ' ' << *r.replace;
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
	bool ok[ kColorCount ];

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
				ok[i] = false;
				}
			else
				{
				dataStream >> color[i];
				itsColor[i] = JColorManager::GetColorID(color[i]);
				ok[i]       = true;
				}
			}
		}
	else
		{
		for (JUnsignedOffset i=0; i<kColorCount; i++)
			{
			ok[i] = false;
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
	const bool ok = GetData(kCBDefFontID, &data);
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

	RestoreProgramState() returns true if there was any state to restore.

	ForgetProgramState() is required because we have to save state before
	anything is closed.  If the close fails, we shouldn't save the state.

 ******************************************************************************/

bool
CBPrefsManager::RestoreProgramState()
{
	std::string data;
	if (GetData(kCBDocMgrStateID, &data))
		{
		const bool saveAutoDock = JXWindow::WillAutoDockNewWindows();
		JXWindow::ShouldAutoDockNewWindows(false);

		std::istringstream dataStream(data);
		const bool restored =
			CBGetDocumentManager()->RestoreState(dataStream);
		RemoveData(kCBDocMgrStateID);

		JXWindow::ShouldAutoDockNewWindows(saveAutoDock);
		return restored;
		}
	else
		{
		return false;
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
	if (CBGetDocumentManager()->SaveState(data))
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

bool
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
		return true;
		}
	else
		{
		return false;
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

	Returns true if the file should be opened via another program.
	This only checks the file suffix types since the file isn't open.

 ******************************************************************************/

bool
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
			return false;
			}

		const JUtf8Byte* map[] =
			{
			"f", fileName.GetBytes()
			};
		JGetStringManager()->Replace(cmd, map, sizeof(map));
		return true;
		}
	else
		{
		cmd->Clear();
		return false;
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
	bool*						wordWrap
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
		*wordWrap = true;
		return origType;
		}
	else if (origType == kCBManPageFT)
		{
		*wordWrap = false;
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

bool
CBPrefsManager::CalcFileType
	(
	const CBTextDocument&	doc,
	JIndex*					index
	)
	const
{
	const JString fileName       = CleanFileName(doc.GetFileName());
	const bool checkSuffixes = doc.ExistsOnDisk();

	const JSize count = itsFileTypeList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		FileTypeInfo info   = itsFileTypeList->GetElement(i);
		const JString& text = doc.GetTextEditor()->GetText()->GetText();
		if (info.contentRegex != nullptr)
			{
			// JRegex is so slow on a large file (even with the ^ anchor!)
			// that we must avoid running it at all costs.

			if (info.literalRange.IsNothing())
				{
				info.literalRange = GetLiteralPrefixRange(*(info.suffix));
				itsFileTypeList->SetElement(i, info);
				}

			if (JString::CompareMaxNBytes(
					text.GetBytes(), info.suffix->GetBytes()+1,
					info.literalRange.GetCount(), JString::kCompareCase) == 0 &&
				info.contentRegex->Match(text))
				{
				*index = i;
				return true;
				}
			}
		else if ((info.nameRegex != nullptr && info.nameRegex->Match(fileName)) ||
				 (info.IsPlainSuffix() &&
				  checkSuffixes && fileName.EndsWith(*(info.suffix))))
			{
			*index = i;
			return true;
			}
		}

	*index = 0;
	return false;
}

/******************************************************************************
 GetLiteralPrefixRange (static)

 ******************************************************************************/

JUtf8ByteRange
CBPrefsManager::GetLiteralPrefixRange
	(
	const JString& regexStr
	)
{
	assert( regexStr.GetFirstCharacter() == kCBContentRegexMarker );

	JStringIterator iter(regexStr, kJIteratorStartAfter, 1);
	JUtf8Character c;
	while (iter.Next(&c, kJIteratorStay) && c != '\0' && c != '\\' &&
		   !JRegex::NeedsBackslashToBeLiteral(c))
		{
		iter.SkipNext();
		}

	if (c == '?' && iter.GetPrevCharacterIndex() > 2)
		{
		iter.SkipPrev();
		}

	return JUtf8ByteRange(2, iter.GetPrevByteIndex());
}

/******************************************************************************
 GetScriptPaths (static)

 ******************************************************************************/

bool
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

bool
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
			return true;
			}
		}

	*index = 0;
	return false;
}

/******************************************************************************
 FindCRMRuleListID (static)

 ******************************************************************************/

bool
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
			return true;
			}
		}

	*index = 0;
	return false;
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
			list->Append(*info.suffix);
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
			list->Append(*info.suffix);
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
	JString root, suffix;
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

			JSplitRootAndSuffix(name, &root, &suffix);
			root  += *info.complSuffix;
			*index = i;
			return root;
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

bool
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
			return true;
			}
		}

	return false;
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

		JStringIterator iter(&n, kJIteratorStartAtEnd);
		JUtf8Character c;
		while (iter.Prev(&c) && (c == '~' || c == '#'))
			{
			iter.RemoveNext();
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
	const bool ok = GetData(id, &data);
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
