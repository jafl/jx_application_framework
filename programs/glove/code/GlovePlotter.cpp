/******************************************************************************
 GlovePlotter.cpp

	BASE CLASS = JX2DPlotWidget

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GlovePlotter.h"
#include "GloveHistoryDir.h"
#include "GloveModule.h"
#include "GLPlotApp.h"
#include "GLGlobals.h"

#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXWindowPainter.h>
#include <jXKeysym.h>

#include <JProcess.h>
#include <JOutPipeStream.h>
#include <JUserNotification.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

#undef new
#undef delete

static const JCharacter* kModuleMenuTitleStr = "Cursor modules";
static const JCharacter* kModuleMenuStr = "Reload modules %l";

enum
{
	kReloadCursorModuleCmd = 1
};

const int kASCIIZero = 48;

/******************************************************************************
 Constructor

 ******************************************************************************/

GlovePlotter::GlovePlotter
	(
	GloveHistoryDir* 	sessionDir,
	JXMenuBar* 			menuBar,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JX2DPlotWidget(menuBar, enclosure, hSizing, vSizing, x, y, w, h)
{
	itsSessionDir = sessionDir;
	JXTextMenu* cursorMenu = GetCursorMenu();
	cursorMenu->ShowSeparatorAfter(cursorMenu->GetItemCount());
	cursorMenu->AppendItem(kModuleMenuTitleStr);
	
	itsModuleMenu = new JXTextMenu(cursorMenu, cursorMenu->GetItemCount(), menuBar);
	assert( itsModuleMenu != NULL );
	itsModuleMenu->SetMenuItems(kModuleMenuStr);
	itsModuleMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsModuleMenu);
	
	itsIsProcessingCursor = kJFalse;
	ListenTo(this);
	UpdateModuleMenu();
	itsLink = NULL;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GlovePlotter::~GlovePlotter()
{
	if (itsIsProcessingCursor)
		{
		delete itsCursorProcess;
		delete itsLink;
		}
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GlovePlotter::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsModuleMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleModuleMenu(selection->GetIndex());
		}
		
	else if (message.Is(JProcess::kFinished))
		{
		delete itsCursorProcess;
		delete itsLink;
		itsLink = NULL;
		itsIsProcessingCursor = kJFalse;
		}

	else if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
		{
		if (itsLink->HasMessages())
			{
			JSize count = itsLink->GetMessageCount();
			for (JSize i = 1; i <= count; i++)
				{
				JString str;
				if (itsLink->GetNextMessage(&str))
					{
					if (itsCursorFirstPass)
						{
						JCharacter c = str.GetCharacter(1);
						int val = c - kASCIIZero;
						if (val == kGloveFail)
							{
							str.RemoveSubstring(1,2);
							str.Prepend("Module error: ");
							JGetUserNotification()->ReportError(str);
							}
						itsCursorFirstPass = kJFalse;
						}
					else
						{
						itsSessionDir->AppendText(str);
						}
					}
				}
			}
		}
		
	else
		{
		JX2DPlotWidget::Receive(sender, message);
		}
}

/*******************************************************************************
 UpdateModuleMenu
 

 ******************************************************************************/

void
GlovePlotter::UpdateModuleMenu()
{
	const JSize mCount = itsModuleMenu->GetItemCount();
	JSize i;
	for (i = 2; i <= mCount; i++)
		{
		itsModuleMenu->RemoveItem(2);
		}

	(GLGetApplication())->ReloadCursorModules();
	JPtrArray<JString>* names = (GLGetApplication())->GetCursorModules();
	for (i = 1; i <= names->GetElementCount(); i++)
		{
		itsModuleMenu->AppendItem(*(names->NthElement(i)));
		}
}

/*******************************************************************************
 HandleModuleMenu
 

 ******************************************************************************/

void
GlovePlotter::HandleModuleMenu
	(
	const JIndex index
	)
{
	if (index == 1)
		{
		UpdateModuleMenu();
		}
		
	else
		{
		JString modName;
		(GLGetApplication())->GetCursorModulePath(index - 1, &modName);
		int inFD;
		int outFD;
		JError err = 
			JProcess::Create(&itsCursorProcess, modName,
							kJCreatePipe, &outFD,
							kJCreatePipe, &inFD,
							kJIgnoreConnection, NULL);
		assert(err.OK());
		JOutPipeStream* op = new JOutPipeStream(outFD, kJTrue);
		assert( op != NULL );
		assert( op->good() );

		itsLink = new ProcessLink(inFD);
		assert(itsLink != NULL);
		ListenTo(itsLink);
		ListenTo(itsCursorProcess);

		itsIsProcessingCursor = kJTrue;
		*op << XCursorVisible();
		*op << YCursorVisible();
		*op << DualCursorsVisible() << " ";
		if (XCursorVisible())
			{
//			*op << itsXCursorVal1 << " ";
			if (DualCursorsVisible())
				{
//				*op << itsXCursorVal2 << " ";
				}
			}
		if (YCursorVisible())
			{
//			*op << itsYCursorVal1 << " ";
			if (DualCursorsVisible())
				{
//				*op << itsYCursorVal2 << " ";
				}
			}
		delete op;
		itsCursorFirstPass = kJTrue;
		}
}
