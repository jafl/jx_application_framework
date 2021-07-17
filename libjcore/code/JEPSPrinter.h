/******************************************************************************
 JEPSPrinter.h

	Interface for the JEPSPrinter class

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JEPSPrinter
#define _H_JEPSPrinter

#include "JPainter.h"
#include "JPrinter.h"
#include "JPSPrinterBase.h"

class JEPSPrinter : public JPainter, public JPrinter, public JPSPrinterBase
{
public:

	JEPSPrinter(JFontManager* fontManager);

	virtual ~JEPSPrinter();

	// saving setup information

	void	ReadEPSSetup(std::istream& input);
	void	WriteEPSSetup(std::ostream& output) const;

	// printing control

	bool	OpenDocument(const JRect& bounds);
	void		CloseDocument();

	// printing parameters

	bool	WantsPreview() const;
	void		ShouldPrintPreview(const bool doIt);

	virtual JPainter&	GetPreviewPainter(const JRect& bounds) = 0;

	JRect	GetBounds() const;

	// JPainter functions

	virtual JRect	SetClipRect(const JRect& r);
	virtual void	SetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0);

	virtual void	StringNoSubstitutions(
						const JCoordinate left, const JCoordinate top,
						const JString& str);
	virtual void	String(const JFloat angle, const JCoordinate left,
						   const JCoordinate top, const JString& str,
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

	virtual bool	GetPreviewImage(const JImage** image) const = 0;
	virtual void		DeletePreviewData() = 0;

	virtual const JPoint&	PSGetOrigin() const;
	virtual void			PSResetCoordinates();
	virtual JCoordinate		PSGetPrintableHeight() const;

	virtual bool	PSShouldPrintCurrentPage() const;
	virtual void		PSPrintVersionComment(std::ostream& output);
	virtual void		PSPrintHeaderComments(std::ostream& output);
	virtual void		PSPrintSetupComments(std::ostream& output);

private:

	JRect		itsBounds;
	bool	itsIncludePreviewFlag;

private:

	void	PrintPreview();

	// not allowed

	JEPSPrinter(const JEPSPrinter& source);
	const JEPSPrinter& operator=(const JEPSPrinter& source);
};


/******************************************************************************
 Preview

 ******************************************************************************/

inline bool
JEPSPrinter::WantsPreview()
	const
{
	return itsIncludePreviewFlag;
}

inline void
JEPSPrinter::ShouldPrintPreview
	(
	const bool doIt
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
