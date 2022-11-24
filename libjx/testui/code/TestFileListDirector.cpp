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
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    Show selected files in file manager %h s %k Ctrl-Return"
	"%l| Use wildcard filter %b"
	"  | Use regex filter    %b"
	"%l| Close                               %h c %k Ctrl-W";

enum
{
	kShowLocationCmd = 1,
	kUseWildcardCmd, kUseRegexCmd,
	kCloseCmd
};

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
	if (JConvertToAbsolutePath(JString("../Make.header", JString::kNoCopy), JString::empty, &s1) &&
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

#include <jx-af/image/jx/jx_filter_wildcard.xpm>
#include <jx-af/image/jx/jx_filter_regex.xpm>

void
TestFileListDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,330, JString::empty);
	assert( window != nullptr );

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	itsFLSet =
		jnew JXFileListSet(menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( itsFLSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TestFileListDirector"));
	window->SetWMClass("testjx", "TestFileListDirector");
	window->SetMinSize(150, 150);

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::TestFileListDirector"));
	itsFileMenu->SetShortcuts(JGetString("FileMenuShortcut::TestFileListDirector"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "TestFileListDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	itsFileMenu->SetItemImage(kUseWildcardCmd, jx_filter_wildcard);
	itsFileMenu->SetItemImage(kUseRegexCmd,    jx_filter_regex);
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
 Receive (virtual protected)

 ******************************************************************************/

void
TestFileListDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
	{
		UpdateFileMenu();
	}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
	{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
	}

	else
	{
		JXWindowDirector::Receive(sender, message);
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
