/******************************************************************************
 wwMessages.h

	Message types sent between client and server.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_wwMessages
#define _H_wwMessages

#include <jTypes.h>

const JFileVersion kWWCurrentProtocolVersion = 0;

enum WWMessageType
{
	// initialization

	kWWClientIdentity        = 0,	// from client -- constant in all versions
	kWWConnectionEstablished = 1,	// from server -- constant in all versions

	// ping

	kWWClientAlive,					// from client

	// state changes

	kWWPlayerJoined,				// from server
	kWWPlayerLeft,

	// player chat

	kWWChatMessage					// from either
};

std::istream&	operator>>(std::istream& input, WWMessageType& msg);
std::ostream&	operator<<(std::ostream& output, const WWMessageType msg);

enum WWConnectionErrorType
{
	kWWWrongProtocolVersion = 0,	// constant in all versions
	kWWBadClient,
	kWWPlayerNameUsed,
	kWWGameFull
};

std::istream&	operator>>(std::istream& input, WWConnectionErrorType& err);
std::ostream&	operator<<(std::ostream& output, const WWConnectionErrorType err);

#endif
