#ifndef _H_GDBVarGroupInfo
#define _H_GDBVarGroupInfo

/******************************************************************************
 GDBVarGroupInfo.h

	Copyright © 2010 by John Lindal.  All rights reserved.

 *****************************************************************************/

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
		delete name;
		delete list;
	};

	const JCharacter* GetName()
	{
		return (name == NULL ? "" : name->GetCString());
	};
};

#endif
