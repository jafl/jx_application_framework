/******************************************************************************
 GNBNoteDir.cc

	BASE CLASS = JXDocument

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 ******************************************************************************/

#include "GNBNoteDir.h"
#include "GNBNoteEditor.h"
#include "GNBTreeDir.h"
#include "GNBGlobals.h"
#include "GNBPrefsMgr.h"
#include <GNBApp.h>
#include "GNBPTPrinter.h"

#include "filefloppy.xpm"
#include "fileprint.xpm"

#include <JXToolBar.h>

#include <JXWindow.h>
#include <JXScrollbarSet.h>
#include <JXMenu.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXApplication.h>
#include <JXDocumentManager.h>
#include <JXDocumentMenu.h>
#include <JXChooseSaveFile.h>
#include <JXImage.h>

#include <JString.h>
#include <JChooseSaveFile.h>

#include <jx_plain_file_small.xpm>

#include <jStreamUtil.h>

#include <iostream.h>
#include <jAssert.h>

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"   Import HTML file... %i ImportHTML::Notebook"
	"%l|Export note...      %i ExportNote::Notebook"
	"%l|Edit tool bar...    %i EditToolBar::Notebook"
	"%l|Print %k Meta-P     %i PrintNote::Notebook"
	"  |Close %k Meta-W     %i CloseNote::Notebook";

enum
{
	kImportHTMLCmd = 1,
	kExportCmd,
	kEditToolBarCmd,
	kPrintCmd,
	kCloseCmd
};

const JCharacter* kFileListMenuTitleStr = "Windows";

const JSize kMaxTitleLength	= 40;

const JCoordinate kCloseButtonWidth = 50;

const JFileVersion kCurrentEditorDataVersion	= 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

GNBNoteDir::GNBNoteDir
	(
	GNBTreeDir* supervisor,
	const JFAID_t id
	)
	:
	JXDocument(supervisor),
	itsTextChanged(kJFalse),
	itsID(id),
	itsIsTempForPrinting(kJFalse),
	itsDir(supervisor)
{
	BuildWindow();
	std::string data;
	JFAID jid(id);
	itsDir->GetFileArray()->GetElement(jid, &data);
	if (!data.length() == 0)
		{
		std::istringstream is(data);
		JFileVersion version	= 0;
		if (is.peek() == '\01')
			{
			is.ignore();
			is >> version;
			}
		if (version == 0)
			{
			GetWindow()->ReadGeometry(is);
			JString text;
			JReadAll(is, &text);
			itsEditor->SetText(text);
			}
		else if (version == 1)
			{
			GetWindow()->ReadGeometry(is);
			itsEditor->ReadPrivateFormat(is);
			}
		else if (version == 2)
			{
			GetWindow()->ReadGeometry(is);
			JBoolean isEmpty;
			is >> isEmpty;
			if (!isEmpty)
				{
				itsEditor->ReadPrivateFormat(is);
				}
			}

		const JString& text	= itsEditor->GetText();

		JIndex findex;
		if (text.LocateSubstring("\n", &findex))
			{
			if (findex > 1)
				{
				itsTitle = text.GetSubstring(1, findex - 1);
				if (itsTitle.GetLength() > kMaxTitleLength)
					{
					itsTitle.RemoveSubstring(kMaxTitleLength, itsTitle.GetLength());
					}
				}
			}
		else
			{
			itsTitle = text;
			itsTitle.TrimWhitespace();
			}
		if (!itsTitle.IsEmpty())
			{
			JString wTitle = "Note: " + itsTitle;
			GetWindow()->SetTitle(wTitle);
			}
		}
}

GNBNoteDir::GNBNoteDir
	(
	GNBTreeDir* supervisor
	)
	:
	JXDocument(supervisor),
	itsTextChanged(kJFalse),
	itsIsTempForPrinting(kJTrue),
	itsDir(supervisor)
{
	BuildWindow();
	itsEditor->SetPTPrinter(GNBGetAltPTPrinter());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GNBNoteDir::~GNBNoteDir()
{
}

/******************************************************************************
 BuildWindow

	This is a convenient and organized way of putting all of the initial
	elements into a window. This will keep the constructor less cluttered.

 ******************************************************************************/

void
GNBNoteDir::BuildWindow()
{
	JCoordinate w = 485;
	JCoordinate h = 320;
	JPoint dtl;
//	JBoolean foundWindowPref = gjdbApp->GetCmdWindowSize(&dtl, &w, &h);
	JXWindow* window = new JXWindow(this, w,h, "Note");
	assert( window != NULL );
	window->SetMinSize(300,200);
	window->SetCloseAction(JXWindow::kDeactivateDirector);
	window->SetWMClass(GNBGetWMClassInstance(), GNBGetEditorWindowClass());

//	if (foundWindowPref)
//	 	{
//		window->Place(dtl.x, dtl.y);
//		}

	itsMenuBar =
		new JXMenuBar(window, JXWidget::kHElastic, JXWidget::kFixedTop,
						0,0, w - kCloseButtonWidth,kJXDefaultMenuBarHeight);
	assert( itsMenuBar != NULL );

	itsFileMenu = itsMenuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	JXImage* image = new JXImage(GetDisplay(), JXPM(filefloppy));
	assert(image != NULL);
	itsFileMenu->SetItemImage(kExportCmd, image, kJTrue);

	image = new JXImage(GetDisplay(), JXPM(fileprint));
	assert(image != NULL);
	itsFileMenu->SetItemImage(kPrintCmd, image, kJTrue);

	itsCloseButton =
		new JXTextButton("Close", window,
					JXWidget::kFixedRight, JXWidget::kFixedTop,
					w - kCloseButtonWidth,0,
					kCloseButtonWidth,kJXDefaultMenuBarHeight);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts("#W^[");
	ListenTo(itsCloseButton);

	itsToolBar =
		new JXToolBar(GNBGetPrefsMgr(), kGEditorToolBarID,
			itsMenuBar, 300, 200,
			window, JXWidget::kHElastic, JXWidget::kVElastic,
			0,kJXDefaultMenuBarHeight, w,h - kJXDefaultMenuBarHeight);
	assert( itsToolBar != NULL );

	JSize newHeight = itsToolBar->GetWidgetEnclosure()->GetBoundsHeight();

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(itsToolBar->GetWidgetEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,
			w,newHeight);
	assert( scrollbarSet != NULL );

	itsEditor =
		new GNBNoteEditor(this, itsMenuBar,
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0, 0, 10, 10);
	assert( itsEditor != NULL );
	itsEditor->FitToEnclosure(kJTrue, kJTrue);
	ListenTo(itsEditor);

	JXDocumentMenu* fileListMenu =
		new JXDocumentMenu(kFileListMenuTitleStr, itsMenuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != NULL );
	itsMenuBar->AppendMenu(fileListMenu);

	itsToolBar->LoadPrefs();

	JBoolean empty = itsToolBar->IsEmpty();
	if (empty)
		{
		itsToolBar->AppendButton(itsFileMenu, kExportCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kPrintCmd);
		itsEditor->LoadDefaultToolBar(itsToolBar);
		}

}

/******************************************************************************
 Receive (virtual protected)


 ******************************************************************************/

void
GNBNoteDir::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsEditor && message.Is(JTextEditor::kTextChanged))
		{
		itsTextChanged = kJTrue;
		}
	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Close();
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
GNBNoteDir::UpdateFileMenu()
{
}

/******************************************************************************
 HandleFileMenu

 ******************************************************************************/

void
GNBNoteDir::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kImportHTMLCmd)
		{
		JString filename;
		if (JGetChooseSaveFile()->ChooseFile("", "", &filename))
			{
			ifstream is(filename);
			if (is.good())
				{
				itsEditor->PasteHTML(is);
				}
			}
		}
	else if (index == kExportCmd)
		{
		Export();
		}
	else if (index == kEditToolBarCmd)
		{
		itsToolBar->Edit();
		}
	else if (index == kPrintCmd)
		{
		Print();
		}
	else if (index == kCloseCmd)
		{
		Close();
		}
}

/******************************************************************************
 Print (public)

 ******************************************************************************/

void
GNBNoteDir::Print()
{
	itsEditor->PrintPT();
}

/******************************************************************************
 Export (public)

 ******************************************************************************/

void
GNBNoteDir::Export()
{
	JString name;
	if (JXGetChooseSaveFile()->SaveFile("Choose file name:", "", "", &name))
		{
		itsEditor->WritePlainText(name, JTextEditor::kUNIXText);
		}
}

/******************************************************************************
 OKToClose

 ******************************************************************************/

JBoolean
GNBNoteDir::OKToClose()
{
	if (!itsIsTempForPrinting)
		{
		Save(kJTrue);
		}
	return kJTrue;
}

/******************************************************************************
 NeedsSave

 ******************************************************************************/

JBoolean
GNBNoteDir::NeedsSave()
	const
{
	return itsTextChanged;
}

/******************************************************************************
 SafetySave

 ******************************************************************************/

void
GNBNoteDir::SafetySave
	(
	const JXDocumentManager::SafetySaveReason reason
	)
{
	JBoolean tellApp = kJFalse;
	if (reason == JXDocumentManager::kTimer)
		{
		tellApp = kJTrue;
		}
	if (!itsIsTempForPrinting)
		{
		Save(tellApp);
		}
}

/******************************************************************************
 DiscardChanges

 ******************************************************************************/

void
GNBNoteDir::DiscardChanges()
{

}

/******************************************************************************
 GetMenuIcon (virtual)

	Override of JXDocument::GetMenuIcon().

 ******************************************************************************/

JXPM
GNBNoteDir::GetMenuIcon()
	const
{
	return JXPM(jx_plain_file_small);
}

/******************************************************************************
 Save (public)

 ******************************************************************************/

void
GNBNoteDir::Save
	(
	const JBoolean tellApp
	)
{
	JFAID id(itsID);
	std::ostringstream os;
	os << '\01' << ' ';
	os << kCurrentEditorDataVersion << ' ';

	GetWindow()->WriteGeometry(os);

	JBoolean isEmpty	= itsEditor->GetText().IsEmpty();

	os << isEmpty << ' ';

	if (!isEmpty)
		{
		itsEditor->WritePrivateFormat(os);
		}

	itsDir->GetFileArray()->SetElement(id, os);

	itsTextChanged = kJFalse;
	itsEditor->DeactivateCurrentUndo();
	if (tellApp)
		{
		itsDir->NeedsSave();
		const JString& text = itsEditor->GetText();
		JIndex findex;
		JString title;
		if (text.LocateSubstring("\n", &findex))
			{
			if (findex > 1)
				{
				title = text.GetSubstring(1, findex);
				if (title.GetLength() > kMaxTitleLength)
					{
					title.RemoveSubstring(kMaxTitleLength, title.GetLength());
					}
				}
			}
		else
			{
			title = text;
			}
		title.TrimWhitespace();
		if (title != itsTitle)
			{
			itsTitle = title;
			JString wTitle = "Note";
			if (!title.IsEmpty())
				{
				wTitle = "Note: " + title;
				}
			GetWindow()->SetTitle(wTitle);
			itsDir->NoteNameChanged(itsID, title);
			}
		}
}

/******************************************************************************
 Append (public)

 ******************************************************************************/

void
GNBNoteDir::Append
	(
	const JCharacter* text
	)
{
	JSize count = itsEditor->GetTextLength();
	if (count != 0)
		{
		itsEditor->SetCaretLocation(count+1);
		if (itsEditor->GetText().GetCharacter(count) != '\n')
			{
			itsEditor->Paste("\n\n");
			}
		}

	itsEditor->Paste(text);
}

void
GNBNoteDir::Append
	(
	const JFAID_t id
	)
{
	std::string data;
	JFAID jid(id);
	itsDir->GetFileArray()->GetElement(jid, &data);
	if (!data.length() == 0)
		{
		std::istringstream is(data);
		GetWindow()->ReadGeometry(is);
		JString text;
		JReadAll(is, &text);
		Append(text);
		}
}

/******************************************************************************
 PrintFinished (public)

 ******************************************************************************/

void
GNBNoteDir::PrintFinished()
{
 if (itsIsTempForPrinting)
	{
	Close();
	}
}

/******************************************************************************
 MakeTempForPrinting (public)

 ******************************************************************************/

void
GNBNoteDir::MakeTempForPrinting()
{
	itsIsTempForPrinting = kJTrue;
}

/******************************************************************************
 OKToRevert

 *****************************************************************************/

JBoolean
GNBNoteDir::OKToRevert()
{
	return kJFalse;
}

/******************************************************************************
 CanRevert

 *****************************************************************************/

JBoolean
GNBNoteDir::CanRevert()
{
	return kJFalse;
}
