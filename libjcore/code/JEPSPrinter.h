/******************************************************************************
 JEPSPrinter.h

	Interface for the JEPSPrinter class

	Copyright © 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JEPSPrinter
#define _H_JEPSPrinter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPainter.h>
#include <JPrinter.h>
#include <JPSPrinterBase.h>

class JEPSPrinter : public JPainter, public JPrinter, public JPSPrinterBase
{
public:

	JEPSPrinter(const JFontManager* fontManager, const JColormap* colormap);

	virtual ~JEPSPrinter();

	// saving setup information

	void	ReadEPSSetup(istream& input);
	void	WriteEPSSetup(ostream& output) const;

	// printing control

	JBoolean	OpenDocument(const JRect& bounds);
	void		CloseDocument();

	// printing parameters

	JBoolean	WantsPreview() const;
	void		ShouldPrintPreview(const JBoolean doIt);

	virtual JPainter&	GetPreviewPainter(const JRect& bounds) = 0;

	JRect	GetBounds() const;

	// JPainter functions

	virtual JRect	SetClipRect(const JRect& r);
	virtual void	SetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0);

	virtual void	String(const JCoordinate left, const JCoordinate top,
						   const JCharacter* str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);
	virtual void	String(const JFloat angle, const JCoordinate left,
						   const JCoordinate top, const JCharacter* str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);

	virtual void	Point(const JCoordinate x, const JCoordinate y);

	virtual void	Line(const JCoordinate x1, const JCoordinate y1,
						 const JCoordinate x2, const JCoordinate y2);

	virtual void	Rect(const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h);

	virtual void	Ellipse(const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h);

	virtual void	Arc(const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h,
						const JFloat startAngle, const JFloat deltaAngle);

	virtual void	Polygon(const JCoordinate left, const JCoordinate top,
							const JPolygon& poly);

	virtual void	Image(const JImage& image, const JRect& srcRect, const JRect& destRect);

protected:

	virtual JBoolean	GetPreviewImage(const JImage** image) const = 0;
	virtual void		DeletePreviewData() = 0;

	virtual const JPoint&	PSGetOrigin() const;
	virtual void			PSResetCoordinates();
	virtual JCoordinate		PSGetPrintableHeight() const;

	virtual JBoolean	PSShouldPrintCurrentPage() const;
	virtual void		PSPrintVersionComment(ostream& output);
	virtual void		PSPrintHeaderComments(ostream& output);
	virtual void		PSPrintSetupComments(ostream& output);

private:

	JRect		itsBounds;
	JBoolean	itsIncludePreviewFlag;

private:

	void	PrintPreview();

	// not allowed

	JEPSPrinter(const JEPSPrinter& source);
	const JEPSPrinter& operator=(const JEPSPrinter& source);
};


/******************************************************************************
 Preview

 ******************************************************************************/

inline JBoolean
JEPSPrinter::WantsPreview()
	const
{
	return itsIncludePreviewFlag;
}

inline void
JEPSPrinter::ShouldPrintPreview
	(
	const JBoolean doIt
	)
{
	itsIncludePreviewFlag = doIt;
}

/******************************************************************************
 GetBounds

 ******************************************************************************/

inline JRect
JEPSPrinter::GetBounds()
	const
{
	return itsBounds;
}

#endif
