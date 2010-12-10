/******************************************************************************
 JXVIKeyHandler.cpp

	Class to implement vi keybindings.

	BASE CLASS = JVIKeyHandler

	Copyright © 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXVIKeyHandler.h>
#include <JXTEBase.h>
#include <JXSearchTextDialog.h>
#include <JXScrollbar.h>
#include <jXEventUtil.h>
#include <jXGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXVIKeyHandler::JXVIKeyHandler
	(
	JXTEBase* te
	)
	:
	JVIKeyHandler(te),
	itsJXTE(te)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXVIKeyHandler::~JXVIKeyHandler()
{
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

JBoolean
JXVIKeyHandler::HandleKeyPress
	(
	const JCharacter				key,
	const JBoolean					selectText,
	const JTextEditor::CaretMotion	motion,
	const JBoolean					deleteToTabStop
	)
{
	JBoolean result;
	if (PrehandleKeyPress(key, &result))
		{
		return result;
		}

	if (key == JXCtrl('U') || key == JXCtrl('D'))
		{
		JXScrollbar *h, *v;
		if (itsJXTE->GetScrollbars(&h, &v))
			{
			JSize lineCount = JRound(v->GetPageStepSize() / (JFloat) v->GetStepSize()) / 2;
			if (key == JXCtrl('U'))
				{
				lineCount = -lineCount;
				}

			const JBoolean save = JXTEBase::CaretWillFollowScroll();
			JXTEBase::CaretShouldFollowScroll(kJFalse);
			v->StepLine(lineCount);
			JXTEBase::CaretShouldFollowScroll(save);

			MoveCaretVert(lineCount);
			}
		ClearKeyBuffer();
		return kJTrue;
		}
	else if (key == JXCtrl('B') || key == JXCtrl('F'))
		{
		JXScrollbar *h, *v;
		if (itsJXTE->GetScrollbars(&h, &v))
			{
			const JBoolean save = JXTEBase::CaretWillFollowScroll();
			JXTEBase::CaretShouldFollowScroll(kJTrue);
			v->StepPage(key == JXCtrl('B') ? -1 : +1);
			JXTEBase::CaretShouldFollowScroll(save);
			}
		ClearKeyBuffer();
		return kJTrue;
		}

	else if (key == '/' || key == '?')
		{
		JXSearchTextDialog* dlog = JXGetSearchTextDialog();
		dlog->SetRegexSearch();
		dlog->Activate();
		ClearKeyBuffer();
		return kJTrue;
		}
	else if (key == 'n')
		{
		itsJXTE->SearchForward();
		ClearKeyBuffer();
		return kJTrue;
		}

	else
		{
		return JVIKeyHandler::HandleKeyPress(key, selectText, motion, deleteToTabStop);
		}
}
