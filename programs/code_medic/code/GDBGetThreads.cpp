/******************************************************************************
 GDBGetThreads.cpp

	BASE CLASS = CMGetThreads

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GDBGetThreads.h"
#include "CMThreadsWidget.h"
#include "CMThreadNode.h"
#include <JTree.h>
#include <JRegex.h>
#include <JOrderedSetUtil.h>
#include <jStreamUtil.h>
#include <sstream>
#include <jAssert.h>

const JSize kThreadIndexWidth = 2;	// width of thread index in characters

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetThreads::GDBGetThreads
	(
	JTree*				tree,
	CMThreadsWidget*	widget
	)
	:
	CMGetThreads("set width 0\ninfo threads", widget),
	itsTree(tree)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GDBGetThreads::~GDBGetThreads()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 ******************************************************************************/

static const JRegex prefixPattern = "^([[:digit:]]+)[[:space:]]+";

void
GDBGetThreads::HandleSuccess
	(
	const JString& data
	)
{
	JPtrArray<JString> threadList(JPtrArrayT::kDeleteAll);
	threadList.SetCompareFunction(CompareThreadIndices);
	threadList.SetSortOrder(JOrderedSetT::kSortAscending);

	JIndex currentThreadIndex = 0;

	const std::string s(data.GetCString(), data.GetLength());
	std::istringstream input(s);
	JString line;
	JArray<JIndexRange> matchList;
	while (1)
		{
		line = JReadLine(input);
		if (input.eof() || input.fail())
			{
			break;
			}

		line.TrimWhitespace();
		if (!line.IsEmpty())
			{
			if (line.GetFirstCharacter() == '*')
				{
				line.RemoveSubstring(1,1);
				line.TrimWhitespace();
				ExtractThreadIndex(line, &currentThreadIndex);
				}

			if (prefixPattern.Match(line, &matchList))
				{
				JIndexRange r;
				line.ReplaceSubstring(matchList.GetElement(1),
					line.GetSubstring(matchList.GetElement(2)) + ":  ", &r);

				if (line.GetLength() >= kThreadIndexWidth)
					{
					while (!isdigit(line.GetCharacter(kThreadIndexWidth)))
						{
						line.PrependCharacter('0');
						}
					}

				JString* s = new JString(line);
				assert( s != NULL );
				threadList.InsertSorted(s);
				}
			}
		}

	JTreeNode* root   = itsTree->GetRoot();
	const JSize count = threadList.GetElementCount();
	JIndex threadIndex, lineIndex;
	JString fileName;
	for (JIndex i=1; i<=count; i++)
		{
		const JString* line = threadList.NthElement(i);

		if (ExtractThreadIndex(*line, &threadIndex))
			{
			ExtractLocation(*line, &fileName, &lineIndex);

			CMThreadNode* node = new CMThreadNode(threadIndex, *line, fileName, lineIndex);
			assert( node != NULL );

			root->Append(node);
			}
		}

	GetWidget()->FinishedLoading(currentThreadIndex);
}

/******************************************************************************
 ExtractThreadIndex (static private)

 ******************************************************************************/

static const JRegex indexPattern = "^[[:digit:]]+";

JBoolean
GDBGetThreads::ExtractThreadIndex
	(
	const JString&	line,
	JIndex*			threadIndex
	)
{
	JIndexRange r;
	if (indexPattern.Match(line, &r))
		{
		const JString lineStr = line.GetSubstring(r);
		const JBoolean ok     = lineStr.ConvertToUInt(threadIndex);
		assert( ok );

		return kJTrue;
		}
	else
		{
		*threadIndex = 0;
		return kJFalse;
		}
}

/******************************************************************************
 ExtractLocation (static private)

 ******************************************************************************/

static const JRegex locationPattern = "[ \t]+at[ \t]+(.+):([[:digit:]]+)$";

JBoolean
GDBGetThreads::ExtractLocation
	(
	const JString&	line,
	JString*		fileName,
	JIndex*			lineIndex
	)
{
	JArray<JIndexRange> matchList;
	if (locationPattern.Match(line, &matchList))
		{
		*fileName = line.GetSubstring(matchList.GetElement(2));

		const JString lineStr = line.GetSubstring(matchList.GetElement(3));
		const JBoolean ok     = lineStr.ConvertToUInt(lineIndex);
		assert( ok );

		return kJTrue;
		}
	else
		{
		fileName->Clear();
		*lineIndex = 0;
		return kJFalse;
		}
}

/******************************************************************************
 CompareThreadIndices (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
GDBGetThreads::CompareThreadIndices
	(
	JString* const & l1,
	JString* const & l2
	)
{
	JIndex i1, i2;
	if (!ExtractThreadIndex(*l1, &i1) ||
		!ExtractThreadIndex(*l2, &i1))
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JCompareIndices(i1, i2);
		}
}
