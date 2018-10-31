/******************************************************************************
 GXTextSelection.cpp

	BASE CLASS = public JXTextSelection

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GXTextSelection.h>

#include <jAssert.h>

static const JUtf8Byte* kGloveTextDataXAtomName = "GLOVE_TEXT";

/******************************************************************************
 Constructor

 *****************************************************************************/

GXTextSelection::GXTextSelection
	(
	JXDisplay* 			display, 
	const std::string&	text
	)
	:
	JXTextSelection(display, JString(text.c_str(), kJFalse), nullptr)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GXTextSelection::~GXTextSelection()
{
}

/******************************************************************************
 SetGloveData (public)

 ******************************************************************************/

void
GXTextSelection::SetGloveData
	(
	const std::string& text
	)
{
	itsGloveData = text;
}

/******************************************************************************


 ******************************************************************************/

void
GXTextSelection::AddTypes
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

JBoolean
GXTextSelection::ConvertData
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
		return kJTrue;
		}
	return JXTextSelection::ConvertData(requestType, returnType, data, dataLength, bitsPerBlock);
}
