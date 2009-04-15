/******************************************************************************
 THXApp.h

	Interface for the THXApp class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THXApp
#define _H_THXApp

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXApplication.h>

class JString;
class JXMenuBar;
class JXTextMenu;
class THXVarList;
class THXVarDirector;
class THXExprDirector;
class THX2DPlotDirector;
class THX2DPlotFunctionDialog;
class THX3DPlotDirector;
class THX3DPlotFunctionDialog;
class THXBaseConvDirector;

class THXApp : public JXApplication
{
public:

	THXApp(int* argc, char* argv[]);

	virtual ~THXApp();

	void	DisplayAbout(const JCharacter* prevVersStr = NULL);

	THXVarList*			GetVariableList() const;
	THXExprDirector*	NewExpression(const JBoolean centerOnScreen = kJFalse);
	void				New2DPlot(const THX2DPlotDirector* prevPlot = NULL);
	void				New3DPlot(const THX3DPlotDirector* prevPlot = NULL);
	void				ShowConstants() const;
	void				ShowBaseConversion() const;

	void	BuildPlotMenu(JXTextMenu* menu, const THX2DPlotDirector* origPlot,
						  JIndex* initialChoice) const;

	JXTextMenu*	CreateHelpMenu(JXMenuBar* menuBar, const JCharacter* idNamespace);
	void		UpdateHelpMenu(JXTextMenu* menu);
	void		HandleHelpMenu(JXTextMenu* menu, const JCharacter* windowSectionName,
							   const JIndex index);

	JBoolean	KeyPadIsVisible() const;
	void		SetKeyPadVisible(const JBoolean visible);
	void		ToggleKeyPadVisible();

	static const JCharacter*	GetAppSignature();
	static void					InitStrings();

protected:

	virtual JBoolean	Close();
	virtual void		CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason);
	virtual void		DirectorClosed(JXDirector* theDirector);
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean	itsStartupFlag;
	JString*	itsStatePath;

	THXVarList*						itsVarList;
	THXVarDirector*					itsVarDirector;
	JPtrArray<THXExprDirector>*		itsExprList;
	JBoolean						itsKeyPadVisibleFlag;
	JPtrArray<THX2DPlotDirector>*	its2DPlotList;
	THX2DPlotFunctionDialog*		its2DPlotFnDialog;	// NULL unless asking user
	JPtrArray<THX3DPlotDirector>*	its3DPlotList;
	THX3DPlotFunctionDialog*		its3DPlotFnDialog;	// NULL unless asking user
	THXBaseConvDirector*			itsBCDirector;

private:

	void	RestoreProgramState();
	void	InitProgramState();
	void	SaveProgramState();

	void	Create2DPlot();
	void	Create3DPlot();

	static JOrderedSetT::CompareResult
		Compare2DPlotTitles(THX2DPlotDirector* const & p1, THX2DPlotDirector* const & p2);
	static JOrderedSetT::CompareResult
		Compare3DPlotTitles(THX3DPlotDirector* const & p1, THX3DPlotDirector* const & p2);

	// not allowed

	THXApp(const THXApp& source);
	const THXApp& operator=(const THXApp& source);
};


/******************************************************************************
 GetVariableList

 ******************************************************************************/

inline THXVarList*
THXApp::GetVariableList()
	const
{
	return itsVarList;
}

/******************************************************************************
 KeyPad visible

 ******************************************************************************/

inline JBoolean
THXApp::KeyPadIsVisible()
	const
{
	return itsKeyPadVisibleFlag;
}

inline void
THXApp::ToggleKeyPadVisible()
{
	SetKeyPadVisible(!itsKeyPadVisibleFlag);
}

#endif
