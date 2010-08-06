/******************************************************************************
 GXTextSelection.cpp

	BASE CLASS = public JXTextSelection

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <glStdInc.h>
#include <GXTextSelection.h>

#include <jAssert.h>

static const JCharacter* kGloveTextDataXAtomName = "GLOVE_TEXT";

/******************************************************************************
 Constructor

 *****************************************************************************/

GXTextSelection::GXTextSelection
	(
	JXDisplay* 			display, 
	const std::string&	text
	)
	:
	JXTextSelection(display, text.c_str(), NULL)
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
		*data = new unsigned char[ itsGloveData.GetLength()];
		assert( *data != NULL );
		memcpy(*data, itsGloveData, itsGloveData.GetLength());
		*dataLength = itsGloveData.GetLength();
		*returnType = itsGloveTextXAtom;
		return kJTrue;
		}
	return JXTextSelection::ConvertData(requestType, returnType, data, dataLength, bitsPerBlock);
}
