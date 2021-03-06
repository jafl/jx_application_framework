/******************************************************************************
 CBCommandPathInput.cpp

	Input field for entering a file path which allows @.

	BASE CLASS = JXPathInput

	Copyright (C) 2005 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <CBCommandPathInput.h>
#include <JXColormap.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

CBCommandPathInput::CBCommandPathInput
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
	JXPathInput(enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBCommandPathInput::~CBCommandPathInput()
{
}

/******************************************************************************
 GetPath (virtual)

	Returns kJTrue if the current path is valid.  In this case, *path is
	set to the full path, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

JBoolean
CBCommandPathInput::GetPath
	(
	JString* path
	)
	const
{
	const JString& text = GetText();
	if (text == "@")
		{
		*path = text;
		return kJTrue;
		}
	else
		{
		return JXPathInput::GetPath(path);
		}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
CBCommandPathInput::InputValid()
{
	if (GetText() == "@")
		{
		return kJTrue;
		}
	else
		{
		return JXPathInput::InputValid();
		}
}

/******************************************************************************
 AdjustStylesBeforeRecalc (virtual protected)

	Accept "@"

 ******************************************************************************/

void
CBCommandPathInput::AdjustStylesBeforeRecalc
	(
	const JString&		buffer,
	JRunArray<JFont>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion
	)
{
	if (!buffer.IsEmpty() && buffer.GetFirstCharacter() == '@')
		{
		const JColormap* colormap = GetColormap();
		const JSize totalLength   = buffer.GetLength();
		JFont f                   = styles->GetFirstElement();
		styles->RemoveAll();
		f.SetColor(colormap->GetBlackColor());
		styles->AppendElements(f, totalLength);
		*redrawRange += JIndexRange(1, totalLength);
		}
	else
		{
		return JXPathInput::AdjustStylesBeforeRecalc(buffer, styles, recalcRange,
													 redrawRange, deletion);
		}
}

/******************************************************************************
 GetTextColor (static)

	Draw the entire text red if the path is invalid.  This is provided
	so tables can draw the text the same way as the input field.

	base can be NULL.  If you use CBCommandPathInput for relative paths, base
	should be the path passed to SetBasePath().

 ******************************************************************************/

JColorIndex
CBCommandPathInput::GetTextColor
	(
	const JCharacter*	path,
	const JCharacter*	base,
	const JBoolean		requireWrite,
	const JColormap*	colormap
	)
{
	if (*path == '@')
		{
		return colormap->GetBlackColor();
		}
	else
		{
		return JXPathInput::GetTextColor(path, base, requireWrite, colormap);
		}
}

/******************************************************************************
 GetTextForChoosePath (virtual)

	Returns a string that can safely be passed to JChooseSaveFile::ChooseFile().

 ******************************************************************************/

JString
CBCommandPathInput::GetTextForChoosePath()
	const
{
	if (!GetText().IsEmpty() && GetText().GetFirstCharacter() == '@')
		{
		JString s;
		if (!GetBasePath(&s) &&
			!JGetHomeDirectory(&s))
			{
			s = JGetRootDirectory();
			}
		return s;
		}
	else
		{
		return JXPathInput::GetTextForChoosePath();
		}
}
