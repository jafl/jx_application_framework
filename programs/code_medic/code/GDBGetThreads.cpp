/******************************************************************************
 GDBGetThreads.cpp

	BASE CLASS = CMGetThreads

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#include "GDBGetThreads.h"
#include "CMThreadsWidget.h"
#include "CMThreadNode.h"
#include <JTree.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <JListUtil.h>
#include <jStreamUtil.h>
#include <sstream>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GDBGetThreads::GDBGetThreads
	(
	JTree*				tree,
	CMThreadsWidget*	widget
	)
	:
	CMGetThreads(JString("set width 0\ninfo threads", kJFalse), widget),
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

static const JRegex prefixPattern      = "^([[:digit:]]+)[[:space:]]+";
static const JRegex threadIndexPattern = "^[0-9]{2}:";

void
GDBGetThreads::HandleSuccess
	(
	const JString& data
	)
{
	JPtrArray<JString> threadList(JPtrArrayT::kDeleteAll);
	threadList.SetCompareFunction(CompareThreadIndices);
	threadList.SetSortOrder(JListT::kSortAscending);

	JIndex currentThreadIndex = 0;

	const std::string s(data.GetRawBytes(), data.GetByteCount());
	std::istringstream input(s);
	JString line;
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
				JStringIterator iter1(&line);
				iter1.RemoveNext();
				line.TrimWhitespace();
				ExtractThreadIndex(line, &currentThreadIndex);
				}

			JStringIterator iter2(&line);
			if (iter2.Next(prefixPattern))
				{
				iter2.ReplaceLastMatch(iter2.GetLastMatch().GetSubstring(1) + ":  ");

				while (!threadIndexPattern.Match(line))
					{
					line.Prepend("0");
					}

				JString* s = jnew JString(line);
				assert( s != nullptr );
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
		const JString* line = threadList.GetElement(i);

		if (ExtractThreadIndex(*line, &threadIndex))
			{
			ExtractLocation(*line, &fileName, &lineIndex);

			CMThreadNode* node = jnew CMThreadNode(threadIndex, *line, fileName, lineIndex);
			assert( node != nullptr );

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
	const JStringMatch m = indexPattern.Match(line, kJFalse);
	if (!m.IsEmpty())
		{
		const JBoolean ok = m.GetString().ConvertToUInt(threadIndex);
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
	const JStringMatch m = locationPattern.Match(line, kJTrue);
	if (!m.IsEmpty())
		{
		*fileName = m.GetSubstring(1);

		const JBoolean ok = m.GetSubstring(2).ConvertToUInt(lineIndex);
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

JListT::CompareResult
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
		return JListT::kFirstLessSecond;
		}
	else
		{
		return JCompareIndices(i1, i2);
		}
}
