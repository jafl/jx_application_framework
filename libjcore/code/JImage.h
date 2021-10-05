/******************************************************************************
 JImage.h

	Interface for the JImage class

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JImage
#define _H_JImage

#include "jx-af/jcore/JRect.h"
#include "jx-af/jcore/jColor.h"
#include "jx-af/jcore/JError.h"
#include "jx-af/jcore/JXPM.h"
#include <gd.h>

class JImageMask;
class JColorManager;

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

	JImage(const JCoordinate width, const JCoordinate height, JColorManager* colorManager);
	JImage(const JImage& source);

	virtual ~JImage();

	JCoordinate	GetWidth() const;
	JCoordinate	GetHeight() const;
	JRect		GetBounds() const;

	JColorManager*		GetColorManager() const;
	virtual JColorID	GetColor(const JCoordinate x, const JCoordinate y) const = 0;
	virtual void		SetColor(const JCoordinate x, const JCoordinate y,
								 const JColorID color) = 0;

	virtual bool	GetMask(JImageMask** mask) const = 0;

	JError	WriteGIF(const JString& fileName,
					 const bool compressColorsToFit,
					 const bool interlace = false) const;
	JError	WritePNG(const JString& fileName,
					 const bool useTrueColor = true,
					 const bool compressColorsToFit = false,
					 const bool interlace = false) const;
	JError	WriteJPEG(const JString& fileName,
					  const bool interlace = false,
					  const int quality = -1) const;

	static FileType	GetFileType(const JString& fileName);

	// called by JImageMask

	virtual unsigned long	GetSystemColor(const JColorID color) const = 0;
	virtual unsigned long	GetSystemColor(const JCoordinate x, const JCoordinate y) const = 0;

protected:

	void	SetDimensions(const JCoordinate width, const JCoordinate height);

	JError	ReadGIF(const JString& fileName);
	JError	ReadPNG(const JString& fileName);
	JError	ReadJPEG(const JString& fileName);
	void	ReadFromJXPM(const JXPM& pixmap);

	virtual void	SetImageData(const JSize colorCount, const JColorID* colorTable,
								 unsigned short** imageData,
								 const bool hasMask, const unsigned long maskColor) = 0;
	virtual void	PrepareForImageData() = 0;
	virtual void	ImageDataFinished() = 0;

	static JError	AllocateImageData(const JCoordinate w, const JCoordinate h,
									  unsigned short** data, unsigned short*** cols);

private:

	JCoordinate	itsWidth;
	JCoordinate	itsHeight;
	JColorManager*	itsColorManager;	// we don't own this

private:

	JError	ReadGD(const JString& fileName,
				   gdImagePtr (*imageCreateFromFile)(FILE *fd));
	JError	WriteGD(const JString& fileName, const bool useTrueColor,
					const bool compressColorsToFit, const bool interlace,
					void (*imageWriteToFile)(gdImagePtr im, FILE *out)) const;

	// not allowed

	JImage& operator=(const JImage&) = delete;

public:

	// JError classes

	static const JUtf8Byte* kUnknownFileType;
	static const JUtf8Byte* kFileIsNotGIF;
	static const JUtf8Byte* kGIFNotAvailable;
	static const JUtf8Byte* kFileIsNotPNG;
	static const JUtf8Byte* kPNGNotAvailable;
	static const JUtf8Byte* kFileIsNotJPEG;
	static const JUtf8Byte* kJPEGNotAvailable;
	static const JUtf8Byte* kFileIsNotXPM;
	static const JUtf8Byte* kXPMNotAvailable;
	static const JUtf8Byte* kFileIsNotXBM;
	static const JUtf8Byte* kTooManyColors;

	class UnknownFileType : public JError
	{
	public:

		UnknownFileType(const JString& fileName);
	};

	class FileIsNotGIF : public JError
	{
	public:

		FileIsNotGIF(const JString& fileName);
	};

	class FileIsNotPNG : public JError
	{
	public:

		FileIsNotPNG(const JString& fileName);
	};

	class FileIsNotJPEG : public JError
	{
	public:

		FileIsNotJPEG(const JString& fileName);
	};

	class FileIsNotXPM : public JError
	{
	public:

		FileIsNotXPM(const JString& fileName);
	};

	class FileIsNotXBM : public JError
	{
	public:

		FileIsNotXBM(const JString& fileName);
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
 GetColorManager

 ******************************************************************************/

inline JColorManager*
JImage::GetColorManager()
	const
{
	return itsColorManager;
}

#endif
