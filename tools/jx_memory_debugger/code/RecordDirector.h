/******************************************************************************
 RecordDirector.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#ifndef _H_RecordDirector
#define _H_RecordDirector

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class JXToolBar;
class JXPSPrinter;
class RecordList;
class RecordTable;

class RecordDirector : public JXWindowDirector
{
public:

	RecordDirector(JXDirector* supervisor, RecordList* recordList,
					 const JString& windowTitle);

	virtual	~RecordDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	RecordTable*	itsRecordTable;
	JXPSPrinter*	itsPrinter;
	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	void	BuildWindow(RecordList* recordList, const JString& windowTitle);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	UpdateHelpMenu();
	void	HandleHelpMenu(const JIndex index);
};

#endif
