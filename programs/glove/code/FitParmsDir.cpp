/******************************************************************************
 FitParmsDir.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "FitParmsDir.h"
#include "FitParmsTable.h"
#include "JFitBase.h"
#include "PlotDir.h"
#include "GloveHistoryDir.h"
#include <JXWindow.h>
#include <JXTextMenu.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXScrollbarSet.h>
#include <JPtrArray-JString.h>
#include <jAssert.h>

static const JCharacter* kFitMenuTitleStr = "Fit";

/******************************************************************************
 Constructor

 ******************************************************************************/

FitParmsDir::FitParmsDir
	(
	PlotDir* supervisor,
	JPtrArray<JFitBase>* fits
	)
	:
	JXWindowDirector(supervisor)
{
	itsPlotDir = supervisor;
	itsFits = fits;
	ListenTo(itsFits);
	
	JXWindow* window = jnew JXWindow(this, 260,240, "Fit Parameters");
    assert( window != NULL );
    
    window->LockCurrentSize();
	
	JXScrollbarSet* scrollbarSet = 
		jnew JXScrollbarSet(window,
			JXWidget::kHElastic, JXWidget::kVElastic, 
			0,0,260,190);
	
	itsTable = 
		jnew FitParmsTable(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic, 
			0,0,260,190);
	assert (itsTable != NULL);
	
	itsFitMenu = 
		jnew JXTextMenu(kFitMenuTitleStr, window, 
			JXWidget::kHElastic, JXWidget::kVElastic, 
			10, 210, 90, 20);
	assert( itsFitMenu != NULL );
	itsFitMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFitMenu);
	UpdateFitMenu();
	
	itsCloseButton = 
		jnew JXTextButton("Close", window,
			JXWidget::kHElastic, JXWidget::kVElastic, 
			90, 210, 70, 20);
	assert( itsCloseButton != NULL );
	itsCloseButton->SetShortcuts("#W");
	ListenTo(itsCloseButton);
	
	itsSessionButton = 
		jnew JXTextButton("Session", window,
			JXWidget::kHElastic, JXWidget::kVElastic, 
			180, 210, 70, 20);
	assert( itsSessionButton != NULL );
	ListenTo(itsSessionButton);
	
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FitParmsDir::~FitParmsDir()
{
}

/******************************************************************************
 ShowFit

 ******************************************************************************/

void
FitParmsDir::ShowFit
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
FitParmsDir::Receive
	(
	JBroadcaster* sender,
	const JBroadcaster::Message& message
	)
{
	if (sender == itsFits && 
		( 	message.Is(JOrderedSetT::kElementsInserted) ||
			message.Is(JOrderedSetT::kElementsRemoved)))
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
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
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
FitParmsDir::UpdateFitMenu()
{
	itsFitMenu->RemoveAllItems();
	JSize i;
	const JSize count = itsFits->GetElementCount();
	if (count == 0)
		{
		itsFitMenu->Deactivate();
		}
	else
		{
		itsFitMenu->Activate();
		}
	for (i = 1; i <= count; i++)
		{
		JString numS(i);
		JString str = "Fit " + numS;
		itsFitMenu->AppendItem(str);
		}
}

/******************************************************************************
 HandleFitMenu

 ******************************************************************************/

void
FitParmsDir::HandleFitMenu
	(
	const JIndex index
	)
{
	assert ( index <= itsFits->GetElementCount());
	itsCurrentIndex = index;
	itsTable->Clear();
	JFitBase* fit = itsFits->NthElement(index);
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
FitParmsDir::SendAllToSession()
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
FitParmsDir::SendToSession
	(
	const JIndex index
	)
{
	assert ( index <= itsFits->GetElementCount());
	GloveHistoryDir* dir = itsPlotDir->GetSessionDir();
	JFitBase* fit = itsFits->NthElement(index);
	const JSize count = fit->GetParameterCount();
	JString str;
	str = "Fit " + JString(index);
	dir->AppendText(str);
	str = "Function: " + fit->GetFitFunctionString();
	dir->AppendText(str);
	JSize i;
	for (i = 1; i <= count; i++)
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
	dir->AppendText("\n");
}
