/******************************************************************************
 CMCommandDirector.h

	Copyright © 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMCommandDirector
#define _H_CMCommandDirector

#include <JXWindowDirector.h>
#include <JPtrArray-JString.h>

class JXMenuBar;
class JXTextMenu;
class JXTextButton;
class JXTEBase;
class JXInputField;
class JXStaticText;
class JXDownRect;
class JXToolBar;
class JXDockWidget;
class JXStringHistoryMenu;
class JXKeyModifiers;
class CMLink;
class CMCommandOutputDisplay;
class CMCommandInput;
class CMSourceDirector;
class CMThreadsDir;
class CMStackDir;
class CMBreakpointsDir;
class CMVarTreeDir;
class CMLocalVarsDir;
class CMArray1DDir;
class CMArray2DDir;
class CMPlot2DDir;
class CMMemoryDir;
class CMRegistersDir;
class CMFileListDir;
class CMDebugDir;
class CMGetInitArgs;

class CMCommandDirector : public JXWindowDirector
{
public:

	CMCommandDirector(JXDirector* supervisor);

	virtual ~CMCommandDirector();

	virtual JBoolean		Close();
	virtual const JString&	GetName() const;
	virtual JBoolean		GetMenuIcon(const JXImage** icon) const;

	void	InitializeCommandOutput();
	void	PrepareCommand(const JCharacter* cmd);

	void	RunProgram();

	void	OpenSourceFiles();
	void	OpenSourceFile(const JCharacter* file, const JSize lineIndex = 0,
						   const JBoolean askDebuggerWhenRelPath = kJTrue);
	void	ReportUnreadableSourceFile(const JCharacter* fileName) const;

	void	DisassembleFunction(const JCharacter* fn, const JCharacter* addr = NULL);

	void	LoadConfig();
	void	SaveConfig();

	void	DisplayExpression(const JCharacter* expr);
	void	Display1DArray(const JCharacter* expr);
	void	Display2DArray(const JCharacter* expr);
	void	Plot1DArray(const JCharacter* expr);

	void	CloseDynamicDirectors();

	JXTextMenu*	CreateDebugMenu(JXMenuBar* menuBar);
	void		AddDebugMenuItemsToToolBar(JXToolBar* toolBar, JXTextMenu* debugMenu,
										   const JBoolean includeStepAsm);
	void		AdjustDebugMenu(JXTextMenu* menu);
	void		UpdateDebugMenu(JXTextMenu* menu, JXTEBase* te1, JXTEBase* te2);
	void		HandleDebugMenu(JXTextMenu* menu, const JIndex index, JXTEBase* te1, JXTEBase* te2);

	void		CreateWindowsMenuAndToolBar(JXMenuBar* menuBar, JXToolBar* toolBar,
											const JBoolean includeStepAsm,
											const JBoolean includeCmdLine,
											const JBoolean includeCurrSrc,
											JXTextMenu* debugMenu, JXTextMenu* prefsMenu,
											JXTextMenu* helpMenu, const JIndex tocCmd,
											const JIndex thisWindowCmd);
	void		AddWindowsMenuItemsToToolBar(JXToolBar* toolBar,
											 JXTextMenu* windowsMenu,
											 const JBoolean includeCmdLine,
											 const JBoolean includeCurrSrc);

	CMSourceDirector*	GetCurrentSourceDir();
	CMThreadsDir*		GetThreadsDir();
	CMStackDir*			GetStackDir();
	CMBreakpointsDir*	GetBreakpointsDir();
	CMVarTreeDir*		GetVarTreeDir();
	CMLocalVarsDir*		GetLocalVarsDir();
	CMRegistersDir*		GetRegistersDir();
	CMFileListDir*		GetFileListDir();
	CMDebugDir*			GetDebugDir();

	// called by CMSourceText

	void	TransferKeyPressToInput(const int key, const JXKeyModifiers& modifiers);

	// called by dynamically created directors

	void	SourceWindowClosed(CMSourceDirector* dir);

	void	DirectorCreated(CMArray1DDir* dir);
	void	DirectorDeleted(CMArray1DDir* dir);

	void	DirectorCreated(CMArray2DDir* dir);
	void	DirectorDeleted(CMArray2DDir* dir);

	void	DirectorCreated(CMPlot2DDir* dir);
	void	DirectorDeleted(CMPlot2DDir* dir);

	void	DirectorCreated(CMMemoryDir* dir);
	void	DirectorDeleted(CMMemoryDir* dir);

	// called by CMDockManager

	void	DockAll(JXDockWidget* srcDock, JXDockWidget* infoDock,
					JXDockWidget* dataDock);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	CMLink*				itsLink;
	CMSourceDirector*	itsCurrentSourceDir;
	CMSourceDirector*	itsCurrentAsmDir;
	CMThreadsDir*		itsThreadsDir;
	CMStackDir*			itsStackDir;
	CMBreakpointsDir*	itsBreakpointsDir;
	CMVarTreeDir*		itsVarTreeDir;
	CMLocalVarsDir*		itsLocalVarsDir;
	CMRegistersDir*		itsRegistersDir;
	CMFileListDir*		itsFileListDir;
	CMDebugDir*			itsDebugDir;

	JPtrArray<CMSourceDirector>*	itsSourceDirs;
	JPtrArray<CMSourceDirector>*	itsAsmDirs;
	JPtrArray<CMArray1DDir>*		itsArray1DDirs;
	JPtrArray<CMArray2DDir>*		itsArray2DDirs;
	JPtrArray<CMPlot2DDir>*			itsPlot2DDirs;
	JPtrArray<CMMemoryDir>*			itsMemoryDirs;

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsDebugMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

	JIndex		itsHistoryIndex;
	JString		itsCurrentHistoryFile;
	JString		itsCurrentCommand;
	JBoolean	itsWaitingToRunFlag;

	CMGetInitArgs*	itsGetArgsCmd;		// NULL except at startup; deleted by CMLink

// begin JXLayout

	JXToolBar*           itsToolBar;
	JXTextButton*        itsProgramButton;
	JXDownRect*          itsDownRect;
	JXStaticText*        itsFakePrompt;
	CMCommandInput*      itsCommandInput;
	JXStringHistoryMenu* itsHistoryMenu;
	JXInputField*        itsArgInput;

// end JXLayout

	CMCommandOutputDisplay*	itsCommandOutput;

private:

	JXMenuBar*	BuildWindow();
	void		UpdateWindowTitle(const JCharacter* programName);

	void	HandleUserInput();
	void	HandleCompletionRequest();
	void	ShowHistoryCommand(const JInteger delta);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	SaveInCurrentFile();
	void	SaveInNewFile();

	// Debug menu

	void	ChangeProgram();
	void	ReloadProgram();
	void	ChooseCoreFile();
	void	ChooseProcess();

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMCommandDirector(const CMCommandDirector& source);
	const CMCommandDirector& operator=(const CMCommandDirector& source);
};


/******************************************************************************
 Directors

 *****************************************************************************/

inline CMSourceDirector*
CMCommandDirector::GetCurrentSourceDir()
{
	return itsCurrentSourceDir;
}

inline CMThreadsDir*
CMCommandDirector::GetThreadsDir()
{
	return itsThreadsDir;
}

inline CMStackDir*
CMCommandDirector::GetStackDir()
{
	return itsStackDir;
}

inline CMBreakpointsDir*
CMCommandDirector::GetBreakpointsDir()
{
	return itsBreakpointsDir;
}

inline CMVarTreeDir*
CMCommandDirector::GetVarTreeDir()
{
	return itsVarTreeDir;
}

inline CMLocalVarsDir*
CMCommandDirector::GetLocalVarsDir()
{
	return itsLocalVarsDir;
}

inline CMRegistersDir*
CMCommandDirector::GetRegistersDir()
{
	return itsRegistersDir;
}

inline CMFileListDir*
CMCommandDirector::GetFileListDir()
{
	return itsFileListDir;
}

inline CMDebugDir*
CMCommandDirector::GetDebugDir()
{
	return itsDebugDir;
}

/******************************************************************************
 Track dynamically created directors

 ******************************************************************************/

inline void
CMCommandDirector::SourceWindowClosed
	(
	CMSourceDirector* dir
	)
{
	itsSourceDirs->Remove(dir);
	itsAsmDirs->Remove(dir);
}

inline void
CMCommandDirector::DirectorCreated
	(
	CMArray1DDir* dir
	)
{
	itsArray1DDirs->Append(dir);
}

inline void
CMCommandDirector::DirectorDeleted
	(
	CMArray1DDir* dir
	)
{
	itsArray1DDirs->Remove(dir);
}

inline void
CMCommandDirector::DirectorCreated
	(
	CMArray2DDir* dir
	)
{
	itsArray2DDirs->Append(dir);
}

inline void
CMCommandDirector::DirectorDeleted
	(
	CMArray2DDir* dir
	)
{
	itsArray2DDirs->Remove(dir);
}

inline void
CMCommandDirector::DirectorCreated
	(
	CMPlot2DDir* dir
	)
{
	itsPlot2DDirs->Append(dir);
}

inline void
CMCommandDirector::DirectorDeleted
	(
	CMPlot2DDir* dir
	)
{
	itsPlot2DDirs->Remove(dir);
}

inline void
CMCommandDirector::DirectorCreated
	(
	CMMemoryDir* dir
	)
{
	itsMemoryDirs->Append(dir);
}

inline void
CMCommandDirector::DirectorDeleted
	(
	CMMemoryDir* dir
	)
{
	itsMemoryDirs->Remove(dir);
}

#endif
