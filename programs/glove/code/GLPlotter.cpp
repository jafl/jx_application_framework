/******************************************************************************
 GLPlotter.cpp

	BASE CLASS = JX2DPlotWidget

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "GLPlotter.h"
#include "GLHistoryDir.h"
#include "GloveModule.h"
#include "GLPlotApp.h"
#include "GLGlobals.h"

#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXWindowPainter.h>

#include <JProcess.h>
#include <JOutPipeStream.h>
#include <JStringIterator.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jAssert.h>

static const JUtf8Byte* kModuleMenuStr = "Reload modules %l";

enum
{
	kReloadCursorModuleCmd = 1
};

const int kASCIIZero = 48;

/******************************************************************************
 Constructor

 ******************************************************************************/

GLPlotter::GLPlotter
	(
	GLHistoryDir* 	sessionDir,
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
	cursorMenu->AppendItem(JGetString("ModuleMenuTitle::GLPlotter"));
	
	itsModuleMenu = jnew JXTextMenu(cursorMenu, cursorMenu->GetItemCount(), menuBar);
	assert( itsModuleMenu != nullptr );
	itsModuleMenu->SetMenuItems(kModuleMenuStr);
	itsModuleMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsModuleMenu);
	
	itsIsProcessingCursor = false;
	ListenTo(this);
	UpdateModuleMenu();
	itsLink = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLPlotter::~GLPlotter()
{
	if (itsIsProcessingCursor)
		{
		jdelete itsCursorProcess;
		delete itsLink;
		}
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GLPlotter::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsModuleMenu && message.Is(JXMenu::kItemSelected))
		{
		 const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleModuleMenu(selection->GetIndex());
		}
		
	else if (message.Is(JProcess::kFinished))
		{
		jdelete itsCursorProcess;
		delete itsLink;
		itsLink = nullptr;
		itsIsProcessingCursor = false;
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
						JUtf8Byte c = str.GetRawBytes()[0];
						int val = c - kASCIIZero;
						if (val == kGloveFail)
							{
							JStringIterator iter(&str);
							iter.SkipNext(2);
							iter.RemoveAllPrev();
							iter.Invalidate();

							str.Prepend(JGetString("ModuleError::GLPlotter"));
							JGetUserNotification()->ReportError(str);
							}
						itsCursorFirstPass = false;
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
GLPlotter::UpdateModuleMenu()
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
		itsModuleMenu->AppendItem(*(names->GetElement(i)));
		}
}

/*******************************************************************************
 HandleModuleMenu
 

 ******************************************************************************/

void
GLPlotter::HandleModuleMenu
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
							kJIgnoreConnection, nullptr);
		assert(err.OK());
		auto* op = jnew JOutPipeStream(outFD, true);
		assert( op != nullptr );
		assert( op->good() );

		itsLink = new ProcessLink(inFD);
		assert(itsLink != nullptr);
		ListenTo(itsLink);
		ListenTo(itsCursorProcess);

		itsIsProcessingCursor = true;
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
		jdelete op;
		itsCursorFirstPass = true;
		}
}
