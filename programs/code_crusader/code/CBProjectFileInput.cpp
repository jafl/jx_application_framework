/******************************************************************************
 CBProjectFileInput.cpp

	Input field for entering a path + project file.

	BASE CLASS = JXFileInput

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBProjectFileInput.h"
#include "CBProjectDocument.h"
#include <JColorManager.h>
#include <JStringIterator.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBProjectFileInput::CBProjectFileInput
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXFileInput(jnew StyledText(this, enclosure->GetFontManager()),
				enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBProjectFileInput::~CBProjectFileInput()
{
}

/******************************************************************************
 ComputeErrorLength (virtual protected)

	Draw the file in red if it is not a project file.

 ******************************************************************************/

JSize
CBProjectFileInput::StyledText::ComputeErrorLength
	(
	JXFSInputBase*	field,
	const JSize		totalLength,	// original character count
	const JString&	fullName		// modified path
	)
	const
{
	if (fullName.IsEmpty())
		{
		return totalLength;
		}

	if (CBProjectDocument::CanReadFile(fullName) != JXFileDocument::kFileReadable)
		{
		JStringIterator iter(fullName, kJIteratorStartAtEnd);
		if (iter.Prev(ACE_DIRECTORY_SEPARATOR_STR))
			{
			return fullName.GetCharacterCount() - iter.GetNextCharacterIndex();
			}
		else
			{
			return totalLength;
			}
		}
	else
		{
		return JXFileInput::StyledText::ComputeErrorLength(field, totalLength, fullName);
		}
}
