/******************************************************************************
 JImage.cpp

	Pure virtual base class to represent an offscreen picture.

	BASE CLASS = none

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#include <JImage.h>
#include <JImageMask.h>
#include <JColorManager.h>
#include <JStdError.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

const int kGDNoTransparentColor = -1;
const int kGDColorScale         = 65535/255;

// JError data

const JUtf8Byte* JImage::kUnknownFileType  = "UnknownFileType::JImage";
const JUtf8Byte* JImage::kFileIsNotGIF     = "FileIsNotGIF::JImage";
const JUtf8Byte* JImage::kGIFNotAvailable  = "GIFNotAvailable::JImage";
const JUtf8Byte* JImage::kFileIsNotPNG     = "FileIsNotPNG::JImage";
const JUtf8Byte* JImage::kPNGNotAvailable  = "PNGNotAvailable::JImage";
const JUtf8Byte* JImage::kFileIsNotJPEG    = "FileIsNotJPEG::JImage";
const JUtf8Byte* JImage::kJPEGNotAvailable = "JPEGNotAvailable::JImage";
const JUtf8Byte* JImage::kFileIsNotXPM     = "FileIsNotXPM::JImage";
const JUtf8Byte* JImage::kXPMNotAvailable  = "XPMNotAvailable::JImage";
const JUtf8Byte* JImage::kFileIsNotXBM     = "FileIsNotXBM::JImage";
const JUtf8Byte* JImage::kTooManyColors    = "TooManyColors::JImage";

/******************************************************************************
 Constructor

 ******************************************************************************/

JImage::JImage
	(
	const JCoordinate	width,
	const JCoordinate	height,
	JColorManager*			colorManager
	)
	:
	itsWidth( width ),
	itsHeight( height ),
	itsColorManager( colorManager )
{
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JImage::JImage
	(
	const JImage& source
	)
	:
	itsWidth( source.itsWidth ),
	itsHeight( source.itsHeight ),
	itsColorManager( source.itsColorManager )
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JImage::~JImage()
{
}

/******************************************************************************
 GetFileType (static)

	Returns the type of image stored in the file.

	We do not deal with compressed files because most image formats are
	already inherently compressed.

 ******************************************************************************/

JImage::FileType
JImage::GetFileType
	(
	const JString& fileName
	)
{
	assert( !fileName.IsEmpty() );

	FILE* input = fopen(fileName.GetBytes(), "rb");
	if (input == nullptr)
		{
		return kUnknownType;
		}

	const JSize bufSize = 10;
	char buffer [ bufSize ];
	const JSize count = fread(buffer, sizeof(char), bufSize, input);
	fclose(input);

	if (JString::CompareMaxNBytes(buffer, "GIF", 3, kJTrue) == 0)
		{
		return kGIFType;
		}
	else if (JString::CompareMaxNBytes(buffer, "\x89PNG", 4, kJTrue) == 0)
		{
		return kPNGType;
		}
	else if (JString::CompareMaxNBytes(buffer, "\xFF\xD8\xFF", 3, kJTrue) == 0)
		{
		return kJPEGType;
		}
	else if (JString::CompareMaxNBytes(buffer, "/* XPM */", 9, kJTrue) == 0)
		{
		return kXPMType;
		}
	else if (JString::CompareMaxNBytes(buffer, "#define", 7, kJTrue) == 0 ||
			 JString::CompareMaxNBytes(buffer, "/*"     , 2, kJTrue) == 0)
		{
		return kXBMType;
		}
	else
		{
		return kUnknownType;
		}
}

/******************************************************************************
 ReadGIF (protected)

 ******************************************************************************/

JError
JImage::ReadGIF
	(
	const JString& fileName
	)
{
	const JError err = ReadGD(fileName, gdImageCreateFromGif);
	if (err.Is(kUnknownFileType))
		{
		return FileIsNotGIF(fileName);
		}
	else
		{
		return err;
		}
}

/******************************************************************************
 WriteGIF

 ******************************************************************************/

JError
JImage::WriteGIF
	(
	const JString&	fileName,
	const JBoolean	compressColorsToFit,
	const JBoolean	interlace
	)
	const
{
	return WriteGD(fileName, kJFalse, compressColorsToFit, interlace, gdImageGif);
}

/******************************************************************************
 ReadPNG (protected)

 ******************************************************************************/

JError
JImage::ReadPNG
	(
	const JString& fileName
	)
{
	const JError err = ReadGD(fileName, gdImageCreateFromPng);
	if (err.Is(kUnknownFileType))
		{
		return FileIsNotPNG(fileName);
		}
	else
		{
		return err;
		}
}

/******************************************************************************
 WritePNG

 ******************************************************************************/

JError
JImage::WritePNG
	(
	const JString&	fileName,
	const JBoolean	useTrueColor,
	const JBoolean	compressColorsToFit,
	const JBoolean	interlace
	)
	const
{
	return WriteGD(fileName, useTrueColor, compressColorsToFit, interlace, gdImagePng);
}

/******************************************************************************
 ReadJPEG (protected)

 ******************************************************************************/

JError
JImage::ReadJPEG
	(
	const JString& fileName
	)
{
	const JError err = ReadGD(fileName, gdImageCreateFromJpeg);
	if (err.Is(kUnknownFileType))
		{
		return FileIsNotJPEG(fileName);
		}
	else
		{
		return err;
		}
}

/******************************************************************************
 WriteJPEG

	If quality is negative, you get the default tradeoff.  Otherwise,
	quality must lie in the range [0,95].

 ******************************************************************************/

static int jQuality = -1;
static void imageJpeg(gdImagePtr im, FILE *out);

void
imageJpeg
	(
	gdImagePtr	im,
	FILE*		out
	)
{
	gdImageJpeg(im, out, jQuality);
}

JError
JImage::WriteJPEG
	(
	const JString&	fileName,
	const JBoolean	interlace,
	const int		quality
	)
	const
{
	jQuality = quality;
	return WriteGD(fileName, kJTrue, kJTrue, interlace, imageJpeg);
}

/******************************************************************************
 ReadGD (private)

	Returns an error if the file does not exist, does not contain a file
	that can be read by the specified function, or contains too many
	colors.

	Color approximation is controlled by the settings in the colorManager object.

	If this function returns UnknownFileType, it should be translated to
	the approrpriate FileIsNot* message.

 ******************************************************************************/

JError
JImage::ReadGD
	(
	const JString&	fileName,
	gdImagePtr		(*imageCreateFromFile)(FILE *fd)
	)
{
	// read file

	FILE* input;
	const JError fopenErr = JFOpen(fileName, "rb", &input);
	if (!fopenErr.OK())
		{
		return fopenErr;
		}
	gdImagePtr image = imageCreateFromFile(input);
	fclose(input);
	if (image == nullptr)
		{
		return UnknownFileType(fileName);
		}

	itsWidth  = gdImageSX(image);
	itsHeight = gdImageSY(image);

	const int maskColor     = gdImageGetTransparent(image);
	const JBoolean hasMask  = JNegate( maskColor == kGDNoTransparentColor );

	if (gdImageTrueColor(image))
		{
		PrepareForImageData();

		for (JCoordinate x=0; x<itsWidth; x++)
			{
			for (JCoordinate y=0; y<itsHeight; y++)
				{
				const int c = gdImageGetPixel(image, x,y);
				JColorID color =
					JColorManager::GetColorID(JRGB(
						gdImageRed  (image, c) * kGDColorScale,
						gdImageGreen(image, c) * kGDColorScale,
						gdImageBlue (image, c) * kGDColorScale));
				SetColor(x,y, color);
				}
			}

		ImageDataFinished();

		gdImageDestroy(image);
		image = nullptr;

		return JNoError();
		}
	else
		{
		// allocate space for image data

		unsigned short* data;
		unsigned short** cols;
		const JError allocErr = AllocateImageData(itsWidth, itsHeight, &data, &cols);
		if (!allocErr.OK())
			{
			gdImageDestroy(image);
			return allocErr;
			}

		// build color table

		const JSize colorCount = gdImageColorsTotal(image);

		JColorID* colorTable = jnew JColorID [ colorCount ];
		assert( colorTable != nullptr );

		const JColorID blackColor = JColorManager::GetBlackColor();

		for (JIndex i=0; i<colorCount; i++)
			{
			if (!hasMask || i != (JIndex) maskColor)
				{
				colorTable[i] =
					JColorManager::GetColorID(JRGB(
						gdImageRed  (image, i) * kGDColorScale,
						gdImageGreen(image, i) * kGDColorScale,
						gdImageBlue (image, i) * kGDColorScale));
				}
			}

		// convert image data

		for (JCoordinate x=0; x<itsWidth; x++)
			{
			for (JCoordinate y=0; y<itsHeight; y++)
				{
				cols[x][y] = gdImageGetPixel(image, x,y);
				}
			}

		gdImageDestroy(image);	// free up memory as soon as possible
		image = nullptr;

		SetImageData(colorCount, colorTable, cols, hasMask, maskColor);

		// clean up

		jdelete [] colorTable;
		jdelete [] data;
		jdelete [] cols;

		return JNoError();
		}
}

/******************************************************************************
 WriteGD (private)

	If useTrueColor==kJTrue, you have no worries about color issues.
	Otherwise, returns JNoError if there were at most 256 colors or
	compressColorsToFit==kJTrue.  Otherwise, returns TooManyColors to
	indicate that the image could not be written.

	If compressColorsToFit==kJTrue, colors beyond 256 will be ignored.  The
	correct way to handle this is to adjust -all- the colors in the image
	so there are less than 256, which is beyond the scope of this function,
	since there are so many ways to do it.  Obviously, compressColorsToFit
	is irrelevant if useTrueColor==kJTrue.

	If the image has a mask, this uses up one color.

 ******************************************************************************/

JError
JImage::WriteGD
	(
	const JString&	fileName,
	const JBoolean	useTrueColor,
	const JBoolean	compressColorsToFit,
	const JBoolean	interlace,
	void			(*imageWriteToFile)(gdImagePtr im, FILE *out)
	)
	const
{
	gdImagePtr image = nullptr;
	if (useTrueColor)
		{
		image = gdImageCreateTrueColor(itsWidth, itsHeight);
		}
	else
		{
		image = gdImageCreate(itsWidth, itsHeight);
		}
	if (image == nullptr)
		{
		return JNoProcessMemory();
		}

	gdImageInterlace(image, interlace);

	// check if we have a mask

	int maxColorCount = gdMaxColors;

	JImageMask* mask       = nullptr;
	const JBoolean hasMask = GetMask(&mask);
	if (hasMask)
		{
		maxColorCount--;	// need space for transparent color
		}

	// convert image data

	JRGB color;
	for (JCoordinate x=0; x<itsWidth; x++)
		{
		for (JCoordinate y=0; y<itsHeight; y++)
			{
			if (!hasMask || mask->ContainsPixel(x,y))
				{
				color          = JColorManager::GetRGB(GetColor(x,y)) / kGDColorScale;
				int colorIndex = gdImageColorExact(image, color.red, color.green, color.blue);
				if (colorIndex == -1 &&
					(useTrueColor || gdImageColorsTotal(image) < maxColorCount))
					{
					colorIndex = gdImageColorAllocate(image, color.red, color.green, color.blue);
					if (colorIndex == -1 && !compressColorsToFit)
						{
						gdImageDestroy(image);
						return TooManyColors();
						}
					}
				if (colorIndex == -1)
					{
					colorIndex = 0;
					}

				gdImageSetPixel(image, x,y, colorIndex);
				}
			}
		}

	// The mask color has to be a color that is not used elsewhere
	// in the image.

	if (useTrueColor)
		{
		// The user would need to give us a color because searching the
		// entire color space would take too long.
		}
	else if (hasMask)
		{
		int maskColor;

		// The only way to find such a color is to scan the color table
		// after building it.

		color.Set(1,1,1);	// unlikely color, since so close to black
		while (1)
			{
			if (gdImageColorExact(image, color.red, color.green, color.blue) == -1)
				{
				maskColor = gdImageColorAllocate(image, color.red, color.green, color.blue);
				assert( maskColor != -1 );
				break;
				}
			(color.red)++;
			if (color.red > 255)
				{
				color.red = 0;
				(color.green)++;
				}
			if (color.green > 255)
				{
				color.green = 0;
				(color.blue)++;
				}
			}

		gdImageSaveAlpha(image, 0);		// allow transparent color
		gdImageColorTransparent(image, maskColor);

		for (JCoordinate x=0; x<itsWidth; x++)
			{
			for (JCoordinate y=0; y<itsHeight; y++)
				{
				if (!mask->ContainsPixel(x,y))
					{
					gdImageSetPixel(image, x,y, maskColor);
					}
				}
			}
		}

	// write file

	FILE* output;
	const JError fopenErr = JFOpen(fileName, "wb", &output);
	if (!fopenErr.OK())
		{
		gdImageDestroy(image);
		return fopenErr;
		}
	imageWriteToFile(image, output);
	const int fileErr = ferror(output);
	fclose(output);

	// clean up

	gdImageDestroy(image);

	if (fileErr == 0)
		{
		return JNoError();
		}
	else
		{
		return JUnknownError(fileErr);
		}
}

/******************************************************************************
 ReadFromJXPM (protected)

	This only works for XPM's with up to 255 colors.  This shouldn't be
	a problem, because even with that many, you shouldn't be using XPM's.

	Color approximation is controlled by the settings in the colorManager object.

 ******************************************************************************/

void
JImage::ReadFromJXPM
	(
	const JXPM& pixmap
	)
{
JIndex i;

	// decode width and height

	JSize colorCount, imageCharSize;
	const int readCount = sscanf(pixmap.xpm[0], "%ld %ld %lu %lu",
								 &itsWidth, &itsHeight, &colorCount, &imageCharSize);
	assert( readCount == 4 );
	assert( colorCount < 256 );

	// init color table (never more than 255 unique colors)

	unsigned char charToCTIndex[256];	// maps XPM chars to color table indices
	for (i=0; i<256; i++)
		{
		charToCTIndex[i] = 0;
		}

	JColorID* colorTable = jnew JColorID [ colorCount ];
	assert( colorTable != nullptr );

	// decode color table

	const JColorID blackColor = JColorManager::GetBlackColor();

	JBoolean hasMask        = kJFalse;
	unsigned long maskColor = 0;

	for (i=1; i<=colorCount; i++)
		{
		charToCTIndex[ (unsigned char) pixmap.xpm[i][0] ] = (unsigned char) i-1;

		JIndex j = 0;
		JUtf8Byte c;
		do
			{
			c = pixmap.xpm[i][j+4];
			j++;
			}
			while (c != '\0' && c != '\t');

		const JString colorName(pixmap.xpm[i] + 4, j-1);
		if (JString::Compare(colorName, "none", kJFalse) == 0)
			{
			hasMask         = kJTrue;
			maskColor       = i-1;
			colorTable[i-1] = blackColor;
			}
		else if (!itsColorManager->GetColorID(colorName, &colorTable[i-1]))
			{
			colorTable[i-1] = blackColor;
			}
		}

	// allocate space for image data

	unsigned short* data;
	unsigned short** cols;
	const JError allocErr = AllocateImageData(itsWidth, itsHeight, &data, &cols);
	assert_ok( allocErr /* internal XPM's should not be huge */ );

	// decode image data

	JIndex lineIndex = colorCount+1;
	for (JCoordinate y=0; y<itsHeight; y++)
		{
		JIndex charIndex = 0;
		for (JCoordinate x=0; x<itsWidth; x++)
			{
			cols[x][y] = charToCTIndex[ (unsigned char) pixmap.xpm[lineIndex][charIndex] ];
			charIndex += imageCharSize;
			}
		lineIndex++;
		}

	SetImageData(colorCount, colorTable, cols, hasMask, maskColor);

	// clean up

	jdelete [] colorTable;
	jdelete [] data;
	jdelete [] cols;
}

/******************************************************************************
 AllocateImageData (protected)

	Allocate space for image data of the given width and height.
	Delete both arrays with "delete []".  This data can be passed to
	SetImageData().

	We return an error because it is not unreasonable that the user might
	request an image larger than will fit in memory.

 ******************************************************************************/

JError
JImage::AllocateImageData
	(
	const JSize			w,
	const JSize			h,
	unsigned short**	data,
	unsigned short***	cols
	)
{
	*data = jnew unsigned short [ w * h ];
	if (*data == nullptr)
		{
		return JNoProcessMemory();
		}

	*cols = jnew unsigned short* [ w ];
	if (*cols == nullptr)
		{
		return JNoProcessMemory();
		}

	for (JIndex i=0; i<w; i++)
		{
		(*cols)[i] = *data + i*h;
		}

	return JNoError();
}

/******************************************************************************
 JError constructors

 ******************************************************************************/

JImage::UnknownFileType::UnknownFileType
	(
	const JString& fileName
	)
	:
	JError(JImage::kUnknownFileType, "")
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JImage::FileIsNotGIF::FileIsNotGIF
	(
	const JString& fileName
	)
	:
	JError(JImage::kFileIsNotGIF, "")
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JImage::FileIsNotPNG::FileIsNotPNG
	(
	const JString& fileName
	)
	:
	JError(JImage::kFileIsNotPNG, "")
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JImage::FileIsNotJPEG::FileIsNotJPEG
	(
	const JString& fileName
	)
	:
	JError(JImage::kFileIsNotJPEG, "")
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JImage::FileIsNotXPM::FileIsNotXPM
	(
	const JString& fileName
	)
	:
	JError(JImage::kFileIsNotXPM, "")
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}

JImage::FileIsNotXBM::FileIsNotXBM
	(
	const JString& fileName
	)
	:
	JError(JImage::kFileIsNotXBM, "")
{
	const JUtf8Byte* map[] = { "name", fileName.GetBytes() };
	SetMessage(map, sizeof(map));
}
