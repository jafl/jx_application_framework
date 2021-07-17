/******************************************************************************
 CBEmulator.cpp

	Copyright © 2010 by John Lindal.

 ******************************************************************************/

#include "CBEmulator.h"
#include "CBVIKeyHandler.h"
#include "CBTextEditor.h"
#include <jAssert.h>

/******************************************************************************
 CBInstallEmulator

 ******************************************************************************/

bool
CBInstallEmulator
	(
	const CBEmulator	type,
	CBTextEditor*		editor,
	JTEKeyHandler**		handler
	)
{
	*handler = nullptr;

	if (type == kCBVIEmulator)
		{
		*handler = jnew CBVIKeyHandler(editor);
		assert( *handler != nullptr );
		}

	editor->SetKeyHandler(*handler);
	return *handler != nullptr;
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
