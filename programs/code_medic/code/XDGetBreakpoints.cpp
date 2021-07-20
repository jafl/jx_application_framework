/******************************************************************************
 XDGetBreakpoints.cpp

	This is the only way to get all the relevant information about each
	breakpoint.  gdb does not print enough information when "break" is
	used.

	BASE CLASS = CMGetBreakpoints

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#include "XDGetBreakpoints.h"
#include "CMBreakpointManager.h"
#include "XDLink.h"
#include "cmGlobals.h"
#include <JStringIterator.h>
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetBreakpoints::XDGetBreakpoints()
	:
	CMGetBreakpoints(JString("breakpoint_list", JString::kNoCopy))
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

XDGetBreakpoints::~XDGetBreakpoints()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

void
XDGetBreakpoints::HandleSuccess
	(
	const JString& data
	)
{
	auto* link = dynamic_cast<XDLink*>(CMGetLink());
	xmlNode* root;
	if (link == nullptr || !link->GetParsedData(&root))
		{
		return;
		}

	(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(false);

	JPtrArray<CMBreakpoint> bpList(JPtrArrayT::kForgetAll);	// ownership taken by CMBreakpointManager
	bpList.SetCompareFunction(CMBreakpointManager::CompareBreakpointLocations);
	bpList.SetSortOrder(JListT::kSortAscending);

	JPtrArray<CMBreakpoint> otherList(JPtrArrayT::kForgetAll);	// ownership taken by CMBreakpointManager

	xmlNode* node = root->children;
	JString type, idStr, fileName, lineStr, state;
	while (node != nullptr)
		{
		type = JGetXMLNodeAttr(node, "type");
		if (type == "line")
			{
			idStr    = JGetXMLNodeAttr(node, "id");
			fileName = JGetXMLNodeAttr(node, "filename");
			lineStr  = JGetXMLNodeAttr(node, "lineno");
			state    = JGetXMLNodeAttr(node, "state");

			JIndex bpIndex;
			bool ok = idStr.ConvertToUInt(&bpIndex);
			assert( ok );

			if (fileName.BeginsWith("file://"))
				{
				JStringIterator iter(&fileName);
				iter.RemoveNext(7);
				}

			JIndex lineNumber;
			ok = lineStr.ConvertToUInt(&lineNumber);
			assert( ok );

			const bool enabled = state == "enabled";

			auto* bp =
				jnew CMBreakpoint(bpIndex, fileName, lineNumber, JString::empty, JString::empty,
								 enabled, CMBreakpoint::kKeepBreakpoint, JString::empty, 0);
			assert( bp != nullptr );
			bpList.InsertSorted(bp);

			if (true)	// no way to know if it is temporary -- may be deleted or other status change
				{
				(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(true);
				}
			}

		node = node->next;
		}

	(CMGetLink()->GetBreakpointManager())->UpdateBreakpoints(bpList, otherList);
}
