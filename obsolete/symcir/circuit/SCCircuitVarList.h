/******************************************************************************
 SCCircuitVarList.h

	Interface for SCCircuitVarList class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCCircuitVarList
#define _H_SCCircuitVarList

#include <JVariableList.h>
#include <JArray.h>

class JString;
class JFunction;

class SCCircuitVarList : public JVariableList
{
	friend std::ostream& operator<<(std::ostream&, const SCCircuitVarList&);

public:

	SCCircuitVarList();
	SCCircuitVarList(std::istream& input, const JFileVersion vers);

	virtual ~SCCircuitVarList();

	JBoolean	AddVariable(const JCharacter* name, const JFloat value,
							const JBoolean visible);
	JBoolean	AddFunction(const JCharacter* name, const JFunction& f,
							const JBoolean visible);

	JBoolean	SetVariableName(const JIndex index, const JCharacter* name);

	JBoolean	IsVariable(const JIndex index) const;
	JFloat		GetValue(const JIndex index) const;
	JBoolean	SetValue(const JIndex index, const JFloat value);

	JBoolean	IsFunction(const JIndex index) const;
	JBoolean	GetFunction(const JIndex index, const JFunction** f) const;
	JBoolean	SetFunction(const JIndex index, const JFunction& f);

	JBoolean	IsVisible(const JIndex index) const;

	JIndex		GetFrequencyVarIndex() const;

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

	struct VarInfo
	{
		JString*	name;
		JFloat		value;
		JFunction*	f;			// can be NULL
		JBoolean	visible;	// kJTrue if user can see it

		VarInfo()
			:
			name(NULL), value(0.0), f(NULL), visible(kJFalse)
		{ };

		VarInfo(JString* n, const JFloat v, JFunction* fn, const JBoolean vis)
			:
			name(n), value(v), f(fn), visible(vis)
		{ };
	};

private:

	JArray<VarInfo>*	itsVars;

private:

	void	SCCircuitVarListX();

	// not allowed

	SCCircuitVarList(const SCCircuitVarList&);
	const SCCircuitVarList& operator=(const SCCircuitVarList& source);
};


/******************************************************************************
 IsVariable

 ******************************************************************************/

inline JBoolean
SCCircuitVarList::IsVariable
	(
	const JIndex index
	)
	const
{
	const VarInfo info = itsVars->GetElement(index);
	return JConvertToBoolean( info.f == NULL );
}

/******************************************************************************
 GetValue

 ******************************************************************************/

inline JFloat
SCCircuitVarList::GetValue
	(
	const JIndex index
	)
	const
{
	const VarInfo info = itsVars->GetElement(index);
	return info.value;
}

/******************************************************************************
 IsFunction

 ******************************************************************************/

inline JBoolean
SCCircuitVarList::IsFunction
	(
	const JIndex index
	)
	const
{
	const VarInfo info = itsVars->GetElement(index);
	return JConvertToBoolean( info.f != NULL );
}

/******************************************************************************
 GetFunction

	Returns kJTrue if the specified variable is a function.

 ******************************************************************************/

inline JBoolean
SCCircuitVarList::GetFunction
	(
	const JIndex		index,
	const JFunction**	f
	)
	const
{
	const VarInfo info = itsVars->GetElement(index);
	*f = info.f;
	return JConvertToBoolean( *f != NULL );
}

/******************************************************************************
 IsVisible

 ******************************************************************************/

inline JBoolean
SCCircuitVarList::IsVisible
	(
	const JIndex index
	)
	const
{
	const VarInfo info = itsVars->GetElement(index);
	return info.visible;
}

/******************************************************************************
 GetFrequencyVarIndex

	Returns the index of 'f'.

 ******************************************************************************/

inline JIndex
SCCircuitVarList::GetFrequencyVarIndex()
	const
{
	return 1;
}

#endif
