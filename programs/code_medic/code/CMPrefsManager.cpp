/******************************************************************************
 CMPrefsManager.cpp

	BASE CLASS = public JXPrefsManager

	Copyright (C) 1998 by Glenn W. Bach.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "CMPrefsManager.h"
#include "CMEditPrefsDialog.h"
#include "cmGlobals.h"
#include "cmFileVersions.h"

#include "CBCStyler.h"
#include "CBFnMenuUpdater.h"
#include "cbmUtil.h"

#include <JXWindow.h>
#include <JXStringHistoryMenu.h>
#include <JXPSPrinter.h>
#include <JX2DPlotEPSPrinter.h>
#include <JXPTPrinter.h>
#include <JXDisplay.h>
#include <JXColorManager.h>
#include <JXFontManager.h>
#include <JXChooseSaveFile.h>
#include <JXSearchTextDialog.h>
#include <JXWebBrowser.h>

#include <JStringIterator.h>
#include <jDirUtil.h>
#include <jAssert.h>

// from CBPrefsManager
const JFileVersion kCurrentTextColorVers = 0;

const JSize kStackLineMaxDefault = 100;

// JBroadcaster messages

const JUtf8Byte* CMPrefsManager::kFileTypesChanged      = "FileTypesChanged::CMPrefsManager";
const JUtf8Byte* CMPrefsManager::kCustomCommandsChanged = "CustomCommandsChanged::CMPrefsManager";
const JUtf8Byte* CMPrefsManager::kTextColorChanged      = "TextColorChanged::CMPrefsManager";

/******************************************************************************
 Constructor

 *****************************************************************************/

CMPrefsManager::CMPrefsManager
	(
	JBoolean* isNew
	)
	:
	JXPrefsManager(kCurrentPrefsFileVersion, kJTrue)
{
	itsEditPrefsDialog = nullptr;

	*isNew = JPrefsManager::UpgradeData();

	JXChooseSaveFile* csf = JXGetChooseSaveFile();
	csf->SetPrefInfo(this, kgCSFSetupID);
	csf->JPrefObject::ReadPrefs();

	ReadColors();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMPrefsManager::~CMPrefsManager()
{
	SaveAllBeforeDestruct();
}

/******************************************************************************
 SaveAllBeforeDestruct (virtual protected)

 ******************************************************************************/

void
CMPrefsManager::SaveAllBeforeDestruct()
{
	WriteColors();
	SaveSearchPrefs();

	SetData(kCMProgramVersionID, CMGetVersionNumberStr());

	JXPrefsManager::SaveAllBeforeDestruct();
}

/******************************************************************************
 GetMedicVersionStr

 ******************************************************************************/

JString
CMPrefsManager::GetMedicVersionStr()
	const
{
	std::string data;
	if (GetData(kCMProgramVersionID, &data))
		{
		return JString(data);
		}
	else
		{
		return JString("< 0.5.0");		// didn't exist before this version
		}
}

/******************************************************************************
 GetExpirationTimeStamp

 ******************************************************************************/

JBoolean
CMPrefsManager::GetExpirationTimeStamp
	(
	time_t* t
	)
	const
{
	std::string data;
	if (GetData(kCMExpireTimeStampID, &data))
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
CMPrefsManager::SetExpirationTimeStamp
	(
	const time_t t
	)
{
	std::ostringstream data;
	data << t;

	SetData(kCMExpireTimeStampID, data);
}

/******************************************************************************
 EditPrefs

 ******************************************************************************/

void
CMPrefsManager::EditPrefs()
{
	assert( itsEditPrefsDialog == nullptr );

	const JString gdbCmd = GetGDBCommand();
	const JString jvmCmd = GetJVMCommand();

	JString editFileCmd, editFileLineCmd;
	GetEditFileCmds(&editFileCmd, &editFileLineCmd);

	JPtrArray<JString> cSourceSuffixes(JPtrArrayT::kDeleteAll),
					   cHeaderSuffixes(JPtrArrayT::kDeleteAll),
					   javaSuffixes(JPtrArrayT::kDeleteAll),
					   phpSuffixes(JPtrArrayT::kDeleteAll),
					   fortranSuffixes(JPtrArrayT::kDeleteAll);
	GetSuffixes(kCSourceSuffixID, &cSourceSuffixes);
	GetSuffixes(kCHeaderSuffixID, &cHeaderSuffixes);
	GetSuffixes(kJavaSuffixID, &javaSuffixes);
	GetSuffixes(kPHPSuffixID, &phpSuffixes);
	GetSuffixes(kFortranSuffixID, &fortranSuffixes);

	itsEditPrefsDialog =
		jnew CMEditPrefsDialog(JXGetApplication(),
							  gdbCmd, jvmCmd, editFileCmd, editFileLineCmd,
							  cSourceSuffixes, cHeaderSuffixes,
							  javaSuffixes, phpSuffixes, fortranSuffixes);
	assert( itsEditPrefsDialog != nullptr );
	itsEditPrefsDialog->BeginDialog();
	ListenTo(itsEditPrefsDialog);
}

/******************************************************************************
 UpdatePrefs (private)

 ******************************************************************************/

void
CMPrefsManager::UpdatePrefs
	(
	const CMEditPrefsDialog* dlog
	)
{
	JString gdbCmd, jvmCmd, editFileCmd, editFileLineCmd;
	JPtrArray<JString> cSourceSuffixes(JPtrArrayT::kDeleteAll),
					   cHeaderSuffixes(JPtrArrayT::kDeleteAll),
					   javaSuffixes(JPtrArrayT::kDeleteAll),
					   phpSuffixes(JPtrArrayT::kDeleteAll),
					   fortranSuffixes(JPtrArrayT::kDeleteAll);
	dlog->GetPrefs(&gdbCmd, &jvmCmd,
				   &editFileCmd, &editFileLineCmd,
				   &cSourceSuffixes, &cHeaderSuffixes,
				   &javaSuffixes, &phpSuffixes, &fortranSuffixes);

	SetGDBCommand(gdbCmd);
	SetJVMCommand(jvmCmd);
	CMGetLink()->ChangeDebugger();

	SetEditFileCmds(editFileCmd, editFileLineCmd);

	SetSuffixes(kCSourceSuffixID, cSourceSuffixes);
	SetSuffixes(kCHeaderSuffixID, cHeaderSuffixes);
	SetSuffixes(kJavaSuffixID, javaSuffixes);
	SetSuffixes(kPHPSuffixID, phpSuffixes);
	SetSuffixes(kFortranSuffixID, fortranSuffixes);

	Broadcast(FileTypesChanged());
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMPrefsManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsEditPrefsDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			UpdatePrefs(itsEditPrefsDialog);
			}
		itsEditPrefsDialog = nullptr;
		}
	else
		{
		JXPrefsManager::Receive(sender, message);
		}
}

/******************************************************************************
 UpgradeData (virtual protected)

 ******************************************************************************/

void
CMPrefsManager::UpgradeData
	(
	const JBoolean		isNew,
	const JFileVersion	currentVersion
	)
{
	if (isNew)
		{
		std::ostringstream cSourceSuffixData;
		cSourceSuffixData << 7;
		cSourceSuffixData << ' ' << JString(".c")   << ' ' << JString(".cc");
		cSourceSuffixData << ' ' << JString(".cpp") << ' ' << JString(".cxx");
		cSourceSuffixData << ' ' << JString(".c++") << ' ' << JString(".C");
		cSourceSuffixData << ' ' << JString(".tmpl");
		SetData(kCSourceSuffixID, cSourceSuffixData);

		std::ostringstream cHeaderSuffixData;
		cHeaderSuffixData << 6;
		cHeaderSuffixData << ' ' << JString(".h")   << ' ' << JString(".hh");
		cHeaderSuffixData << ' ' << JString(".hpp") << ' ' << JString(".hxx");
		cHeaderSuffixData << ' ' << JString(".h++") << ' ' << JString(".H");
		SetData(kCHeaderSuffixID, cHeaderSuffixData);

		SetEditFileCmds(
			JString("jcc $f", kJFalse),
			JString("jcc +$l $f", kJFalse));

		std::ostringstream gdbCmdData;
		gdbCmdData << JString("gdb");
		SetData(kGDBCommandID, gdbCmdData);
		}
	else
		{
		JString cmd = GetGDBCommand();
		if (cmd.EndsWith(" -f"))
			{
			JStringIterator iter(&cmd, kJIteratorStartAtEnd);
			iter.RemovePrev(3);
			iter.Invalidate();

			cmd.TrimWhitespace();
			SetGDBCommand(cmd);
			}
		}

	JXDisplay* display = JXGetApplication()->GetDisplay(1);
	if (currentVersion < 2)
		{
		SetDefaultFont(JFontManager::GetDefaultMonospaceFontName(),
					   JFontManager::GetDefaultMonospaceFontSize());
		}
	else
		{
		JString name;
		JSize size;
		GetDefaultFont(&name, &size);
		if (name == "6x13")
			{
			SetDefaultFont(JFontManager::GetDefaultMonospaceFontName(),
						   JFontManager::GetDefaultMonospaceFontSize());
			}
		}

	if (currentVersion < 3)
		{
		RemoveData(33);
		}

	if (!isNew && currentVersion < 4)
		{
		JString editFileCmd, editFileLineCmd;
		GetEditFileCmds(&editFileCmd, &editFileLineCmd);
		JXWebBrowser::ConvertVarNames(&editFileCmd, "fl");
		JXWebBrowser::ConvertVarNames(&editFileLineCmd, "fl");
		SetEditFileCmds(editFileCmd, editFileLineCmd);
		}

	if (currentVersion < 5)
		{
		RemoveData(34);
		}

	if (currentVersion < 6)
		{
		std::ostringstream javaSuffixData;
		javaSuffixData << 1;
		javaSuffixData << ' ' << JString(".java");
		SetData(kJavaSuffixID, javaSuffixData);

		std::ostringstream fortranSuffixData;
		fortranSuffixData << 12;
		fortranSuffixData << ' ' << JString(".f")   << ' ' << JString(".F");
		fortranSuffixData << ' ' << JString(".for") << ' ' << JString(".FOR");
		fortranSuffixData << ' ' << JString(".ftn") << ' ' << JString(".FTN");
		fortranSuffixData << ' ' << JString(".f77") << ' ' << JString(".F77");
		fortranSuffixData << ' ' << JString(".f90") << ' ' << JString(".F90");
		fortranSuffixData << ' ' << JString(".f95") << ' ' << JString(".F95");
		SetData(kFortranSuffixID, fortranSuffixData);
		}

	if (currentVersion < 7)
		{
		std::string data;
		if (GetData(kFileListSetupID, &data))
			{
			std::ostringstream newData;
			newData << 0 << ' ' << data.c_str();
			SetData(kFileListSetupID, newData);
			}
		}

	if (currentVersion < 8)
		{
		std::ostringstream data;
#ifdef _J_OSX
		data << (long) kLLDBType;
#else
		data << (long) kGDBType;
#endif
		SetData(kDebuggerTypeID, data);
		}

	if (currentVersion < 9)
		{
		RemoveData(kJVMCommandID);

		std::ostringstream jvmCmdData;
		jvmCmdData << JString("java");
		SetData(kJVMCommandID, jvmCmdData);
		}

	if (currentVersion < 10)
		{
		std::ostringstream phpSuffixData;
		phpSuffixData << 2;
		phpSuffixData << ' ' << JString(".php");
		phpSuffixData << ' ' << JString(".inc");
		SetData(kPHPSuffixID, phpSuffixData);
		}
}

/******************************************************************************
 Debugger type

 ******************************************************************************/

CMPrefsManager::DebuggerType
CMPrefsManager::GetDebuggerType()
	const
{
	std::string data;
	const JBoolean ok = GetData(kDebuggerTypeID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	long type;
	dataStream >> type;
	return (DebuggerType) type;
}

JBoolean
CMPrefsManager::SetDebuggerType
	(
	const DebuggerType type
	)
{
	if (type != GetDebuggerType())
		{
		std::ostringstream data;
		data << (long) type;

		SetData(kDebuggerTypeID, data);

		CMStartDebugger();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 gdb command

 ******************************************************************************/

JString
CMPrefsManager::GetGDBCommand()
	const
{
	std::string data;
	const JBoolean ok = GetData(kGDBCommandID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString cmd;
	dataStream >> cmd;
	return cmd;
}

void
CMPrefsManager::SetGDBCommand
	(
	const JString& command
	)
{
	std::ostringstream data;
	data << ' ' << command;

	SetData(kGDBCommandID, data);
}

/******************************************************************************
 JVM command

 ******************************************************************************/

JString
CMPrefsManager::GetJVMCommand()
	const
{
	std::string data;
	const JBoolean ok = GetData(kJVMCommandID, &data);
	assert( ok );

	std::istringstream dataStream(data);

	JString cmd;
	dataStream >> cmd;
	return cmd;
}

void
CMPrefsManager::SetJVMCommand
	(
	const JString& command
	)
{
	std::ostringstream data;
	data << ' ' << command;

	SetData(kJVMCommandID, data);
}

/******************************************************************************
 Stack line max

 ******************************************************************************/

JSize
CMPrefsManager::GetStackLineMax()
	const
{
	std::string data;
	if (GetData(kStackLineMaxID, &data))
		{
		std::istringstream dataStream(data);

		JSize max;
		dataStream >> max;
		return max;
		}
	else
		{
		return kStackLineMaxDefault;
		}
}

void
CMPrefsManager::SetStackLineMax
	(
	const JSize max
	)
{
	std::ostringstream data;
	data << max;

	SetData(kStackLineMaxID, data);
}

/******************************************************************************
 GetFileType

 ******************************************************************************/

struct SuffixTypeMap
{
	long			id;
	CBTextFileType	type;
};

static const SuffixTypeMap kSuffixTypeMap[] =
{
	{ kCSourceSuffixID, kCBCSourceFT    },
	{ kCHeaderSuffixID, kCBCHeaderFT    },
	{ kJavaSuffixID,    kCBJavaSourceFT },
	{ kFortranSuffixID, kCBFortranFT    },
	{ kPHPSuffixID,     kCBPHPFT        }
};

const JSize kSuffixTypeMapCount = sizeof(kSuffixTypeMap) / sizeof(SuffixTypeMap);

CBTextFileType
CMPrefsManager::GetFileType
	(
	const JString& fileName
	)
	const
{
	JPtrArray<JString> suffixList(JPtrArrayT::kDeleteAll);
	for (JUnsignedOffset i=0; i<kSuffixTypeMapCount; i++)
		{
		if (GetSuffixes(kSuffixTypeMap[i].id, &suffixList))
			{
			const JSize count = suffixList.GetElementCount();
			for (JIndex j=1; j<=count; j++)
				{
				if (fileName.EndsWith(*(suffixList.GetElement(j))))
					{
					return kSuffixTypeMap[i].type;
					}
				}
			}
		}

	return kCBUnknownFT;
}

/******************************************************************************
 File suffixes (private)

 ******************************************************************************/

JBoolean
CMPrefsManager::GetSuffixes
	(
	const JPrefID&		id,
	JPtrArray<JString>*	suffixList
	)
	const
{
	suffixList->DeleteAll();

	if (IDValid(id))
		{
		std::string data;
		GetData(id, &data);
		std::istringstream dataStream(data);
		dataStream >> *suffixList;
		}

	return !suffixList->IsEmpty();
}

void
CMPrefsManager::SetSuffixes
	(
	const JPrefID&				id,
	const JPtrArray<JString>&	suffixList
	)
{
	std::ostringstream data;
	data << suffixList;
	SetData(id, data);
}

/******************************************************************************
 Edit file cmds

 ******************************************************************************/

void
CMPrefsManager::GetEditFileCmds
	(
	JString*	editFileCmd,
	JString*	editFileLineCmd
	)
	const
{
	std::string data;
	const JBoolean ok = GetData(kEditFileCmdID, &data);
	assert( ok );

	std::istringstream dataStream(data);
	dataStream >> *editFileCmd >> *editFileLineCmd;
}

void
CMPrefsManager::SetEditFileCmds
	(
	const JString& editFileCmd,
	const JString& editFileLineCmd
	)
{
	std::ostringstream data;
	data << editFileCmd << ' ' << editFileLineCmd;

	SetData(kEditFileCmdID, data);
}

/******************************************************************************
 Custom command list

 ******************************************************************************/

void
CMPrefsManager::GetCmdList
	(
	JPtrArray<JString>*	cmdList
	)
	const
{
	std::string data;
	if (GetData(kCmdListID, &data))
		{
		std::istringstream dataStream(data);
		dataStream >> *cmdList;
		}
}

void
CMPrefsManager::SetCmdList
	(
	const JPtrArray<JString>& cmdList
	)
{
	std::ostringstream data;
	data << cmdList;

	SetData(kCmdListID, data);
	Broadcast(CustomCommandsChanged());
}

/******************************************************************************
 Printer setup

 ******************************************************************************/

void
CMPrefsManager::ReadPrinterSetup
	(
	JXPSPrinter* printer
	)
{
	std::string data;
	if (GetData(kPSPrinterSetupID, &data))
		{
		std::istringstream dataStream(data);
		printer->ReadXPSSetup(dataStream);
		}
}

void
CMPrefsManager::WritePrinterSetup
	(
	JXPSPrinter* printer
	)
{
	std::ostringstream data;
	printer->WriteXPSSetup(data);
	SetData(kPSPrinterSetupID, data);
}

void
CMPrefsManager::ReadPrinterSetup
	(
	JX2DPlotEPSPrinter* printer
	)
{
	std::string data;
	if (GetData(kPlotEPSPrinterSetupID, &data))
		{
		std::istringstream dataStream(data);
		printer->ReadX2DEPSSetup(dataStream);
		}
}

void
CMPrefsManager::WritePrinterSetup
	(
	JX2DPlotEPSPrinter* printer
	)
{
	std::ostringstream data;
	printer->WriteX2DEPSSetup(data);
	SetData(kPlotEPSPrinterSetupID, data);
}

void
CMPrefsManager::ReadPrinterSetup
	(
	JXPTPrinter* printer
	)
{
	std::string data;
	if (GetData(kPTPrinterSetupID, &data))
		{
		std::istringstream dataStream(data);
		printer->ReadXPTSetup(dataStream);
		}
}

void
CMPrefsManager::WritePrinterSetup
	(
	JXPTPrinter* printer
	)
{
	std::ostringstream data;
	printer->WriteXPTSetup(data);
	SetData(kPTPrinterSetupID, data);
}

/******************************************************************************
 History menu

 ******************************************************************************/

void
CMPrefsManager::ReadHistoryMenuSetup
	(
	JXStringHistoryMenu* menu
	)
{
	std::string data;
	if (GetData(kHistoryMenuID, &data))
		{
		std::istringstream dataStream(data);
		menu->ReadSetup(dataStream);
		}
}

void
CMPrefsManager::WriteHistoryMenuSetup
	(
	JXStringHistoryMenu* menu
	)
{
	std::ostringstream data;
	menu->WriteSetup(data);
	SetData(kHistoryMenuID, data);
}

/******************************************************************************
 Window size

 ******************************************************************************/

void
CMPrefsManager::GetWindowSize
	(
	const JPrefID	id,
	JXWindow*		window,
	const JBoolean	skipDocking
	)
	const
{
	std::string data;
	if (GetData(id, &data))
		{
		std::istringstream dataStream(data);
		window->ReadGeometry(dataStream, skipDocking);
		window->Deiconify();	// never iconic since never initially visible
		}
}

void
CMPrefsManager::SaveWindowSize
	(
	const JPrefID	id,
	JXWindow*		window
	)
{
	std::ostringstream data;
	window->WriteGeometry(data);
	SetData(id, data);
}

/******************************************************************************
 Default font

 ******************************************************************************/

void
CMPrefsManager::GetDefaultFont
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
CMPrefsManager::SetDefaultFont
	(
	const JString&	name,
	const JSize		size
	)
{
	std::ostringstream data;
	data << name << ' ' << size;
	SetData(kCBDefFontID, data);
}

/******************************************************************************
 Tab char count

 ******************************************************************************/

JSize
CMPrefsManager::GetTabCharCount()
	const
{
	std::string data;
	if (GetData(kCBTabCharCountID, &data))
		{
		JSize count;
		std::istringstream dataStream(data);
		dataStream >> count;
		return count;
		}
	else
		{
		return 4;
		}
}

void
CMPrefsManager::SetTabCharCount
	(
	const JSize	count
	)
{
	std::ostringstream data;
	data << count;
	SetData(kCBTabCharCountID, data);
}

/******************************************************************************
 Colors

 ******************************************************************************/

JColorID
CMPrefsManager::GetColor
	(
	const JIndex index
	)
	const
{
	assert( ColorIndexValid(index) );
	return itsColor [ index-1 ];
}

// private

void
CMPrefsManager::ReadColors()
{
	JRGB color[ kColorCount ];

	std::string data;
	const JBoolean hasColors = GetData(kCBTextColorID, &data);
	if (hasColors)
		{
		std::istringstream dataStream(data);

		JFileVersion vers;
		dataStream >> vers;
		assert( vers <= kCurrentTextColorVers );

		for (JUnsignedOffset i=0; i<kColorCount; i++)
			{
			dataStream >> color[i];
			}
		}

	SetColorList(hasColors, color);
}

void
CMPrefsManager::SetColorList
	(
	const JBoolean	hasColors,
	JRGB			colorList[]
	)
{
	JBoolean ok[ kColorCount ];
	if (hasColors)
		{
		for (JUnsignedOffset i=0; i<kColorCount; i++)
			{
			itsColor[i] = JColorManager::GetColorID(colorList[i]);
			ok[i]       = kJTrue;
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
CMPrefsManager::WriteColors()
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
 Search dialog

 ******************************************************************************/

void
CMPrefsManager::LoadSearchPrefs()
{
	std::string data;
	if (GetData(kSearchTextDialogPrefsID, &data))
		{
		std::istringstream dataStream(data);
		JXGetSearchTextDialog()->ReadSetup(dataStream);
		}
}

void
CMPrefsManager::SaveSearchPrefs()
{
	std::ostringstream data;
	JXGetSearchTextDialog()->WriteSetup(data);

	SetData(kSearchTextDialogPrefsID, data);
}

/******************************************************************************
 SyncWithCodeCrusader

 ******************************************************************************/

void
CMPrefsManager::SyncWithCodeCrusader()
{
	JString fontName;
	JSize size, tabCharCount;
	JBoolean sort, includeNS, pack, openOnTop;
	JRGB colorList[kColorCount];
	JPtrArray<JString> cSourceSuffixes(JPtrArrayT::kDeleteAll),
					   cHeaderSuffixes(JPtrArrayT::kDeleteAll),
					   fortranSuffixList(JPtrArrayT::kDeleteAll),
					   javaSuffixList(JPtrArrayT::kDeleteAll),
					   phpSuffixList(JPtrArrayT::kDeleteAll);
	if (CBMReadSharedPrefs(&fontName, &size, &tabCharCount, &sort, &includeNS, &pack,
						   &openOnTop, kColorCount, colorList,
						   &cSourceSuffixes, &cHeaderSuffixes,
						   &fortranSuffixList, &javaSuffixList,
						   &phpSuffixList))
		{
		SetDefaultFont(fontName, size);
		SetTabCharCount(tabCharCount);
		SetColorList(kJTrue, colorList);
		SetSuffixes(kCSourceSuffixID, cSourceSuffixes);
		SetSuffixes(kCHeaderSuffixID, cHeaderSuffixes);

		if (!fortranSuffixList.IsEmpty())
			{
			SetSuffixes(kFortranSuffixID, fortranSuffixList);
			}
		if (!javaSuffixList.IsEmpty())
			{
			SetSuffixes(kJavaSuffixID, javaSuffixList);
			}
		if (!phpSuffixList.IsEmpty())
			{
			SetSuffixes(kPHPSuffixID, phpSuffixList);
			}

		CBFnMenuUpdater* updater = CMGetFnMenuUpdater();
		updater->ShouldSortFnNames(sort);
		updater->ShouldIncludeNamespace(includeNS);
		updater->ShouldPackFnNames(pack);

		JXGetSearchTextDialog()->SetFont(JFontManager::GetFont(fontName, size));
		}
}
