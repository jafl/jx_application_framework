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
#undef JTemplateType

#define JTemplateName ACE_Svc_Handler
#define JTemplateType ACE_SOCK_STREAM, ACE_SYNCH
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#define JTemplateType ACE_LSOCK_STREAM
#include <JNetworkProtocolBase.tmpl>
#include <JMessageProtocol.tmpl>
#include <JAsynchDataReceiver.tmpl>
#undef JTemplateType

#define JTemplateName ACE_Svc_Handler
#define JTemplateType ACE_LSOCK_STREAM, ACE_SYNCH
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Task
#define JTemplateType ACE_NULL_SYNCH
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Message_Queue
#define JTemplateType ACE_NULL_SYNCH
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Module
#define JTemplateType ACE_NULL_SYNCH
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Thru_Task
#define JTemplateType ACE_NULL_SYNCH
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Node
#define JTemplateType int, ACE_Unbounded_Set_Default_Comparator<int>
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Unbounded_Set
#define JTemplateType int
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Unbounded_Set_Iterator
#define JTemplateType int
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Unbounded_Set_Ex
#define JTemplateType int, ACE_Unbounded_Set_Default_Comparator<int>
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Unbounded_Set_Ex_Iterator
#define JTemplateType int, ACE_Unbounded_Set_Default_Comparator<int>
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateName ACE_Unbounded_Set_Ex_Const_Iterator
#define JTemplateType int, ACE_Unbounded_Set_Default_Comparator<int>
#include <instantiate_template.h>
#undef JTemplateName
#undef JTemplateType

#define JTemplateType iovec
#include <JArray.tmpls>
#undef JTemplateType
