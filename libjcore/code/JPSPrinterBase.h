/******************************************************************************
 JPSPrinterBase.h

	Interface for the JPSPrinterBase class

	Copyright (C) 1996-97 by John Lindal & Glenn Bach.

 ******************************************************************************/

#ifndef _H_JPSPrinterBase
#define _H_JPSPrinterBase

#include <JFont.h>
#include <JRect.h>
#include <JPolygon.h>
#include <JString.h>
#include <jFStreamUtil.h>

class JString;
class JFontManager;
class JColorManager;
class JImage;
class JImageMask;

class JPSPrinterBase
{
public:

	enum
	{
		kPixelsPerInch = 72
	};

public:

	JPSPrinterBase(const JFontManager* fontManager, const JColorManager* colorManager);

	virtual ~JPSPrinterBase();

	// header info

	void	SetCreator(const JString& str);
	void	SetTitle(const JString& str);

	// printing control

	JBoolean	PSOpenDocument();
	void		PSCloseDocument();
	void		PSCancelDocument();
	JBoolean	PSDocumentIsOpen() const;

	// printing parameters

	const JString&	GetOutputFileName() const;
	void			SetOutputFileName(const JString& name);

	JBoolean	PSWillPrintBlackWhite() const;
	void		PSPrintBlackWhite(const JBoolean doIt);

	// JPainter functions

	void	PSSetClipRect(const JRect& r);

	void	PSSetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0);

	void	PSString(const JFont& font, const JCoordinate ascent,
					 const JCoordinate aligndx, const JCoordinate aligndy,
					 const JFloat angle, const JCoordinate left,
					 const JCoordinate top, const JString& str);

	void	PSLine(const JCoordinate x1, const JCoordinate y1,
				   const JCoordinate x2, const JCoordinate y2,
				   const JColorID color, const JSize lineWidth,
				   const JBoolean drawDashedLines);

	void	PSRect(const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h,
				   const JColorID color, const JSize lineWidth,
				   const JBoolean drawDashedLines, const JBoolean fill);

	void	PSArc(const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h,
				  const JFloat startAngle, const JFloat deltaAngle,
				  const JColorID color, const JSize lineWidth,
				  const JBoolean drawDashedLines, const JBoolean fill);

	void	PSPolygon(const JCoordinate left, const JCoordinate top,
					  const JPolygon& poly, const JColorID color,
					  const JSize lineWidth, const JBoolean drawDashedLines,
					  const JBoolean fill);

	void	PSColorImage(const JImage& image, const JRect& srcRect, const JRect& destRect);

protected:

	std::ostream&	GetOutputStream();

	void	PSSaveGraphicsState();
	void	PSRestoreGraphicsState();

	JPoint	ConvertToPS(const JCoordinate x, const JCoordinate y) const;
	JPoint	ConvertToPS(const JPoint& pt) const;

	virtual const JPoint&	PSGetOrigin() const = 0;
	virtual void			PSResetCoordinates() = 0;
	virtual JCoordinate		PSGetPrintableHeight() const = 0;

	virtual JBoolean	PSShouldPrintCurrentPage() const = 0;
	virtual void		PSPrintVersionComment(std::ostream& output) = 0;
	virtual void		PSPrintHeaderComments(std::ostream& output) = 0;
	virtual void		PSPrintSetupComments(std::ostream& output) = 0;

	void	PSConvertToRGB(const JColorID color, JSize* red,
						   JSize* green, JSize* blue) const;

private:

	JBoolean	itsDocOpenFlag;
	JBoolean	itsBWFlag;

	JString	itsCreator;
	JString	itsTitle;

	JSize			itsDashOffset;
	JArray<JSize>*	itsDashList;		// can be NULL

	// used only during printing

	JString			itsOutputFileName;
	std::ofstream*	itsFile;

	// buffered values

	const JFontManager*		itsFontManager;
	const JColorManager*	itsColorManager;

	JBoolean	itsFontSetFlag;
	JFont		itsLastFont;
	JColorID	itsLastColor;
	JBoolean	itsLastLineWidthInit;	// not everybody guarantees default value of 1
	JSize		itsLastLineWidth;
	JBoolean	itsLastDrawDashedLinesFlag;

private:

	void	ResetBufferedValues();

	void	PSColorImageNoMask(const JImage& image, const JRect& srcRect,
							   const JCoordinate destX, const JCoordinate destY);
	void	PSColorImageWithMask(const JImage& image, const JImageMask& mask,
								 const JRect& srcRect, const JCoordinate destX,
								 const JCoordinate destY);

	void	PSSetFont(const JFont& font);
	void	PSSetColor(const JColorID color);
	void	PSSetLineWidth(const JSize width);
	void	PSSetLineDashes(const JBoolean drawDashedLines);

	void	AdjustFontName(JString* name, const JFontStyle& style);
	void	ApplyStyles(JString* name, const JFontStyle& style,
						const JUtf8Byte* defaultStr, const JUtf8Byte* italicStr);

	// not allowed

	JPSPrinterBase(const JPSPrinterBase& source);
	const JPSPrinterBase& operator=(const JPSPrinterBase& source);
};


/******************************************************************************
 PSDocumentIsOpen

 ******************************************************************************/

inline JBoolean
JPSPrinterBase::PSDocumentIsOpen()
	const
{
	return itsDocOpenFlag;
}

/******************************************************************************
 SetCreator

 ******************************************************************************/

inline void
JPSPrinterBase::SetCreator
	(
	const JString& str
	)
{
	itsCreator = str;
}

/******************************************************************************
 SetTitle

 ******************************************************************************/

inline void
JPSPrinterBase::SetTitle
	(
	const JString& str
	)
{
	itsTitle = str;
}

/******************************************************************************
 OutputFileName

 ******************************************************************************/

inline const JString&
JPSPrinterBase::GetOutputFileName()
	const
{
	return itsOutputFileName;
}

inline void
JPSPrinterBase::SetOutputFileName
	(
	const JString& name
	)
{
	itsOutputFileName = name;
}

/******************************************************************************
 Black&White printing

 ******************************************************************************/

inline JBoolean
JPSPrinterBase::PSWillPrintBlackWhite()
	const
{
	return itsBWFlag;
}

inline void
JPSPrinterBase::PSPrintBlackWhite
	(
	const JBoolean doIt
	)
{
	itsBWFlag = doIt;
}

/******************************************************************************
 Coordinate conversion (protected)

 ******************************************************************************/

inline JPoint
JPSPrinterBase::ConvertToPS
	(
	const JCoordinate x,
	const JCoordinate y
	)
	const
{
	const JPoint& o = PSGetOrigin();
	return JPoint(o.x + x, PSGetPrintableHeight() - (o.y + y));
}

inline JPoint
JPSPrinterBase::ConvertToPS
	(
	const JPoint& pt
	)
	const
{
	const JPoint& o = PSGetOrigin();
	return JPoint(o.x + pt.x, PSGetPrintableHeight() - (o.y + pt.y));
}

#endif
