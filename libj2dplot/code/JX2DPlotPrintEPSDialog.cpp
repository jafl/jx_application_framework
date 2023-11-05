/******************************************************************************
 JX2DPlotPrintEPSDialog.cpp

	BASE CLASS = JXEPSPrintSetupDialog

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JX2DPlotPrintEPSDialog.h"
#include "J2DPlotWidget.h"
#include "JX2DPlotEPSPrinter.h"
#include <jx-af/jcore/JPSPrinter.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXFloatInput.h>
#include <jx-af/jx/JXFileInput.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

// Unit menu

static const JUtf8Byte* kUnitMenuStr = "mm %r | cm %r | inches %r";

static const JFloat kUnitToPixel[] =
{
	1.0,										// kPixels (internal)
	JPSPrinterBase::kPixelsPerInch / 254.0,		// kMillimeters
	JPSPrinterBase::kPixelsPerInch / 2.54,		// kCentimeters
	JPSPrinterBase::kPixelsPerInch / 1.0		// kInches
};

// Predefined Sizes menu

static const JUtf8Byte* kPredefSizeMenuStr =
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

static bool kPredefInit = false;
static JCoordinate kPredefWidth  [ kPaperTypeCount * kPredefSizeCount ];
static JCoordinate kPredefHeight [ kPaperTypeCount * kPredefSizeCount ];

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

JX2DPlotPrintEPSDialog*
JX2DPlotPrintEPSDialog::Create
	(
	const JString&		fileName,
	const bool		printPreview,
	const bool		bw,
	const JCoordinate	w,
	const JCoordinate	h,
	const Unit			unit
	)
{
	auto* dlog = jnew JX2DPlotPrintEPSDialog;
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

		for (JUnsignedOffset i=0; i<kPaperTypeCount; i++)
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

		kPredefInit = true;
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JX2DPlotPrintEPSDialog::~JX2DPlotPrintEPSDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JX2DPlotPrintEPSDialog::BuildWindow
	(
	const JString&		fileName,
	const bool		printPreview,
	const bool		bw,
	const JCoordinate	w,
	const JCoordinate	h,
	const Unit			unit
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,190, JString::empty);

	auto* chooseFileButton =
		jnew JXTextButton(JGetString("chooseFileButton::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 80,20);
	assert( chooseFileButton != nullptr );
	chooseFileButton->SetShortcuts(JGetString("chooseFileButton::JX2DPlotPrintEPSDialog::shortcuts::JXLayout"));

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 230,160, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::JX2DPlotPrintEPSDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 80,160, 70,20);
	assert( cancelButton != nullptr );
	cancelButton->SetShortcuts(JGetString("cancelButton::JX2DPlotPrintEPSDialog::shortcuts::JXLayout"));

	auto* bwCB =
		jnew JXTextCheckbox(JGetString("bwCB::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 190,120, 150,20);
	assert( bwCB != nullptr );
	bwCB->SetShortcuts(JGetString("bwCB::JX2DPlotPrintEPSDialog::shortcuts::JXLayout"));

	auto* previewCB =
		jnew JXTextCheckbox(JGetString("previewCB::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,120, 130,20);
	assert( previewCB != nullptr );
	previewCB->SetShortcuts(JGetString("previewCB::JX2DPlotPrintEPSDialog::shortcuts::JXLayout"));

	auto* fileInput =
		jnew JXFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 100,20, 250,20);

	itsWidthInput =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,60, 60,20);

	itsHeightInput =
		jnew JXFloatInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 70,80, 60,20);

	auto* widthLabel =
		jnew JXStaticText(JGetString("widthLabel::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 50,20);
	assert( widthLabel != nullptr );
	widthLabel->SetToLabel();

	auto* heightLabel =
		jnew JXStaticText(JGetString("heightLabel::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,80, 50,20);
	assert( heightLabel != nullptr );
	heightLabel->SetToLabel();

	itsUnitMenu =
		jnew JXTextMenu(JGetString("itsUnitMenu::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 130,70, 60,20);
	assert( itsUnitMenu != nullptr );

	itsPredefSizeMenu =
		jnew JXTextMenu(JGetString("itsPredefSizeMenu::JX2DPlotPrintEPSDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 220,65, 120,30);
	assert( itsPredefSizeMenu != nullptr );

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
	itsUnitMenu->SetToPopupChoice(true, unit);
	itsUnitMenu->AttachHandlers(this,
		&JX2DPlotPrintEPSDialog::UpdateUnitMenu,
		&JX2DPlotPrintEPSDialog::HandleUnitMenu);

	// predefined sizes

	itsPredefSizeMenu->SetMenuItems(kPredefSizeMenuStr);
	itsPredefSizeMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPredefSizeMenu->AttachHandler(this, &JX2DPlotPrintEPSDialog::HandlePredefSizeMenu);

	// last, because file chooser may open, and that runs FTC

	SetObjects(okButton, cancelButton, fileInput, fileName, chooseFileButton,
			   previewCB, printPreview, bwCB, bw);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
JX2DPlotPrintEPSDialog::OKToDeactivate()
{
	if (!JXEPSPrintSetupDialog::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		return true;
	}
	else if (!itsWidthInput->InputValid())
	{
		itsWidthInput->Focus();
		return false;
	}
	else if (!itsHeightInput->InputValid())
	{
		itsHeightInput->Focus();
		return false;
	}

	JCoordinate w,h;
	GetSize(&w, &h);
	if (w < 50 || h < 50)
	{
		JGetUserNotification()->ReportError(JGetString("TooSmall::JX2DPlotPrintEPSDialog"));
		return false;
	}
	else
	{
		return true;
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
 SetParameters (virtual)

	Derived classes can override this to extract extra information.

 ******************************************************************************/

bool
JX2DPlotPrintEPSDialog::SetParameters
	(
	JXEPSPrinter* p
	)
	const
{
	bool changed = JXEPSPrintSetupDialog::SetParameters(p);

	auto* pp = dynamic_cast<JX2DPlotEPSPrinter*>(p);
	if (pp != nullptr)
	{
		JCoordinate w,h;
		GetSize(&w, &h);

		const auto r = pp->GetPlotBounds();
		changed      = changed || w != r.width() || h != r.height();

		pp->SetPlotSize(w, h, itsUnit);
	}

	return changed;
}

/******************************************************************************
 GetSize (private)

 ******************************************************************************/

void
JX2DPlotPrintEPSDialog::GetSize
	(
	JCoordinate* w,
	JCoordinate* h
	)
	const
{
JFloat v;

	bool ok = itsWidthInput->GetValue(&v);
	assert( ok );
	*w = JRound(v * kUnitToPixel [ itsUnit ]);

	ok = itsHeightInput->GetValue(&v);
	assert( ok );
	*h = JRound(v * kUnitToPixel [ itsUnit ]);
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
