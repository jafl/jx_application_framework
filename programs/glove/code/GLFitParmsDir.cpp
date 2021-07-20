/******************************************************************************
 GLFitParmsDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#include "GLFitParmsDir.h"
#include "GLFitParmsTable.h"
#include "GLFitBase.h"
#include "GLPlotDir.h"
#include "GLHistoryDir.h"
#include "GLGlobals.h"
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JPtrArray-JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLFitParmsDir::GLFitParmsDir
	(
	GLPlotDir* supervisor,
	JPtrArray<GLFitBase>* fits
	)
	:
	JXWindowDirector(supervisor)
{
	itsPlotDir = supervisor;
	itsFits = fits;
	ListenTo(itsFits);

	auto* window = jnew JXWindow(this, 260,240, "Fit Parameters");
	assert( window != nullptr );

	window->LockCurrentSize();

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,260,190);

	itsTable =
		jnew GLFitParmsTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,0,260,190);
	assert (itsTable != nullptr);

	itsFitMenu =
		jnew JXTextMenu("Fit", window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			10, 210, 90, 20);
	assert( itsFitMenu != nullptr );
	itsFitMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFitMenu);
	UpdateFitMenu();

	itsCloseButton =
		jnew JXTextButton("Close", window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			90, 210, 70, 20);
	assert( itsCloseButton != nullptr );
	itsCloseButton->SetShortcuts("#W");
	ListenTo(itsCloseButton);

	itsSessionButton =
		jnew JXTextButton("Session", window,
			JXWidget::kHElastic, JXWidget::kVElastic,
			180, 210, 70, 20);
	assert( itsSessionButton != nullptr );
	ListenTo(itsSessionButton);

}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLFitParmsDir::~GLFitParmsDir()
{
}

/******************************************************************************
 ShowFit

 ******************************************************************************/

void
GLFitParmsDir::ShowFit
	(
	const JIndex index
	)
{
	HandleFitMenu(index);
}

/******************************************************************************
 Receive

 ******************************************************************************/

void
GLFitParmsDir::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsFits &&
		( 	message.Is(JListT::kElementsInserted) ||
			message.Is(JListT::kElementsRemoved)))
		{
		UpdateFitMenu();
		}

	else if (sender == itsCloseButton && message.Is(JXButton::kPushed))
		{
		Deactivate();
		}

	else if (sender == itsSessionButton && message.Is(JXButton::kPushed))
		{
		SendToSession(itsCurrentIndex);
		}

	else if (sender == itsFitMenu && message.Is(JXMenu::kItemSelected))
		{
		const auto* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFitMenu(selection->GetIndex());
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateFitMenu

 ******************************************************************************/

void
GLFitParmsDir::UpdateFitMenu()
{
	itsFitMenu->RemoveAllItems();

	const JSize count = itsFits->GetElementCount();
	if (count == 0)
		{
		itsFitMenu->Deactivate();
		}
	else
		{
		itsFitMenu->Activate();
		}

	GLBuildColumnMenus("FitMenuItem::GLGlobal", count, itsFitMenu, nullptr);
}

/******************************************************************************
 HandleFitMenu

 ******************************************************************************/

void
GLFitParmsDir::HandleFitMenu
	(
	const JIndex index
	)
{
	assert ( index <= itsFits->GetElementCount());
	itsCurrentIndex = index;
	itsTable->Clear();
	GLFitBase* fit = itsFits->GetElement(index);
	const JSize count = fit->GetParameterCount();
	itsTable->Append("Function:", fit->GetFitFunctionString());
	JSize i;
	for (i = 1; i <= count; i++)
		{
		JString str;
		fit->GetParameterName(i, &str);
		JFloat value;
		fit->GetParameter(i, &value);
		JString pstr = str + ":";
		if ((value < 0.001) || (value > 100000))
			{
			itsTable->Append(pstr, JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5));
			}
		else
			{
			itsTable->Append(pstr, JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5));
			}
		if (fit->GetParameterError(i, &value))
			{
			str += " error:";
			if ((value < 0.001) || (value > 100000))
				{
				itsTable->Append(str, JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5));
				}
			else
				{
				itsTable->Append(str, JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5));
				}
			}
		}
	if (fit->HasGoodnessOfFit())
		{
		JString str;
		fit->GetGoodnessOfFitName(&str);
		JFloat value;
		if (fit->GetGoodnessOfFit(&value))
			{
			str += ":";
			if ((value < 0.001) || (value > 100000))
				{
				itsTable->Append(str, JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5));
				}
			else
				{
				itsTable->Append(str, JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5));
				}
			}
		}
}

/******************************************************************************
 SendAllToSession

 ******************************************************************************/

void
GLFitParmsDir::SendAllToSession()
{
	JSize count = itsFits->GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		SendToSession(i);
		}
}

/******************************************************************************
 SendToSession

 ******************************************************************************/

void
GLFitParmsDir::SendToSession
	(
	const JIndex index
	)
{
	assert ( index <= itsFits->GetElementCount());
	GLHistoryDir* dir = itsPlotDir->GetSessionDir();
	GLFitBase* fit = itsFits->GetElement(index);
	const JSize count = fit->GetParameterCount();

	JString str((JUInt64) index);
	const JUtf8Byte* map[] =
		{
		"i", str.GetBytes()
		};
	str = JGetString("FitMenuItem::GLGlobal", map, sizeof(map));
	dir->AppendText(str);

	str = "Function: " + fit->GetFitFunctionString();
	dir->AppendText(str);

	for (JIndex i = 1; i <= count; i++)
		{
		fit->GetParameterName(i, &str);
		JFloat value;
		fit->GetParameter(i, &value);
		JString pstr = str + ": ";
		if ((value < 0.001) || (value > 100000))
			{
			pstr = pstr + JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5);
			}
		else
			{
			pstr = pstr + JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5);
			}
		dir->AppendText(pstr);
		if (fit->GetParameterError(i, &value))
			{
			str += " error: ";
			if ((value < 0.001) || (value > 100000))
				{
				str = str + JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5);
				}
			else
				{
				str = str + JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5);
				}
			dir->AppendText(str);
			}
		}
	if (fit->HasGoodnessOfFit())
		{
		fit->GetGoodnessOfFitName(&str);
		JFloat value;
		if (fit->GetGoodnessOfFit(&value))
			{
			str += ": ";
			if ((value < 0.001) || (value > 100000))
				{
				str = str + JString(value, JString::kPrecisionAsNeeded, JString::kForceExponent, 0, 5);
				}
			else
				{
				str = str + JString(value, JString::kPrecisionAsNeeded, JString::kStandardExponent, 0, 5);
				}
			dir->AppendText(str);
			}
		}
	dir->AppendText(JString::newline);
}
