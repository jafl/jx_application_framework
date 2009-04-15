/******************************************************************************
 JEPSPrinter.cpp

	This class implements the functions required to draw to an
	Encapsulated PostScript file.

	Clients are required to check WantsPreview() before calling
	OpenDocument().  If this returns kJTrue, they should call
	GetPreviewPainter() and draw to the resulting JPainter.
	OpenDocument() then prints the preview automatically.

	Derived classes must implement the following routines:

		GetPreviewPainter
			Create an image, create a painter for it, set the coordinates
			to match the bounds passed to OpenDocument(), and return
			the painter.

		GetPreviewImage
			Return the preview image, if any.

		DeletePreviewData
			Delete the image and painter created in GetPreviewPainter().
			This is only called if GetPreviewImage() returns kJTrue;

	BASE CLASS = JPainter, JPrinter, JPSPrinterBase

	Copyright © 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JEPSPrinter.h>
#include <JImage.h>
#include <JString.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;
const JCharacter kSetupDataEndDelimiter = '\1';

/******************************************************************************
 Constructor

	We set the default clip rect in OpenDocument().

 ******************************************************************************/

JEPSPrinter::JEPSPrinter
	(
	const JFontManager*	fontManager,
	const JColormap*	colormap
	)
	:
	JPainter(fontManager, colormap, JRect(0,0,0,0)),
	JPrinter(),
	JPSPrinterBase(colormap),
	itsBounds(0,0,0,0)
{
	itsIncludePreviewFlag = kJFalse;
	PSResetCoordinates();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JEPSPrinter::~JEPSPrinter()
{
}

/******************************************************************************
 ReadEPSSetup

 ******************************************************************************/

void
JEPSPrinter::ReadEPSSetup
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;

	if (vers <= kCurrentSetupVersion)
		{
		JString fileName;
		JBoolean bwFlag;
		input >> fileName >> itsIncludePreviewFlag >> bwFlag;
		SetOutputFileName(fileName);
		PSPrintBlackWhite(bwFlag);
		}

	JIgnoreUntil(input, kSetupDataEndDelimiter);
}

/******************************************************************************
 WriteEPSSetup

 ******************************************************************************/

void
JEPSPrinter::WriteEPSSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion;
	output << ' ' << GetOutputFileName();
	output << ' ' << itsIncludePreviewFlag;
	output << ' ' << PSWillPrintBlackWhite();
	output << kSetupDataEndDelimiter;
}

/******************************************************************************
 OpenDocument

 ******************************************************************************/

JBoolean
JEPSPrinter::OpenDocument
	(
	const JRect& bounds
	)
{
	assert( !bounds.IsEmpty() );

	itsBounds = bounds;
	PSResetCoordinates();

	if (PSOpenDocument())
		{
		PrintPreview();

		PSSaveGraphicsState();
		Reset();
		SetDefaultClipRect(itsBounds);
		SetClipRect(itsBounds);
		return kJTrue;
		}
	else
		{
		(JGetUserNotification())->ReportError(
			"Unable to create the file.  Please check that the directory "
			"is writable and that the disk is not full.");

		DeletePreviewData();
		return kJFalse;
		}
}

/******************************************************************************
 PSPrintVersionComment (virtual protected)

 ******************************************************************************/

void
JEPSPrinter::PSPrintVersionComment
	(
	ostream& output
	)
{
	output << "%!PS-Adobe-3.0 EPSF-3.0\n";
}

/******************************************************************************
 PSPrintHeaderComments (virtual protected)

 ******************************************************************************/

void
JEPSPrinter::PSPrintHeaderComments
	(
	ostream& output
	)
{
	output << "%%BoundingBox: " << itsBounds.left << " 0 ";
	output << itsBounds.right << ' ' << itsBounds.height() << '\n';
}

/******************************************************************************
 PSPrintSetupComments (virtual protected)

 ******************************************************************************/

void
JEPSPrinter::PSPrintSetupComments
	(
	ostream& output
	)
{
}

/******************************************************************************
 PrintPreview (private)

 ******************************************************************************/

void
JEPSPrinter::PrintPreview()
{
	const JImage* image;
	if (!GetPreviewImage(&image))
		{
		return;
		}

	assert( image->GetBounds() == itsBounds );

	ostream& output = GetOutputStream();

	const JCoordinate w = image->GetWidth();
	const JCoordinate h = image->GetHeight();

	const JSize kBytesPerLine = 100;
	const JCoordinate area    = w*h;
	JSize lineCount = area/kBytesPerLine;
	if (area % kBytesPerLine != 0)
		{
		lineCount++;
		}

	output << "%%BeginPreview: " << w << ' ' << h << " 8 " << lineCount << '\n';

	output.setf(ios::hex, ios::basefield);

	JSize c[3];	// r,g,b
	JIndex lineLength = 0;
	for (JCoordinate y=itsBounds.bottom-1; y>=itsBounds.top; y--)
		{
		for (JCoordinate x=itsBounds.left; x<itsBounds.right; x++)
			{
			if (lineLength == 0)
				{
				output << '%';
				}

			// Intensity formula from X11, Vol 1, p 211

			const JColorIndex color = image->GetColor(x,y);
			PSConvertToRGB(color, &(c[0]), &(c[1]), &(c[2]));
			const JSize intensity = JRound(0.30 * c[0] + 0.59 * c[1] + 0.11 * c[2]);
			if (intensity <= 0x0F)
				{
				output << '0';	// must print two characters
				}
			output << intensity;

			lineLength++;
			if (lineLength >= kBytesPerLine)
				{
				output << '\n';
				lineLength = 0;
				}
			}
		}
	if (lineLength > 0)
		{
		output << '\n';
		}

	output.setf(ios::dec, ios::basefield);

	output << "%%EndPreview\n";

	DeletePreviewData();
}

/******************************************************************************
 CloseDocument

 ******************************************************************************/

void
JEPSPrinter::CloseDocument()
{
	PSRestoreGraphicsState();
	PSCloseDocument();
}

/******************************************************************************
 PSShouldPrintCurrentPage (virtual protected)

 ******************************************************************************/

JBoolean
JEPSPrinter::PSShouldPrintCurrentPage()
	const
{
	return kJTrue;
}

/******************************************************************************
 PSGetOrigin (virtual protected)

 ******************************************************************************/

const JPoint&
JEPSPrinter::PSGetOrigin()
	const
{
	return GetOrigin();
}

/******************************************************************************
 PSResetCoordinates (virtual protected)

 ******************************************************************************/

void
JEPSPrinter::PSResetCoordinates()
{
	SetOrigin(itsBounds.left, itsBounds.top);
}

/******************************************************************************
 PSGetPrintableHeight (virtual protected)

 ******************************************************************************/

JCoordinate
JEPSPrinter::PSGetPrintableHeight()
	const
{
	return itsBounds.bottom;
}

/******************************************************************************
 JPainter functions

 ******************************************************************************/

/******************************************************************************
 SetClipRect

 ******************************************************************************/

JRect
JEPSPrinter::SetClipRect
	(
	const JRect& userRect
	)
{
	const JRect r = JPainter::SetClipRect(userRect);
	PSSetClipRect(r);
	return r;
}

/******************************************************************************
 SetDashList

 ******************************************************************************/

void
JEPSPrinter::SetDashList
	(
	const JArray<JSize>&	dashList,
	const JSize				dashOffset
	)
{
	JPainter::SetDashList(dashList, dashOffset);
	PSSetDashList(dashList, dashOffset);
}

/******************************************************************************
 String

 ******************************************************************************/

void
JEPSPrinter::String
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JCharacter*	str,
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

	PSString(GetFontManager(), GetFontID(), GetFontSize(), GetFontStyle(),
			 ascent, dx,dy, 0, left,top, str);
}

void
JEPSPrinter::String
	(
	const JFloat		userAngle,
	const JCoordinate	left,
	const JCoordinate	top,
	const JCharacter*	str,
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

	PSString(GetFontManager(), GetFontID(), GetFontSize(), GetFontStyle(),
			 ascent, dx,dy, userAngle, left,top, str);
}

/******************************************************************************
 Point

 ******************************************************************************/

void
JEPSPrinter::Point
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
JEPSPrinter::Line
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
JEPSPrinter::Rect
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
JEPSPrinter::Ellipse
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
JEPSPrinter::Arc
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
JEPSPrinter::Polygon
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
 Image (virtual)

 ******************************************************************************/

void
JEPSPrinter::Image
	(
	const JImage&	image,
	const JRect&	srcRect,
	const JRect&	destRect
	)
{
	PSColorImage(image, srcRect, destRect);
}
