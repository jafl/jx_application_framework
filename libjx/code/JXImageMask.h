/******************************************************************************
 JXImageMask.h

	Interface for the JXImageMask class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageMask
#define _H_JXImageMask

#include <JXImage.h>
#include <JImageMask.h>
#include <jXConstants.h>		// need definition of kJXTransparentColor

class JXColormap;
class JXWindowIcon;

class JXImageMask : public JXImage, public JImageMask
{
	friend class JXWindowIcon;

public:

	static const JColorIndex kPixelOff;

public:

	JXImageMask(JXDisplay* display,
				const JCoordinate width, const JCoordinate height,
				const JBoolean filled);

	JXImageMask(JXDisplay* display, const JConstBitmap& bitmap);

	JXImageMask(JXDisplay* display, Drawable source);
	JXImageMask(JXDisplay* display, Drawable source, const JRect& rect);

	JXImageMask(const JXImage& image, const JColorIndex color);

	JXImageMask(const JXImageMask& source);
	JXImageMask(const JXImageMask& source, const JRect& rect);

	virtual ~JXImageMask();

	static JError	CreateFromXBM(JXDisplay* display,
								  const JString& fileName, JXImageMask** mask);

	JError	WriteXBM(const JString& fileName,
					 const JPoint& hotSpot = JPoint(-1,-1)) const;

	virtual JXImage*	Copy() const;

	virtual JBoolean	ContainsPixel(const JCoordinate x, const JCoordinate y) const;
	virtual void		AddPixel(const JCoordinate x, const JCoordinate y);
	virtual void		RemovePixel(const JCoordinate x, const JCoordinate y);

	static unsigned long	ColorToBit(const JColorIndex color);
	static JColorIndex		BitToColor(const unsigned long bit,
									   JXColormap* colormap);

private:

	JXImageMask(const Pixmap bitmap,
				const JCoordinate width, const JCoordinate height,
				JXDisplay* display);

	// not allowed

	const JXImageMask& operator=(const JXImageMask& source);
};


/******************************************************************************
 ColorToBit (static)

	Convert the given JColorIndex to a bit for use in an X bitmap.

	Any color other than kPixelOff converts to "pixel on".  This way,
	the drawing code that produces an image can be reused with a JXImagePainter
	attached to the JXImageMask to generate a mask.

 ******************************************************************************/

inline unsigned long
JXImageMask::ColorToBit
	(
	const JColorIndex color
	)
{
	return (color == kPixelOff ? 0 : 1);
}

#endif
