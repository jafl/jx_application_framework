/******************************************************************************
 JXVIKeyHandler.cpp

	Class to implement vi keybindings.

	BASE CLASS = JVIKeyHandler

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "JXVIKeyHandler.h"
#include "JXTEBase.h"
#include "JXSearchTextDialog.h"
#include "JXScrollbar.h"
#include "jXEventUtil.h"
#include "jXGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXVIKeyHandler::JXVIKeyHandler()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXVIKeyHandler::~JXVIKeyHandler()
{
}

/******************************************************************************
 Initialize (virtual)

 ******************************************************************************/

void
JXVIKeyHandler::Initialize
	(
	JTextEditor* te
	)
{
	JVIKeyHandler::Initialize(te);

	itsJXTE = dynamic_cast<JXTEBase*>(te);
	assert( itsJXTE != nullptr );
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

bool
JXVIKeyHandler::HandleKeyPress
	(
	const JUtf8Character&			key,
	const bool					selectText,
	const JTextEditor::CaretMotion	motion,
	const bool					deleteToTabStop
	)
{
	bool result;
	if (PrehandleKeyPress(key, &result))
	{
		return result;
	}

	if (key == JXCtrl('U') || key == JXCtrl('D'))
	{
		JXScrollbar *h, *v;
		if (itsJXTE->GetScrollbars(&h, &v))
		{
			JInteger lineCount = JRound(v->GetPageStepSize() / (JFloat) v->GetStepSize()) / 2;
			if (key == JXCtrl('U'))
			{
				lineCount = -lineCount;
			}

			const bool save = JXTEBase::CaretWillFollowScroll();
			JXTEBase::CaretShouldFollowScroll(false);
			v->StepLine(lineCount);
			JXTEBase::CaretShouldFollowScroll(save);

			MoveCaretVert(lineCount);
		}
		ClearKeyBuffers();
		return true;
	}
	else if (key == JXCtrl('B') || key == JXCtrl('F'))
	{
		JXScrollbar *h, *v;
		if (itsJXTE->GetScrollbars(&h, &v))
		{
			const bool save = JXTEBase::CaretWillFollowScroll();
			JXTEBase::CaretShouldFollowScroll(true);
			v->StepPage(key == JXCtrl('B') ? -1 : +1);
			JXTEBase::CaretShouldFollowScroll(save);
		}
		ClearKeyBuffers();
		return true;
	}

	else if (key == '/' || key == '?')
	{
		JXSearchTextDialog* dlog = JXGetSearchTextDialog();
		dlog->SetRegexSearch();
		dlog->Activate();
		ClearKeyBuffers();
		return true;
	}
	else if (key == 'n')
	{
		itsJXTE->SearchForward();
		ClearKeyBuffers();
		return true;
	}

	else
	{
		return JVIKeyHandler::HandleKeyPress(key, selectText, motion, deleteToTabStop);
	}
}
