/******************************************************************************
 CBStylerTableMenu.h

	Interface for the CBStylerTableMenu class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBStylerTableMenu
#define _H_CBStylerTableMenu

#include <JXStyleMenu.h>

class JXStyleTable;

class CBStylerTableMenu : public JXStyleMenu
{
public:

	CBStylerTableMenu(JXStyleTable* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CBStylerTableMenu();

	void	PrepareForPopup(const JPoint& cell);

protected:

	virtual JFontStyle	GetFontStyleForMenuUpdate() const;
	virtual void		HandleMenuItem(const JIndex menuItem);

private:

	JXStyleTable*	itsTable;		// not owned
	JPoint			itsPopupCell;	// cell that was clicked to pop up menu

private:

	void	UpdateStyle(const JIndex index, const JFontStyle& origStyle,
						JFontStyle* style);

	// not allowed

	CBStylerTableMenu(const CBStylerTableMenu& source);
	const CBStylerTableMenu& operator=(const CBStylerTableMenu& source);
};

/******************************************************************************
 PrepareForPopup

 ******************************************************************************/

inline void
CBStylerTableMenu::PrepareForPopup
	(
	const JPoint& cell
	)
{
	itsPopupCell = cell;
}

#endif
