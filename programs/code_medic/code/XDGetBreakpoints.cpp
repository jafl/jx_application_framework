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
#include <JRegex.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

XDGetBreakpoints::XDGetBreakpoints()
	:
	CMGetBreakpoints("breakpoint_list")
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
	XDLink* link = dynamic_cast<XDLink*>(CMGetLink());
	xmlNode* root;
	if (link == nullptr || !link->GetParsedData(&root))
		{
		return;
		}

	(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(kJFalse);

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
			JBoolean ok = idStr.ConvertToUInt(&bpIndex);
			assert( ok );

			if (fileName.BeginsWith("file://"))
				{
				fileName.RemoveSubstring(1, 7);
				}

			JIndex lineNumber;
			ok = lineStr.ConvertToUInt(&lineNumber);
			assert( ok );

			const JBoolean enabled = JI2B(state == "enabled");

			CMBreakpoint* bp =
				jnew CMBreakpoint(bpIndex, fileName, lineNumber, "", "",
								 enabled, CMBreakpoint::kKeepBreakpoint, "", 0);
			assert( bp != nullptr );
			bpList.InsertSorted(bp);

			if (true)	// no way to know if it is temporary -- may be deleted or other status change
				{
				(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(kJTrue);
				}
			}

		node = node->next;
		}

	(CMGetLink()->GetBreakpointManager())->UpdateBreakpoints(bpList, otherList);
}
