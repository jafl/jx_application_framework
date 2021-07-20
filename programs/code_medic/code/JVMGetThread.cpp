/******************************************************************************
 JVMGetThread.cpp

	BASE CLASS = CMGetThread

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetThread.h"
#include "CMThreadsWidget.h"
#include "JVMLink.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetThread::JVMGetThread
	(
	CMThreadsWidget* widget
	)
	:
	CMGetThread(JString("NOP", JString::kNoCopy), widget)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetThread::~JVMGetThread()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThread::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<JVMLink*>(CMGetLink());
	GetWidget()->FinishedLoading(link->GetCurrentThreadID());
}
