/******************************************************************************
 jExprUIUtil.h

	Interface for the jExprUIUtil module

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_jExprUIUtil
#define _H_jExprUIUtil

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
