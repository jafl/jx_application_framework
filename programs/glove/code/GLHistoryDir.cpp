/******************************************************************************
 GLHistoryDir.cpp

	BASE CLASS = JXWindowDirector

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#include "GLHistoryDir.h"
#include "GLHistory.h"

#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <JXMenu.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXApplication.h>
#include <jXGlobals.h>

#include <JChooseSaveFile.h>
#include <jx_plain_file_small.xpm>
#include <jAssert.h>

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"Save session %k Meta-S"
	"|Save session as... %l|Save session window size"
	"%l|Print %k Meta-P %l|Close %k Meta-W | Quit %k Meta-Q";

enum
{
	kSaveCmd = 1,
	kSaveAsCmd,
	kSaveWindowSizeCmd,
	kPrintCmd,
	kCloseCmd,
	kQuitCmd
};

const JCharacter* GLHistoryDir::kSessionChanged = "kSessionChanged::GLHistoryDir";

/******************************************************************************
 Constructor

 ******************************************************************************/

GLHistoryDir::GLHistoryDir
	(
	JXDirector* supervisor
	)
	:
	JXDocument(supervisor)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLHistoryDir::~GLHistoryDir()
{
}

/******************************************************************************
 BuildWindow

	This is a convenient and organized way of putting all of the initial
	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
GLHistoryDir::BuildWindow()
{
	JCoordinate w = 485;
	JCoordinate h = 320;
	JPoint dtl;
//	JBoolean foundWindowPref = gjdbApp->GetCmdWindowSize(&dtl, &w, &h);
	JXWindow* window = jnew JXWindow(this, w,h, "Glove session");
	assert( window != nullptr );
	window->SetMinSize(300,200);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
//	if (foundWindowPref)
//	 	{
//		window->Place(dtl.x, dtl.y);
//		}

	itsMenuBar =
		jnew JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
						0,0, w,kJXDefaultMenuBarHeight);
	assert( itsMenuBar != nullptr );

	itsFileMenu = itsMenuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kJXDefaultMenuBarHeight,
			w,h - kJXDefaultMenuBarHeight);
	assert( scrollbarSet != nullptr );

	itsHistory =
		jnew GLHistory(itsMenuBar,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( itsHistory != nullptr );

	itsHistory->FitToEnclosure(kJTrue, kJTrue);

	ListenTo(itsHistory);

}

/******************************************************************************
 Receive (virtual protected)


 ******************************************************************************/

void
GLHistoryDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsHistory && message.Is(JTextEditor::kTextChanged))
		{
		Broadcast(SessionChanged());
		}
	else
		{
		JXWindowDirector::Receive(sender,message);
		}
}

/******************************************************************************
 UpdateFileMenu


 ******************************************************************************/

void
GLHistoryDir::UpdateFileMenu()
{
}

/******************************************************************************
 HandleFileMenu


 ******************************************************************************/

void
GLHistoryDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kSaveWindowSizeCmd)
		{
//		gjdbApp->SaveCmdWindowSize(GetWindow());
		}
	else if (index == kCloseCmd)
		{
//		GetWindow()->Iconify();
		Deactivate();
		}
	else if (index == kQuitCmd)
		{
		JXGetApplication()->Quit();
		}
}

/******************************************************************************
 AppendText


 ******************************************************************************/

void
GLHistoryDir::AppendText
	(
	const JCharacter* 	text,
	const JBoolean 		show
	)
{
	JSize count = itsHistory->GetTextLength();
	if (count != 0)
		{
		const JString& history = itsHistory->GetText();
		itsHistory->SetCaretLocation(count+1);
		if (history.GetCharacter(count) != '\n')
			{
			itsHistory->Paste("\n");
			}
		}
	itsHistory->Paste(text);
	if (!IsActive() && show)
		{
		Activate();
		}
}

/******************************************************************************
 Print


 ******************************************************************************/

void
GLHistoryDir::Print()
{
	itsHistory->PrintPT();
}

/******************************************************************************
 WriteData


 ******************************************************************************/

void
GLHistoryDir::WriteData
	(
	std::ostream& os
	)
{
	JString text = itsHistory->GetText();
	os << text << ' ';
}

/******************************************************************************
 ReadData


 ******************************************************************************/

void
GLHistoryDir::ReadData
	(
	std::istream& is
	)
{
	JString text;
	is >> text;
	itsHistory->Paste(text);
}

/******************************************************************************
 OKToClose

 ******************************************************************************/

JBoolean
GLHistoryDir::OKToClose()
{
	return kJTrue;
}

/******************************************************************************
 OKToRevert

 ******************************************************************************/

JBoolean
GLHistoryDir::OKToRevert()
{
	return kJTrue;
}

/******************************************************************************
 CanRevert

 ******************************************************************************/

JBoolean
GLHistoryDir::CanRevert()
{
	return kJTrue;
}

/******************************************************************************
 NeedsSave

 ******************************************************************************/

JBoolean
GLHistoryDir::NeedsSave()
	const
{
	return kJFalse;
}

/******************************************************************************
 SafetySave

 ******************************************************************************/

void
GLHistoryDir::SafetySave
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{

}

/******************************************************************************
 DiscardChanges

 ******************************************************************************/

void
GLHistoryDir::DiscardChanges()
{

}

/******************************************************************************
 GetMenuIcon (virtual)

	Override of JXDocument::GetMenuIcon().

 ******************************************************************************/

JXPM
GLHistoryDir::GetMenuIcon()
	const
{
	return JXPM(jx_plain_file_small);
}