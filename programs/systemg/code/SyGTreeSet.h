/******************************************************************************
 SyGTreeSet.h

	Interface for the SyGTreeSet class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_SyGTreeSet
#define _H_SyGTreeSet

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

	SyGTreeSet(JXMenuBar* menuBar, const JString& pathName,
			   SyGPathInput* pathInput, JXCurrentPathMenu* pathMenu,
			   SyGTrashButton* trashButton, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	SyGTreeSet(std::istream& input, const JFileVersion vers,
			   JXMenuBar* menuBar, const JString& pathName,
			   SyGPathInput* pathInput, JXCurrentPathMenu* pathMenu,
			   SyGTrashButton* trashButton, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual	~SyGTreeSet();

	SyGFileTreeTable* GetTable() const;

	void	GoToItsPath();
	void	SetWildcardFilter(const JString& filter);
	void	SavePreferences(std::ostream& os);

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
	JXTextButton*			itsEmptyButton;		// nullptr unless is trash can
	JXStaticText*			itsFilterLabel;
	SyGFilterInput*			itsFilterInput;
	JXStringHistoryMenu*	itsFilterHistory;
	JXScrollbarSet*			itsScrollbarSet;
	SyGFileTreeTable*		itsTable;
	SyGPathInput*			itsPathInput;
	JXCurrentPathMenu*		itsPathMenu;

private:

	void SyGTreeSetX(JXMenuBar* menuBar, const JString& pathName,
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
