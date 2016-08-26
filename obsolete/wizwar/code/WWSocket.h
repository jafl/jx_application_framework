/******************************************************************************
 WWSocket.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_WWSocket
#define _H_WWSocket

#include <JMessageProtocol.h>
#include <j_prep_ace.h>
#include <ace/SOCK_Stream.h>

typedef JMessageProtocol<ACE_SOCK_STREAM>	WWSocket;

#endif
