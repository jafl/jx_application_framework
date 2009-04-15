/******************************************************************************
 SyGTreeSet.h

	Interface for the SyGTreeSet class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGTreeSet
#define _H_SyGTreeSet

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidgetSet.h>

class JString;
class JXMenuBar;
class JXTextButton;
class JXStaticText;
class JXScrollbarSet;
class JXContainer;
class JXStringHistoryMenu;
class JXCurrentPathMenu;

class SyGFileTreeTable;
class SyGFileTree;
class SyGFilterInput;
class SyGPathInput;
class SyGTrashButton;

class SyGTreeSet : public JXWidgetSet
{
public:

	SyGTreeSet(JXMenuBar* menuBar, const JCharacter* pathName,
			   SyGPathInput* pathInput, JXCurrentPathMenu* pathMenu,
			   SyGTrashButton* trashButton, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	SyGTreeSet(istream& input, const JFileVersion vers,
			   JXMenuBar* menuBar, const JCharacter* pathName,
			   SyGPathInput* pathInput, JXCurrentPathMenu* pathMenu,
			   SyGTrashButton* trashButton, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual	~SyGTreeSet();

	SyGFileTreeTable* GetTable() const;

	void	GoToItsPath();
	void	SetWildcardFilter(const JCharacter* filter);
	void	SavePreferences(ostream& os);

	JBoolean	FilterVisible() const;
	void		ShowFilter(const JBoolean show);
	void		ToggleFilter();
	void		SaveFilterPref();

	void	UpdateDisplay(const JString& path);

protected:

	virtual void Receive(JBroadcaster* sender, const Message& message);

private:

	SyGFileTree*			itsFileTree;
	JXMenuBar*				itsMenuBar;
	JXTextButton*			itsEmptyButton;		// NULL unless is trash can
	JXStaticText*			itsFilterLabel;
	SyGFilterInput*			itsFilterInput;
	JXStringHistoryMenu*	itsFilterHistory;
	JXScrollbarSet*			itsScrollbarSet;
	SyGFileTreeTable*		itsTable;
	SyGPathInput*			itsPathInput;
	JXCurrentPathMenu*		itsPathMenu;

private:

	void SyGTreeSetX(JXMenuBar* menuBar, const JCharacter* pathName,
					 SyGPathInput* pathInput, JXCurrentPathMenu* pathMenu,
					 SyGTrashButton* trashButton, 
					 const JCoordinate w, const JCoordinate h);

	// not allowed

	SyGTreeSet(const SyGTreeSet& source);
	const SyGTreeSet& operator=(const SyGTreeSet& source);
};

/******************************************************************************
 GetTable

 ******************************************************************************/

inline SyGFileTreeTable*
SyGTreeSet::GetTable()
	const
{
	return itsTable;
}

#endif
