/******************************************************************************
 testgif.cpp

	Writes 4 gif files that can be used for testing:

		red_scale.gif, green_scale.gif, blue_scale.gif, gray_scale.gif

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include <gd.h>
#include <stdlib.h>
#include <jTypes.h>
#include <jAssert.h>

void CreateGIF(const bool incr[], const JUtf8Byte* fileName);

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
{
	const bool incr[3] = { true, false, false };
	CreateGIF(incr, "red_scale.gif");
}

{
	const bool incr[3] = { false, true, false };
	CreateGIF(incr, "green_scale.gif");
}

{
	const bool incr[3] = { false, false, true };
	CreateGIF(incr, "blue_scale.gif");
}

{
	const bool incr[3] = { true, true, true };
	CreateGIF(incr, "gray_scale.gif");
}

	return 0;
}

/******************************************************************************
 CreateGIF

	If incr[i]==true, then the i-th color component scans from 0 to 255.

 ******************************************************************************/

const JCoordinate kImageHeight = 10;

void
CreateGIF
	(
	const bool		incr[],
	const JUtf8Byte*	fileName
	)
{
	gdImagePtr image = gdImageCreate(256, kImageHeight);
	assert( image != nullptr );

	unsigned long rgb[3] = {0,0,0};
	for (JCoordinate x=0; x<256; x++)
	{
		const int colorIndex = gdImageColorAllocate(image, rgb[0], rgb[1], rgb[2]);
		assert( colorIndex != -1 );

		gdImageLine(image, x,0, x,kImageHeight-1, colorIndex);

		for (JUnsignedOffset i=0; i<3; i++)
		{
			if (incr[i])
			{
				rgb[i]++;
			}
		}
	}

	FILE* output = fopen(fileName, "wb");
	assert( output != nullptr );
	gdImageGif(image, output);
	assert( ferror(output) == 0 );
	fclose(output);

	gdImageDestroy(image);
}
