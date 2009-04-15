/******************************************************************************
 jExprUIUtil.h

	Interface for the jExprUIUtil module

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jExprUIUtil
#define _H_jExprUIUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

class JFunction;
class JVariableList;
class JUserInputFunction;
class JFontManager;
class JColormap;

JBoolean
JApplyFunction
	(
	const JCharacter*		fnName,
	const JVariableList*	varList,
	const JFunction&		origF,
	const JFontManager*		fontMgr,
	JColormap*				colormap,

	JFunction**				newF,
	JFunction**				newArg,
	JUserInputFunction**	newUIF
	);

#endif
