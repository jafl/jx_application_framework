/******************************************************************************
 JACETemplates.cpp

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
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

#include <ace/SOCK_Stream.h>
#include <ace/INET_Addr.h>
#define JTemplateType ACE_SOCK_STREAM
#include <JNetworkProtocolBase.tmpl>
#include <JMessageProtocol.tmpl>
#include <JAsynchDataReceiver.tmpl>
template class ACE_Svc_Handler<JTemplateType, ACE_SYNCH>;
#undef JTemplateType

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#define JTemplateType ACE_LSOCK_STREAM
#include <JNetworkProtocolBase.tmpl>
#include <JMessageProtocol.tmpl>
#include <JAsynchDataReceiver.tmpl>
template class ACE_Svc_Handler<JTemplateType, ACE_SYNCH>;
#undef JTemplateType

template class ACE_Task<ACE_NULL_SYNCH>;
template class ACE_Message_Queue<ACE_NULL_SYNCH>;
template class ACE_Module<ACE_NULL_SYNCH>;
template class ACE_Thru_Task<ACE_NULL_SYNCH>;

#define JTemplateType iovec
#include <JArray.tmpls>
#undef JTemplateType
