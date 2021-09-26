/******************************************************************************
 JACETemplates.cpp

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/JMessageProtocol.h"
#include "jx-af/jcore/JAsynchDataReceiver.h"

// Protocol definitions

const JUtf8Byte* JMessageProtocolT::kUNIXSeparatorStr  = "\n";
const JSize JMessageProtocolT::kUNIXSeparatorByteCount = 1;

const JUtf8Byte* JMessageProtocolT::kMacintoshSeparatorStr  = "\r";
const JSize JMessageProtocolT::kMacintoshSeparatorByteCount = 1;

const JUtf8Byte* JMessageProtocolT::kDOSSeparatorStr  = "\r\n";
const JSize JMessageProtocolT::kDOSSeparatorByteCount = 2;

const JUtf8Byte* JMessageProtocolT::kStdDisconnectStr  = "\0";
const JSize JMessageProtocolT::kStdDisconnectByteCount = 1;

// JBroadcaster message types

const JUtf8Byte* JMessageProtocolT::kMessageReady = "MessageReady::JMessageProtocolT";
const JUtf8Byte* JMessageProtocolT::kReceivedDisconnect =
	"ReceivedDisconnect::JMessageProtocolT";

const JUtf8Byte* JAsynchDataReceiverT::kDataReady = "DataReady::JAsynchDataReceiverT";
