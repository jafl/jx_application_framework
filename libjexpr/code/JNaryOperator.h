/******************************************************************************
 JNaryOperator.h

	Interface for JNaryOperator class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JNaryOperator
#define _H_JNaryOperator

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
