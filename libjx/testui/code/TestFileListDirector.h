/******************************************************************************
 TestFileListDirector.h

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_TestFileListDirector
#define _H_TestFileListDirector

#include <jx-af/jx/JXWindowDirector.h>

class JXTextMenu;
class JXFileListSet;

class TestFileListDirector : public JXWindowDirector
{
public:

	TestFileListDirector(JXDirector* supervisor);

	~TestFileListDirector() override;

private:

	JXTextMenu*		itsFileMenu;

// begin JXLayout

	JXFileListSet* itsFLSet;

// end JXLayout

private:

	void	BuildWindow();
	void	AddDirectory(const JUtf8Byte* path);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);
};

#endif
