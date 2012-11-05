/******************************************************************************
 CMPrefsManager.h

	Interface for the CMPrefsManager class

	Copyright � 1998 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_CMPrefsManager
#define _H_CMPrefsManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXPrefsManager.h>
#include "CBTextFileType.h"
#include <JPtrArray-JString.h>
#include <jColor.h>

class JXWindow;
class JXStringHistoryMenu;
class JXPSPrinter;
class JX2DPlotEPSPrinter;
class JXPTPrinter;
class CMLink;
class CMEditPrefsDialog;
class CBCStyler;

enum
{
	kCSourceSuffixID = 1,
	kCHeaderSuffixID,
	kEditFileCmdID,
	kCmdWindSizeID,
	kCodeWindSizeID,
	kGDBCommandID,
	kFileWindSizeID,
	kMainCodeWindSizeID,
	kVarTreeWindSizeID,
	kCMExpireTimeStampID,
	kCMDockPrefID,
	kJVMCommandID,
	kHistoryMenuID,
	kCMProgramVersionID,
	kStackWindowSizeID,
	kStackLineMaxID,
	kFileListSetupID,
	kPTPrinterSetupID,
	kCBFnMenuUpdaterID,
	kCBCStyleID,
	kCBTextColorID,
	kCBDefFontID,
	kCmdListID,
	kMainSrcToolBarID,
	kOtherSrcToolBarID,
	kCmdWindowToolBarID,
	kVarTreeToolBarID,
	kStackToolBarID,
	kFileListToolBarID,
	kArray1DWindowToolBarID,
	kEditCommandsDialogID,
	kCBTabCharCountID,
	kgCSFSetupID,
	kDebuggerTypeID,
	kArray1DWindSizeID,
	kLocalVarWindSizeID,
	kJavaSuffixID,
	kFortranSuffixID,
	kDebugWindSizeID,
	kCBJavaStyleID,
	kCMThreadWindowSizeID,
	kArray2DWindSizeID,
	kArray2DWindowToolBarID,
	kPSPrinterSetupID,
	kCBHTMLStyleID,
	kPHPSuffixID,
	kPlot2DWindSizeID,
	kPlotEPSPrinterSetupID,
	kVersionCheckID,
	kBreakpointsWindowSizeID,
	kBreakpointTableID,
	kMemoryWindSizeID,
	kMainAsmWindSizeID,
	kMainAsmToolBarID,
	kOtherAsmToolBarID,
	kAsmWindSizeID,
	kRegistersWindowSizeID
};

class CMPrefsManager : public JXPrefsManager
{
public:

	// Do not change these values once assigned because they
	// are stored in the prefs file.

	enum DebuggerType
	{
		kGDBType,
		kXdebugType,
		kJavaType
	};

	enum
	{
		kTextColorIndex = 1,
		kBackColorIndex,
		kCaretColorIndex,
		kSelColorIndex,
		kSelLineColorIndex,
		kRightMarginColorIndex,		// = kColorCount

		kColorCount = kRightMarginColorIndex
	};

public:

	CMPrefsManager(JBoolean* isNew);

	virtual	~CMPrefsManager();

	JString	GetMedicVersionStr() const;

	JBoolean	GetExpirationTimeStamp(time_t* t) const;
	void		SetExpirationTimeStamp(const time_t t);

	void	EditPrefs();

	DebuggerType	GetDebuggerType() const;
	JBoolean		SetDebuggerType(const DebuggerType type);

	JString	GetGDBCommand() const;
	void	SetGDBCommand(const JCharacter* command);

	JString	GetJVMCommand() const;
	void	SetJVMCommand(const JCharacter* command);

	JSize	GetStackLineMax() const;
	void	SetStackLineMax(const JSize max);

	CBTextFileType	GetFileType(const JString& fileName) const;
	void			GetEditFileCmds(JString* editFileCmd, JString* editFileLineCmd) const;

	void	GetCmdList(JPtrArray<JString>* cmdList) const;
	void	SetCmdList(const JPtrArray<JString>& cmdList);

	void	ReadPrinterSetup(JXPSPrinter* printer);
	void	WritePrinterSetup(JXPSPrinter* printer);

	void	ReadPrinterSetup(JX2DPlotEPSPrinter* printer);
	void	WritePrinterSetup(JX2DPlotEPSPrinter* printer);

	void	ReadPrinterSetup(JXPTPrinter* printer);
	void	WritePrinterSetup(JXPTPrinter* printer);

	void	ReadHistoryMenuSetup(JXStringHistoryMenu* menu);
	void	WriteHistoryMenuSetup(JXStringHistoryMenu* menu);

	void	GetWindowSize(const JPrefID id, JXWindow* window,
						  const JBoolean skipDocking = kJFalse) const;
	void	SaveWindowSize(const JPrefID id, JXWindow* window);

// these were taken from Code Crusader for compatibility with shared prefs

	void	GetDefaultFont(JString* name, JSize* size) const;
	void	SetDefaultFont(const JCharacter* name, const JSize size);

	JSize	GetTabCharCount() const;
	void	SetTabCharCount(const JSize count);

	JColorIndex		GetColor(const JIndex index) const;
	static JBoolean	ColorIndexValid(const JIndex index);

	void	SyncWithCodeCrusader();

protected:

	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion);
	virtual void	SaveAllBeforeDestruct();
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CMEditPrefsDialog* itsEditPrefsDialog;

	// these were taken from CBPrefsManager for compatibility with shared files
	JColorIndex	itsColor [ kColorCount ];

private:

	void	UpdatePrefs(const CMEditPrefsDialog* dlog);

	JBoolean	GetSuffixes(const JPrefID& id, JPtrArray<JString>* suffixList) const;
	void		SetSuffixes(const JPrefID& id, const JPtrArray<JString>& suffixList);

	void	SetEditFileCmds(const JString& editFileCmd, const JString& editFileLineCmd);

	void	SetColorList(const JBoolean hasColors, JRGB colorList[]);

// these were taken from CBPrefsManager for compatibility with shared prefs

	void	ReadColors();
	void	WriteColors();

	// not allowed

	CMPrefsManager(const CMPrefsManager& source);
	const CMPrefsManager& operator=(const CMPrefsManager& source);

public:

	static const JCharacter* kFileTypesChanged;
	static const JCharacter* kCustomCommandsChanged;
	static const JCharacter* kTextColorChanged;

	class FileTypesChanged : public JBroadcaster::Message
		{
		public:

			FileTypesChanged()
				:
				JBroadcaster::Message(kFileTypesChanged)
				{ };
		};

	class CustomCommandsChanged : public JBroadcaster::Message
		{
		public:

			CustomCommandsChanged()
				:
				JBroadcaster::Message(kCustomCommandsChanged)
				{ };
		};

	// required by CBStylerBase

	class TextColorChanged : public JBroadcaster::Message
		{
		public:

			TextColorChanged()
				:
				JBroadcaster::Message(kTextColorChanged)
				{ };
		};
};

/******************************************************************************
 ColorIndexValid (static)

 ******************************************************************************/

inline JBoolean
CMPrefsManager::ColorIndexValid
	(
	const JIndex index
	)
{
	return JI2B( 1 <= index && index <= kColorCount );
}

#endif
