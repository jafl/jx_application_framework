/******************************************************************************
 JPSPrinterBase.cpp

	This class implements the functions required to draw to a PostScript file.
	This class is required because JEPSPrinter cannot inherit from JPagePrinter.

	To generate postscript output beyond what is provided by this class,
	create a derived class.  Inside each customized function, call
	GetOutputStream() to get the output file and then write directly to it.
	You *must* our functions whenever possible, because otherwise, you will
	destroy our buffered values, and the results will be unpredictable.

	Derived classes must implement the following routines:

		PSGetOrigin
			Return the current JPainter origin.

		PSResetCoordinates
			Set the origin and page size.

		PSGetPrintableHeight
			Return the height of the printable area.  This is used to
			transform screen coordinates into PS coordinates.

		PSShouldPrintCurrentPage
			Return kJFalse if the drawing commands should do nothing.

		PSPrintVersionComment
			Print the first line in the PostScript file.

		PSPrintHeaderComments
			Print any required header comments.

		PSPrintSetupComments
			Print any required setup comments.

	BASE CLASS = none

	Copyright © 1996-97 by John Lindal & Glenn Bach. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JPSPrinterBase.h>
#include <JImage.h>
#include <JImageMask.h>
#include <jFileUtil.h>
#include <JFontManager.h>
#include <JColormap.h>
#include <JString.h>
#include <jTime.h>
#include <stdlib.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Derived classes must call PSResetCoordinates().

 ******************************************************************************/

JPSPrinterBase::JPSPrinterBase
	(
	const JColormap* colormap
	)
	:
	itsColormap(colormap)
{
	itsDocOpenFlag = kJFalse;
	itsBWFlag      = kJFalse;
	itsDashOffset  = 0;
	itsDashList    = NULL;
	itsFile        = NULL;

	itsCreator = new JString;
	assert( itsCreator != NULL );

	itsTitle = new JString;
	assert( itsTitle != NULL );

	itsOutputFileName = new JString;
	assert( itsOutputFileName != NULL );

	ResetBufferedValues();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPSPrinterBase::~JPSPrinterBase()
{
	assert( !itsDocOpenFlag && itsFile == NULL );

	delete itsCreator;
	delete itsTitle;
	delete itsDashList;
	delete itsOutputFileName;
}

/******************************************************************************
 SetCreator

 ******************************************************************************/

void
JPSPrinterBase::SetCreator
	(
	const JCharacter* str
	)
{
	*itsCreator = str;
}

/******************************************************************************
 SetTitle

 ******************************************************************************/

void
JPSPrinterBase::SetTitle
	(
	const JCharacter* str
	)
{
	*itsTitle = str;
}

/******************************************************************************
 SetOutputFileName

 ******************************************************************************/

void
JPSPrinterBase::SetOutputFileName
	(
	const JCharacter* name
	)
{
	*itsOutputFileName = name;
}

/******************************************************************************
 PSOpenDocument

 ******************************************************************************/

JBoolean
JPSPrinterBase::PSOpenDocument()
{
	assert( !itsDocOpenFlag && !itsOutputFileName->IsEmpty() );

	itsFile = new ofstream(*itsOutputFileName);
	assert( itsFile != NULL );

	if (itsFile->fail())
		{
		delete itsFile;
		itsFile = NULL;
		return kJFalse;
		}

	itsDocOpenFlag = kJTrue;
	ResetBufferedValues();

	// PostScript file header

	PSPrintVersionComment(*itsFile);

	if (!itsCreator->IsEmpty())
		{
		*itsFile << "%%Creator: ";
		itsCreator->Print(*itsFile);
		*itsFile << '\n';
		}

	const JString dateStr = JGetTimeStamp();
	*itsFile << "%%CreationDate: ";
	dateStr.Print(*itsFile);
	*itsFile << '\n';

	if (!itsTitle->IsEmpty())
		{
		*itsFile << "%%Title: ";
		itsTitle->Print(*itsFile);
		*itsFile << '\n';
		}

	const JCharacter* userName = getenv("USER");
	if (userName != NULL)
		{
		*itsFile << "%%For: " << userName << '\n';
		}

	PSPrintHeaderComments(*itsFile);

	*itsFile << "%%EndComments\n";
	*itsFile << "%%BeginSetup\n";

	PSPrintSetupComments(*itsFile);

	*itsFile << "%%EndSetup\n";

	return kJTrue;
}

/******************************************************************************
 PSCloseDocument

 ******************************************************************************/

void
JPSPrinterBase::PSCloseDocument()
{
	assert( itsDocOpenFlag );

	*itsFile << "%%EOF\n";

	delete itsFile;
	itsFile = NULL;

	itsDocOpenFlag = kJFalse;
	PSResetCoordinates();
}

/******************************************************************************
 PSCancelDocument

 ******************************************************************************/

void
JPSPrinterBase::PSCancelDocument()
{
	assert( itsDocOpenFlag );

	delete itsFile;
	itsFile = NULL;
	const JError err = JRemoveFile(*itsOutputFileName);
	assert_ok( err );

	itsDocOpenFlag = kJFalse;
	PSResetCoordinates();
}

/******************************************************************************
 PSSetClipRect

	This can be called even if a document is not open.

 ******************************************************************************/

void
JPSPrinterBase::PSSetClipRect
	(
	const JRect& r
	)
{
	const JBoolean shouldPrint =
		JConvertToBoolean( itsDocOpenFlag && PSShouldPrintCurrentPage() );

	if (shouldPrint && !r.IsEmpty())
		{
		PSRestoreGraphicsState();
		PSSaveGraphicsState();

		*itsFile << "newpath\n";

		JPoint psPt = ConvertToPS(r.topLeft());
		*itsFile << psPt.x << ' ' << psPt.y;
		*itsFile << " moveto\n";

		psPt = ConvertToPS(r.topRight());
		*itsFile << psPt.x << ' ' << psPt.y;
		*itsFile << " lineto\n";

		psPt = ConvertToPS(r.bottomRight());
		*itsFile << psPt.x << ' ' << psPt.y;
		*itsFile << " lineto\n";

		psPt = ConvertToPS(r.bottomLeft());
		*itsFile << psPt.x << ' ' << psPt.y;
		*itsFile << " lineto\n";

		*itsFile << "closepath clip\n";
		}
	else if (shouldPrint)
		{
		*itsFile << "newpath clip\n";
		}
}

/******************************************************************************
 PSSetDashList

	This can be called even if a document is not open.

 ******************************************************************************/

void
JPSPrinterBase::PSSetDashList
	(
	const JArray<JSize>&	dashList,
	const JSize				dashOffset
	)
{
	if (itsDashList == NULL)
		{
		itsDashList = new JArray<JSize>(dashList);
		assert( itsDashList != NULL );
		}
	else
		{
		*itsDashList = dashList;
		}

	// if odd # of dashes, double the list

	const JSize dashCount = itsDashList->GetElementCount();
	if (dashCount % 2 == 1)
		{
		for (JIndex i=1; i<=dashCount; i++)
			{
			itsDashList->AppendElement(itsDashList->GetElement(i));
			}
		}

	itsDashOffset = dashOffset;

	const JBoolean shouldPrint =
		JConvertToBoolean( itsDocOpenFlag && PSShouldPrintCurrentPage() );
	if (shouldPrint && itsLastDrawDashedLinesFlag)
		{
		itsLastDrawDashedLinesFlag = kJFalse;
		PSSetLineDashes(kJTrue);
		}
}

/******************************************************************************
 PSString

 ******************************************************************************/

void
JPSPrinterBase::PSString
	(
	const JFontManager*	fontManager,
	const JFontID		fontID,
	const JSize			fontSize,
	const JFontStyle&	fontStyle,

	const JCoordinate	ascent,
	const JCoordinate	aligndx,
	const JCoordinate	aligndy,

	const JFloat		userAngle,
	const JCoordinate	left,
	const JCoordinate	top,
	const JCharacter*	str
	)
{
	if (!PSShouldPrintCurrentPage())
		{
		return;
		}

	PSSetFont(fontManager, fontID, fontSize, fontStyle);

	*itsFile << "gsave\n";

	// rotate the coordinate system around topLeft

	const JPoint psPt = ConvertToPS(left,top);
	*itsFile << psPt.x << ' ' << psPt.y << " translate\n";

	JFloat angle = userAngle;
	while (angle <= -45.0)
		{
		angle += 360.0;
		}
	while (angle > 315.0)
		{
		angle -= 360.0;
		}

	if (45.0 < angle && angle <= 135.0)
		{
		*itsFile << "90 rotate\n";
		}
	else if (135.0 < angle && angle <= 225.0)
		{
		*itsFile << "180 rotate\n";
		}
	else if (225.0 < angle && angle <= 315.0)
		{
		*itsFile << "270 rotate\n";
		}

	// decide where to place the string

	const JCoordinate x = aligndx;
	const JCoordinate y = -ascent - aligndy;	// postscript y increases up the page

	// draw the string

	*itsFile << x << ' ' << y << " moveto\n";
	*itsFile << '(';
	JSize lineLength = 1;
	const JSize strLen = strlen(str);
	for (JIndex i=1; i<=strLen; i++)
		{
		const JCharacter c = str[i-1];
		if (c == '\\' || c == '(' || c == ')')
			{
			*itsFile << '\\';
			lineLength++;
			}
		*itsFile << c;
		lineLength++;

		if (lineLength >= 200 && i < strLen)
			{
			*itsFile << ") show\n";
			*itsFile << '(';
			lineLength = 1;
			}
		}
	*itsFile << ") show\n";

	// add the rest of the styles

	const JSize strWidth = fontManager->GetStringWidth(fontID, fontSize, fontStyle, str);

	if (fontStyle.underlineCount > 0)
		{
		const JSize ulWidth = fontManager->GetUnderlineThickness(fontSize);

		JCoordinate yu = JLCeil(y - 1.5 * ulWidth);			// thick line is centered on path
		for (JIndex i=1; i<=fontStyle.underlineCount; i++)
			{
			*itsFile << "newpath\n";
			*itsFile << x << ' ' << yu << " moveto\n";
			*itsFile << x+strWidth << ' ' << yu << " lineto\n";
			*itsFile << ulWidth << " setlinewidth\n";		// gsave => can't call usual functions
			*itsFile << "[ ] 0 setdash\n";
			*itsFile << "stroke\n";

			yu += 2 * ulWidth;
			}
		}

	if (fontStyle.strike)
		{
		const JSize strikeWidth = fontManager->GetStrikeThickness(fontSize);
		const JCoordinate ys    = y + ascent/2;			// thick line is centered on path

		*itsFile << "newpath\n";
		*itsFile << x << ' ' << ys << " moveto\n";
		*itsFile << x+strWidth << ' ' << ys << " lineto\n";
		*itsFile << strikeWidth << " setlinewidth\n";	// gsave => can't call usual functions
		*itsFile << "[ ] 0 setdash\n";
		*itsFile << "stroke\n";
		}

	// clean up

	*itsFile << "grestore\n";
}

/******************************************************************************
 PSLine

 ******************************************************************************/

void
JPSPrinterBase::PSLine
	(
	const JCoordinate	x1,
	const JCoordinate	y1,
	const JCoordinate	x2,
	const JCoordinate	y2,
	const JColorIndex	color,
	const JSize			lineWidth,
	const JBoolean		drawDashedLines
	)
{
	if (!PSShouldPrintCurrentPage())
		{
		return;
		}

	PSSetColor(color);

	const JPoint psPt1 = ConvertToPS(x1,y1);
	const JPoint psPt2 = ConvertToPS(x2,y2);

	*itsFile << "newpath\n";
	*itsFile << psPt1.x << ' ' << psPt1.y << " moveto\n";
	*itsFile << psPt2.x << ' ' << psPt2.y << " lineto\n";

	PSSetLineWidth(lineWidth);
	PSSetLineDashes(drawDashedLines);
	*itsFile << "stroke\n";
}

/******************************************************************************
 PSRect

 ******************************************************************************/

void
JPSPrinterBase::PSRect
	(
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h,
	const JColorIndex	color,
	const JSize			lineWidth,
	const JBoolean		drawDashedLines,
	const JBoolean		fill
	)
{
	if (!PSShouldPrintCurrentPage())
		{
		return;
		}

	PSSetColor(color);

	const JPoint psPt1 = ConvertToPS(x  , y  );
	const JPoint psPt2 = ConvertToPS(x+w, y  );
	const JPoint psPt3 = ConvertToPS(x+w, y+h);
	const JPoint psPt4 = ConvertToPS(x  , y+h);

	*itsFile << "newpath\n";
	*itsFile << psPt1.x << ' ' << psPt1.y << " moveto\n";
	*itsFile << psPt2.x << ' ' << psPt2.y << " lineto\n";
	*itsFile << psPt3.x << ' ' << psPt3.y << " lineto\n";
	*itsFile << psPt4.x << ' ' << psPt4.y << " lineto\n";
	*itsFile << "closepath\n";

	if (!fill)
		{
		PSSetLineWidth(lineWidth);
		PSSetLineDashes(drawDashedLines);
		*itsFile << "stroke\n";
		}
	else
		{
		*itsFile << "eofill\n";
		}
}

/******************************************************************************
 PSArc

 ******************************************************************************/

void
JPSPrinterBase::PSArc
	(
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h,
	const JFloat		startAngle,
	const JFloat		deltaAngle,
	const JColorIndex	color,
	const JSize			lineWidth,
	const JBoolean		drawDashedLines,
	const JBoolean		fill
	)
{
	if (!PSShouldPrintCurrentPage())
		{
		return;
		}

	PSSetColor(color);

	const JPoint psPt = ConvertToPS(x+w/2, y+h/2);

	*itsFile << "gsave\n";
	*itsFile << "newpath\n";
	*itsFile << "matrix currentmatrix\n";

	*itsFile << psPt.x << ' ' << psPt.y << " translate\n";
	*itsFile << w/2 << ' ' << h/2 << " scale\n";

	if (fill)
		{
		*itsFile << "0 0 moveto\n";
		}
	*itsFile << "0 0 1 ";
	if (deltaAngle >= 0.0)
		{
		*itsFile << startAngle;
		*itsFile << ' ' << startAngle + deltaAngle;
		}
	else
		{
		*itsFile << startAngle + deltaAngle;
		*itsFile << ' ' << startAngle;
		}
	*itsFile << " arc\n";
	if (fill)
		{
		*itsFile << "closepath\n";
		}

	*itsFile << "setmatrix\n";

	if (!fill)
		{
		// since we called gsave, we can't call the functions normally

		*itsFile << lineWidth << " setlinewidth\n";

		JBoolean savedDDL = itsLastDrawDashedLinesFlag;
		PSSetLineDashes(drawDashedLines);
		itsLastDrawDashedLinesFlag = savedDDL;

		*itsFile << "stroke\n";
		}
	else 
		{
		*itsFile << "eofill\n";
		}

	*itsFile << "grestore\n";
}

/******************************************************************************
 PSPolygon

 ******************************************************************************/

void
JPSPrinterBase::PSPolygon
	(
	const JCoordinate	left,
	const JCoordinate	top,
	const JPolygon&		poly,
	const JColorIndex	color,
	const JSize			lineWidth,
	const JBoolean		drawDashedLines,
	const JBoolean		fill
	)
{
	if (!PSShouldPrintCurrentPage())
		{
		return;
		}

	PSSetColor(color);

	const JSize count  = poly.GetElementCount();
	const JPoint start = poly.GetElement(1);

	JPoint psPt = ConvertToPS(left+start.x, top+start.y);

	*itsFile << "newpath\n";
	*itsFile << psPt.x << ' ' << psPt.y << " moveto\n";
	for (JSize i=2; i<=count; i++)
		{
		const JPoint curr = poly.GetElement(i);
		psPt = ConvertToPS(left+curr.x, top+curr.y);
		*itsFile << psPt.x << ' ' << psPt.y << " lineto\n";
		}
	*itsFile << "closepath\n";

	if (!fill)
		{
		PSSetLineWidth(lineWidth);
		PSSetLineDashes(drawDashedLines);
		*itsFile << "stroke\n";
		}
	else 
		{
		*itsFile << "eofill\n";
		}
}

/******************************************************************************
 PSColorImage (virtual)

 ******************************************************************************/

void
JPSPrinterBase::PSColorImage
	(
	const JImage&	image,
	const JRect&	userSrcRect,
	const JRect&	destRect
	)
{
	JRect srcRect;
	if (!PSShouldPrintCurrentPage() ||
		!JIntersection(userSrcRect, image.GetBounds(), &srcRect))
		{
		return;
		}

	const JCoordinate destX = (destRect.left + destRect.right - srcRect.width())/2;
	const JCoordinate destY = (destRect.top + destRect.bottom - srcRect.height())/2;

	JImageMask* mask;
	if (image.GetMask(&mask))
		{
		PSColorImageWithMask(image, *mask, srcRect, destX, destY);
		}
	else
		{
		PSColorImageNoMask(image, srcRect, destX, destY);
		}
}

// private

void
JPSPrinterBase::PSColorImageNoMask
	(
	const JImage&		image,
	const JRect&		srcRect,
	const JCoordinate	destX,
	const JCoordinate	destY
	)
{
	const JCoordinate w = image.GetWidth();
	const JCoordinate h = image.GetHeight();

	*itsFile << "gsave\n";

	const JPoint psPt = ConvertToPS(destX, destY + h);
	*itsFile << psPt.x << ' ' << psPt.y << " translate\n";
	*itsFile << w << ' ' << h << " scale\n";

	*itsFile << "/picstr 3 string def\n";
	*itsFile << w << ' ' << h << " 8 [";
	*itsFile << w << " 0 0 " << -h << " 0 " << h << "]\n";
	*itsFile << "{currentfile picstr readhexstring pop} false 3 colorimage\n";

	itsFile->setf(ios::hex, ios::basefield);

	JSize c[3];	// r,g,b
	JIndex lineLength = 0;
	for (JCoordinate y=srcRect.top; y<srcRect.bottom; y++)
		{
		for (JCoordinate x=srcRect.left; x<srcRect.right; x++)
			{
			const JColorIndex color = image.GetColor(x,y);
			PSConvertToRGB(color, &(c[0]), &(c[1]), &(c[2]));
			for (JIndex i=0; i<=2; i++)
				{
				if (c[i] <= 0x0F)
					{
					*itsFile << '0';	// must print two characters
					}
				*itsFile << c[i];
				}
			lineLength += 6;
			if (lineLength >= 252)
				{
				*itsFile << '\n';
				lineLength = 0;
				}
			}
		}
	if (lineLength > 0)
		{
		*itsFile << '\n';
		}

	itsFile->setf(ios::dec, ios::basefield);
	*itsFile << "grestore\n";
}

// private

void
JPSPrinterBase::PSColorImageWithMask
	(
	const JImage&		image,
	const JImageMask&	mask,
	const JRect&		srcRect,
	const JCoordinate	destX,
	const JCoordinate	destY
	)
{
	for (JCoordinate y=srcRect.top; y<srcRect.bottom; y++)
		{
		const JCoordinate y1 = destY + y - srcRect.top;

		for (JCoordinate x=srcRect.left; x<srcRect.right; x++)
			{
			if (mask.ContainsPixel(x,y))
				{
				const JCoordinate x1 = destX + x - srcRect.left;
				PSLine(x1,y1, x1+1,y1, image.GetColor(x,y), 1, kJFalse);
				}
			}
		}
}

/******************************************************************************
 PSSaveGraphicsState (protected)

	Save the current graphics state.  This is mainly needed for adjusting
	the clipping.

 ******************************************************************************/

void
JPSPrinterBase::PSSaveGraphicsState()
{
	*itsFile << "gsave\n";
}

/******************************************************************************
 PSRestoreGraphicsState (protected)

	Restore the original graphics state.  This balances PSSaveGraphicsState().

 ******************************************************************************/

void
JPSPrinterBase::PSRestoreGraphicsState()
{
	*itsFile << "grestore\n";
	ResetBufferedValues();
}

/******************************************************************************
 ResetBufferedValues (private)

 ******************************************************************************/

void
JPSPrinterBase::ResetBufferedValues()
{
	itsLastFontID    = 0;
	itsLastFontSize  = 0;
	itsLastFontStyle = JFontStyle();
	itsLastColor     = itsColormap->GetBlackColor();

	itsLastLineWidthInit = kJFalse;
	itsLastLineWidth     = 1;

	itsLastDrawDashedLinesFlag = kJFalse;
}

/******************************************************************************
 PSSetFont (private)

	Convert to Postscript font and use ISOLatin1 encoding.  To do this,
	we use the code from page 275 of the PostScript Language Reference
	Manual, 2nd edition.

	The new font is stored in the key /JPSPrinterBase_CurrFont.

 ******************************************************************************/

static const JCharacter* kCurrFontName = "/JPSPrinterBase_CurrFont";

void
JPSPrinterBase::PSSetFont
	(
	const JFontManager*	fontManager,
	const JFontID		id,
	const JSize			size,
	const JFontStyle&	style
	)
{
	if (id           != itsLastFontID ||
		size         != itsLastFontSize ||
		style.bold   != itsLastFontStyle.bold ||
		style.italic != itsLastFontStyle.italic)
		{
		itsLastFontID    = id;
		itsLastFontSize  = size;
		itsLastFontStyle = style;

		JString fontName, charSet;
		JFontManager::ExtractCharacterSet(fontManager->GetFontName(id),
										  &fontName, &charSet);
		*itsFile << '/';
		fontName.Print(*itsFile);

		if (style.bold && style.italic)
			{
			*itsFile << "-BoldOblique";
			}
		else if (style.bold)
			{
			*itsFile << "-Bold";
			}
		else if (style.italic)
			{
			*itsFile << "-Oblique";
			}

		*itsFile << " findfont\n";
		*itsFile << "dup length dict begin\n";
		*itsFile << "  {1 index /FID ne {def} {pop pop} ifelse} forall\n";
		*itsFile << "  /Encoding ISOLatin1Encoding def\n";
		*itsFile << "  currentdict\n";
		*itsFile << "end\n";
		*itsFile << kCurrFontName << " exch definefont\n";

		*itsFile << size << " scalefont setfont\n";
		}

	PSSetColor(style.color);
}

/******************************************************************************
 PSSetColor (private)

	Convert system dependent color to Postscript color.

 ******************************************************************************/

void
JPSPrinterBase::PSSetColor
	(
	const JColorIndex color
	)
{
	if (color != itsLastColor)
		{
		itsLastColor = color;

		JSize r,g,b;
		PSConvertToRGB(color, &r, &g, &b);
		*itsFile << r/255.0 << ' ' << g/255.0 << ' ' << b/255.0;
		*itsFile << " setrgbcolor\n";
		}
}

/******************************************************************************
 PSConvertToRGB (protected)

	Convert JColorIndex (16-bit RGB) to 8-bit RGB Postscript color.

 ******************************************************************************/

void
JPSPrinterBase::PSConvertToRGB
	(
	const JColorIndex	color,
	JSize*				red,
	JSize*				green,
	JSize*				blue
	)
	const
{
	if (itsBWFlag && color != itsColormap->GetWhiteColor())
		{
		*red = *green = *blue = 0;		// black
		}
	else if (itsBWFlag)
		{
		*red = *green = *blue = 255;	// white
		}
	else
		{
		itsColormap->GetRGB(color, red, green, blue);
		*red   /= 256;
		*green /= 256;
		*blue  /= 256;
		}
}

/******************************************************************************
 PSSetLineWidth (private)

 ******************************************************************************/

void
JPSPrinterBase::PSSetLineWidth
	(
	const JSize width
	)
{
	if (width != itsLastLineWidth || !itsLastLineWidthInit)
		{
		itsLastLineWidthInit = kJTrue;
		itsLastLineWidth     = width;

		*itsFile << width << " setlinewidth\n";
		}
}

/******************************************************************************
 PSSetLineDashes (private)

 ******************************************************************************/

void
JPSPrinterBase::PSSetLineDashes
	(
	const JBoolean drawDashedLines
	)
{
	if (drawDashedLines != itsLastDrawDashedLinesFlag)
		{
		itsLastDrawDashedLinesFlag = drawDashedLines;

		if (drawDashedLines)
			{
			assert( itsDashList != NULL );

			*itsFile << '[';

			const JSize dashCount = itsDashList->GetElementCount();
			for (JIndex i=1; i<=dashCount; i++)
				{
				if (i > 1)
					{
					*itsFile << ' ';
					}
				*itsFile << itsDashList->GetElement(i);
				}

			*itsFile << "] " << itsDashOffset << " setdash\n";
			}
		else
			{
			*itsFile << "[ ] 0 setdash\n";
			}
		}
}

/******************************************************************************
 GetOutputStream (protected)

 ******************************************************************************/

ostream&
JPSPrinterBase::GetOutputStream()
{
	return *itsFile;
}
