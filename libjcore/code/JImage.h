/******************************************************************************
 JImage.h

	Interface for the JImage class

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JImage
#define _H_JImage

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JRect.h>
#include <jColor.h>
#include <JError.h>
#include <JXPM.h>

#if defined _J_HAS_GD
#include <gd.h>
#endif

class JImageMask;
class JColormap;

class JImage
{
public:

	enum FileType
	{
		kUnknownType,
		kGIFType,
		kPNGType,
		kJPEGType,
		kXPMType,
		kXBMType
	};

public:

	JImage(const JCoordinate width, const JCoordinate height, JColormap* colormap);
	JImage(const JImage& source);

	virtual ~JImage();

	JCoordinate	GetWidth() const;
	JCoordinate	GetHeight() const;
	JRect		GetBounds() const;

	JColormap*			GetColormap() const;
	virtual JColorIndex	GetColor(const JCoordinate x, const JCoordinate y) const = 0;
	virtual void		SetColor(const JCoordinate x, const JCoordinate y,
								 const JColorIndex color) = 0;

	virtual JBoolean	GetMask(JImageMask** mask) const = 0;

	JError	WriteGIF(const JCharacter* fileName,
					 const JBoolean compressColorsToFit,
					 const JBoolean interlace = kJFalse) const;
	JError	WritePNG(const JCharacter* fileName,
					 const JBoolean useTrueColor = kJTrue,
					 const JBoolean compressColorsToFit = kJFalse,
					 const JBoolean interlace = kJFalse) const;
	JError	WriteJPEG(const JCharacter* fileName,
					  const JBoolean interlace = kJFalse,
					  const int quality = -1) const;

	static FileType	GetFileType(const JCharacter* fileName);

	// called by JImageMask

	virtual unsigned long	GetSystemColor(const JColorIndex color) const = 0;
	virtual unsigned long	GetSystemColor(const JCoordinate x, const JCoordinate y) const = 0;

protected:

	void	SetDimensions(const JCoordinate width, const JCoordinate height);

	JError	ReadGIF(const JCharacter* fileName);
	JError	ReadPNG(const JCharacter* fileName);
	JError	ReadJPEG(const JCharacter* fileName);
	void	ReadFromJXPM(const JXPM& pixmap);

	virtual void	SetImageData(const JSize colorCount, const JColorIndex* colorTable,
								 unsigned short** imageData,
								 const JBoolean hasMask, const unsigned long maskColor) = 0;
	virtual void	PrepareForImageData() = 0;
	virtual void	ImageDataFinished() = 0;

	JError	AllocateImageData(const JSize w, const JSize h, unsigned short** data,
							  unsigned short*** cols);

private:

	JCoordinate	itsWidth;
	JCoordinate	itsHeight;
	JColormap*	itsColormap;	// we don't own this

private:

	#if defined _J_HAS_GD

	JError	ReadGD(const JCharacter* fileName,
				   gdImagePtr (*imageCreateFromFile)(FILE *fd));
	JError	WriteGD(const JCharacter* fileName, const JBoolean useTrueColor,
					const JBoolean compressColorsToFit, const JBoolean interlace,
					void (*imageWriteToFile)(gdImagePtr im, FILE *out)) const;

	#endif

	// not allowed

	const JImage& operator=(const JImage& source);

public:

	// JError classes

	static const JCharacter* kUnknownFileType;
	static const JCharacter* kFileIsNotGIF;
	static const JCharacter* kGIFNotAvailable;
	static const JCharacter* kFileIsNotPNG;
	static const JCharacter* kPNGNotAvailable;
	static const JCharacter* kFileIsNotJPEG;
	static const JCharacter* kJPEGNotAvailable;
	static const JCharacter* kFileIsNotXPM;
	static const JCharacter* kXPMNotAvailable;
	static const JCharacter* kFileIsNotXBM;
	static const JCharacter* kTooManyColors;

	class UnknownFileType : public JError
		{
		public:

			UnknownFileType(const JCharacter* fileName);
		};

	class FileIsNotGIF : public JError
		{
		public:

			FileIsNotGIF(const JCharacter* fileName);
		};

	class GIFNotAvailable : public JError
		{
		public:

			GIFNotAvailable()
				:
				JError(kGIFNotAvailable)
			{ };
		};

	class FileIsNotPNG : public JError
		{
		public:

			FileIsNotPNG(const JCharacter* fileName);
		};

	class PNGNotAvailable : public JError
		{
		public:

			PNGNotAvailable()
				:
				JError(kPNGNotAvailable)
			{ };
		};

	class FileIsNotJPEG : public JError
		{
		public:

			FileIsNotJPEG(const JCharacter* fileName);
		};

	class JPEGNotAvailable : public JError
		{
		public:

			JPEGNotAvailable()
				:
				JError(kJPEGNotAvailable)
			{ };
		};

	class FileIsNotXPM : public JError
		{
		public:

			FileIsNotXPM(const JCharacter* fileName);
		};

	class XPMNotAvailable : public JError
		{
		public:

			XPMNotAvailable()
				:
				JError(kXPMNotAvailable)
			{ };
		};

	class FileIsNotXBM : public JError
		{
		public:

			FileIsNotXBM(const JCharacter* fileName);
		};

	class TooManyColors : public JError
		{
		public:

			TooManyColors()
				:
				JError(kTooManyColors)
			{ };
		};
};


/******************************************************************************
 Dimensions

 ******************************************************************************/

inline JCoordinate
JImage::GetWidth()
	const
{
	return itsWidth;
}

inline JCoordinate
JImage::GetHeight()
	const
{
	return itsHeight;
}

inline JRect
JImage::GetBounds()
	const
{
	return JRect(0, 0, itsHeight, itsWidth);
}

inline void
JImage::SetDimensions
	(
	const JCoordinate width,
	const JCoordinate height
	)
{
	itsWidth  = width;
	itsHeight = height;
}

/******************************************************************************
 GetColormap

 ******************************************************************************/

inline JColormap*
JImage::GetColormap()
	const
{
	return itsColormap;
}

#endif
