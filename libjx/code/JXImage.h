/******************************************************************************
 JXImage.h

	Interface for the JXImage class

	Copyright (C) 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXImage
#define _H_JXImage

#include <jx-af/jcore/JImage.h>
#include <jx-af/jcore/JConstBitmap.h>
#include <jx-af/jcore/JBroadcaster.h>
#include <jx-af/jcore/JArray.h>
#include "jXConstants.h"
#include <X11/Xlib.h>
#include <X11/xpm.h>

class JXDisplay;
class JXGC;
class JXColorManager;
class JXImageMask;
class JXImagePainter;

class JXImage : public JImage, public virtual JBroadcaster
{
	friend class JXWindow;			// for setting window icon
	friend class JXImagePainter;

public:

	enum State
	{
		kLocalStorage,	// program
		kRemoteStorage	// X server
	};

public:

	JXImage(JXDisplay* display,
			const JCoordinate width, const JCoordinate height,
			const JColorID initColor = kJXTransparentColor, // placeholder for GetDefaultBackColor()
			const JSize depth = 0, const State initState = kRemoteStorage);

	JXImage(JXDisplay* display, Drawable source);
	JXImage(JXDisplay* display, Drawable source, const JRect& rect);

	JXImage(JXDisplay* display, const JConstBitmap& bitmap,
			const JColorID foreColor = kJXTransparentColor, // placeholder for GetBlackColor()
			const JColorID backColor = kJXTransparentColor, // placeholder for GetDefaultBackColor()
			const JSize depth = 0);

	JXImage(JXDisplay* display, const JXPM& data);		// < 256 colors

	JXImage(const JXImage& source);
	JXImage(const JXImage& source, const JRect& rect);

	~JXImage() override;

	static JError	CreateFromFile(JXDisplay* display,
								   const JString& fileName, JXImage** image);
	static JError	CreateFromGIF(JXDisplay* display,
								  const JString& fileName, JXImage** image);
	static JError	CreateFromPNG(JXDisplay* display,
								  const JString& fileName, JXImage** image);
	static JError	CreateFromJPEG(JXDisplay* display,
								   const JString& fileName, JXImage** image);
	static JError	CreateFromXPM(JXDisplay* display,
								  const JXPM& data, JXImage** image);
	static JError	CreateFromXPM(JXDisplay* display,
								  const JString& fileName, JXImage** image);

	JError	WriteXPM(const JString& fileName) const;

	virtual JXImage*	Copy() const;

	JXDisplay*	GetDisplay() const;
	JSize		GetDepth() const;

	bool		HasMask() const;
	bool		GetMask(JImageMask** mask) const override;
	bool		GetMask(JXImageMask** mask) const;
	void		SetMask(JImageMask* mask) override;
	void		SetMask(JXImageMask* mask);
	void		ClearMask();
	JImageMask*	CreateEmptyMask() const override;

	void	Draw(const Drawable drawable, JXGC* gc,
				 const JRect& srcRect, const JRect& destRect) const;

	JXImagePainter*	CreatePainter();

	JColorID	GetColor(const JCoordinate x, const JCoordinate y) const override;
	void		SetColor(const JCoordinate x, const JCoordinate y,
						 const JColorID color) override;

	State	GetDefaultState() const;
	void	SetDefaultState(const State state);
	void	ConvertToDefaultState();

	State	GetCurrentState() const;
	void	ConvertToLocalStorage();
	void	ConvertToRemoteStorage();

	Pixmap	CreatePixmap() const;
	XImage*	CreateXImage() const;

	// called by JImageMask

	unsigned long	GetSystemColor(const JColorID color) const override;
	unsigned long	GetSystemColor(const JCoordinate x, const JCoordinate y) const override;

protected:

	XImage*	GetImage() const;
	void	ConvertToImage() const;

	Pixmap	GetPixmap() const;
	void	ConvertToPixmap() const;

	const JXGC*	GetGC() const;
	void		ForcePrivateGC();

	void	SetImageData(const JSize colorCount, const JColorID* colorTable,
						 unsigned short** imageData,
						 const bool hasMask, const unsigned long maskColor) override;
	void	PrepareForImageData() override;
	void	ImageDataFinished() override;

	// used by JXImageMask

	JXImage(const Pixmap bitmap,
			const JCoordinate width, const JCoordinate height,
			JXDisplay* display);

private:

	JXDisplay*	itsDisplay;		// we don't own this
	JXGC*		itsGC;			// nullptr if using display's default GC
	JSize		itsDepth;

	State			itsDefState;
	Drawable		itsPixmap;
	XImage*			itsImage;
	JXImageMask*	itsMask;	// can be nullptr

	JArray<JColorID>*	itsColorList;		// can be nullptr

private:

	JXImage(JXDisplay* display);

	void	JXImageX(JXDisplay* display, const JSize depth = 0);
	void	JXImageFromDrawable(JXDisplay* display, Drawable source, const JRect& rect);

	static void		InitXPMAttributes(JXDisplay* display, XpmAttributes* attr);
	static JXImage*	CreateImageAndMaskFromXPMData(JXDisplay* display,
												  const Pixmap image_pixmap,
												  const Pixmap mask_pixmap);
};


/******************************************************************************
 GetDisplay

 ******************************************************************************/

inline JXDisplay*
JXImage::GetDisplay()
	const
{
	return itsDisplay;
}

/******************************************************************************
 GetDepth

 ******************************************************************************/

inline JSize
JXImage::GetDepth()
	const
{
	return itsDepth;
}

/******************************************************************************
 HasMask

 ******************************************************************************/

inline bool
JXImage::HasMask()
	const
{
	return itsMask != nullptr;
}

/******************************************************************************
 GetMask

 ******************************************************************************/

inline bool
JXImage::GetMask
	(
	JXImageMask** mask
	)
	const
{
	*mask = itsMask;
	return itsMask != nullptr;
}

/******************************************************************************
 Default state

 ******************************************************************************/

inline JXImage::State
JXImage::GetDefaultState()
	const
{
	return itsDefState;
}

inline void
JXImage::SetDefaultState
	(
	const State state
	)
{
	itsDefState = state;
}

/******************************************************************************
 ConvertToLocalStorage

	Convert the image to local storage on the client side.

 ******************************************************************************/

inline void
JXImage::ConvertToLocalStorage()
{
	ConvertToImage();
}

/******************************************************************************
 ConvertToRemoteStorage

	Convert the image to remote storage on the server side.

 ******************************************************************************/

inline void
JXImage::ConvertToRemoteStorage()
{
	ConvertToPixmap();
}

/******************************************************************************
 Get X structure (protected)

 ******************************************************************************/

inline XImage*
JXImage::GetImage()
	const
{
	ConvertToImage();
	return itsImage;
}

inline Pixmap
JXImage::GetPixmap()
	const
{
	ConvertToPixmap();
	return itsPixmap;
}

#endif
