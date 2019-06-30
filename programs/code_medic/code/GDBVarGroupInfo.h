#ifndef _H_GDBVarGroupInfo
#define _H_GDBVarGroupInfo

/******************************************************************************
 GDBVarGroupInfo.h

	Copyright (C) 2010 by John Lindal.

 *****************************************************************************/

#include <JPtrArray-JString.h>

class CMVarNode;

struct GDBVarGroupInfo
{
	JString*				name;
	JPtrArray<CMVarNode>*	list;

	GDBVarGroupInfo(JString* n, JPtrArray<CMVarNode>* l)
		:
		name(n), list(l)
	{ };

	~GDBVarGroupInfo()
	{
		jdelete name;
		jdelete list;
	};

	const JString& GetName()
	{
		return (name == nullptr ? JString::empty : *name);
	};
};

#endif
