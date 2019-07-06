/******************************************************************************
 GDBGetStackArguments.h

	Copyright (C) 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetStackArguments
#define _H_GDBGetStackArguments

#include "CMCommand.h"

class JTree;

class GDBGetStackArguments : public CMCommand
{
public:

	GDBGetStackArguments(JTree* tree);

	virtual	~GDBGetStackArguments();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	JTree*	itsTree;		// not owned

private:

	// not allowed

	GDBGetStackArguments(const GDBGetStackArguments& source);
	const GDBGetStackArguments& operator=(const GDBGetStackArguments& source);
};

#endif
