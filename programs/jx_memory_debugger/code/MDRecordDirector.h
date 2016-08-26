/******************************************************************************
 MDRecordDirector.h

	Copyright © 2010 by John Lindal. All rights reserved.

 *****************************************************************************/

#ifndef _H_MDRecordDirector
#define _H_MDRecordDirector

#include <JXWindowDirector.h>

class JXTextMenu;
class JXToolBar;
class JXPSPrinter;
class MDRecordList;
class MDRecordTable;

class MDRecordDirector : public JXWindowDirector
{
public:

	MDRecordDirector(JXDirector* supervisor, MDRecordList* recordList,
					 const JCharacter* windowTitle);

	virtual	~MDRecordDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	MDRecordTable*	itsRecordTable;
	JXPSPrinter*	itsPrinter;
	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	void	BuildWindow(MDRecordList* recordList, const JCharacter* windowTitle);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UpdateHelpMenu();
	void	HandleHelpMenu(const JIndex index);

	// not allowed

	MDRecordDirector(const MDRecordDirector& source);
	const MDRecordDirector& operator=(const MDRecordDirector& source);
};

#endif
