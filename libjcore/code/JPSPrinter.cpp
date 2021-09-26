/******************************************************************************
 JPSPrinter.cpp

	This class implements the functions required to draw to a PostScript file.

	BASE CLASS = JPagePrinter, JPSPrinterBase

	Copyright (C) 1996-97 by John Lindal & Glenn Bach.

 ******************************************************************************/

#include "jx-af/jcore/JPSPrinter.h"
#include "jx-af/jcore/JString.h"
#include "jx-af/jcore/JLatentPG.h"
#include "jx-af/jcore/jStreamUtil.h"
#include "jx-af/jcore/jGlobals.h"
#include "jx-af/jcore/jAssert.h"

// setup information

const JFileVersion kCurrentSetupVersion = 4;
const JUtf8Byte kSetupDataEndDelimiter = '\1';
// version  1 split data between JPSPrinter and JXPSPrinter
// version  2 added Paper types A4,B5,USExecutive
// version  3 added ending delimiter
// version  4 added file name

// private class data

static const JUtf8Byte* kOrientationStr[] =
{
	"Portrait", "Landscape"
};

// Paper sizes

const JCoordinate kMarginWidth = JPSPrinterBase::kPixelsPerInch/2;

static const JFloat kPaperWidth[] =
{
	8.5, 8.5, 7.5, 8.26,  7.17
};
static const JFloat kPaperHeight[] =
{
	11,  14,  10,  11.7, 10.13
};

/******************************************************************************
 Constructor

	We set the default clip rect in NewPage().

 ******************************************************************************/

JPSPrinter::JPSPrinter
	(
	JFontManager* fontManager
	)
	:
	JPagePrinter(fontManager, JRect()),
	JPSPrinterBase(fontManager)
{
	itsPaperType = kUSLetter;

	itsCopyCount      = 1;
	itsFirstPageIndex = 0;
	itsLastPageIndex  = 0;
	itsPageCount      = 0;

	itsPG = nullptr;

	PSResetCoordinates();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPSPrinter::~JPSPrinter()
{
	if (itsPG != nullptr)
	{
		itsPG->ProcessFinished();
		jdelete itsPG;
	}
}

/******************************************************************************
 WillPrintBlackWhite (virtual)

 ******************************************************************************/

bool
JPSPrinter::WillPrintBlackWhite()
	const
{
	return PSWillPrintBlackWhite();
}

/******************************************************************************
 ImageOrientationChanged (virtual protected)

 ******************************************************************************/

void
JPSPrinter::ImageOrientationChanged
	(
	const ImageOrientation orient
	)
{
	PSResetCoordinates();
}

/******************************************************************************
 ReadPSSetup

 ******************************************************************************/

void
JPSPrinter::ReadPSSetup
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers > 0 );		// JXPSPrinter reads all of version 0

	bool bwFlag = false;
	if (vers <= 3)
	{
		ImageOrientation orient;
		input >> itsPaperType >> orient >> JBoolFromString(bwFlag);
		SetOrientation(orient);
	}
	else if (vers <= kCurrentSetupVersion)
	{
		JString fileName;
		ImageOrientation orient;
		input >> fileName >> itsPaperType >> orient >> JBoolFromString(bwFlag);
		SetOutputFileName(fileName);
		SetOrientation(orient);
	}

	if (vers >= 3)
	{
		JIgnoreUntil(input, kSetupDataEndDelimiter);
	}

	PSPrintBlackWhite(bwFlag);
	PSResetCoordinates();
}

/******************************************************************************
 WritePSSetup

 ******************************************************************************/

void
JPSPrinter::WritePSSetup
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << GetOutputFileName();
	output << ' ' << itsPaperType;
	output << ' ' << GetOrientation();
	output << ' ' << JBoolToString(PSWillPrintBlackWhite());
	output << kSetupDataEndDelimiter;
}

/******************************************************************************
 OpenDocument (virtual)

 ******************************************************************************/

bool
JPSPrinter::OpenDocument()
{
	SetPageIndex(0);
	itsPageCount = 0;

	if (!PSOpenDocument())
	{
		return false;
	}

	assert( itsPG == nullptr );
	itsPG = jnew JLatentPG;
	itsPG->VariableLengthProcessBeginning(JGetString("Printing::JPSPrinter"), true, false);

	return true;
}

/******************************************************************************
 PSPrintVersionComment (virtual protected)

 ******************************************************************************/

void
JPSPrinter::PSPrintVersionComment
	(
	std::ostream& output
	)
{
	output << "%!PS-Adobe-2.0\n";
}

/******************************************************************************
 PSPrintHeaderComments (virtual protected)

 ******************************************************************************/

void
JPSPrinter::PSPrintHeaderComments
	(
	std::ostream& output
	)
{
	output << "%%Pages: (atend)\n"; 
	output << "%%PageOrder: Ascend\n"; 
	output << "%%Orientation: " << kOrientationStr[ GetOrientation() ] << '\n';

	if (itsCopyCount > 1)
	{
		output << "%%Requirements: numcopies(" << itsCopyCount;
		output << ") collate\n";
	}
}

/******************************************************************************
 PSPrintSetupComments (virtual protected)

 ******************************************************************************/

void
JPSPrinter::PSPrintSetupComments
	(
	std::ostream& output
	)
{
	if (itsCopyCount > 1)
	{
		output << "/#copies " << itsCopyCount << " def\n";
	}
}

/******************************************************************************
 NewPage (virtual)

	Returns false if printing was cancelled.

 ******************************************************************************/

bool
JPSPrinter::NewPage()
{
	assert( PSDocumentIsOpen() );

	std::ostream& output = GetOutputStream();
	if (!itsPG->IncrementProgress())
	{
		CancelDocument();
		return false;
	}
	else if (output.fail())
	{
		JGetUserNotification()->ReportError(JGetString("Error::JPSPrinter"));

		CancelDocument();
		return false;
	}

	JIndex pageIndex = GetPageIndex();
	if (pageIndex > 0)
	{
		ClosePage();
	}

	pageIndex++;
	SetPageIndex(pageIndex);

	if (PSShouldPrintCurrentPage())
	{
		output << "%%Page: " << pageIndex << ' ' << itsPageCount+1 << '\n';
		PSSaveGraphicsState();

		if (GetOrientation() == kLandscape)
		{
			output << "90 rotate\n";
			output << "0 " << -GetPaperHeight() << " translate\n";
			PSSaveGraphicsState();		// save again so SetClipRect() doesn't change it
		}
	}

	PSResetCoordinates();
	Reset();

	SetDefaultClipRect(JRect(kMarginWidth, kMarginWidth,
							 GetPaperHeight() - kMarginWidth,
							 GetPaperWidth()  - kMarginWidth));
	SetClipRect(JRect(0,0, GetPageHeight(), GetPageWidth()));

	return true;
}

/******************************************************************************
 ClosePage (private)

 ******************************************************************************/

void
JPSPrinter::ClosePage()
{
	if (PSShouldPrintCurrentPage())
	{
		PSRestoreGraphicsState();
		if (GetOrientation() == kLandscape)
		{
			PSRestoreGraphicsState();	// state had to be saved twice -- see NewPage()
		}

		std::ostream& output = GetOutputStream();
		output << "showpage\n";

		itsPageCount++;
	}
}

/******************************************************************************
 PSShouldPrintCurrentPage (virtual protected)

 ******************************************************************************/

bool
JPSPrinter::PSShouldPrintCurrentPage()
	const
{
	const JIndex currPageIndex = GetPageIndex();
	return currPageIndex > 0 &&
			itsFirstPageIndex <= currPageIndex &&
			(itsLastPageIndex == 0 || currPageIndex <= itsLastPageIndex);
}

/******************************************************************************
 PSResetCoordinates (virtual protected)

 ******************************************************************************/

void
JPSPrinter::PSResetCoordinates()
{
	SetOrigin(kMarginWidth, kMarginWidth);
	SetPageSize(GetPaperWidth()  - 2*kMarginWidth,
				GetPaperHeight() - 2*kMarginWidth);
}

/******************************************************************************
 CloseDocument (virtual)

 ******************************************************************************/

void
JPSPrinter::CloseDocument()
{
	ClosePage();

	std::ostream& output = GetOutputStream();
	output << "%%Pages: " << itsPageCount << '\n';

	PSCloseDocument();

	itsPG->ProcessFinished();
	jdelete itsPG;
	itsPG = nullptr;
}

/******************************************************************************
 CancelDocument (virtual)

 ******************************************************************************/

void
JPSPrinter::CancelDocument()
{
	PSCancelDocument();

	itsPG->ProcessFinished();
	jdelete itsPG;
	itsPG = nullptr;
}

/******************************************************************************
 Get paper size

 ******************************************************************************/

JCoordinate
JPSPrinter::GetPaperWidth()
	const
{
	if (GetOrientation() == kPortrait)
	{
		return JRound(kPaperWidth[ itsPaperType ] * kPixelsPerInch);
	}
	else
	{
		assert( GetOrientation() == kLandscape );
		return JRound(kPaperHeight[ itsPaperType ] * kPixelsPerInch);
	}
}

JCoordinate
JPSPrinter::GetPaperHeight()
	const
{
	if (GetOrientation() == kPortrait)
	{
		return JRound(kPaperHeight[ itsPaperType ] * kPixelsPerInch);
	}
	else
	{
		assert( GetOrientation() == kLandscape );
		return JRound(kPaperWidth[ itsPaperType ] * kPixelsPerInch);
	}
}

JRect
JPSPrinter::GetPaperRect()
	const
{
	return JRect(-kMarginWidth, -kMarginWidth,
				 GetPaperHeight()-kMarginWidth, GetPaperWidth()-kMarginWidth);
}

/******************************************************************************
 Get paper size (static)

	These versions assume portrait orientation.

 ******************************************************************************/

JCoordinate
JPSPrinter::GetPaperWidth
	(
	const PaperType type
	)
{
	return JRound(kPaperWidth[ type ] * kPixelsPerInch);
}

JCoordinate
JPSPrinter::GetPaperHeight
	(
	const PaperType type
	)
{
	return JRound(kPaperHeight[ type ] * kPixelsPerInch);
}

/******************************************************************************
 PSGetOrigin (virtual protected)

 ******************************************************************************/

const JPoint&
JPSPrinter::PSGetOrigin()
	const
{
	return GetOrigin();
}

/******************************************************************************
 PSGetPrintableHeight (virtual protected)

 ******************************************************************************/

JCoordinate
JPSPrinter::PSGetPrintableHeight()
	const
{
	return GetPaperHeight();
}

/******************************************************************************
 JPainter functions

 ******************************************************************************/

/******************************************************************************
 SetClipRect

 ******************************************************************************/

JRect
JPSPrinter::SetClipRect
	(
	const JRect& userRect
	)
{
	const JRect r = JPagePrinter::SetClipRect(userRect);
	PSSetClipRect(r);
	return r;
}

/******************************************************************************
 SetDashList

 ******************************************************************************/

void
JPSPrinter::SetDashList
	(
	const JArray<JSize>&	dashList,
	const JSize				dashOffset
	)
{
	JPagePrinter::SetDashList(dashList, dashOffset);
	PSSetDashList(dashList, dashOffset);
}

/******************************************************************************
 String

 ******************************************************************************/

void
JPSPrinter::StringNoSubstitutions
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JString&		str
	)
{
	JCoordinate ascent, descent;
	GetLineHeight(&ascent, &descent);

	PSString(GetFont(), ascent, 0,0, 0, left,top, str);
}

void
JPSPrinter::String
	(
	const JFloat		userAngle,
	const JCoordinate	left,
	const JCoordinate	top,
	const JString&		str,
	const JCoordinate	width,
	const HAlignment	hAlign,
	const JCoordinate	height,
	const VAlignment	vAlign
	)
{
	JCoordinate ascent, descent;
	GetLineHeight(&ascent, &descent);

	JCoordinate dx = 0, dy = 0;
	AlignString(&dx,&dy, str, width, hAlign, height, vAlign);

	PSString(GetFont(), ascent, dx,dy, userAngle, left,top, str);
}

/******************************************************************************
 Point

 ******************************************************************************/

void
JPSPrinter::Point
	(
	const JCoordinate x,
	const JCoordinate y
	)
{
	const JPoint origPenLoc = GetPenLocation();
	Line(x,y, x+1,y);
	SetPenLocation(origPenLoc);
}

/******************************************************************************
 Line

 ******************************************************************************/

void
JPSPrinter::Line
	(
	const JCoordinate x1,
	const JCoordinate y1,
	const JCoordinate x2,
	const JCoordinate y2
	)
{
	PSLine(x1,y1, x2,y2, GetPenColor(), GetLineWidth(), LinesAreDashed());
	SetPenLocation(x2,y2);
}

/******************************************************************************
 Rect

 ******************************************************************************/

void
JPSPrinter::Rect
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	PSRect(x,y, w,h, GetPenColor(), GetLineWidth(), LinesAreDashed(), IsFilling());
}

/******************************************************************************
 Ellipse

 ******************************************************************************/

void
JPSPrinter::Ellipse
	(
	const JCoordinate x,
	const JCoordinate y,
	const JCoordinate w,
	const JCoordinate h
	)
{
	Arc(x,y, w,h, 0.0, 360.0);
}

/******************************************************************************
 Arc

 ******************************************************************************/

void
JPSPrinter::Arc
	(
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h,
	const JFloat		startAngle,
	const JFloat		deltaAngle
	)
{
	PSArc(x,y, w,h, startAngle, deltaAngle, GetPenColor(),
		  GetLineWidth(), LinesAreDashed(), IsFilling());
}

/******************************************************************************
 Polygon

 ******************************************************************************/

void
JPSPrinter::Polygon
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JPolygon&		poly
	)
{
	PSPolygon(left, top, poly, GetPenColor(),
			  GetLineWidth(), LinesAreDashed(), IsFilling());
}

/******************************************************************************
 Image

 ******************************************************************************/

void
JPSPrinter::Image
	(
	const JImage&	image,
	const JRect&	srcRect,
	const JRect&	destRect
	)
{
	PSColorImage(image, srcRect, destRect);
}

/******************************************************************************
 Global functions for JPSPrinter::PaperType

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&				input,
	JPSPrinter::PaperType&	type
	)
{
	long temp;
	input >> temp;
	type = (JPSPrinter::PaperType) temp;
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&					output,
	const JPSPrinter::PaperType	type
	)
{
	output << (long) type;
	return output;
}
