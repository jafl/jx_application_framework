/******************************************************************************
 JPtrArrayT.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPtrArrayT
#define _H_JPtrArrayT

struct JPtrArrayT
{
	enum CleanUpAction
	{
		kForgetAll,
		kDeleteAll,
		kDeleteAllAsArrays
	};

	enum SetElementAction
	{
		kForget,
		kDelete,
		kDeleteAsArray
	};
};

#endif
