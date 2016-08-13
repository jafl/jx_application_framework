/******************************************************************************
 JACETemplates.cpp

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JMessageProtocol.h>
#include <JAsynchDataReceiver.h>

// Protocol definitions

const JCharacter* JMessageProtocolT::kUNIXSeparatorStr = "\n";
const JSize JMessageProtocolT::kUNIXSeparatorLength    = 1;

const JCharacter* JMessageProtocolT::kMacintoshSeparatorStr = "\r";
const JSize JMessageProtocolT::kMacintoshSeparatorLength    = 1;

const JCharacter* JMessageProtocolT::kDOSSeparatorStr = "\r\n";
const JSize JMessageProtocolT::kDOSSeparatorLength    = 2;

const JCharacter* JMessageProtocolT::kStdDisconnectStr = "\0";
const JSize JMessageProtocolT::kStdDisconnectLength    = 1;

// JBroadcaster message types

const JCharacter* JMessageProtocolT::kMessageReady = "MessageReady::JMessageProtocolT";
const JCharacter* JMessageProtocolT::kReceivedDisconnect =
	"ReceivedDisconnect::JMessageProtocolT";

const JCharacter* JAsynchDataReceiverT::kDataReady = "DataReady::JAsynchDataReceiverT";
