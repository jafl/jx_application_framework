/******************************************************************************
 JXTextSelection.cpp

	text/plain, text/x-jxstyled0 for all, and STRING, TEXT for all but DND.
	DELETE is supported if a JTextEditor is provided.

	BASE CLASS = JXSelectionData

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include <JXTextSelection.h>
#include <JXDisplay.h>
#include <sstream>
#include <jAssert.h>

static const JUtf8Byte* kStyledText0XAtomName = "text/x-jxstyled0";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextSelection::JXTextSelection
	(
	JXDisplay*				display,
	const JString&			text,
	const JRunArray<JFont>*	style
	)
	:
	JXSelectionData(display)
{
	JXTextSelectionX();
	SetData(text, style);
}

JXTextSelection::JXTextSelection
	(
	JXDisplay*			display,
	JString*			text,
	JRunArray<JFont>*	style
	)
	:
	JXSelectionData(display)
{
	JXTextSelectionX();
	SetData(text, style);
}

JXTextSelection::JXTextSelection
	(
	JXDisplay*					display,
	const JPtrArray<JString>&	list
	)
	:
	JXSelectionData(display)
{
	JXTextSelectionX();
	SetData(list);
}

JXTextSelection::JXTextSelection
	(
	JXWidget*			widget,
	const JUtf8Byte*	id
	)
	:
	JXSelectionData(widget, id)
{
	JXTextSelectionX();
}

// private

void
JXTextSelection::JXTextSelectionX()
{
	itsText  = nullptr;
	itsStyle = nullptr;
	itsTE    = nullptr;

	itsStyledText0XAtom = None;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXTextSelection::~JXTextSelection()
{
	jdelete itsText;
	jdelete itsStyle;
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
JXTextSelection::AddTypes
	(
	const Atom selectionName
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();

	AddType(selMgr->GetMimePlainTextXAtom());
	itsStyledText0XAtom = AddType(kStyledText0XAtomName);

	if (selectionName != GetDNDSelectionName())
		{
		AddType(selMgr->GetUtf8StringXAtom());
		if (itsText == nullptr || itsText->IsAscii())
			{
			AddType(XA_STRING);
			}
		}
}

/******************************************************************************
 SetData

	Makes a copy of text and style.

	colormap and style can be nullptr.
	If style != nullptr, colormap must not be nullptr.

 ******************************************************************************/

void
JXTextSelection::SetData
	(
	const JString&			text,
	const JRunArray<JFont>*	style
	)
{
	if (itsText != nullptr)
		{
		*itsText = text;
		}
	else
		{
		itsText = jnew JString(text);
		assert( itsText != nullptr );
		}

	if (style != nullptr && itsStyle != nullptr)
		{
		*itsStyle = *style;
		}
	else if (style != nullptr && itsStyle == nullptr)
		{
		itsStyle = jnew JRunArray<JFont>(*style);
		assert( itsStyle != nullptr );
		}
	else
		{
		jdelete itsStyle;
		itsStyle = nullptr;
		}

	SetTextEditor(nullptr, JCharacterRange());
}

/******************************************************************************
 SetData

	Takes ownership of text and style.

	colormap and style can be nullptr.
	If style != nullptr, colormap must not be nullptr.

 ******************************************************************************/

void
JXTextSelection::SetData
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
{
	jdelete itsText;
	itsText = text;

	jdelete itsStyle;
	itsStyle = style;

	SetTextEditor(nullptr, JCharacterRange());
}

/******************************************************************************
 SetData

	If there is a single string, this becomes the text.
	If there are multiple strings, they are separated with newlines.

 ******************************************************************************/

void
JXTextSelection::SetData
	(
	const JPtrArray<JString>& list
	)
{
	if (itsText == nullptr)
		{
		itsText = jnew JString;
		assert( itsText != nullptr );
		}
	else
		{
		itsText->Clear();
		}

	jdelete itsStyle;
	itsStyle = nullptr;

	SetTextEditor(nullptr, JCharacterRange());

	*itsText = list.Join("\n");
	if (list.GetElementCount() > 1)
		{
		itsText->Append("\n");
		}
}

/******************************************************************************
 SetTextEditor

	Call this to support DELETE.

 ******************************************************************************/

void
JXTextSelection::SetTextEditor
	(
	JTextEditor*			te,
	const JCharacterRange&	selection
	)
{
	if (itsTE != nullptr)
		{
		StopListening(itsTE);
		}
	itsTE = te;
	if (itsTE != nullptr)
		{
		ClearWhenGoingAway(itsTE, &itsTE);
		}

	itsSelection = selection;
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
JXTextSelection::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	*bitsPerBlock = 8;

	JXSelectionManager* selMgr = GetSelectionManager();
	JCharacterRange selection;

	if ((requestType == XA_STRING ||
		 requestType == selMgr->GetUtf8StringXAtom() ||
		 requestType == selMgr->GetMimePlainTextXAtom()) &&
		itsText != nullptr)
		{
		*returnType = requestType;
		*dataLength = itsText->GetByteCount();
		*data       = jnew unsigned char[ *dataLength ];
		if (*data != nullptr)
			{
			memcpy(*data, itsText->GetRawBytes(), *dataLength);
			return kJTrue;
			}
		}

	else if (requestType == itsStyledText0XAtom &&
			 itsText != nullptr && itsStyle != nullptr)
		{
		const JFileVersion vers = 1;
		std::ostringstream dataStream;
		JStyledText::WritePrivateFormat(dataStream, vers, *itsText, *itsStyle,
										JCharacterRange(1, itsText->GetCharacterCount()));

		const std::string s = dataStream.str();
		*returnType         = itsStyledText0XAtom;
		*dataLength         = s.length();
		*data               = jnew unsigned char[ *dataLength ];
		if (*data != nullptr)
			{
			memcpy(*data, s.data(), *dataLength);
			return kJTrue;
			}
		}

	else if (requestType == selMgr->GetDeleteSelectionXAtom() &&
			 itsTE != nullptr && IsCurrent() &&
			 itsTE->GetSelection(&selection) && selection == itsSelection)
		{
		itsTE->DeleteSelection();

		*data       = jnew unsigned char[1];
		*dataLength = 0;
		*returnType = selMgr->GetNULLXAtom();
		return kJTrue;
		}

	*data       = nullptr;
	*dataLength = 0;
	*returnType = None;
	return kJFalse;
}

/******************************************************************************
 GetStyledText0XAtomName (static)

 ******************************************************************************/

const JUtf8Byte*
JXTextSelection::GetStyledText0XAtomName()
{
	return kStyledText0XAtomName;
}
