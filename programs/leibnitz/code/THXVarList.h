/******************************************************************************
 THXVarList.h

	Interface for THXVarList class.

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_THXVarList
#define _H_THXVarList

#include <JVariableList.h>
#include <JPtrArray.h>

class JString;
class JFunction;
class JConstantValue;

class THXVarList : public JVariableList
{
	friend std::ostream& operator<<(std::ostream&, const THXVarList&);

public:

	enum
	{
		kXIndex = 1,
		kYIndex = 2,
		kTIndex = 3,

		kUserFnOffset = 3
	};

public:

	THXVarList();
	THXVarList(std::istream& input, const JFileVersion vers);

	virtual ~THXVarList();

	JIndex		NewFunction();
	JBoolean	SetVariableName(const JIndex index, const JCharacter* name);

	const JFunction*	GetFunction(const JIndex index) const;
	JBoolean			SetFunction(const JIndex index, const JCharacter* expr);

	JBoolean	OKToRemoveFunction(const JIndex index) const;
	void		RemoveFunction(const JIndex index);

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
										const JIndex elementIndex,
										JFloat* value) const;
	virtual JBoolean	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex,
										JComplex* value) const;

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

	JPtrArray<JString>*		itsNames;
	JPtrArray<JFunction>*	itsFunctions;

	// store extra copies to avoid downcasting

	JConstantValue*	itsXValue;
	JConstantValue*	itsYValue;
	JConstantValue*	itsTValue;

private:

	void	THXVarListX();

	// not allowed

	THXVarList(const THXVarList&);
	const THXVarList& operator=(const THXVarList& source);
};


/******************************************************************************
 GetFunction

 ******************************************************************************/

inline const JFunction*
THXVarList::GetFunction
	(
	const JIndex index
	)
	const
{
	return itsFunctions->GetElement(index);
}

/******************************************************************************
 OKToRemoveFunction

 ******************************************************************************/

inline JBoolean
THXVarList::OKToRemoveFunction
	(
	const JIndex index
	)
	const
{
	return JI2B( index > kUserFnOffset &&
				 OKToRemoveVariable(index) );
}

#endif
