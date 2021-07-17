/******************************************************************************
 CBCommandPathInput.cpp

	Input field for entering a file path which allows @.

	BASE CLASS = JXPathInput

	Copyright © 2005 by John Lindal.

 ******************************************************************************/

#include <CBCommandPathInput.h>
#include <JXColorManager.h>
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
	JXPathInput(jnew StyledText(this, enclosure->GetFontManager()),
				enclosure, hSizing, vSizing, x,y, w,h)
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

	Returns true if the current path is valid.  In this case, *path is
	set to the full path, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

bool
CBCommandPathInput::GetPath
	(
	JString* path
	)
	const
{
	const JString& text = GetText().GetText();
	if (text == "@")
		{
		*path = text;
		return true;
		}
	else
		{
		return JXPathInput::GetPath(path);
		}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

bool
CBCommandPathInput::InputValid()
{
	if (GetText()->GetText() == "@")
		{
		return true;
		}
	else
		{
		return JXPathInput::InputValid();
		}
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

	Accept paths starting with "@"

 ******************************************************************************/

void
CBCommandPathInput::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const bool			deletion
	)
{
	if (!text.IsEmpty() && text.GetFirstCharacter() == '@')
		{
		const JSize totalLength = text.GetCharacterCount();
		JFont f                 = styles->GetFirstElement();
		styles->RemoveAll();
		f.SetColor(JColorManager::GetBlackColor());
		styles->AppendElements(f, totalLength);

		*recalcRange = *redrawRange = JStyledText::TextRange(
			JCharacterRange(1, totalLength),
			JUtf8ByteRange(1, text.GetByteCount()));
		}
	else
		{
		return JXPathInput::StyledText::AdjustStylesBeforeBroadcast(
			text, styles, recalcRange, redrawRange, deletion);
		}
}

/******************************************************************************
 GetTextColor (static)

	Draw the entire text red if the path is invalid.  This is provided
	so tables can draw the text the same way as the input field.

	base can be nullptr.  If you use CBCommandPathInput for relative paths, base
	should be the path passed to SetBasePath().

 ******************************************************************************/

JColorID
CBCommandPathInput::GetTextColor
	(
	const JString&	path,
	const JString&	base,
	const bool	requireWrite
	)
{
	if (path.GetFirstCharacter() == '@')
		{
		return JColorManager::GetBlackColor();
		}
	else
		{
		return JXPathInput::GetTextColor(path, base, requireWrite);
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
	if (!GetText().IsEmpty() && GetText().GetText().GetFirstCharacter() == '@')
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
