/******************************************************************************
 TestVarList.h

	Interface for TestVarList class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_TestVarList
#define _H_TestVarList

#include <JVariableList.h>
#include <JPtrArray.h>

class JString;

typedef JArray<JFloat>	TVLNArray;

class TestVarList : public JVariableList
{
public:

	TestVarList();
	TestVarList(istream& input);

	virtual ~TestVarList();

	JBoolean	AddNumericVar(const JCharacter* name, const JFloat value);
	JBoolean	AddNumericArray(const JCharacter* name, const TVLNArray& values);

// implementation of JVariableList

	virtual const JString&	GetVariableName(const JIndex index) const;
	virtual void			GetVariableName(const JIndex index, JString* name,
											JString* subscript) const;

	virtual JBoolean	IsNumeric(const JIndex index) const;
	virtual JBoolean	IsDiscrete(const JIndex index) const;

	virtual JBoolean	IsArray(const JIndex index) const;
	virtual JBoolean	ArrayIndexValid(const JIndex variableIndex,
										const JIndex elementIndex) const;

	virtual JBoolean		ValueIsKnown(const JIndex variableIndex,
										 const JIndex elementIndex) const;
	virtual const JString&	GetUnknownValueSymbol(const JIndex index) const;

	virtual JBoolean	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex, JFloat* value) const;
	virtual JBoolean	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex, JComplex* value) const;

	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JFloat value);
	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JComplex& value);

	virtual JIndex			GetDiscreteValue(const JIndex variableIndex,
											 const JIndex elementIndex) const;
	virtual JSize			GetDiscreteValueCount(const JIndex index) const;
	virtual const JString&	GetDiscreteValueName(const JIndex variableIndex,
												 const JIndex valueIndex) const;

	virtual void	SetDiscreteValue(const JIndex variableIndex,
									 const JIndex elementIndex,
									 const JIndex valueIndex);

private:

	JPtrArray<JString>*		itsNumericNames;
	JArray<JFloat>*			itsNumericValues;
	JPtrArray<TVLNArray>*	itsNumericArrays;

private:

	void	TestVarListX();

	// not allowed

	TestVarList(const TestVarList&);
	const TestVarList& operator=(const TestVarList& source);
};

#endif
