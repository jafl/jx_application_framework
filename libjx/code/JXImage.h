/******************************************************************************
 JXImage.h

	Interface for the JXImage class

	Copyright (C) 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXImage
#define _H_JXImage

#include <JImage.h>
#include <JConstBitmap.h>
#include <JBroadcaster.h>
#include <JArray.h>
#include <jXConstants.h>
#include <X11/Xlib.h>

class JXDisplay;
class JXGC;
class JXColormap;
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
			const JColorIndex initColor = kJXTransparentColor, // placeholder for GetDefaultBackColor()
			const JSize depth = 0, const State initState = kRemoteStorage);

	JXImage(JXDisplay* display, Drawable source);
	JXImage(JXDisplay* display, Drawable source, const JRect& rect);

	JXImage(JXDisplay* display, const JConstBitmap& bitmap,
			const JColorIndex foreColor = kJXTransparentColor, // placeholder for GetBlackColor()
			const JColorIndex backColor = kJXTransparentColor, // placeholder for GetDefaultBackColor()
			const JSize depth = 0);

	JXImage(JXDisplay* display, const JXPM& data);

	JXImage(const JXImage& source);
	JXImage(const JXImage& source, const JRect& rect);

	virtual ~JXImage();

	static JError	CreateFromFile(JXDisplay* display,
								   const JString& fileName, JXImage** image);
	static JError	CreateFromGIF(JXDisplay* display,
								  const JString& fileName, JXImage** image);
	static JError	CreateFromPNG(JXDisplay* display,
								  const JString& fileName, JXImage** image);
	static JError	CreateFromJPEG(JXDisplay* display,
								   const JString& fileName, JXImage** image);
	static JError	CreateFromXPM(JXDisplay* display,
								  const JString& fileName, JXImage** image);

	JError	WriteXPM(const JString& fileName) const;

	virtual JXImage*	Copy() const;

	JXDisplay*	GetDisplay() const;
	JXColormap*	GetXColormap() const;
	JSize		GetDepth() const;

	JBoolean			HasMask() const;
	virtual JBoolean	GetMask(JImageMask** mask) const;
	JBoolean			GetMask(JXImageMask** mask) const;
	void				SetMask(JXImageMask* mask);
	void				ClearMask();

	void	Draw(const Drawable drawable, JXGC* gc,
				 const JRect& srcRect, const JRect& destRect) const;

	JXImagePainter*	CreatePainter();

	virtual JColorIndex	GetColor(const JCoordinate x, const JCoordinate y) const;
	virtual void		SetColor(const JCoordinate x, const JCoordinate y,
								 const JColorIndex color);

	State	GetDefaultState() const;
	void	SetDefaultState(const State state);
	void	ConvertToDefaultState();

	State	GetCurrentState() const;
	void	ConvertToLocalStorage();
	void	ConvertToRemoteStorage();

	Pixmap	CreatePixmap() const;
	XImage*	CreateXImage() const;

	// called by JImageMask

	virtual unsigned long	GetSystemColor(const JColorIndex color) const;
	virtual unsigned long	GetSystemColor(const JCoordinate x, const JCoordinate y) const;

protected:

	XImage*	GetImage() const;
	void	ConvertToImage() const;

	Pixmap	GetPixmap() const;
	void	ConvertToPixmap() const;

	const JXGC*	GetGC() const;
	void		ForcePrivateGC();

	virtual void	SetImageData(const JSize colorCount, const JColorIndex* colorTable,
								 unsigned short** imageData,
								 const JBoolean hasMask, const unsigned long maskColor);
	virtual void	PrepareForImageData();
	virtual void	ImageDataFinished();

	// used by JXImageMask

	JXImage(const Pixmap bitmap,
			const JCoordinate width, const JCoordinate height,
			JXDisplay* display);

private:

	JXDisplay*	itsDisplay;		// we don't own this
	JXColormap*	itsXColormap;	// we don't own this
	JXGC*		itsGC;			// NULL if using display's default GC
	JSize		itsDepth;

	State			itsDefState;
	Drawable		itsPixmap;
	XImage*			itsImage;
	JXImageMask*	itsMask;	// can be NULL

	JArray<JColorIndex>*	itsColorList;		// can be NULL

private:

	JXImage(JXDisplay* display);

	void	JXImageX(JXDisplay* display, const JSize depth = 0);
	void	JXImageFromDrawable(JXDisplay* display, Drawable source, const JRect& rect);

	// not allowed

	const JXImage& operator=(const JXImage& source);
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
 GetXColormap

 ******************************************************************************/

inline JXColormap*
JXImage::GetXColormap()
	const
{
	return itsXColormap;
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

inline JBoolean
JXImage::HasMask()
	const
{
	return JConvertToBoolean( itsMask != NULL );
}

/******************************************************************************
 GetMask

 ******************************************************************************/

inline JBoolean
JXImage::GetMask
	(
	JXImageMask** mask
	)
	const
{
	*mask = itsMask;
	return JConvertToBoolean( itsMask != NULL );
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
