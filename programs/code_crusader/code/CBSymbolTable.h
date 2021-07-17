/******************************************************************************
 CBSymbolTable.h

	Interface for the CBSymbolTable class

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSymbolTable
#define _H_CBSymbolTable

#include <JXTable.h>

class JError;
class JString;
class JRegex;
class JXImage;
class CBSymbolDirector;
class CBSymbolList;

class CBSymbolTable : public JXTable
{
public:

	CBSymbolTable(CBSymbolDirector* symbolDirector, CBSymbolList* symbolList,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~CBSymbolTable();

	bool	HasSelection() const;
	void		SelectSingleEntry(const JIndex index, const bool scroll = true);
	void		ClearSelection();
	void		CopySelectedSymbolNames() const;
	void		DisplaySelectedSymbols() const;
	void		FindSelectedSymbols(const JXMouseButton button) const;
	void		GetFileNamesForSelection(JPtrArray<JString>* fileNameList,
										 JArray<JIndex>* lineIndexList) const;

	void	ShowAll();
	JError	SetNameFilter(const JString& filterStr, const bool isRegex);
	void	SetDisplayList(const JArray<JIndex>& list);

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual void	HandleFocusEvent() override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBSymbolDirector*	itsSymbolDirector;			// not owned
	CBSymbolList*		itsSymbolList;				// not owned
	JArray<JIndex>*		itsVisibleList;				// index of each visible symbol
	bool			itsVisibleListLockedFlag;	// true => RebuildTable() doesn't change it
	JRegex*				itsNameFilter;				// nullptr if not used
	JString*			itsNameLiteral;				// nullptr if not used

	JSize			itsMaxStringWidth;
	mutable JString	itsKeyBuffer;

private:

	void		RebuildTable();
	bool	CalcColWidths(const JString& symbolName, const JString* signature);
	void		AdjustColWidths();

	JIndex	CellToSymbolIndex(const JPoint& cell) const;

	// not allowed

	CBSymbolTable(const CBSymbolTable& source);
	const CBSymbolTable& operator=(const CBSymbolTable& source);
};

#endif
