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

	~JEPSPrinter() override;

	// saving setup information

	void	ReadEPSSetup(std::istream& input);
	void	WriteEPSSetup(std::ostream& output) const;

	// printing control

	bool	OpenDocument(const JRect& bounds);
	void	CloseDocument();

	// printing parameters

	bool	WantsPreview() const;
	void	ShouldPrintPreview(const bool doIt);

	virtual JPainter&	GetPreviewPainter(const JRect& bounds) = 0;

	JRect	GetBounds() const;

	// JPainter functions

	JRect	SetClipRect(const JRect& r) override;
	void	SetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0) override;

	void	StringNoSubstitutions(const JCoordinate left, const JCoordinate top,
								  const JString& str) override;
	void	String(const JFloat angle, const JCoordinate left,
				   const JCoordinate top, const JString& str,
				   const JCoordinate width = 0,
				   const HAlign hAlign = HAlign::kLeft,
				   const JCoordinate height = 0,
				   const VAlign vAlign = VAlign::kTop) override;

	void	Point(const JCoordinate x, const JCoordinate y) override;

	void	Line(const JCoordinate x1, const JCoordinate y1,
				 const JCoordinate x2, const JCoordinate y2) override;

	void	Rect(const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h) override;

	void	Ellipse(const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h) override;

	void	Arc(const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h,
				const JFloat startAngle, const JFloat deltaAngle) override;

	void	Polygon(const JCoordinate left, const JCoordinate top,
					const JPolygon& poly) override;

	void	Image(const JImage& image, const JRect& srcRect, const JRect& destRect) override;

protected:

	virtual bool	GetPreviewImage(const JImage** image) const = 0;
	virtual void	DeletePreviewData() = 0;

	const JPoint&	PSGetOrigin() const override;
	void			PSResetCoordinates() override;
	JCoordinate		PSGetPrintableHeight() const override;

	bool	PSShouldPrintCurrentPage() const override;
	void	PSPrintVersionComment(std::ostream& output) override;
	void	PSPrintHeaderComments(std::ostream& output) override;
	void	PSPrintSetupComments(std::ostream& output) override;

private:

	JRect	itsBounds;
	bool	itsIncludePreviewFlag;

private:

	void	PrintPreview();
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
