/******************************************************************************
 JXFileListSet.h

	Interface for the JXFileListSet class

	Copyright © 1998 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JXFileListSet
#define _H_JXFileListSet

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidgetSet.h>

class JXMenuBar;
class JXTEBase;
class JXScrollbarSet;
class JXStringHistoryMenu;
class JXFileListTable;
class JXFLInputBase;
class JXFLRegexInput;
class JXFLWildcardInput;

class JXFileListSet : public JXWidgetSet
{
public:

	enum FilterType
	{
		kNoFilter,
		kWildcardFilter,
		kRegexFilter
	};

public:

	JXFileListSet(JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);
	JXFileListSet(JXMenuBar* menuBar, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);
	JXFileListSet(JXTEBase* te, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~JXFileListSet();

	void	AppendEditMenu(JXMenuBar* menuBar);

	FilterType	GetFilterType() const;
	void		SetFilterType(const FilterType type);
	void		ToggleWildcardFilter();
	void		ToggleRegexFilter();

	JXScrollbarSet*		GetScrollbarSet() const;
	JXFileListTable*	GetTable() const;
	void				SetTable(JXFileListTable* table);

	void	ReadSetup(istream& input);
	void	WriteSetup(ostream& output) const;

private:

	JXFileListTable*		itsTable;
	JXScrollbarSet*			itsTableScroll;

	FilterType				itsFilterType;

	JXWidgetSet*			itsWildcardSet;
	JXFLWildcardInput*		itsWildcardInput;
	JXStringHistoryMenu*	itsWildcardMenu;

	JXWidgetSet*			itsRegexSet;
	JXFLRegexInput*			itsRegexInput;
	JXStringHistoryMenu*	itsRegexMenu;

private:

	void	JXFileListSetX(const JCoordinate w, const JCoordinate h);
	void	ShowFilter(JXWidgetSet* filterSet, JXFLInputBase* filterInput,
					   const FilterType otherType,
					   JXWidgetSet* otherSet, JXFLInputBase* otherInput);

	// not allowed

	JXFileListSet(const JXFileListSet& source);
	const JXFileListSet& operator=(const JXFileListSet& source);
};


/******************************************************************************
 GetTable

 ******************************************************************************/

inline JXFileListTable*
JXFileListSet::GetTable()
	const
{
	return itsTable;
}

/******************************************************************************
 GetFilterType

 ******************************************************************************/

inline JXFileListSet::FilterType
JXFileListSet::GetFilterType()
	const
{
	return itsFilterType;
}

/******************************************************************************
 GetScrollbarSet

 ******************************************************************************/

inline JXScrollbarSet*
JXFileListSet::GetScrollbarSet()
	const
{
	return itsTableScroll;
}

#endif
