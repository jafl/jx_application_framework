/******************************************************************************
 GDBArray2DCommand.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBArray2DCommand
#define _H_GDBArray2DCommand

#include "CMArray2DCommand.h"

class GDBArray2DCommand : public CMArray2DCommand
{
public:

	GDBArray2DCommand(CMArray2DDir* dir,
					 JXStringTable* table, JStringTableData* data);

	virtual	~GDBArray2DCommand();

	virtual void	PrepareToSend(const UpdateType type, const JIndex index,
								  const JInteger arrayIndex) override;

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	GDBArray2DCommand(const GDBArray2DCommand& source);
	const GDBArray2DCommand& operator=(const GDBArray2DCommand& source);
};

#endif
