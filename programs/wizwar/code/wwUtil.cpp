/******************************************************************************
 wwUtil.cpp

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include "wwUtil.h"
#include <JString.h>
#include <jDirUtil.h>
#include <jAssert.h>

static const JCharacter* kWWAppSignature  = "jx_wizwar";
static const JCharacter* kWWDirectoryRoot = "~/.jx_wizwar/";

/******************************************************************************
 WWGetAppSignature

 ******************************************************************************/

const JCharacter*
WWGetAppSignature()
{
	return kWWAppSignature;
}

/******************************************************************************
 WWGetDataFileName

	Returns kJTrue if the user's home directory exists.  Does *not* check
	if *fullName exists.

 ******************************************************************************/

JBoolean
WWGetDataFileName
	(
	const JCharacter*	fileName,
	JString*			fullName
	)
{
	const JString relName = JCombinePathAndName(kWWDirectoryRoot, fileName);
	return JExpandHomeDirShortcut(relName, fullName);
}

/******************************************************************************
 WWSetProtocol

 ******************************************************************************/

void
WWSetProtocol
	(
	JMessageProtocol<ACE_SOCK_STREAM>* socket
	)
{
	socket->SetProtocol("\x01", 1, "\x00", 1);
}
