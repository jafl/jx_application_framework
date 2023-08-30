/******************************************************************************
 TestPainter.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPainter
#define _H_TestPainter

#include "JPainter.h"

class TestPainter : public JPainter
{
public:

	TestPainter(JFontManager* fontMgr);

	~TestPainter() override;

	void	Reset() override;

	JRect	SetClipRect(const JRect& r) override;
	void	SetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0) override;

	void	StringNoSubstitutions(
						const JCoordinate left, const JCoordinate top,
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
};

#endif
