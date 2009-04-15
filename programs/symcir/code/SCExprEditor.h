/******************************************************************************
 SCExprEditor.h

	Interface for the SCExprEditor class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCExprEditor
#define _H_SCExprEditor

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXExprEditor.h>

class JVariableValue;
class SCCircuitDocument;
class SCCircuitVarList;
class SCNewPlotDialog;
class SCAddToPlotDialog;

class SCExprEditor : public JXExprEditor
{
public:

	SCExprEditor(SCCircuitDocument* doc, JXMenuBar* menuBar,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	SCExprEditor(SCCircuitDocument* doc, SCExprEditor* menuProvider,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~SCExprEditor();

	JXTextMenu*	GetExtraMenu() const;

	void	ReadState(istream& input, const JFileVersion vers);
	void	WriteState(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	SCCircuitDocument*		itsDocument;		// not owned
	const SCCircuitVarList*	itsVarList;			// not owned
	JXTextMenu*				itsExtraMenu;
	SCNewPlotDialog*		itsNewPlotDialog;
	SCAddToPlotDialog*		itsAddToPlotDialog;

private:

	void	SCExprEditorX(SCCircuitDocument* doc);

	void	UpdateExtraMenu();
	void	HandleExtraMenu(const JIndex item);

	void	AskCreatePlot();
	void	CreatePlot();

	void	AskAddToPlot();
	void	AddToPlot();

	void		SubstituteDefinition();
	JBoolean	SelectionIsVarFn(const JVariableValue** varF) const;

	// not allowed

	SCExprEditor(const SCExprEditor& source);
	const SCExprEditor& operator=(const SCExprEditor& source);
};


/******************************************************************************
 GetExtraMenu

 ******************************************************************************/

inline JXTextMenu*
SCExprEditor::GetExtraMenu()
	const
{
	return itsExtraMenu;
}

#endif
