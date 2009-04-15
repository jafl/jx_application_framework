/******************************************************************************
 JFunction.h

	Interface for JFunction class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFunction
#define _H_JFunction

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFunctionType.h>
#include <JComplex.h>
#include <JRect.h>

class JString;
class JVariableList;
class JExprNodeList;

class JFunctionWithArgs;
class JUnaryFunction;
class JBinaryFunction;
class JBinaryOperator;
class JNaryFunction;
class JNaryOperator;
class JFunctionWithVar;

class JExprRenderer;
class JExprRectList;

class JFunction
{
public:

	JFunction(const JFunctionType type);
	JFunction(const JFunction& source);

	virtual ~JFunction();

	JFunctionType		GetType() const;

	virtual JBoolean	Evaluate(JFloat* result) const = 0;
	virtual JBoolean	Evaluate(JComplex* result) const = 0;
	JString				Print() const;
	virtual void		Print(ostream& output) const = 0;
	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;
	virtual JFunction*	Copy() const = 0;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode) = 0;

	static JFunction*	StreamIn(istream& input, const JVariableList* theVariableList,
								 const JBoolean allowUIF = kJFalse);
	void				StreamOut(ostream& output) const;

	JFunction*	Copy(const JVariableList* newVariableList) const;

	JString	PrintForMathematica() const;
	void	PrintForMathematica(ostream& output) const;

	// called by JVariableList -- must call inherited

	virtual JBoolean	UsesVariable(const JIndex variableIndex) const;
	virtual void		VariablesInserted(const JIndex firstIndex, const JSize count);
	virtual void		VariablesRemoved(const JIndex firstIndex, const JSize count);
	virtual void		VariableMoved(const JIndex origIndex, const JIndex newIndex);
	virtual void		VariablesSwapped(const JIndex index1, const JIndex index2);

	// provides safe downcasting

	virtual JFunctionWithArgs*			CastToJFunctionWithArgs();
	virtual const JFunctionWithArgs*	CastToJFunctionWithArgs() const;
	virtual JUnaryFunction*				CastToJUnaryFunction();
	virtual const JUnaryFunction*		CastToJUnaryFunction() const;
	virtual JBinaryFunction*			CastToJBinaryFunction();
	virtual const JBinaryFunction*		CastToJBinaryFunction() const;
	virtual JBinaryOperator*			CastToJBinaryOperator();
	virtual const JBinaryOperator*		CastToJBinaryOperator() const;
	virtual JNaryFunction*				CastToJNaryFunction();
	virtual const JNaryFunction*		CastToJNaryFunction() const;
	virtual JNaryOperator*				CastToJNaryOperator();
	virtual const JNaryOperator*		CastToJNaryOperator() const;
	virtual JFunctionWithVar*			CastToJFunctionWithVar();
	virtual const JFunctionWithVar*		CastToJFunctionWithVar() const;

public:		// ought to be protected, but Visual C++ complains about GetPrintDestination()

	enum JFunctionPrintDest
	{
		kStandardDest,
		kMathematica
	};

protected:

	JFunctionPrintDest	GetPrintDestination() const;

private:

	const JFunctionType itsType;

	static JFunctionPrintDest	itsPrintDest;

private:

	// not allowed

	const JFunction& operator=(const JFunction& source);
};


inline int
operator==
	(
	const JFunction& f1,
	const JFunction& f2
	)
{
	return f1.SameAs(f2);
}

inline int
operator!=
	(
	const JFunction& f1,
	const JFunction& f2
	)
{
	return !(f1 == f2);
}

/******************************************************************************
 GetType

 ******************************************************************************/

inline JFunctionType
JFunction::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 GetPrintDestination

 ******************************************************************************/

inline JFunction::JFunctionPrintDest
JFunction::GetPrintDestination()
	const
{
	return itsPrintDest;
}

#endif
