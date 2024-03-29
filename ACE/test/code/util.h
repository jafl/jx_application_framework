/******************************************************************************
 util.h

	Code used by both client and server.

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_util
#define _H_util

#include <jTypes.h>

class ACE_SOCK_Stream;

const JSize kMaxWaitSeconds = 5;

bool	TalkToClient(ACE_SOCK_Stream& socket);
void		TalkToServer(ACE_SOCK_Stream& socket);
bool	ReceiveMessage(ACE_SOCK_Stream& socket);

#endif
