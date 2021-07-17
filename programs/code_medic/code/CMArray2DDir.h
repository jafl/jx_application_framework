/******************************************************************************
 CMArray2DDir.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMArray2DDir
#define _H_CMArray2DDir

#include <JXWindowDirector.h>
#include "CMMemoryDir.h"	// need defn of DisplayType
#include <JString.h>
#include <JRange.h>
#include <JPoint.h>

class JStringTableData;
class JXTextButton;
class JXTextMenu;
class JXRowHeaderWidget;
class JXColHeaderWidget;
class CMLink;
class CMArray2DTable;
class CMArray2DCommand;
class CMArrayExprInput;
class CMArrayIndexInput;
class CMCommandDirector;

class CMArray2DDir : public JXWindowDirector
{
public:

	CMArray2DDir(CMCommandDirector* supervisor, const JString& expr);
	CMArray2DDir(std::istream& input, const JFileVersion vers,
				 CMCommandDirector* supervisor);

	virtual	~CMArray2DDir();

	const JString&	GetExpression() const;
	bool		ArrayRowIndexValid(const JInteger index) const;
	bool		ArrayColIndexValid(const JInteger index) const;

	void	Display1DArray();
	void	Plot1DArray();
	void	Display2DArray();
	void	WatchExpression();
	void	WatchLocation();
	void	ExamineMemory(const CMMemoryDir::DisplayType type);

	virtual void		Activate() override;
	virtual bool	GetMenuIcon(const JXImage** icon) const override;

	void	StreamOut(std::ostream& output);

	// called by CMArray2DCommand

	void	UpdateNext();
	JString	GetExpression(const JPoint& cell) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CMLink*				itsLink;
	CMCommandDirector*	itsCommandDir;
	JStringTableData*	itsData;
	JString				itsExpr;
	JIntRange			itsRowDisplayRange;
	JIntRange			itsRowRequestRange;
	JIntRange			itsColDisplayRange;
	JIntRange			itsColRequestRange;
	bool			itsShouldUpdateFlag;
	JArray<JIntRange>*	itsRowUpdateList;
	JArray<JIntRange>*	itsColUpdateList;
	CMArray2DCommand*	itsUpdateCmd;
	bool			itsWaitingForReloadFlag;

	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsActionMenu;
	JXTextMenu*			itsHelpMenu;

// begin JXLayout

	CMArrayExprInput*  itsExprInput;
	CMArrayIndexInput* itsRowStartIndex;
	CMArrayIndexInput* itsRowEndIndex;
	CMArrayIndexInput* itsColStartIndex;
	CMArrayIndexInput* itsColEndIndex;

// end JXLayout

// begin tableLayout

	CMArray2DTable*    itsTable;
	JXColHeaderWidget* itsColHeader;
	JXRowHeaderWidget* itsRowHeader;

// end tableLayout

private:

	void	CMArray2DDirX(CMCommandDirector* supervisor);
	void	BuildWindow();
	void	UpdateWindowTitle();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	void	FitToRanges();
	void	SetRowTitles(const JIntRange& r);
	void	SetColTitles(const JIntRange& r);

	void	ShouldUpdate(const bool update);
	void	UpdateAll();
	void	BeginRowUpdate(const JIntRange& r);
	void	BeginColUpdate(const JIntRange& r);
	void	BeginUpdate(JArray<JIntRange>* list, const JIntRange& range);

	// not allowed

	CMArray2DDir(const CMArray2DDir& source);
	const CMArray2DDir& operator=(const CMArray2DDir& source);
};


/******************************************************************************
 GetExpression

 ******************************************************************************/

inline const JString&
CMArray2DDir::GetExpression()
	const
{
	return itsExpr;
}

/******************************************************************************
 Array index valid

 ******************************************************************************/

inline bool
CMArray2DDir::ArrayRowIndexValid
	(
	const JInteger index
	)
	const
{
	return itsRowDisplayRange.Contains(index);
}

inline bool
CMArray2DDir::ArrayColIndexValid
	(
	const JInteger index
	)
	const
{
	return itsColDisplayRange.Contains(index);
}

/******************************************************************************
 BeginRowUpdate (private)

 ******************************************************************************/

inline void
CMArray2DDir::BeginRowUpdate
	(
	const JIntRange& range
	)
{
	BeginUpdate(itsRowUpdateList, range);
}

/******************************************************************************
 BeginColUpdate (private)

 ******************************************************************************/

inline void
CMArray2DDir::BeginColUpdate
	(
	const JIntRange& range
	)
{
	BeginUpdate(itsColUpdateList, range);
}

#endif
