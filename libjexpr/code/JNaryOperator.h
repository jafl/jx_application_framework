/******************************************************************************
 JNaryOperator.h

	Interface for JNaryOperator class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JNaryOperator
#define _H_JNaryOperator

#include <JNaryFunction.h>

class JNaryOperator : public JNaryFunction
{
public:

	JNaryOperator(const JFnNameIndex nameIndex, const JFunctionType type);
	JNaryOperator(const JNaryOperator& source);

	virtual ~JNaryOperator();

	virtual JBoolean	SameAs(const JFunction& theFunction) const;

	// provides safe downcasting

	virtual JNaryOperator*			CastToJNaryOperator();
	virtual const JNaryOperator*	CastToJNaryOperator() const;

private:

	// not allowed

	const JNaryOperator& operator=(const JNaryOperator& source);
};

#endif
