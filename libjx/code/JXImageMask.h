/******************************************************************************
 JXImageMask.h

	Interface for the JXImageMask class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImageMask
#define _H_JXImageMask

#include "JXImage.h"
#include <jx-af/jcore/JImageMask.h>
#include "jXConstants.h"		// need definition of kJXTransparentColor

class JXWindowIcon;

class JXImageMask : public JXImage, public JImageMask
{
	friend class JXWindowIcon;

public:

	static const JColorID kPixelOff;

public:

	JXImageMask(JXDisplay* display,
				const JCoordinate width, const JCoordinate height,
				const bool filled);

	JXImageMask(JXDisplay* display, const JConstBitmap& bitmap);

	JXImageMask(JXDisplay* display, Drawable source);
	JXImageMask(JXDisplay* display, Drawable source, const JRect& rect);

	JXImageMask(const JXImage& image, const JColorID color);

	JXImageMask(const JXImageMask& source);
	JXImageMask(const JXImageMask& source, const JRect& rect);

	~JXImageMask() override;

	static JError	CreateFromXBM(JXDisplay* display,
								  const JString& fileName, JXImageMask** mask);

	JError	WriteXBM(const JString& fileName,
					 const JPoint& hotSpot = JPoint(-1,-1)) const;

	JXImage*	Copy() const override;

	bool	ContainsPixel(const JCoordinate x, const JCoordinate y) const override;
	void	AddPixel(const JCoordinate x, const JCoordinate y) override;
	void	RemovePixel(const JCoordinate x, const JCoordinate y) override;

	static unsigned long	ColorToBit(const JColorID color);
	static JColorID			BitToColor(const unsigned long bit);

private:

	JXImageMask(const Pixmap bitmap,
				const JCoordinate width, const JCoordinate height,
				JXDisplay* display);
};


/******************************************************************************
 ColorToBit (static)

	Convert the given JColorID to a bit for use in an X bitmap.

	Any color other than kPixelOff converts to "pixel on".  This way,
	the drawing code that produces an image can be reused with a JXImagePainter
	attached to the JXImageMask to generate a mask.

 ******************************************************************************/

inline unsigned long
JXImageMask::ColorToBit
	(
	const JColorID color
	)
{
	return (color == kPixelOff ? 0 : 1);
}

#endif
