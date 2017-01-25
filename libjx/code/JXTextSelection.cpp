/******************************************************************************
 JXTextSelection.cpp

	text/plain, text/x-jxstyled0 for all, and STRING, TEXT for all but DND.
	DELETE is supported if a JTextEditor is provided.

	BASE CLASS = JXSelectionData

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXTextSelection.h>
#include <JXDisplay.h>
#include <JXColormap.h>
#include <sstream>
#include <jAssert.h>

static const JCharacter* kStyledText0XAtomName = "text/x-jxstyled0";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXTextSelection::JXTextSelection
	(
	JXDisplay*				display,
	const JCharacter*		text,
	const JRunArray<JFont>*	style
	)
	:
	JXSelectionData(display)
{
	JXTextSelectionX();
	SetData(text, display->GetColormap(), style);
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
	SetData(text, display->GetColormap(), style);
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
	const JCharacter*	id
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
	itsText     = NULL;
	itsStyle    = NULL;
	itsColormap = NULL;
	itsTE       = NULL;

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
		AddType(XA_STRING);
		AddType(selMgr->GetTextXAtom());
		}
}

/******************************************************************************
 SetData

	Makes a copy of text and style.

	colormap and style can be NULL.
	If style != NULL, colormap must not be NULL.

 ******************************************************************************/

void
JXTextSelection::SetData
	(
	const JCharacter*		text,
	const JXColormap*		colormap,
	const JRunArray<JFont>*	style
	)
{
	assert( style == NULL || colormap != NULL );

	if (itsText != NULL)
		{
		*itsText = text;
		}
	else
		{
		itsText = jnew JString(text);
		assert( itsText != NULL );
		}

	SetColormap(colormap);

	if (style != NULL && itsStyle != NULL)
		{
		*itsStyle = *style;
		}
	else if (style != NULL && itsStyle == NULL)
		{
		itsStyle = jnew JRunArray<JFont>(*style);
		assert( itsStyle != NULL );
		}
	else
		{
		jdelete itsStyle;
		itsStyle = NULL;
		}

	SetTextEditor(NULL, JIndexRange());
}

/******************************************************************************
 SetData

	Takes ownership of text and style.

	colormap and style can be NULL.
	If style != NULL, colormap must not be NULL.

 ******************************************************************************/

void
JXTextSelection::SetData
	(
	JString*			text,
	const JXColormap*	colormap,
	JRunArray<JFont>*	style
	)
{
	assert( style == NULL || colormap != NULL );

	jdelete itsText;
	itsText = text;

	SetColormap(colormap);

	jdelete itsStyle;
	itsStyle = style;

	SetTextEditor(NULL, JIndexRange());
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
	if (itsText == NULL)
		{
		itsText = jnew JString;
		assert( itsText != NULL );
		}
	else
		{
		itsText->Clear();
		}

	SetColormap(NULL);

	jdelete itsStyle;
	itsStyle = NULL;

	SetTextEditor(NULL, JIndexRange());

	const JSize count = list.GetElementCount();
	JSize selectCount = 0;
	for (JIndex i=1; i<=count; i++)
		{
		if (selectCount > 0)
			{
			itsText->AppendCharacter('\n');
			}
		*itsText += *(list.GetElement(i));
		selectCount++;
		}

	if (selectCount > 1)
		{
		itsText->AppendCharacter('\n');
		}
}

/******************************************************************************
 SetColormap (private)

 ******************************************************************************/

void
JXTextSelection::SetColormap
	(
	const JXColormap* colormap
	)
{
	if (itsColormap != NULL)
		{
		StopListening(itsColormap);
		}
	itsColormap = colormap;
	if (itsColormap != NULL)
		{
		ClearWhenGoingAway(itsColormap, &itsColormap);
		}
}

/******************************************************************************
 SetTextEditor

	Call this to support DELETE.

 ******************************************************************************/

void
JXTextSelection::SetTextEditor
	(
	JTextEditor*		te,
	const JIndexRange&	selection
	)
{
	if (itsTE != NULL)
		{
		StopListening(itsTE);
		}
	itsTE = te;
	if (itsTE != NULL)
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
	const Atom dndName = GetDNDSelectionName();
	*bitsPerBlock = 8;

	JXSelectionManager* selMgr = GetSelectionManager();
	const Atom mimeText        = selMgr->GetMimePlainTextXAtom();

	JIndexRange selection;

	if ((requestType == XA_STRING ||
		 requestType == mimeText ||
		 requestType == selMgr->GetTextXAtom()) &&
		itsText != NULL)
		{
		*returnType = (requestType == mimeText) ? mimeText : XA_STRING;
		*dataLength = itsText->GetLength();
		*data       = jnew unsigned char[ *dataLength ];
		if (*data != NULL)
			{
			memcpy(*data, itsText->GetCString(), *dataLength);
			return kJTrue;
			}
		}

	else if (requestType == itsStyledText0XAtom &&
			 itsText != NULL && itsColormap != NULL && itsStyle != NULL)
		{
		const JFileVersion vers = 1;
		std::ostringstream dataStream;
		JTextEditor::WritePrivateFormat(dataStream,
										itsColormap, vers, *itsText, *itsStyle,
										1, itsText->GetLength());

		const std::string s = dataStream.str();
		*returnType         = itsStyledText0XAtom;
		*dataLength         = s.length();
		*data               = jnew unsigned char[ *dataLength ];
		if (*data != NULL)
			{
			memcpy(*data, s.data(), *dataLength);
			return kJTrue;
			}
		}

	else if (requestType == selMgr->GetDeleteSelectionXAtom() &&
			 itsTE != NULL && IsCurrent() &&
			 itsTE->GetSelection(&selection) && selection == itsSelection)
		{
		itsTE->DeleteSelection();

		*data       = jnew unsigned char[1];
		*dataLength = 0;
		*returnType = selMgr->GetNULLXAtom();
		return kJTrue;
		}

	*data       = NULL;
	*dataLength = 0;
	*returnType = None;
	return kJFalse;
}

/******************************************************************************
 GetStyledText0XAtomName (static)

 ******************************************************************************/

const JCharacter*
JXTextSelection::GetStyledText0XAtomName()
{
	return kStyledText0XAtomName;
}
