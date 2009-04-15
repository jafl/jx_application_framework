/***************************************************************************************
 JMathematicaSM.h

	Interface for JMathematicaSM class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ***************************************************************************************/

#ifndef _H_JMathematicaSM
#define _H_JMathematicaSM

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JSymbolicMath.h>

#if __MWERKS__
	#define MACINTOSH_MATHLINK 1
#elif defined _MSC_VER
	#define WINDOWS_MATHLINK 1
#elif defined _J_UNIX
	#define UNIX_MATHLINK 1
#else
	figure out what to do!
#endif
#include <mathlink.h>

class JFunction;
class JProgressDisplay;

class JMathematicaSM : public JSymbolicMath
{
public:

	static JBoolean	Create(JMathematicaSM** obj);

	virtual ~JMathematicaSM();

	virtual JString		Print(const JFunction& f) const;
	virtual void		SendSysCmd(const JCharacter* cmd) const;

	virtual JBoolean	Simplify(const JCharacter* function, JString* result) const;
	virtual JBoolean	Simplify(const JFunction* f, const JVariableList* varList,
								 JFunction** result) const;

	virtual JBoolean	Evaluate(const JCharacter* function, const JCharacter* values,
								 JString* result) const;
	virtual JBoolean	Evaluate(const JFunction* f, const JCharacter* values,
								 const JVariableList* varList, JFunction** result) const;

	virtual JBoolean	SolveEquations(const JPtrArray<JString>& lhs,
									   const JPtrArray<JString>& rhs,
									   const JPtrArray<JString>& variables,
									   JString* result) const;
	virtual JBoolean	SolveEquations(const JPtrArray<JFunction>& lhs,
									   const JPtrArray<JFunction>& rhs,
									   const JPtrArray<JString>& variables,
									   JString* result) const;

protected:

	JMathematicaSM(MLEnvironment env, MLINK link);

private:

	MLEnvironment itsEnv;
	MLINK itsLink;

private:

	JBoolean	PrivateSolveEqns(const JString& equations,
								 const JPtrArray<JString>& variables,
								 JString* result) const;

	JString	ConvertToMathematica(const JFunction* f) const;
	void	ConvertFromMathematica(JString* str) const;

	JBoolean	SendRecvText(const JCharacter* command, JString* outputName, JString* result) const;
	void		SendText(const JCharacter* command) const;
	JBoolean	ReceiveText(JString* outputName, JString* result) const;
	JBoolean	WaitForPacket(const int packetType, JProgressDisplay* pg) const;
	void		FlushInput() const;

	JProgressDisplay*	CreatePG() const;
	void				DeletePG(JProgressDisplay* pg) const;

	// not allowed

	JMathematicaSM(const JMathematicaSM& source);
	JMathematicaSM& operator=(const JMathematicaSM& source);
};

#endif
