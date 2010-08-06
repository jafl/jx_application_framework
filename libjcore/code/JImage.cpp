/******************************************************************************
 JImage.cpp

	Pure virtual base class to represent an offscreen picture.

	BASE CLASS = none

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JImage.h>
#include <JImageMask.h>
#include <JColormap.h>
#include <JStdError.h>
#include <jFileUtil.h>
#include <jGlobals.h>
#include <jAssert.h>

const int kGDNoTransparentColor = -1;
const int kGDColorScale         = 65535/255;

// JError data

const JCharacter* JImage::kUnknownFileType  = "UnknownFileType::JImage";
const JCharacter* JImage::kFileIsNotGIF     = "FileIsNotGIF::JImage";
const JCharacter* JImage::kGIFNotAvailable  = "GIFNotAvailable::JImage";
const JCharacter* JImage::kFileIsNotPNG     = "FileIsNotPNG::JImage";
const JCharacter* JImage::kPNGNotAvailable  = "PNGNotAvailable::JImage";
const JCharacter* JImage::kFileIsNotJPEG    = "FileIsNotJPEG::JImage";
const JCharacter* JImage::kJPEGNotAvailable = "JPEGNotAvailable::JImage";
const JCharacter* JImage::kFileIsNotXPM     = "FileIsNotXPM::JImage";
const JCharacter* JImage::kXPMNotAvailable  = "XPMNotAvailable::JImage";
const JCharacter* JImage::kFileIsNotXBM     = "FileIsNotXBM::JImage";
const JCharacter* JImage::kTooManyColors    = "TooManyColors::JImage";

/******************************************************************************
 Constructor

 ******************************************************************************/

JImage::JImage
	(
	const JCoordinate	width,
	const JCoordinate	height,
	JColormap*			colormap
	)
	:
	itsWidth( width ),
	itsHeight( height ),
	itsColormap( colormap )
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
	itsColormap( source.itsColormap )
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
	const JCharacter* fileName
	)
{
	assert( !JStringEmpty(fileName) );

	FILE* input = fopen(fileName, "rb");
	if (input == NULL)
		{
		return kUnknownType;
		}

	const JSize bufSize = 10;
	char buffer [ bufSize ];
	const JSize count = fread(buffer, sizeof(char), bufSize, input);
	fclose(input);

	if (JCompareMaxN(buffer, "GIF", 3, kJTrue))
		{
		return kGIFType;
		}
	else if (JCompareMaxN(buffer, "\x89PNG", 4, kJTrue))
		{
		return kPNGType;
		}
	else if (JCompareMaxN(buffer, "\xFF\xD8\xFF", 3, kJTrue))
		{
		return kJPEGType;
		}
	else if (JCompareMaxN(buffer, "/* XPM */", 9, kJTrue))
		{
		return kXPMType;
		}
	else if (JCompareMaxN(buffer, "#define", 7, kJTrue) ||
			 JCompareMaxN(buffer, "/*"     , 2, kJTrue))
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
	const JCharacter* fileName
	)
{
#ifdef _J_HAS_GIF

	const JError err = ReadGD(fileName, gdImageCreateFromGif);
	if (err.Is(kUnknownFileType))
		{
		return FileIsNotGIF(fileName);
		}
	else
		{
		return err;
		}

#else

	return GIFNotAvailable();

#endif
}

/******************************************************************************
 WriteGIF

 ******************************************************************************/

JError
JImage::WriteGIF
	(
	const JCharacter*	fileName,
	const JBoolean		compressColorsToFit,
	const JBoolean		interlace
	)
	const
{
#ifdef _J_HAS_GIF

	return WriteGD(fileName, kJFalse, compressColorsToFit, interlace, gdImageGif);

#else

	return GIFNotAvailable();

#endif
}

/******************************************************************************
 ReadGD (private)

	Returns an error if the file does not exist, does not contain a file
	that can be read by the specified function, or contains too many
	colors.

	Color approximation is controlled by the settings in the colormap object.

	If this function returns UnknownFileType, it should be translated to
	the approrpriate FileIsNot* message.

 ******************************************************************************/

JError
JImage::ReadGD
	(
	const JCharacter*	fileName,
	gdImagePtr			(*imageCreateFromFile)(FILE *fd)
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
	if (image == NULL)
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
				JColorIndex color =
					itsColormap->GetColor(gdImageRed  (image, c) * kGDColorScale,
										  gdImageGreen(image, c) * kGDColorScale,
										  gdImageBlue (image, c) * kGDColorScale);
				SetColor(x,y, color);
				}
			}

		ImageDataFinished();

		gdImageDestroy(image);
		image = NULL;

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

		JColorIndex* colorTable = new JColorIndex [ colorCount ];
		assert( colorTable != NULL );

		const JColorIndex blackColor = itsColormap->GetBlackColor();

		for (JIndex i=0; i<colorCount; i++)
			{
			if (!hasMask || i != (JIndex) maskColor)
				{
				colorTable[i] =
					itsColormap->GetColor(gdImageRed  (image, i) * kGDColorScale,
										  gdImageGreen(image, i) * kGDColorScale,
										  gdImageBlue (image, i) * kGDColorScale);
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
		image = NULL;

		SetImageData(colorCount, colorTable, cols, hasMask, maskColor);

		// clean up

		delete [] colorTable;
		delete [] data;
		delete [] cols;

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
	const JCharacter*	fileName,
	const JBoolean		useTrueColor,
	const JBoolean		compressColorsToFit,
	const JBoolean		interlace,
	void				(*imageWriteToFile)(gdImagePtr im, FILE *out)
	)
	const
{
	gdImagePtr image = NULL;
	if (useTrueColor)
		{
		image = gdImageCreateTrueColor(itsWidth, itsHeight);
		}
	else
		{
		image = gdImageCreate(itsWidth, itsHeight);
		}
	if (image == NULL)
		{
		return JNoProcessMemory();
		}

	gdImageInterlace(image, interlace);

	// check if we have a mask

	int maxColorCount = gdMaxColors;

	JImageMask* mask       = NULL;
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
				color          = itsColormap->GetRGB(GetColor(x,y)) / kGDColorScale;
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

	Color approximation is controlled by the settings in the colormap object.

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

	JColorIndex* colorTable = new JColorIndex [ colorCount ];
	assert( colorTable != NULL );

	// decode color table

	const JColorIndex blackColor = itsColormap->GetBlackColor();

	JBoolean hasMask        = kJFalse;
	unsigned long maskColor = 0;

	for (i=1; i<=colorCount; i++)
		{
		charToCTIndex[ (unsigned char) pixmap.xpm[i][0] ] = (unsigned char) i-1;

		JIndex j = 0;
		JCharacter c;
		do
			{
			c = pixmap.xpm[i][j+4];
			j++;
			}
			while (c != '\0' && c != '\t');

		const JString colorName(pixmap.xpm[i] + 4, j-1);
		if (JStringCompare(colorName, "none", kJFalse) == 0)
			{
			hasMask         = kJTrue;
			maskColor       = i-1;
			colorTable[i-1] = blackColor;
			}
		else if (!itsColormap->GetColor(colorName, &colorTable[i-1]))
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

	delete [] colorTable;
	delete [] data;
	delete [] cols;
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
	*data = new unsigned short [ w * h ];
	if (*data == NULL)
		{
		return JNoProcessMemory();
		}

	*cols = new unsigned short* [ w ];
	if (*cols == NULL)
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

static const JCharacter* kJImageMsgMap[] =
	{
	"name", NULL
	};


JImage::UnknownFileType::UnknownFileType
	(
	const JCharacter* fileName
	)
	:
	JError(JImage::kUnknownFileType, "")
{
	kJImageMsgMap[1] = fileName;
	SetMessage(kJImageMsgMap, sizeof(kJImageMsgMap));
}

JImage::FileIsNotGIF::FileIsNotGIF
	(
	const JCharacter* fileName
	)
	:
	JError(JImage::kFileIsNotGIF, "")
{
	kJImageMsgMap[1] = fileName;
	SetMessage(kJImageMsgMap, sizeof(kJImageMsgMap));
}

JImage::FileIsNotPNG::FileIsNotPNG
	(
	const JCharacter* fileName
	)
	:
	JError(JImage::kFileIsNotPNG, "")
{
	kJImageMsgMap[1] = fileName;
	SetMessage(kJImageMsgMap, sizeof(kJImageMsgMap));
}

JImage::FileIsNotJPEG::FileIsNotJPEG
	(
	const JCharacter* fileName
	)
	:
	JError(JImage::kFileIsNotJPEG, "")
{
	kJImageMsgMap[1] = fileName;
	SetMessage(kJImageMsgMap, sizeof(kJImageMsgMap));
}

JImage::FileIsNotXPM::FileIsNotXPM
	(
	const JCharacter* fileName
	)
	:
	JError(JImage::kFileIsNotXPM, "")
{
	kJImageMsgMap[1] = fileName;
	SetMessage(kJImageMsgMap, sizeof(kJImageMsgMap));
}

JImage::FileIsNotXBM::FileIsNotXBM
	(
	const JCharacter* fileName
	)
	:
	JError(JImage::kFileIsNotXBM, "")
{
	kJImageMsgMap[1] = fileName;
	SetMessage(kJImageMsgMap, sizeof(kJImageMsgMap));
}
