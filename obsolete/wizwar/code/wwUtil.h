/******************************************************************************
 wwUtil.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_wwUtil
#define _H_wwUtil

#include <JMessageProtocol.h>
#include <j_prep_ace.h>
#include <ace/SOCK_Stream.h>

class JString;

const JCharacter*	WWGetAppSignature();
JBoolean			WWGetDataFileName(const JCharacter* fileName, JString* fullName);

void	WWSetProtocol(JMessageProtocol<ACE_SOCK_STREAM>* socket);

#endif
