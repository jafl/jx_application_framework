/******************************************************************************
 XDArray2DCommand.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_XDArray2DCommand
#define _H_XDArray2DCommand

#include "CMArray2DCommand.h"

class XDArray2DCommand : public CMArray2DCommand
{
public:

	XDArray2DCommand(CMArray2DDir* dir,
					 JXStringTable* table, JStringTableData* data);

	virtual	~XDArray2DCommand();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	// not allowed

	XDArray2DCommand(const XDArray2DCommand& source);
	const XDArray2DCommand& operator=(const XDArray2DCommand& source);
};

#endif
