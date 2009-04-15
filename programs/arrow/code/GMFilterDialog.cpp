/******************************************************************************
 GMFilterDialog.cc

	BASE CLASS = public JXDialogDirector

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMFilterDialog.h>
#include <GMFilterNameTable.h>
#include <GMFilterConditionTable.h>
#include <GMFilterActionTable.h>
#include <GMFilter.h>
#include <GPrefsMgr.h>
#include <GHelpText.h>

#include <GMGlobals.h>

#include <JXColHeaderWidget.h>
#include <JXHelpManager.h>
#include <JXScrollbarSet.h>
#include <JXTextButton.h>
#include <JXVertPartition.h>
#include <JXWidgetSet.h>
#include <JXWindow.h>

#include <jAssert.h>

const JFileVersion kCurrentPrefsVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GMFilterDialog::GMFilterDialog
	(
	const JPtrArray<GMFilter>& filters
	)
	:
	JXDialogDirector(JXGetApplication(), kJFalse),
	JPrefObject(GGetPrefsMgr(), kGEditFilterDialogID),
	itsHelpButton(NULL)
{
	itsFilters	= new JPtrArray<GMFilter>(JPtrArrayT::kForgetAll);
	assert(itsFilters != NULL);

	const JSize count = filters.GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GMFilter* filter = new GMFilter(*(filters.NthElement(i)));
		assert(filter != NULL);
		itsFilters->Append(filter);
		}

	BuildWindow();

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMFilterDialog::~GMFilterDialog()
{
	JPrefObject::WritePrefs();
	itsFilters->DeleteAll();
	delete itsFilters;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GMFilterDialog::BuildWindow()
{
	JArray<JCoordinate> heights(3);
	heights.AppendElement(130);
	heights.AppendElement(130);
	heights.AppendElement(120);

	const JIndex elasticIndex = 2;

	JArray<JCoordinate> minHeights(3);
	minHeights.AppendElement(80);
	minHeights.AppendElement(80);
	minHeights.AppendElement(110);

// begin JXLayout

    JXWindow* window = new JXWindow(this, 470,450, "");
    assert( window != NULL );
    SetWindow(window);

    itsVertPartition =
        new JXVertPartition(heights, elasticIndex, minHeights, window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 430,390);
    assert( itsVertPartition != NULL );

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::GMFilterDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kVElastic, 85,420, 60,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts(JGetString("cancelButton::GMFilterDialog::shortcuts::JXLayout"));

    JXTextButton* okButton =
        new JXTextButton(JGetString("okButton::GMFilterDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kVElastic, 325,420, 60,20);
    assert( okButton != NULL );
    okButton->SetShortcuts(JGetString("okButton::GMFilterDialog::shortcuts::JXLayout"));

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::GMFilterDialog::JXLayout"), window,
                    JXWidget::kFixedLeft, JXWidget::kVElastic, 205,420, 60,20);
    assert( itsHelpButton != NULL );

// end JXLayout

	window->SetTitle("Edit Filters");
	SetButtons(okButton, cancelButton);
	UseModalPlacement(kJFalse);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsHelpButton);

	// create condition table

	JXContainer* container = itsVertPartition->GetCompartment(3);

// begin actionLayout

    const JRect actionLayout_Frame    = container->GetFrame();
    const JRect actionLayout_Aperture = container->GetAperture();
    container->AdjustSize(430 - actionLayout_Aperture.width(), 120 - actionLayout_Aperture.height());

    JXTextButton* newActionButton =
        new JXTextButton(JGetString("newActionButton::GMFilterDialog::actionLayout"), container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 360,20, 70,20);
    assert( newActionButton != NULL );

    JXTextButton* removeActionButton =
        new JXTextButton(JGetString("removeActionButton::GMFilterDialog::actionLayout"), container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 360,50, 70,20);
    assert( removeActionButton != NULL );

    JXWidgetSet* mboxColHeaderEncl =
        new JXWidgetSet(container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 350,20);
    assert( mboxColHeaderEncl != NULL );

    JXScrollbarSet* mboxScrollbarSet =
        new JXScrollbarSet(container,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 350,100);
    assert( mboxScrollbarSet != NULL );

    JXTextButton* chooseActionButton =
        new JXTextButton(JGetString("chooseActionButton::GMFilterDialog::actionLayout"), container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 360,80, 70,20);
    assert( chooseActionButton != NULL );

    container->SetSize(actionLayout_Frame.width(), actionLayout_Frame.height());

// end actionLayout

	itsActionTable =
		GMFilterActionTable::Create(this, newActionButton, removeActionButton, chooseActionButton,
							  mboxScrollbarSet, mboxScrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 350,100);
	assert( itsActionTable != NULL );

	JXColHeaderWidget* colHeader =
		new JXColHeaderWidget(itsActionTable, mboxScrollbarSet, mboxColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 350,20);
	assert( colHeader != NULL );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, "Action");
	colHeader->SetColTitle(2, "Copy");
	colHeader->SetColTitle(3, "Destination");

	// create condition table

	container = itsVertPartition->GetCompartment(2);

// begin conditionLayout

    const JRect conditionLayout_Frame    = container->GetFrame();
    const JRect conditionLayout_Aperture = container->GetAperture();
    container->AdjustSize(430 - conditionLayout_Aperture.width(), 130 - conditionLayout_Aperture.height());

    JXTextButton* newConditionButton =
        new JXTextButton(JGetString("newConditionButton::GMFilterDialog::conditionLayout"), container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 360,20, 70,20);
    assert( newConditionButton != NULL );

    JXTextButton* removeConditionButton =
        new JXTextButton(JGetString("removeConditionButton::GMFilterDialog::conditionLayout"), container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 360,50, 70,20);
    assert( removeConditionButton != NULL );

    JXWidgetSet* conditionColHeaderEncl =
        new JXWidgetSet(container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 350,20);
    assert( conditionColHeaderEncl != NULL );

    JXScrollbarSet* conditionScrollbarSet =
        new JXScrollbarSet(container,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 350,110);
    assert( conditionScrollbarSet != NULL );

    container->SetSize(conditionLayout_Frame.width(), conditionLayout_Frame.height());

// end conditionLayout

	itsConditionTable =
		GMFilterConditionTable::Create(this, newConditionButton, removeConditionButton,
							  conditionScrollbarSet, conditionScrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 350,110);
	assert( itsConditionTable != NULL );

	colHeader =
		new JXColHeaderWidget(itsConditionTable, conditionScrollbarSet, conditionColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 350,20);
	assert( colHeader != NULL );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, "Type");
	colHeader->SetColTitle(2, "Relation");
	colHeader->SetColTitle(3, "Pattern");

	// create filter table

	container = itsVertPartition->GetCompartment(1);

// begin filterLayout

    const JRect filterLayout_Frame    = container->GetFrame();
    const JRect filterLayout_Aperture = container->GetAperture();
    container->AdjustSize(430 - filterLayout_Aperture.width(), 130 - filterLayout_Aperture.height());

    JXTextButton* newFilterButton =
        new JXTextButton(JGetString("newFilterButton::GMFilterDialog::filterLayout"), container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 360,20, 70,20);
    assert( newFilterButton != NULL );

    JXTextButton* removeFilterButton =
        new JXTextButton(JGetString("removeFilterButton::GMFilterDialog::filterLayout"), container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 360,50, 70,20);
    assert( removeFilterButton != NULL );

    JXWidgetSet* filterColHeaderEncl =
        new JXWidgetSet(container,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 350,20);
    assert( filterColHeaderEncl != NULL );

    JXScrollbarSet* filterScrollbarSet =
        new JXScrollbarSet(container,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,20, 350,110);
    assert( filterScrollbarSet != NULL );

    container->SetSize(filterLayout_Frame.width(), filterLayout_Frame.height());

// end filterLayout

	itsFilterNameTable =
		GMFilterNameTable::Create(this, itsFilters, itsConditionTable, itsActionTable,
							  newFilterButton, removeFilterButton,
							  filterScrollbarSet, filterScrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 350,110);
	assert( itsFilterNameTable != NULL );

	colHeader =
		new JXColHeaderWidget(itsFilterNameTable, filterScrollbarSet, filterColHeaderEncl,
							  JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 350,20);
	assert( colHeader != NULL );
	colHeader->FitToEnclosure();
	colHeader->SetColTitle(1, "Filter");
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GMFilterDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kGFilterHelpName);
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
GMFilterDialog::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
		{
		return;
		}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	itsVertPartition->ReadGeometry(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
GMFilterDialog::WritePrefs
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentPrefsVersion << ' ';

	(GetWindow())->WriteGeometry(output);
	output << ' ';

	itsVertPartition->WriteGeometry(output);
	output << ' ';
}
