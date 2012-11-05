/******************************************************************************
 GDBGetThreads.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GDBGetThreads
#define _H_GDBGetThreads

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetThreads.h"

class JTree;

class GDBGetThreads : public CMGetThreads
{
public:

	GDBGetThreads(JTree* tree, CMThreadsWidget* widget);

	virtual	~GDBGetThreads();

	static JBoolean	ExtractThreadIndex(const JString& line, JIndex* threadIndex);
	static JBoolean	ExtractLocation(const JString& line,
									JString* fileName, JIndex* lineIndex);

protected:

	virtual void	HandleSuccess(const JString& data);

private:

	JTree*	itsTree;	// not owned

private:

	static JOrderedSetT::CompareResult
		CompareThreadIndices(JString* const & l1, JString* const & l2);

	// not allowed

	GDBGetThreads(const GDBGetThreads& source);
	const GDBGetThreads& operator=(const GDBGetThreads& source);
};

#endif
