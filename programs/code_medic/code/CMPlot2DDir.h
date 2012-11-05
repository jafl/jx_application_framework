/******************************************************************************
 CMPlot2DDir.h

	Copyright � 2009 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMPlot2DDir
#define _H_CMPlot2DDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JStringTableData;
class JXTextButton;
class JXTextMenu;
class JXColHeaderWidget;
class JX2DPlotWidget;
class CMLink;
class CMCommandDirector;
class CMPlot2DExprTable;
class CMPlot2DCommand;

class CMPlot2DDir : public JXWindowDirector
{
public:

	CMPlot2DDir(CMCommandDirector* supervisor, const JCharacter* expr);
	CMPlot2DDir(istream& input, const JFileVersion vers,
				CMCommandDirector* supervisor);

	virtual	~CMPlot2DDir();

	virtual void		Activate();
	virtual JBoolean	GetMenuIcon(const JXImage** icon) const;

	void	StreamOut(ostream& output);

	JString	GetXExpression(const JIndex curveIndex, const JInteger i) const;
	JString	GetYExpression(const JIndex curveIndex, const JInteger i) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	CMLink*						itsLink;
	CMCommandDirector*			itsCommandDir;
	JXColHeaderWidget*			itsColHeader;
	JStringTableData*			itsExprData;
	CMPlot2DExprTable*			itsExprTable;
	JBoolean					itsShouldUpdateFlag;
	JPtrArray<CMPlot2DCommand>*	itsUpdateCmdList;
	JPtrArray<JArray<JFloat> >*	itsXData;
	JPtrArray<JArray<JFloat> >*	itsYData;
	JBoolean					itsWaitingForReloadFlag;

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsActionMenu;
	JXTextMenu*	itsHelpMenu;

// begin JXLayout

	JX2DPlotWidget* itsPlotWidget;
	JXTextButton*   itsAddPlotButton;
	JXTextButton*   itsDuplicatePlotButton;
	JXTextButton*   itsRemovePlotButton;

// end JXLayout

private:

	void	CMPlot2DDirX1(CMCommandDirector* supervisor);
	void	CMPlot2DDirX2();
	void	BuildWindow();
	void	UpdateWindowTitle();
	void	UpdateButtons();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	void	ShouldUpdate(const JBoolean update);
	void	UpdateAll();
	void	Update(const JIndex first, const JIndex last);

	void	DuplicateSelectedPlots();
	void	RemoveSelectedPlots();

	// not allowed

	CMPlot2DDir(const CMPlot2DDir& source);
	const CMPlot2DDir& operator=(const CMPlot2DDir& source);
};

#endif
