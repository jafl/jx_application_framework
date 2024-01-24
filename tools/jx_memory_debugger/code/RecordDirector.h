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

	~RecordDirector() override;

private:

	JXPSPrinter*	itsPrinter;
	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsPrefsMenu;

// begin JXLayout

	JXToolBar*   itsToolBar;
	RecordTable* itsRecordTable;

// end JXLayout

private:

	void	BuildWindow(RecordList* recordList, const JString& windowTitle);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	HandlePrefsMenu(const JIndex index);
};

#endif
