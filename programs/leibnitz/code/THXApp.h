/******************************************************************************
 THXApp.h

	Interface for the THXApp class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_THXApp
#define _H_THXApp

#include <JXApplication.h>

class JString;
class JXMenuBar;
class JXTextMenu;
class THXVarList;
class THXVarDirector;
class THXExprDirector;
class THX2DPlotDirector;
class THX2DPlotFunctionDialog;
class THXBaseConvDirector;

class THXApp : public JXApplication
{
public:

	THXApp(int* argc, char* argv[]);

	virtual ~THXApp();

	void	DisplayAbout(const JString& prevVersStr = JString::empty);

	THXVarList*			GetVariableList() const;
	THXExprDirector*	NewExpression(const bool centerOnScreen = false);
	void				New2DPlot(const THX2DPlotDirector* prevPlot = nullptr);
	void				ShowConstants() const;
	void				ShowBaseConversion() const;

	void	BuildPlotMenu(JXTextMenu* menu, const THX2DPlotDirector* origPlot,
						  JIndex* initialChoice) const;

	JXTextMenu*	CreateHelpMenu(JXMenuBar* menuBar, const JUtf8Byte* idNamespace);
	void		UpdateHelpMenu(JXTextMenu* menu);
	void		HandleHelpMenu(JXTextMenu* menu, const JUtf8Byte* windowSectionName,
							   const JIndex index);

	bool	KeyPadIsVisible() const;
	void		SetKeyPadVisible(const bool visible);
	void		ToggleKeyPadVisible();

	static const JUtf8Byte*	GetAppSignature();
	static void				InitStrings();

protected:

	virtual bool	Close() override;
	virtual void		CleanUpBeforeSuddenDeath(const JXDocumentManager::SafetySaveReason reason) override;
	virtual void		DirectorClosed(JXDirector* theDirector) override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsStartupFlag;
	JString		itsStatePath;

	THXVarList*						itsVarList;
	THXVarDirector*					itsVarDirector;
	JPtrArray<THXExprDirector>*		itsExprList;
	bool						itsKeyPadVisibleFlag;
	JPtrArray<THX2DPlotDirector>*	its2DPlotList;
	THX2DPlotFunctionDialog*		its2DPlotFnDialog;	// nullptr unless asking user
	THXBaseConvDirector*			itsBCDirector;

private:

	void	RestoreProgramState();
	void	InitProgramState();
	void	SaveProgramState();

	void	Create2DPlot();

	static JListT::CompareResult
		Compare2DPlotTitles(THX2DPlotDirector* const & p1, THX2DPlotDirector* const & p2);

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

inline bool
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
