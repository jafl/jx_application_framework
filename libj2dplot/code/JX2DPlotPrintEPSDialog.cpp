/******************************************************************************
 JX2DPlotPrintEPSDialog.cpp

	BASE CLASS = JXEPSPrintSetupDialog

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JX2DPlotPrintEPSDialog.h>
#include <J2DPlotWidget.h>
#include <JPSPrinter.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXStaticText.h>
#include <JXFloatInput.h>
#include <JXFileInput.h>
#include <JXTextMenu.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

// Unit menu

static const JCharacter* kUnitMenuStr = "mm %r | cm %r | inches %r";

static const JFloat kUnitToPixel[] =
{
	1.0,										// kPixels (internal)
	JPSPrinterBase::kPixelsPerInch / 254.0,		// kMillimeters
	JPSPrinterBase::kPixelsPerInch / 2.54,		// kCentimeters
	JPSPrinterBase::kPixelsPerInch / 1.0		// kInches
};

// Predefined Sizes menu

static const JCharacter* kPredefSizeMenuStr =
	"    US letter    %k 1/4 page"
	"  | .            %k 1/2 page"
	"  | .            %k full page"
	"  | .            %k full page (landscape)"
	"%l| US legal     %k 1/4 page"
	"  | .            %k 1/2 page"
	"  | .            %k full page"
	"  | .            %k full page (landscape)"
	"%l| US executive %k 1/4 page"
	"  | .            %k 1/2 page"
	"  | .            %k full page"
	"  | .            %k full page (landscape)"
	"%l| A4 letter    %k 1/4 page"
	"  | .            %k 1/2 page"
	"  | .            %k full page"
	"  | .            %k full page (landscape)"
	"%l| B5 letter    %k 1/4 page"
	"  | .            %k 1/2 page"
	"  | .            %k full page"
	"  | .            %k full page (landscape)";

static const JPSPrinter::PaperType kPaperType[] =
{
	JPSPrinter::kUSLetter,
	JPSPrinter::kUSLegal,
	JPSPrinter::kUSExecutive,
	JPSPrinter::kA4Letter,
	JPSPrinter::kB5Letter
};

const JSize kPaperTypeCount  = sizeof(kPaperType) / sizeof(JPSPrinter::PaperType);
const JSize kPredefSizeCount = 4;

static JBoolean kPredefInit = kJFalse;
static JCoordinate kPredefWidth  [ kPaperTypeCount * kPredefSizeCount ];
static JCoordinate kPredefHeight [ kPaperTypeCount * kPredefSizeCount ];

// string ID's

static const JCharacter* kTooSmallID = "TooSmall::JX2DPlotPrintEPSDialog";

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JX2DPlotPrintEPSDialog*
JX2DPlotPrintEPSDialog::Create
	(
	const JCharacter*	fileName,
	const JBoolean		printPreview,
	const JBoolean		bw,
	const JCoordinate	w,
	const JCoordinate	h,
	const Unit			unit
	)
{
	JX2DPlotPrintEPSDialog* dlog = jnew JX2DPlotPrintEPSDialog;
	assert( dlog != NULL );
	dlog->BuildWindow(fileName, printPreview, bw, w, h, unit);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JX2DPlotPrintEPSDialog::JX2DPlotPrintEPSDialog()
	:
	JXEPSPrintSetupDialog()
{
	if (!kPredefInit)
		{
		assert( kPredefSizeCount == 4 );

		for (JIndex i=0; i<kPaperTypeCount; i++)
			{
			const JCoordinate w = JPSPrinter::GetPaperWidth(kPaperType[i]);
			const JCoordinate h = JPSPrinter::GetPaperHeight(kPaperType[i]);

			const JSize offset = i * kPredefSizeCount;

			kPredefWidth [ offset ] = w/4;
			kPredefHeight[ offset ] = h/4;

			kPredefWidth [ offset + 1 ] = w/2;
			kPredefHeight[ offset + 1 ] = h/2;

			J2DPlotWidget::GetPSPortraitPrintSize(w,h, &kPredefWidth [ offset + 2 ],
													   &kPredefHeight[ offset + 2 ]);

			J2DPlotWidget::GetPSLandscapePrintSize(h,w, &kPredefWidth [ offset + 3 ],
														&kPredefHeight[ offset + 3 ]);
			}

		kPredefInit = kJTrue;
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX2DPlotPrintEPSDialog::~JX2DPlotPrintEPSDialog()
{
}

/******************************************************************************
 GetPlotSize

 ******************************************************************************/

void
JX2DPlotPrintEPSDialog::GetPlotSize
	(
	JCoordinate*	w,
	JCoordinate*	h,
	Unit*			unit
	)
{
JFloat v;

	JBoolean ok = itsWidthInput->GetValue(&v);
	assert( ok );
	*w = JRound(v * kUnitToPixel [ itsUnit ]);

	ok = itsHeightInput->GetValue(&v);
	assert( ok );
	*h = JRound(v * kUnitToPixel [ itsUnit ]);

	*unit = itsUnit;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JX2DPlotPrintEPSDialog::BuildWindow
	(
	const JCharacter*	fileName,
	const JBoolean		printPreview,
	const JBoolean		bw,
	const JCoordinate	w,
	const JCoordinate	h,
	const Unit			unit
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 370,190, "");
	assert( window != NULL );

	JXTextButton* chooseFileButton =
		jnew JXTextButton(JGetString("chooseFileButton::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 80,20);
	assert( chooseFileButton != NULL );
	chooseFileButton->SetShortcuts(JGetString("chooseFileButton::JX2DPlotPrintEPSDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		jnew JXTextButton(JGetString("okButton::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 230,160, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::JX2DPlotPrintEPSDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,160, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::JX2DPlotPrintEPSDialog::shortcuts::JXLayout"));

	JXTextCheckbox* bwCB =
		jnew JXTextCheckbox(JGetString("bwCB::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 190,120, 150,20);
	assert( bwCB != NULL );
	bwCB->SetShortcuts(JGetString("bwCB::JX2DPlotPrintEPSDialog::shortcuts::JXLayout"));

	JXTextCheckbox* previewCB =
		jnew JXTextCheckbox(JGetString("previewCB::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,120, 130,20);
	assert( previewCB != NULL );
	previewCB->SetShortcuts(JGetString("previewCB::JX2DPlotPrintEPSDialog::shortcuts::JXLayout"));

	JXFileInput* fileInput =
		jnew JXFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 100,20, 250,20);
	assert( fileInput != NULL );

	itsWidthInput =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,60, 60,20);
	assert( itsWidthInput != NULL );

	itsHeightInput =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,80, 60,20);
	assert( itsHeightInput != NULL );

	JXStaticText* widthLabel =
		jnew JXStaticText(JGetString("widthLabel::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 50,20);
	assert( widthLabel != NULL );
	widthLabel->SetToLabel();

	JXStaticText* heightLabel =
		jnew JXStaticText(JGetString("heightLabel::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 50,20);
	assert( heightLabel != NULL );
	heightLabel->SetToLabel();

	itsUnitMenu =
		jnew JXTextMenu(JGetString("itsUnitMenu::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 130,70, 60,20);
	assert( itsUnitMenu != NULL );

	itsPredefSizeMenu =
		jnew JXTextMenu(JGetString("itsPredefSizeMenu::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,65, 120,30);
	assert( itsPredefSizeMenu != NULL );

// end JXLayout

	// size

	itsWidthInput->SetValue(w);
	itsWidthInput->SetLowerLimit(0.01);

	itsHeightInput->SetValue(h);
	itsHeightInput->SetLowerLimit(0.01);

	// unit

	itsUnit = kPixels;
	HandleUnitMenu(unit);

	itsUnitMenu->SetMenuItems(kUnitMenuStr);
	itsUnitMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsUnitMenu->SetToPopupChoice(kJTrue, unit);
	ListenTo(itsUnitMenu);

	// predefined sizes

	itsPredefSizeMenu->SetMenuItems(kPredefSizeMenuStr);
	itsPredefSizeMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsPredefSizeMenu);

	// last, because file chooser may open, and that runs FTC

	SetObjects(okButton, cancelButton, fileInput, fileName, chooseFileButton,
			   previewCB, printPreview, bwCB, bw);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
JX2DPlotPrintEPSDialog::OKToDeactivate()
{
	if (!JXEPSPrintSetupDialog::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}
	else if (!itsWidthInput->InputValid())
		{
		itsWidthInput->Focus();
		return kJFalse;
		}
	else if (!itsHeightInput->InputValid())
		{
		itsHeightInput->Focus();
		return kJFalse;
		}

	JCoordinate w,h;
	Unit u;
	GetPlotSize(&w, &h, &u);
	if (w < 50 || h < 50)
		{
		(JGetUserNotification())->ReportError(JGetString(kTooSmallID));
		return kJFalse;
		}
	else
		{
		return kJTrue;
		}
}

/*******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JX2DPlotPrintEPSDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsUnitMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateUnitMenu();
		}
	else if (sender == itsUnitMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleUnitMenu(selection->GetIndex());
		}

	else if (sender == itsPredefSizeMenu && message.Is(JXMenu::kItemSelected))
		{
		 const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandlePredefSizeMenu(selection->GetIndex());
		}

	else
		{
		JXEPSPrintSetupDialog::Receive(sender,message);
		}
}

/*******************************************************************************
 HandlePredefSizeMenu (private)

 ******************************************************************************/

void
JX2DPlotPrintEPSDialog::HandlePredefSizeMenu
	(
	const JIndex index
	)
{
	itsWidthInput ->SetValue(kPredefWidth [ index-1 ]);
	itsHeightInput->SetValue(kPredefHeight [ index-1 ]);

	const Unit origUnit = itsUnit;
	itsUnit             = kPixels;
	HandleUnitMenu(origUnit);
}

/*******************************************************************************
 UpdateUnitMenu (private)

 ******************************************************************************/

void
JX2DPlotPrintEPSDialog::UpdateUnitMenu()
{
	itsUnitMenu->CheckItem(itsUnit);
}

/*******************************************************************************
 HandleUnitMenu (private)

 ******************************************************************************/

void
JX2DPlotPrintEPSDialog::HandleUnitMenu
	(
	const JIndex index
	)
{
	assert( index > kPixels );

	UpdateSize(itsUnit, (Unit) index, itsWidthInput);
	UpdateSize(itsUnit, (Unit) index, itsHeightInput);

	itsUnit = (Unit) index;
}

/*******************************************************************************
 UpdateSize (private)

 ******************************************************************************/

void
JX2DPlotPrintEPSDialog::UpdateSize
	(
	const Unit		origUnit,
	const Unit		newUnit,
	JXFloatInput*	input
	)
{
	JFloat v;
	if (input->GetValue(&v))
		{
		v *= kUnitToPixel [ origUnit ];
		v /= kUnitToPixel [ newUnit ];
		input->SetValue(v);
		}
}

/******************************************************************************
 Stream functions for JX2DPlotPrintEPSDialog::Unit

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&						input,
	JX2DPlotPrintEPSDialog::Unit&	u
	)
{
	long temp;
	input >> temp;
	u = (JX2DPlotPrintEPSDialog::Unit) temp;

	// kPixels is for internal use only
	assert( u == JX2DPlotPrintEPSDialog::kMillimeters ||
			u == JX2DPlotPrintEPSDialog::kCentimeters ||
			u == JX2DPlotPrintEPSDialog::kInches );

	return input;
}

std::ostream&
operator<<
	(
	std::ostream&							output,
	const JX2DPlotPrintEPSDialog::Unit	u
	)
{
	output << (long) u;
	return output;
}
