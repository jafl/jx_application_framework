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

	~JNamedConstant() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	void		Print(std::ostream& output) const override;
	JFunction*	Copy() const override;

	static bool	IsNamedConstant(const JString& name);

private:

	JNamedConstIndex	itsNameIndex;
};

#endif
