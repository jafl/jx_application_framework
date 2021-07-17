/******************************************************************************
 GDBGetThreads.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetThreads
#define _H_GDBGetThreads

#include "CMGetThreads.h"

class JTree;

class GDBGetThreads : public CMGetThreads
{
public:

	GDBGetThreads(JTree* tree, CMThreadsWidget* widget);

	virtual	~GDBGetThreads();

	static bool	ExtractThreadIndex(const JString& line, JIndex* threadIndex);
	static bool	ExtractLocation(const JString& line,
									JString* fileName, JIndex* lineIndex);

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;	// not owned

private:

	static JListT::CompareResult
		CompareThreadIndices(JString* const & l1, JString* const & l2);

	// not allowed

	GDBGetThreads(const GDBGetThreads& source);
	const GDBGetThreads& operator=(const GDBGetThreads& source);
};

#endif
