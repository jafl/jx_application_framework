/******************************************************************************
 GVarList.h

	Interface for GVarList class.

	Copyright (C) 1997 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GVarList
#define _H_GVarList

#include <JVariableList.h>
#include <JPtrArray.h>

class JString;

typedef JArray<JFloat>	GNArray;

class GVarList : public JVariableList
{
public:

	GVarList();
	GVarList(const GVarList& list);
	GVarList(istream& input);

	virtual ~GVarList();

	JBoolean	AddVariable(const JCharacter* name, const JFloat value);
	void		RemoveVariable(const JIndex index);
	JBoolean	AddArray(const JCharacter* name, const GNArray& values);

	JBoolean	IsVariable(const JIndex index) const;
	JBoolean	SetValue(const JIndex index, const JFloat value);
	JSize		GetVariableCount() const;
	JBoolean	SetElementValue(const JIndex variableIndex, 
								const JIndex elementIndex,
								const JFloat value);
	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JFloat value);
	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JComplex& value);
	virtual void	SetDiscreteValue(const JIndex variableIndex,
									 const JIndex elementIndex,
									 const JIndex valueIndex);
	
// implementation of JVariableList

	virtual const JString&	GetVariableName(const JIndex index) const;
	virtual void			GetVariableName(const JIndex index, JString* name,
											JString* subscript) const;
	JBoolean				SetVariableName(const JIndex index, const JString& str);

	const JPtrArray<JString>&	GetVariables() const;

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

	virtual JIndex			GetDiscreteValue(const JIndex variableIndex,
											 const JIndex elementIndex) const;
	virtual JSize			GetDiscreteValueCount(const JIndex index) const;
	virtual const JString&	GetDiscreteValueName(const JIndex variableIndex,
												 const JIndex valueIndex) const;

private:

	JPtrArray<JString>*		itsNames;
	JArray<JFloat>*			itsValues;
	JPtrArray<GNArray>*		itsArrays;

private:

	void	GVarListX();

	// not allowed

	const GVarList& operator=(const GVarList& source);
};

/******************************************************************************
 IsVariable

 ******************************************************************************/

inline JBoolean
GVarList::IsVariable
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean(!(IsArray(index)));
}

/******************************************************************************
 GetVariables (public)

 ******************************************************************************/

inline const JPtrArray<JString>&
GVarList::GetVariables()
	const
{
	return *itsNames;
}

#endif
