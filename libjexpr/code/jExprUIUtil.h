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

JBoolean
JApplyFunction
	(
	const JUtf8Byte*		fnName,
	const JVariableList*	varList,
	const JFunction&		origF,
	JFontManager*			fontMgr,

	JFunction**				newF,
	JFunction**				newArg,
	JUserInputFunction**	newUIF
	);

#endif
