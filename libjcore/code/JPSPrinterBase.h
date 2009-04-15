/******************************************************************************
 JPSPrinterBase.h

	Interface for the JPSPrinterBase class

	Copyright © 1996-97 by John Lindal & Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPSPrinterBase
#define _H_JPSPrinterBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFontStyle.h>
#include <JRect.h>
#include <JPolygon.h>
#include <jFStreamUtil.h>

class JString;
class JFontManager;
class JColormap;
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

	JPSPrinterBase(const JColormap* colormap);

	virtual ~JPSPrinterBase();

	// header info

	void	SetCreator(const JCharacter* str);
	void	SetTitle(const JCharacter* str);

	// printing control

	JBoolean	PSOpenDocument();
	void		PSCloseDocument();
	void		PSCancelDocument();
	JBoolean	PSDocumentIsOpen() const;

	// printing parameters

	const JString&	GetOutputFileName() const;
	void			SetOutputFileName(const JCharacter* name);

	JBoolean	PSWillPrintBlackWhite() const;
	void		PSPrintBlackWhite(const JBoolean doIt);

	// JPainter functions

	void	PSSetClipRect(const JRect& r);

	void	PSSetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0);

	void	PSString(const JFontManager* fontManager,
					 const JFontID fontID, const JSize fontSize,
					 const JFontStyle& fontStyle, const JCoordinate ascent,
					 const JCoordinate aligndx, const JCoordinate aligndy,
					 const JFloat angle, const JCoordinate left,
					 const JCoordinate top, const JCharacter* str);

	void	PSLine(const JCoordinate x1, const JCoordinate y1,
				   const JCoordinate x2, const JCoordinate y2,
				   const JColorIndex color, const JSize lineWidth,
				   const JBoolean drawDashedLines);

	void	PSRect(const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h,
				   const JColorIndex color, const JSize lineWidth,
				   const JBoolean drawDashedLines, const JBoolean fill);

	void	PSArc(const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h,
				  const JFloat startAngle, const JFloat deltaAngle,
				  const JColorIndex color, const JSize lineWidth,
				  const JBoolean drawDashedLines, const JBoolean fill);

	void	PSPolygon(const JCoordinate left, const JCoordinate top,
					  const JPolygon& poly, const JColorIndex color,
					  const JSize lineWidth, const JBoolean drawDashedLines,
					  const JBoolean fill);

	void	PSColorImage(const JImage& image, const JRect& srcRect, const JRect& destRect);

protected:

	ostream&	GetOutputStream();

	void	PSSaveGraphicsState();
	void	PSRestoreGraphicsState();

	JPoint	ConvertToPS(const JCoordinate x, const JCoordinate y) const;
	JPoint	ConvertToPS(const JPoint& pt) const;

	virtual const JPoint&	PSGetOrigin() const = 0;
	virtual void			PSResetCoordinates() = 0;
	virtual JCoordinate		PSGetPrintableHeight() const = 0;

	virtual JBoolean	PSShouldPrintCurrentPage() const = 0;
	virtual void		PSPrintVersionComment(ostream& output) = 0;
	virtual void		PSPrintHeaderComments(ostream& output) = 0;
	virtual void		PSPrintSetupComments(ostream& output) = 0;

	void	PSConvertToRGB(const JColorIndex color, JSize* red,
						   JSize* green, JSize* blue) const;

private:

	JBoolean	itsDocOpenFlag;
	JBoolean	itsBWFlag;

	JString*	itsCreator;
	JString*	itsTitle;

	JSize			itsDashOffset;
	JArray<JSize>*	itsDashList;		// can be NULL

	// used only during printing

	JString*	itsOutputFileName;
	ofstream*	itsFile;

	// buffered values

	const JColormap*	itsColormap;

	JFontID		itsLastFontID;
	JSize		itsLastFontSize;
	JFontStyle	itsLastFontStyle;
	JColorIndex itsLastColor;
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

	void	PSSetFont(const JFontManager* fontManager, const JFontID id,
					  const JSize size, const JFontStyle& style);
	void	PSSetColor(const JColorIndex color);
	void	PSSetLineWidth(const JSize width);
	void	PSSetLineDashes(const JBoolean drawDashedLines);

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
 GetOutputFileName

 ******************************************************************************/

inline const JString&
JPSPrinterBase::GetOutputFileName()
	const
{
	return *itsOutputFileName;
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
