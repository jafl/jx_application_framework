/******************************************************************************
 JVMGetThreads.cpp

	BASE CLASS = CMGetThreads

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#include "JVMGetThreads.h"
#include "CMThreadsWidget.h"
#include "JVMThreadNode.h"
#include "JVMLink.h"
#include "cmGlobals.h"
#include <JTree.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JVMGetThreads::JVMGetThreads
	(
	JTree*				tree,
	CMThreadsWidget*	widget
	)
	:
	CMGetThreads(JString("NOP", JString::kNoCopy), widget),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JVMGetThreads::~JVMGetThreads()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

void
JVMGetThreads::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<JVMLink*>(CMGetLink());
	CopyTree(link->GetThreadRoot(), itsTree->GetRoot());

	GetWidget()->FinishedLoading(link->GetCurrentThreadID());
}

/******************************************************************************
 CopyTree (private)

 ******************************************************************************/

void
JVMGetThreads::CopyTree
	(
	JVMThreadNode*	src,
	JTreeNode*		dest
	)
{
	const JSize count = src->GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		auto* child1 = dynamic_cast<JVMThreadNode*>(src->GetChild(i));

		auto* child2 = jnew JVMThreadNode(*child1);
		assert( child2 != nullptr );

		if (dest->IsRoot())
			{
			dest->Append(child2);
			}
		else
			{
			dynamic_cast<CMThreadNode*>(dest)->AppendThread(child2);
			}

		CopyTree(child1, child2);
		}
}
