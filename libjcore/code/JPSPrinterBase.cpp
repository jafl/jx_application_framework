/******************************************************************************
 JPSPrinterBase.cpp

	This class implements the functions required to draw to a PostScript file.
	This class is required because JEPSPrinter cannot inherit from JPagePrinter.

	To generate postscript output beyond what is provided by this class,
	create a derived class.  Inside each customized function, call
	GetOutputStream() to get the output file and then write directly to it.
	You *must* our functions whenever possible, because otherwise, you will
	destroy our buffered values, and the results will be unpredictable.

	Derived classes must implement the following functions:

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

	Copyright (C) 1996-97 by John Lindal & Glenn Bach.

 ******************************************************************************/

#include <JPSPrinterBase.h>
#include <JImage.h>
#include <JImageMask.h>
#include <jFileUtil.h>
#include <JFontManager.h>
#include <JColorManager.h>
#include <JStringIterator.h>
#include <jTime.h>
#include <stdlib.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

	Derived classes must call PSResetCoordinates().

 ******************************************************************************/

JPSPrinterBase::JPSPrinterBase
	(
	JFontManager* fontManager
	)
	:
	itsFontManager(fontManager),
	itsFontSetFlag(kJFalse),
	itsLastFont(JFontManager::GetDefaultFont())
{
	itsDocOpenFlag = kJFalse;
	itsBWFlag      = kJFalse;
	itsDashOffset  = 0;
	itsDashList    = NULL;
	itsFile        = NULL;

	ResetBufferedValues();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JPSPrinterBase::~JPSPrinterBase()
{
	assert( !itsDocOpenFlag && itsFile == NULL );

	jdelete itsDashList;
}

/******************************************************************************
 PSOpenDocument

 ******************************************************************************/

JBoolean
JPSPrinterBase::PSOpenDocument()
{
	assert( !itsDocOpenFlag && !itsOutputFileName.IsEmpty() );

	itsFile = jnew std::ofstream(itsOutputFileName.GetBytes());
	assert( itsFile != NULL );

	if (itsFile->fail())
		{
		jdelete itsFile;
		itsFile = NULL;
		return kJFalse;
		}

	itsDocOpenFlag = kJTrue;
	ResetBufferedValues();

	// PostScript file header

	PSPrintVersionComment(*itsFile);

	if (!itsCreator.IsEmpty())
		{
		*itsFile << "%%Creator: ";
		itsCreator.Print(*itsFile);
		*itsFile << '\n';
		}

	const JString dateStr = JGetTimeStamp();
	*itsFile << "%%CreationDate: ";
	dateStr.Print(*itsFile);
	*itsFile << '\n';

	if (!itsTitle.IsEmpty())
		{
		*itsFile << "%%Title: ";
		itsTitle.Print(*itsFile);
		*itsFile << '\n';
		}

	const JUtf8Byte* userName = getenv("USER");
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

	jdelete itsFile;
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

	jdelete itsFile;
	itsFile = NULL;
	const JError err = JRemoveFile(itsOutputFileName);
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
		itsDashList = jnew JArray<JSize>(dashList);
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
	const JFont&		font,

	const JCoordinate	ascent,
	const JCoordinate	aligndx,
	const JCoordinate	aligndy,

	const JFloat		userAngle,
	const JCoordinate	left,
	const JCoordinate	top,
	const JString&		str
	)
{
	if (!PSShouldPrintCurrentPage())
		{
		return;
		}

	PSSetFont(font);

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

	JStringIterator iter(str);
	JUtf8Character c;
	JSize lineLength = 1;
	while (iter.Next(&c))
		{
		if (c == '\\' || c == '(' || c == ')')
			{
			*itsFile << '\\';
			lineLength++;
			}
		itsFile->write(c.GetBytes(), c.GetByteCount());
		lineLength += c.GetByteCount();

		if (lineLength >= 200)
			{
			*itsFile << ") show\n";
			*itsFile << '(';
			lineLength = 1;
			}
		}
	*itsFile << ") show\n";

	// add the rest of the styles

	const JSize strWidth = font.GetStringWidth(itsFontManager, str);

	const JSize underlineCount = font.GetStyle().underlineCount;
	if (underlineCount > 0)
		{
		const JSize ulWidth = font.GetUnderlineThickness();

		JCoordinate yu = JLCeil(y - 1.5 * ulWidth);			// thick line is centered on path
		for (JIndex i=1; i<=underlineCount; i++)
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

	if (font.GetStyle().strike)
		{
		const JSize strikeWidth = font.GetStrikeThickness();
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
	const JColorID	color,
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
	const JColorID	color,
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
	const JColorID	color,
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
	const JColorID	color,
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

	itsFile->setf(std::ios::hex, std::ios::basefield);

	JSize c[3];	// r,g,b
	JIndex lineLength = 0;
	for (JCoordinate y=srcRect.top; y<srcRect.bottom; y++)
		{
		for (JCoordinate x=srcRect.left; x<srcRect.right; x++)
			{
			const JColorID color = image.GetColor(x,y);
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

	itsFile->setf(std::ios::dec, std::ios::basefield);
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
	itsFontSetFlag = kJFalse;
	itsLastColor   = JColorManager::GetBlackColor();

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

static const JUtf8Byte* kCurrFontName = "/JPSPrinterBase_CurrFont";

void
JPSPrinterBase::PSSetFont
	(
	const JFont& font
	)
{
	if (!itsFontSetFlag ||
		font.GetID()           != itsLastFont.GetID() ||
		font.GetSize()         != itsLastFont.GetSize() ||
		font.GetStyle().bold   != itsLastFont.GetStyle().bold ||
		font.GetStyle().italic != itsLastFont.GetStyle().italic)
		{
		itsFontSetFlag = kJTrue;
		itsLastFont    = font;

		JString fontName = font.GetName();
		AdjustFontName(&fontName, font.GetStyle());
		*itsFile << '/';
		fontName.Print(*itsFile);

		*itsFile << " findfont\n";
		*itsFile << "dup length dict begin\n";
		*itsFile << "  {1 index /FID ne {def} {pop pop} ifelse} forall\n";
		*itsFile << "  /Encoding ISOLatin1Encoding def\n";
		*itsFile << "  currentdict\n";
		*itsFile << "end\n";
		*itsFile << kCurrFontName << " exch definefont\n";

		*itsFile << font.GetSize() << " scalefont setfont\n";
		}

	PSSetColor(font.GetStyle().color);
}

/******************************************************************************
 AdjustFontName (private)

	Convert system font name to PS font name.

	http://www.sketchpad.net/ps-font-substitution.htm

 ******************************************************************************/

void
JPSPrinterBase::AdjustFontName
	(
	JString*			name,
	const JFontStyle&	style
	)
{
	if (name->Contains(" Mono"))
		{
		*name = "Courier";
		ApplyStyles(name, style, NULL, "Oblique");
		return;
		}
	else if (name->Contains(" Sans"))
		{
		*name = "Helvetica";
		ApplyStyles(name, style, NULL, "Oblique");
		return;
		}
	else if (*name == "Times" || name->Contains(" Serif"))
		{
		*name = "Times";
		ApplyStyles(name, style, "Roman", "Italic");
		return;
		}
	else if (name->Contains("Century Schoolbook"))
		{
		*name = "NewCenturySchlbk";
		ApplyStyles(name, style, "Roman", "Italic");
		return;
		}
	else if (name->Contains("Bookman"))
		{
		*name = "Bookman";

		if (style.bold && style.italic)
			{
			name->Append("-DemiItalic");
			}
		else if (style.bold)
			{
			name->Append("-Demi");
			}
		else if (style.italic)
			{
			name->Append("-LightItalic");
			}
		else
			{
			name->Append("-Light");
			}
		return;
		}
	else if (name->Contains("Palatino"))
		{
		*name = "Bookman";
		ApplyStyles(name, style, "Roman", "Italic");
		return;
		}
	else if (name->Contains("Chancery"))
		{
		*name = "ZapfChancery-MediumItalic";
		return;
		}
	else if (*name == "Symbol")
		{
		return;
		}

	// default processing
/*
	if (name->EndsWith(" L"))
		{
		name->RemoveSubstring(name->GetLength()-1, name->GetLength());
		}
	else if (name->EndsWith(" Light"))
		{
		name->RemoveSubstring(name->GetLength()-1, name->GetLength());
		}

	const JSize length = name->GetLength();
	for (JIndex i=length; i>=1; i--)
		{
		if (isspace(name->GetCharacter(i)))
			{
			name->RemoveSubstring(i,i);
			}
		}
*/
	*name = "Helvetica";
	ApplyStyles(name, style, NULL, "Oblique");
}

/******************************************************************************
 ApplyStyles (private)

 ******************************************************************************/

void
JPSPrinterBase::ApplyStyles
	(
	JString*			name,
	const JFontStyle&	style,
	const JUtf8Byte*	defaultStr,
	const JUtf8Byte*	italicStr
	)
{
	if (style.bold && style.italic)
		{
		name->Append("-Bold");
		name->Append(italicStr);
		}
	else if (style.bold)
		{
		name->Append("-Bold");
		}
	else if (style.italic)
		{
		name->Append("-");
		name->Append(italicStr);
		}
	else if (!JString::IsEmpty(defaultStr))
		{
		name->Append("-");
		name->Append(defaultStr);
		}
}

/******************************************************************************
 PSSetColor (private)

	Convert system dependent color to Postscript color.

 ******************************************************************************/

void
JPSPrinterBase::PSSetColor
	(
	const JColorID color
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

	Convert JColorID (16-bit RGB) to 8-bit RGB Postscript color.

 ******************************************************************************/

void
JPSPrinterBase::PSConvertToRGB
	(
	const JColorID	color,
	JSize*			red,
	JSize*			green,
	JSize*			blue
	)
	const
{
	if (itsBWFlag && color != JColorManager::GetWhiteColor())
		{
		*red = *green = *blue = 0;		// black
		}
	else if (itsBWFlag)
		{
		*red = *green = *blue = 255;	// white
		}
	else
		{
		const JRGB rgb = JColorManager::GetRGB(color);
		*red   = rgb.red   / 256;
		*green = rgb.green / 256;
		*blue  = rgb.blue  / 256;
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

std::ostream&
JPSPrinterBase::GetOutputStream()
{
	return *itsFile;
}
