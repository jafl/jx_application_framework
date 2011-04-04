/******************************************************************************
 SCCircuitDocument.h

	Interface for the SCCircuitDocument class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCCircuitDocument
#define _H_SCCircuitDocument

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXFileDocument.h>

class SCCircuit;
class SCCircuitVarList;
class SCVarListDirector;
class SCDirectorBase;
class SCPlotDirector;

class JXTextMenu;
class JXStaticText;

class SCCircuitDocument : public JXFileDocument
{
public:

	SCCircuitDocument(JXDirector* supervisor, const JCharacter* fileName);

	virtual ~SCCircuitDocument();

	const SCCircuit*		GetCircuit() const;
	const SCCircuitVarList*	GetVarList() const;

	static FileStatus	CanReadFile(istream& input);

	void	OpenSomething(const JCharacter* fileName = NULL);

	JBoolean		HasPlots() const;
	SCPlotDirector*	GetPlot(const JIndex index) const;
	void			BuildPlotMenu(JXTextMenu* menu) const;

	// called by sub-directors

	void	ExprCreated(SCDirectorBase* dir);
	void	PlotCreated(SCPlotDirector* dir);

protected:

	virtual void	DiscardChanges();
	virtual void	WriteTextFile(ostream& output, const JBoolean safetySave) const;
	virtual void	DirectorClosed(JXDirector* theDirector);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	SCCircuit*			itsCircuit;
	SCCircuitVarList*	itsVarList;

	JPtrArray<SCDirectorBase>*	itsExprList;
	JPtrArray<SCPlotDirector>*	itsPlotList;
	JBoolean					itsIgnoreNewDirectorFlag;

	SCVarListDirector*	itsVarListDir;

	// owned by the menu bar

	JXTextMenu*	itsHelpMenu;
	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsAnalysisMenu;

// begin JXLayout

	JXStaticText* itsNetlistText;

// end JXLayout

private:

	void	BuildWindow();
	void	CloseAllWindows();

	void	UpdateHelpMenu();
	void	HandleHelpMenu(const JIndex item);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex item);

	void	RevertToSaved();

	void	ReadFile(istream& input);
	void	ReadNetlist(const JCharacter* fileName);

	void	UpdateAnalysisMenu();
	void	HandleAnalysisMenu(const JIndex item);

	static JOrderedSetT::CompareResult
		ComparePlotDirectorTitles(SCPlotDirector* const & p1, SCPlotDirector* const & p2);

	// not allowed

	SCCircuitDocument(const SCCircuitDocument& source);
	const SCCircuitDocument& operator=(const SCCircuitDocument& source);
};


/******************************************************************************
 GetCircuit

 ******************************************************************************/

inline const SCCircuit*
SCCircuitDocument::GetCircuit()
	const
{
	return itsCircuit;
}

/******************************************************************************
 GetVarList

 ******************************************************************************/

inline const SCCircuitVarList*
SCCircuitDocument::GetVarList()
	const
{
	return itsVarList;
}

/******************************************************************************
 ExprCreated

	Called by SCDirectorBase.

 ******************************************************************************/

inline void
SCCircuitDocument::ExprCreated
	(
	SCDirectorBase* dir
	)
{
	itsExprList->Append(dir);
	if (!itsIgnoreNewDirectorFlag)
		{
		DataModified();
		}
}

/******************************************************************************
 PlotCreated

	Called by SCPlotDirector.

 ******************************************************************************/

inline void
SCCircuitDocument::PlotCreated
	(
	SCPlotDirector* dir
	)
{
	itsPlotList->Append(dir);
	if (!itsIgnoreNewDirectorFlag)
		{
		DataModified();
		}
}

/******************************************************************************
 HasPlots

 ******************************************************************************/

inline JBoolean
SCCircuitDocument::HasPlots()
	const
{
	return !itsPlotList->IsEmpty();
}

/******************************************************************************
 GetPlot

 ******************************************************************************/

inline SCPlotDirector*
SCCircuitDocument::GetPlot
	(
	const JIndex index
	)
	const
{
	return itsPlotList->NthElement(index);
}

#endif
