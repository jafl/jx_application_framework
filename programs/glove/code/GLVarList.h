/******************************************************************************
 GLVarList.h

	Interface for GLVarList class.

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GVarList
#define _H_GVarList

#include <JVariableList.h>
#include <JPtrArray.h>

class JString;

typedef JArray<JFloat>	GNArray;

class GLVarList : public JVariableList
{
public:

	GLVarList();
	GLVarList(const GLVarList& list);
	GLVarList(std::istream& input);

	virtual ~GLVarList();

	JBoolean	AddVariable(const JString& name, const JFloat value);
	void		RemoveVariable(const JIndex index);
	JBoolean	AddArray(const JString& name, const GNArray& values);

	JBoolean	IsVariable(const JIndex index) const;
	JBoolean	SetValue(const JIndex index, const JFloat value);
	JSize		GetVariableCount() const;
	JBoolean	SetElementValue(const JIndex variableIndex, 
								const JIndex elementIndex,
								const JFloat value);
	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JFloat value) override;
	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JComplex& value) override;
	
// implementation of JVariableList

	virtual const JString&	GetVariableName(const JIndex index) const override;
	virtual void			GetVariableName(const JIndex index, JString* name,
											JString* subscript) const override;
	JBoolean				SetVariableName(const JIndex index, const JString& str);

	const JPtrArray<JString>&	GetVariables() const;

	virtual JBoolean	IsArray(const JIndex index) const override;
	virtual JBoolean	ArrayIndexValid(const JIndex variableIndex,
										const JIndex elementIndex) const override;

	virtual JBoolean	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex, JFloat* value) const override;
	virtual JBoolean	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex, JComplex* value) const override;

private:

	JPtrArray<JString>*		itsNames;
	JArray<JFloat>*			itsValues;
	JPtrArray<GNArray>*		itsArrays;

private:

	void	GVarListX();

	// not allowed

	const GLVarList& operator=(const GLVarList& source);
};

/******************************************************************************
 IsVariable

 ******************************************************************************/

inline JBoolean
GLVarList::IsVariable
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
GLVarList::GetVariables()
	const
{
	return *itsNames;
}

#endif