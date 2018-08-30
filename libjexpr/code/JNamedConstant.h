/******************************************************************************
 JNamedConstant.h

	Interface for JNamedConstant class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JNamedConstant
#define _H_JNamedConstant

#include "JFunction.h"
#include "jFunctionUtil.h"

class JNamedConstant : public JFunction
{
public:

	JNamedConstant(const JNamedConstIndex nameIndex);
	JNamedConstant(const JNamedConstant& source);

	virtual ~JNamedConstant();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual void		Print(std::ostream& output) const override;
	virtual JFunction*	Copy() const override;

private:

	JNamedConstIndex	itsNameIndex;

private:

	// not allowed

	const JNamedConstant& operator=(const JNamedConstant& source);
};

#endif
