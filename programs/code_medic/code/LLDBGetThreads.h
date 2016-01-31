/******************************************************************************
 LLDBGetThreads.h

	Copyright © 2016 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_LLDBGetThreads
#define _H_LLDBGetThreads

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMGetThreads.h"

class JTree;

class LLDBGetThreads : public CMGetThreads
{
public:

	LLDBGetThreads(JTree* tree, CMThreadsWidget* widget);

	virtual	~LLDBGetThreads();

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

	LLDBGetThreads(const LLDBGetThreads& source);
	const LLDBGetThreads& operator=(const LLDBGetThreads& source);
};

#endif
