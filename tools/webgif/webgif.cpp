/******************************************************************************
 webgif.cpp

	Provides the following functions related to GIF files:

		Get info: colors, size, interlacing

		Set transparent color.

		Turn interlacing on/off.

		Pad the colormap to get 17 colors.  This forces Netscape to dither
		it instead of performing color substitution.

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include <gd.h>
#include <JString.h>
#include <jColor.h>
#include <jMath.h>
#include <jFileUtil.h>
#include <JMinMax.h>
#include <jCommandLine.h>
#include <stdlib.h>
#include <jAssert.h>

// Constants

const JSize kMinColorCount        = 17;
const int kNoTransparentColor     = -1;
const int kTransparentColorNotSet = -2;

// Types

enum LaceOption
{
	kTurnInterlaceOff = 0,
	kTurnInterlaceOn  = 1,
	kIgnoreInterlace
};

// Prototypes

gdImagePtr	ReadGIF(const JString& fileName);
void		WriteGIF(gdImagePtr image, const JString& fileName);
void		PrintGIFInfo(gdImagePtr image);
JBoolean	SetTransparentColor(gdImagePtr image, const int color);
JBoolean	SetInterlace(gdImagePtr image, const LaceOption interlace);
JBoolean	PadColormap(gdImagePtr image);

void	Blend(gdImagePtr image, const JFloat alpha, const JRGB& alphaColor,
			  const JString& outputFileName);

void ParseOptions(const JSize argc, char* argv[], JString* fileName,
				  JBoolean* printInfo, int* transparentColor,
				  LaceOption* interlace, JBoolean* padColormap,
				  JBoolean* blend, JFloat* alpha, JRGB* alphaColor,
				  JString* blendOutput);

void PrintHelp();
void PrintVersion();

/******************************************************************************
 main

 ******************************************************************************/

int
main
	(
	int		argc,
	char*	argv[]
	)
{
	JString fileName;
	JBoolean printInfo;
	int transparentColor;
	LaceOption interlace;
	JBoolean padColormap;
	JBoolean blend;
	JFloat alpha;
	JRGB alphaColor;
	JString blendOutput;
	ParseOptions(argc, argv, &fileName, &printInfo, &transparentColor,
				 &interlace, &padColormap, &blend, &alpha, &alphaColor, &blendOutput);

	JBoolean changed = kJFalse;
	gdImagePtr image = ReadGIF(fileName);

	if (printInfo)
		{
		PrintGIFInfo(image);
		}

	if (SetTransparentColor(image, transparentColor))
		{
		changed = kJTrue;
		}

	if (SetInterlace(image, interlace))
		{
		changed = kJTrue;
		}

	if (padColormap && PadColormap(image))
		{
		changed = kJTrue;
		}

	if (blend)
		{
		Blend(image, alpha, alphaColor, blendOutput);
		}

	if (changed)
		{
		WriteGIF(image, fileName);
		}

	gdImageDestroy(image);
	return 0;
}

/******************************************************************************
 ParseOptions

 ******************************************************************************/

void
ParseOptions
	(
	const JSize	argc,
	char*		argv[],
	JString*	fileName,
	JBoolean*	printInfo,
	int*		transparentColor,
	LaceOption*	interlace,
	JBoolean*	padColormap,
	JBoolean*	blend,
	JFloat*		alpha,
	JRGB*		alphaColor,
	JString*	blendOutput
	)
{
	if (argc == 1)
		{
		PrintHelp();
		exit(0);
		}

	fileName->Clear();
	*printInfo        = kJFalse;
	*transparentColor = kTransparentColorNotSet;
	*interlace        = kIgnoreInterlace;
	*padColormap      = kJFalse;
	*blend            = kJFalse;

	JIndex index = 1;
	while (index < argc)
		{
		if (strcmp(argv[index], "-h") == 0 ||
			strcmp(argv[index], "--help") == 0)
			{
			PrintHelp();
			exit(0);
			}
		else if (strcmp(argv[index], "-v") == 0 ||
				 strcmp(argv[index], "--version") == 0)
			{
			PrintVersion();
			exit(0);
			}

		else if (strcmp(argv[index], "-i") == 0)
			{
			*printInfo = kJTrue;
			}

		else if (strcmp(argv[index], "-t") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			if (strcmp(argv[index], "none") == 0)
				{
				*transparentColor = kNoTransparentColor;
				}
			else if ('0' <= argv[index][0] && argv[index][0] <= '9')
				{
				*transparentColor = atoi(argv[index]);
				}
			else
				{
				std::cerr << argv[0] << ": -t requires a number or none" << std::endl;
				exit(1);
				}
			}

		else if (strcmp(argv[index], "-l") == 0)
			{
			JCheckForValues(1, &index, argc, argv);
			if (strcmp(argv[index], "on") == 0)
				{
				*interlace = kTurnInterlaceOn;
				}
			else if (strcmp(argv[index], "off") == 0)
				{
				*interlace = kTurnInterlaceOff;
				}
			else
				{
				std::cerr << argv[0] << ": -l requires either on or off" << std::endl;
				exit(1);
				}
			}

		else if (strcmp(argv[index], "-p") == 0)
			{
			*padColormap = kJTrue;
			}

		else if (strcmp(argv[index], "-b") == 0)
			{
			JCheckForValues(5, &index, argc, argv);
			*blend            = kJTrue;
			*alpha            = atof(argv[index]);
			alphaColor->red   = atoi(argv[index+1]);
			alphaColor->green = atoi(argv[index+2]);
			alphaColor->blue  = atoi(argv[index+3]);
			*blendOutput      = argv[index+4];
			index += 4;
			}

		else if (argv[index][0] == '-')
			{
			std::cerr << argv[0] << ": unknown option " << argv[index] << std::endl;
			exit(1);
			}

		else if (fileName->IsEmpty())
			{
			*fileName = argv[index];
			if (!JFileReadable(*fileName))
				{
				std::cerr << argv[0] << ": file is unreadable" << std::endl;
				exit(1);
				}
			if (!JFileWritable(*fileName))
				{
				std::cerr << argv[0] << ": file is unwriteable" << std::endl;
				exit(1);
				}
			}

		else
			{
			std::cerr << argv[0] << ": too many parameters" << std::endl;
			exit(1);
			}
		index++;
		}

	if (fileName->IsEmpty())
		{
		std::cerr << argv[0] << ": no input file specified" << std::endl;
		exit(1);
		}

	if (argc == 2)
		{
		*printInfo = kJTrue;
		}
}

/******************************************************************************
 PrintHelp

	ruler:	 01234567890123456789012345678901234567890123456789012345678901234567890123456789

 ******************************************************************************/

void
PrintHelp()
{
	std::cout << std::endl;
	std::cout << "Usage: [-i] [-t index|none] [-l on|off] [-p] input_file" << std::endl;
	std::cout << std::endl;
	std::cout << "This program provides the following functions related to GIF files:" << std::endl;
	std::cout << std::endl;
	std::cout << "-v  Prints version information" << std::endl;
	std::cout << "-h  Prints this help" << std::endl;
	std::cout << "-i  Get info: colors, size, interlacing (default if no options specified)" << std::endl;
	std::cout << "-t  Set transparent color (0-255 or none)" << std::endl;
	std::cout << "-l  Turn interlacing on/off" << std::endl;
	std::cout << "-p  Pad the colormap so that Netscape will dither it" << std::endl;
	std::cout << "-b  alpha R G B file -- alpha blend with specified color" << std::endl;
	std::cout << std::endl;
}

/******************************************************************************
 PrintVersion

 ******************************************************************************/

void
PrintVersion()
{
	std::cout << std::endl;
	std::cout << "webgif 1.0.0\n";
	std::cout << std::endl;
	std::cout << "Copyright (C) 1998 by John Lindal." << std::endl;
	std::cout << "This program may be freely distributed at no charge." << std::endl;
	std::cout << std::endl;
}

/******************************************************************************
 ReadGIF

 ******************************************************************************/

gdImagePtr
ReadGIF
	(
	const JString& fileName
	)
{
	FILE* input = fopen(fileName.GetBytes(), "rb");
	assert( input != nullptr );
	gdImagePtr image = gdImageCreateFromGif(input);
	if (image == nullptr)
		{
		std::cerr << fileName << " is not a GIF" << std::endl;
		exit(1);
		}
	fclose(input);
	return image;
}

/******************************************************************************
 WriteGIF

 ******************************************************************************/

void
WriteGIF
	(
	gdImagePtr		image,
	const JString&	fileName
	)
{
	FILE* output = fopen(fileName.GetBytes(), "wb");
	assert( output != nullptr );
	gdImageGif(image, output);
	assert( ferror(output) == 0 );
	fclose(output);
}

/******************************************************************************
 PrintGIFInfo

	Print colormap, # of colors, transparent color index, size,
	and interlacing status.

	By printing the general info last, we ensure that it won't scroll
	off the screen.

 ******************************************************************************/

void
PrintGIFInfo
	(
	gdImagePtr image
	)
{
	std::cout << std::endl;
	std::cout << "Index\tRed\tGreen\tBlue" << std::endl;

	const JSize colorCount = gdImageColorsTotal(image);
	for (JUnsignedOffset i=0; i<colorCount; i++)
		{
		std::cout << i;
		std::cout << '\t' << gdImageRed  (image, i);
		std::cout << '\t' << gdImageGreen(image, i);
		std::cout << '\t' << gdImageBlue (image, i);
		std::cout << std::endl;
		}

	std::cout << std::endl;

	std::cout << "Number of colors:        " << colorCount << std::endl;

	const int transparentColor = gdImageGetTransparent(image);
	std::cout << "Transparent color index: ";
	if (transparentColor == kNoTransparentColor)
		{
		std::cout << "none";
		}
	else
		{
		std::cout << transparentColor;
		}
	std::cout << std::endl;

	std::cout << std::endl;

	std::cout << "Width:      " << gdImageSX(image) << std::endl;
	std::cout << "Height:     " << gdImageSY(image) << std::endl;
	std::cout << "Interlaced: ";
	if (gdImageGetInterlaced(image))
		{
		std::cout << "yes";
		}
	else
		{
		std::cout << "no";
		}
	std::cout << std::endl;

	std::cout << std::endl;
}

/******************************************************************************
 SetTransparentColor

 ******************************************************************************/

JBoolean
SetTransparentColor
	(
	gdImagePtr	image,
	const int	color
	)
{
	if (color != kTransparentColorNotSet)
		{
		gdImageColorTransparent(image, color);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SetInterlace

 ******************************************************************************/

JBoolean
SetInterlace
	(
	gdImagePtr			image,
	const LaceOption	interlace
	)
{
	if (interlace != kIgnoreInterlace)
		{
		gdImageInterlace(image, interlace);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 PadColormap

	Pad colormap by changing pixels in lower right corner.

 ******************************************************************************/

JBoolean
PadColormap
	(
	gdImagePtr image
	)
{
	JSize colorCount = gdImageColorsTotal(image);
	if (colorCount >= kMinColorCount)
		{
		return kJFalse;
		}

	const JSize extraColorCount = kMinColorCount - colorCount;

	int x = gdImageSX(image) - extraColorCount;
	if (x < 0)
		{
		std::cerr << "image is too small to fit extra colors on single raster line" << std::endl;
		exit(1);
		}

	int y = gdImageSY(image) - 1;

	int c = gdImageGetPixel(image, x,y);
	int r = gdImageRed  (image, c);
	int g = gdImageGreen(image, c);
	int b = gdImageBlue (image, c);

	int delta = -1;
	if (r < 127 || g < 127 || b < 127)
		{
		delta = +1;
		}

	for (JIndex i=1; i<=extraColorCount; i++)
		{
		assert( x < gdImageSX(image) );

		while ((c = gdImageColorExact(image, r,g,b)) != -1)
			{
			r = JMax(0, JMin(r + delta, 255));
			g = JMax(0, JMin(g + delta, 255));
			b = JMax(0, JMin(b + delta, 255));
			}

		c = gdImageColorAllocate(image, r,g,b);
		assert( c != -1 );
		gdImageSetPixel(image, x,y, c);

		x++;
		}

	return kJTrue;
}

/******************************************************************************
 Blend

	Alpha blend the image with the given color.

 ******************************************************************************/

inline int
BlendComponent
	(
	const int		c1,
	const int		c2,
	const JFloat	alpha
	)
{
	return JRound((1.0 - alpha) * c1 + alpha * c2);
}

void
Blend
	(
	gdImagePtr		image,
	const JFloat	alpha,
	const JRGB&		alphaColor,
	const JString&	outputFileName
	)
{
	const JSize w = gdImageSX(image);
	const JSize h = gdImageSY(image);

	gdImagePtr i2 = gdImageCreate(w,h);

	for (JIndex x=0; x<w; x++)
		{
		for (JIndex y=0; y<h; y++)
			{
			const int c = gdImageGetPixel(image, x,y);
			const int r = BlendComponent(gdImageRed(image, c),   alphaColor.red,   alpha);
			const int g = BlendComponent(gdImageGreen(image, c), alphaColor.green, alpha);
			const int b = BlendComponent(gdImageBlue(image, c),  alphaColor.blue,  alpha);

			int c2 = gdImageColorExact(i2, r,g,b);
			if (c2 == -1)
				{
				c2 = gdImageColorAllocate(i2, r,g,b);
				if (c2 == -1)
					{
					c2 = gdImageColorClosest(i2, r,g,b);
					}
				}

			gdImageSetPixel(i2, x,y, c2);
			}
		}

	WriteGIF(i2, outputFileName);
	gdImageDestroy(i2);
}
