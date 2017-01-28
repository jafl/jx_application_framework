/******************************************************************************
 jTextUtils.cc

	Utilities built on top of libxml2.

	Copyright (C) 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "jTextUtil.h"
#include "JStringIterator.h"
#include <jAssert.h>

/******************************************************************************
 AnalyzeWhitespace

 *****************************************************************************/

void
JAnalyzeWhitespace
	(
	const JString&	buffer,
	const JSize		tabWidth,
	const JBoolean	defaultUseSpaces,
	JBoolean*		useSpaces,
	JBoolean*		isMixed
	)
{
	assert( tabWidth > 0 );

	*isMixed = kJFalse;

	JSize spaceLines = 0, tinySpaceLines = 0, tabLines = 0;

	JStringIterator iter(buffer);
	JUtf8Character c;
	do
		{
		JSize spaceCount = 0, tailSpaceCount = 0;
		JBoolean tabs = kJFalse;
		while (iter.Next(&c))
			{
			if (c == ' ')
				{
				spaceCount++;
				tailSpaceCount++;
				}
			else if (c == '\t')
				{
				tabs           = kJTrue;
				tailSpaceCount = 0;
				}
			else
				{
				break;
				}
			}

		if (spaceCount == tailSpaceCount && tailSpaceCount < tabWidth)
			{
			if (tabs)
				{
				tabLines++;
				}
			else if (spaceCount > 0)
				{
				tinySpaceLines++;
				}
			}
		else if (spaceCount > 0 && tabs)
			{
			*isMixed = kJTrue;

			if (defaultUseSpaces)
				{
				spaceLines++;
				}
			else
				{
				tabLines++;
				}
			}
		else if (spaceCount > 0)
			{
			spaceLines++;
			}
		}
		while ((c == "\n" || iter.Next("\n")) && !iter.AtEnd());

	if (tabLines > 0)
		{
		tabLines += tinySpaceLines;
		}
	else
		{
		spaceLines += tinySpaceLines;
		}

	if (tabLines > 0 && spaceLines > 0)
		{
		*isMixed = kJTrue;
		}

	*useSpaces = JI2B(spaceLines > tabLines || (spaceLines == tabLines && defaultUseSpaces));

//	std::cout << "space: " << spaceLines << ", tab: " << tabLines << std::endl;
}
