/******************************************************************************
 TestFileListDirector.cpp

	Window to display all files found by CBParseFiles().

	BASE CLASS = JXWindowDirector

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "TestFileListDirector.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXFileListSet.h>
#include <jx-af/jx/JXFileListTable.h>
#include <jx-af/jx/jXGlobals.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestFileListDirector::TestFileListDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	BuildWindow();

	AddDirectory("./");
	AddDirectory("./test-file-list");

	JString s1, s2;
	if (JConvertToAbsolutePath("../Make.header", JString::empty, &s1) &&
		JGetTrueName(s1, &s2))
	{
		itsFLSet->GetTable()->AddFile(s2);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestFileListDirector::~TestFileListDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "TestFileListDirector-File.h"

void
TestFileListDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,330, JGetString("WindowTitle::TestFileListDirector::JXLayout"));
	window->SetWMClass(JXGetApplication()->GetWMName().GetBytes(), "TestFileListDirector");

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	itsFLSet =
		jnew JXFileListSet(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);

// end JXLayout

	window->SetMinSize(150, 150);

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::TestFileListDirector_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&TestFileListDirector::UpdateFileMenu,
		&TestFileListDirector::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);
}

/******************************************************************************
 AddDirectory (private)

 ******************************************************************************/

void
TestFileListDirector::AddDirectory
	(
	const JUtf8Byte* path
	)
{
	JDirInfo* info;
	const bool ok = JDirInfo::Create(JString(path, JString::kNoCopy), &info);
	assert( ok );

	JXFileListTable* table = itsFLSet->GetTable();
	for (const auto* e : *info)
	{
		if (e->IsFile())
		{
			table->AddFile(e->GetFullName());
		}
	}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
TestFileListDirector::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kShowLocationCmd,
		itsFLSet->GetTable()->HasFocus() && itsFLSet->GetTable()->HasSelection() );

	const JXFileListSet::FilterType type = itsFLSet->GetFilterType();
	if (type == JXFileListSet::kWildcardFilter)
	{
		itsFileMenu->CheckItem(kUseWildcardCmd);
	}
	else if (type == JXFileListSet::kRegexFilter)
	{
		itsFileMenu->CheckItem(kUseRegexCmd);
	}
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
TestFileListDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kShowLocationCmd)
	{
		itsFLSet->GetTable()->ShowSelectedFileLocations();
	}

	else if (index == kUseWildcardCmd)
	{
		itsFLSet->ToggleWildcardFilter();
	}
	else if (index == kUseRegexCmd)
	{
		itsFLSet->ToggleRegexFilter();
	}

	else if (index == kCloseCmd)
	{
		GetWindow()->Close();
	}
}
