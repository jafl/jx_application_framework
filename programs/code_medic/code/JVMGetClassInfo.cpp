/******************************************************************************
 JVMGetClassInfo.cpp

	BASE CLASS = CMCommand, virtual JBroadcaster

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "JVMGetClassInfo.h"
#include "JVMLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetClassInfo::JVMGetClassInfo
	(
	const JUInt64 id
	)
	:
	CMCommand("", true, false),
	itsID(id)
{
	CMCommand::Send();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetClassInfo::~JVMGetClassInfo()
{
}

/******************************************************************************
 Starting (virtual)

 *****************************************************************************/

void
JVMGetClassInfo::Starting()
{
	CMCommand::Starting();

	auto* link = dynamic_cast<JVMLink*>(CMGetLink());

	const JSize length  = link->GetObjectIDSize();
	auto* data = (unsigned char*) calloc(length, 1);
	assert( data != nullptr );

	JVMSocket::Pack(length, itsID, data);

	link->Send(this,
		JVMLink::kReferenceTypeCmdSet, JVMLink::kRTSignatureCmd, data, length);

	free(data);
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetClassInfo::HandleSuccess
	(
	const JString& origData
	)
{
	auto* link = dynamic_cast<JVMLink*>(CMGetLink());
	const JVMSocket::MessageReady* msg;
	if (!link->GetLatestMessageFromJVM(&msg))
		{
		return;
		}

	const unsigned char* data = msg->GetData();

	JSize count;
	const JString sig = JVMSocket::UnpackString(data, &count);

	link->AddClass(itsID, sig);
}
