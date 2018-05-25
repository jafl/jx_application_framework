/******************************************************************************
 JXFileListSet.cpp

	BASE CLASS = public JXWidgetSet

	Copyright (C) 1998 by John Lindal.

 *****************************************************************************/

#include <JXFileListSet.h>
#include <JXFileListTable.h>
#include <JXFLRegexInput.h>
#include <JXFLWildcardInput.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXStringHistoryMenu.h>
#include <JXScrollbarSet.h>
#include <JXMenuBar.h>
#include <jGlobals.h>
#include <jAssert.h>

const JCoordinate kDefaultInputHeight = 20;		// FTC may change
const JCoordinate kDefaultLabelWidth  = 45;		// FTC may change
const JCoordinate kHistoryMenuWidth   = 30;
const JSize kHistoryLength            = 10;

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

JXFileListSet::JXFileListSet
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	JXFileListSetX(w,h);
}

JXFileListSet::JXFileListSet
	(
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
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	JXFileListSetX(w,h);
	AppendEditMenu(menuBar);
}

JXFileListSet::JXFileListSet
	(
	JXTEBase* 			te,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidgetSet(enclosure, hSizing, vSizing, x,y, w,h)
{
	JXFileListSetX(w,h);

	itsWildcardInput->ShareEditMenu(te);
	itsRegexInput->ShareEditMenu(te);
	itsTable->SetEditMenuProvider(te);
}

// private

void
JXFileListSet::JXFileListSetX
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	const JCoordinate inputWidth = w - kDefaultLabelWidth - kHistoryMenuWidth;

	// table

	itsTableScroll = jnew JXScrollbarSet(this, kHElastic, kVElastic, 0,0, w,h);
	assert( itsTableScroll != NULL );

	itsTable =
		jnew JXFileListTable(itsTableScroll, itsTableScroll->GetScrollEnclosure(),
							kHElastic, kVElastic, 0,0, 10,10);
	assert( itsTable != NULL );
	itsTable->FitToEnclosure();

	// wildcard input

	itsWildcardSet = jnew JXWidgetSet(this, kHElastic, kFixedTop, 0,0, w,kDefaultInputHeight);
	assert( itsWildcardSet != NULL );

	JXStaticText* filterLabel =
		jnew JXStaticText(JGetString("FilterLabel::JXFileListSet"),
						  itsWildcardSet, kFixedLeft, kFixedTop,
						  0, 0, kDefaultLabelWidth, kDefaultInputHeight);
	assert( filterLabel != NULL );
	filterLabel->SetToLabel();

	itsWildcardMenu =
		jnew JXStringHistoryMenu(kHistoryLength, JString::empty, itsWildcardSet, kFixedRight, kFixedTop,
								 w - kHistoryMenuWidth,0, kHistoryMenuWidth,kDefaultInputHeight);
	assert( itsWildcardMenu != NULL );

	itsWildcardInput =
		jnew JXFLWildcardInput(this, itsWildcardMenu, itsWildcardSet, kHElastic, kFixedTop,
							   kDefaultLabelWidth,0, inputWidth,kDefaultInputHeight);
	assert( itsWildcardInput != NULL );

	// regex input

	itsRegexSet = jnew JXWidgetSet(this, kHElastic, kFixedTop, 0,0, w,kDefaultInputHeight);
	assert( itsRegexSet != NULL );

	JXStaticText* regexLabel =
		jnew JXStaticText(JGetString("RegexLabel::JXFileListSet"),
						  itsRegexSet, kFixedLeft, kFixedTop,
						  0, 0, kDefaultLabelWidth, kDefaultInputHeight);
	assert( regexLabel != NULL );
	regexLabel->SetToLabel();

	itsRegexMenu =
		jnew JXStringHistoryMenu(kHistoryLength, JString::empty, itsRegexSet, kFixedRight, kFixedTop,
								 w - kHistoryMenuWidth,0, kHistoryMenuWidth,kDefaultInputHeight);
	assert( itsRegexMenu != NULL );

	itsRegexInput =
		jnew JXFLRegexInput(this, itsRegexMenu, itsRegexSet, kHElastic, kFixedTop,
							kDefaultLabelWidth,0, inputWidth,kDefaultInputHeight);
	assert( itsRegexInput != NULL );

	// start with no filter

	itsFilterType = kNoFilter;
	itsRegexSet->Hide();
	itsWildcardSet->Hide();

	SetNeedsInternalFTC();
	itsWildcardSet->SetNeedsInternalFTC();
	itsRegexSet->SetNeedsInternalFTC();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXFileListSet::~JXFileListSet()
{
}

/******************************************************************************
 SetTable

 ******************************************************************************/

void
JXFileListSet::SetTable
	(
	JXFileListTable* table
	)
{
	JXTEBase* te;
	const JBoolean hadEditMenu = itsTable->GetEditMenuProvider(&te);

	jdelete itsTable;
	itsTable = table;
	itsTable->SetSizing(kHElastic, kVElastic);
	itsTable->FitToEnclosure();
	if (hadEditMenu)
		{
		itsTable->SetEditMenuProvider(te);
		}
}

/******************************************************************************
 AppendEditMenu

 ******************************************************************************/

void
JXFileListSet::AppendEditMenu
	(
	JXMenuBar* menuBar
	)
{
	itsWildcardInput->AppendEditMenu(menuBar);
	itsRegexInput->ShareEditMenu(itsWildcardInput);
	itsTable->SetEditMenuProvider(itsWildcardInput);
}

/******************************************************************************
 SetFilterType

 ******************************************************************************/

void
JXFileListSet::SetFilterType
	(
	const FilterType type
	)
{
	if (itsFilterType == type)
		{
		return;
		}

	else if (type == kWildcardFilter)
		{
		ShowFilter(itsWildcardSet, itsWildcardInput,
				   kRegexFilter, itsRegexSet, itsRegexInput);
		}
	else if (type == kRegexFilter)
		{
		ShowFilter(itsRegexSet, itsRegexInput,
				   kWildcardFilter, itsWildcardSet, itsWildcardInput);
		}

	else
		{
		if (itsWildcardInput->HasFocus() || itsRegexInput->HasFocus())
			{
			GetWindow()->KillFocus();
			itsTable->Focus();
			}
		itsTable->ClearFilterRegex();

		const JCoordinate h = itsWildcardInput->GetFrameHeight();	// FTC

		itsWildcardSet->Hide();
		itsRegexSet->Hide();
		itsTableScroll->Move(0,-h);
		itsTableScroll->AdjustSize(0,h);
		}

	itsFilterType = type;
}

/******************************************************************************
 ShowFilter (private)

 ******************************************************************************/

void
JXFileListSet::ShowFilter
	(
	JXWidgetSet*		filterSet,
	JXFLInputBase*		filterInput,
	const FilterType	otherType,
	JXWidgetSet*		otherSet,
	JXFLInputBase*		otherInput
	)
{
	if (itsFilterType == otherType)
		{
		if (otherInput->HasFocus())
			{
			GetWindow()->KillFocus();
			}
		otherSet->Hide();
		}
	else
		{
		const JCoordinate h = itsWildcardInput->GetFrameHeight();	// FTC

		itsTableScroll->AdjustSize(0,-h);
		itsTableScroll->Move(0,h);
		}

	filterSet->Show();
	filterInput->Focus();
	const JError err = filterInput->Apply();
	if (!err.OK())
		{
		itsTable->ClearFilterRegex();
		err.ReportIfError();
		}
}

/******************************************************************************
 ToggleWildcardFilter

 ******************************************************************************/

void
JXFileListSet::ToggleWildcardFilter()
{
	if (itsFilterType != kWildcardFilter)
		{
		SetFilterType(kWildcardFilter);
		}
	else
		{
		SetFilterType(kNoFilter);
		}
}

/******************************************************************************
 ToggleRegexFilter

 ******************************************************************************/

void
JXFileListSet::ToggleRegexFilter()
{
	if (itsFilterType != kRegexFilter)
		{
		SetFilterType(kRegexFilter);
		}
	else
		{
		SetFilterType(kNoFilter);
		}
}

/******************************************************************************
 ReadSetup

	We assert that we find a readable version because it is the responsibility
	of the client to use a higher level version number to check that this
	function will succeed.

 ******************************************************************************/

void
JXFileListSet::ReadSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentSetupVersion );

	JBoolean hadFilter;
	input >> hadFilter;
	if (hadFilter)
		{
		JString regexStr;
		input >> regexStr;
		const JError err = itsTable->SetFilterRegex(regexStr);
		assert_ok( err );
		}
	else
		{
		itsTable->ClearFilterRegex();
		}

	long type;
	input >> type;
	itsFilterType = (FilterType) type;
	assert( itsFilterType == kNoFilter ||
			itsFilterType == kWildcardFilter ||
			itsFilterType == kRegexFilter );

	JString s;
	input >> s;
	itsWildcardInput->GetText()->SetText(s);
	itsWildcardMenu->ReadSetup(input);

	input >> s;
	itsRegexInput->GetText()->SetText(s);
	itsRegexMenu->ReadSetup(input);

	// only need to adjust geometry since table's regex is read separately

	if (itsFilterType != kNoFilter)
		{
		const JCoordinate h = itsWildcardInput->GetFrameHeight();	// FTC

		itsTableScroll->AdjustSize(0,-h);
		itsTableScroll->Move(0,h);

		if (itsFilterType == kWildcardFilter)
			{
			itsWildcardSet->Show();
			}
		else
			{
			assert( itsFilterType == kRegexFilter );
			itsRegexSet->Show();
			}
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
JXFileListSet::WriteSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;

	output << ' ';
	JString regexStr;
	if (itsTable->GetFilterRegex(&regexStr))
		{
		output << kJTrue << ' ' << regexStr;
		}
	else
		{
		output << kJFalse;
		}

	output << ' ' << (long) itsFilterType;

	output << ' ' << itsWildcardInput->GetText();
	output << ' ';
	itsWildcardMenu->WriteSetup(output);

	output << ' ' << itsRegexInput->GetText();
	output << ' ';
	itsRegexMenu->WriteSetup(output);

	output << ' ';
}
