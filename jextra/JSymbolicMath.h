/***************************************************************************************
 JSymbolicMath.h

	Interface for JSymbolicMath class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ***************************************************************************************/

#ifndef _H_JSymbolicMath
#define _H_JSymbolicMath

#include <JPtrArray.h>

class JString;
class JFunction;
class JVariableList;

class JSymbolicMath
{
public:

	JSymbolicMath();

	virtual ~JSymbolicMath();

	virtual JString		Print(const JFunction& f) const = 0;
	virtual void		SendSysCmd(const JCharacter* cmd) const = 0;

	virtual JBoolean	Simplify(const JCharacter* function, JString* result) const = 0;
	virtual JBoolean	Simplify(const JFunction* f, const JVariableList* varList,
								 JFunction** result) const = 0;

	virtual JBoolean	Evaluate(const JCharacter* function, const JCharacter* values,
								 JString* result) const = 0;
	virtual JBoolean	Evaluate(const JFunction* f, const JCharacter* values,
								 const JVariableList* varList, JFunction** result) const = 0;

	virtual JBoolean	SolveEquations(const JPtrArray<JString>& lhs,
									   const JPtrArray<JString>& rhs,
									   const JPtrArray<JString>& variables,
									   JString* result) const = 0;
	virtual JBoolean	SolveEquations(const JPtrArray<JFunction>& lhs,
									   const JPtrArray<JFunction>& rhs,
									   const JPtrArray<JString>& variables,
									   JString* result) const = 0;

private:

	// not allowed

	JSymbolicMath(const JSymbolicMath& source);
	JSymbolicMath& operator=(const JSymbolicMath& source);
};

#endif
