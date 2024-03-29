/******************************************************************************
 JNaryOperator.h

	Interface for JNaryOperator class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JNaryOperator
#define _H_JNaryOperator

#include "JNaryFunction.h"

class JNaryOperator : public JNaryFunction
{
public:

	JNaryOperator(const JUtf8Byte* name, JPtrArray<JFunction>* argList = nullptr);
	JNaryOperator(const JNaryOperator& source);

	~JNaryOperator() override;
};

#endif
