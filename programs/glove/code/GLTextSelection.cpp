/******************************************************************************
 GLTextSelection.cpp

	BASE CLASS = public JXTextSelection

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include "GLTextSelection.h"
#include <jAssert.h>

static const JUtf8Byte* kGloveTextDataXAtomName = "GLOVE_TEXT";

/******************************************************************************
 Constructor

 *****************************************************************************/

GLTextSelection::GLTextSelection
	(
	JXDisplay* 			display, 
	const std::string&	text
	)
	:
	JXTextSelection(display, JString(text.c_str(), text.length(), JString::kNoCopy), nullptr)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLTextSelection::~GLTextSelection()
{
}

/******************************************************************************
 SetGloveData (public)

 ******************************************************************************/

void
GLTextSelection::SetGloveData
	(
	const std::string& text
	)
{
	itsGloveData = text;
}

/******************************************************************************


 ******************************************************************************/

void
GLTextSelection::AddTypes
	(
	const Atom selectionName
	)
{
	itsGloveTextXAtom = AddType(kGloveTextDataXAtomName);
	JXTextSelection::AddTypes(selectionName);
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

bool
GLTextSelection::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{

	if (requestType == itsGloveTextXAtom)
		{
		*bitsPerBlock = 8;
		*data = jnew unsigned char[ itsGloveData.GetByteCount()];
		assert( *data != nullptr );
		memcpy(*data, itsGloveData.GetRawBytes(), itsGloveData.GetByteCount());
		*dataLength = itsGloveData.GetByteCount();
		*returnType = itsGloveTextXAtom;
		return true;
		}
	return JXTextSelection::ConvertData(requestType, returnType, data, dataLength, bitsPerBlock);
}
