/******************************************************************************
 TestPainter.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPainter
#define _H_TestPainter

#include <jx-af/jcore/JPainter.h>

class TestPainter : public JPainter
{
public:

	TestPainter(JFontManager* fontMgr);

	virtual ~TestPainter();

	virtual void	Reset() override;

	virtual JRect	SetClipRect(const JRect& r) override;
	virtual void	SetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0) override;

	virtual void	StringNoSubstitutions(
						const JCoordinate left, const JCoordinate top,
						const JString& str) override;
	virtual void	String(const JFloat angle, const JCoordinate left,
						   const JCoordinate top, const JString& str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop) override;

	virtual void	Point(const JCoordinate x, const JCoordinate y) override;

	virtual void	Line(const JCoordinate x1, const JCoordinate y1,
						 const JCoordinate x2, const JCoordinate y2) override;

	virtual void	Rect(const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h) override;

	virtual void	Ellipse(const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h) override;

	virtual void	Arc(const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h,
						const JFloat startAngle, const JFloat deltaAngle) override;

	virtual void	Polygon(const JCoordinate left, const JCoordinate top,
							const JPolygon& poly) override;

	virtual void	Image(const JImage& image, const JRect& srcRect, const JRect& destRect) override;

private:

	// not allowed

	TestPainter(const TestPainter& source);
	const TestPainter& operator=(const TestPainter& source);
};

#endif
