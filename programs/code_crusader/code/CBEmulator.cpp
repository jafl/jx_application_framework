/******************************************************************************
 CBEmulator.cpp

	Copyright (C) 2010 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBEmulator.h"
#include "CBVIKeyHandler.h"
#include "CBTextEditor.h"
#include <jAssert.h>

/******************************************************************************
 CBInstallEmulator

 ******************************************************************************/

JBoolean
CBInstallEmulator
	(
	const CBEmulator	type,
	CBTextEditor*		editor,
	JTEKeyHandler**		handler
	)
{
	*handler = NULL;

	if (type == kCBVIEmulator)
		{
		*handler = jnew CBVIKeyHandler(editor);
		assert( *handler != NULL );
		}

	editor->SetKeyHandler(*handler);
	return JI2B(*handler != NULL);
}

/******************************************************************************
 Stream operators

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&	input,
	CBEmulator&	type
	)
{
	long temp;
	input >> temp;
	type = (CBEmulator) temp;
	assert( kCBFirstEmulator <= type && type <= kCBLastEmulator );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&			output,
	const CBEmulator	type
	)
{
	output << (long) type;
	return output;
}
