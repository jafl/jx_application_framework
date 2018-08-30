/******************************************************************************
 JBinaryOperator.h

	Interface for JBinaryOperator class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JBinaryOperator
#define _H_JBinaryOperator

#include <JBinaryFunction.h>

class JBinaryOperator : public JBinaryFunction
{
public:

	JBinaryOperator(const JUtf8Byte* name, JFunction* arg1, JFunction* arg2);
	JBinaryOperator(const JBinaryOperator& source);

	virtual ~JBinaryOperator();

	virtual void	Print(std::ostream& output) const;

protected:

	void	PrintArg(std::ostream& output, const JFunction* arg) const;

private:

	// not allowed

	const JBinaryOperator& operator=(const JBinaryOperator& source);
};

#endif
