/******************************************************************************
 JVMArray2DCommand.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_JVMArray2DCommand
#define _H_JVMArray2DCommand

#include "CMArray2DCommand.h"

class JVMArray2DCommand : public CMArray2DCommand
{
public:

	JVMArray2DCommand(CMArray2DDir* dir,
					 JXStringTable* table, JStringTableData* data);

	virtual	~JVMArray2DCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	JVMArray2DCommand(const JVMArray2DCommand& source);
	const JVMArray2DCommand& operator=(const JVMArray2DCommand& source);
};

#endif
