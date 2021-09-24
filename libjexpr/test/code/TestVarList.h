/******************************************************************************
 TestVarList.h

	Written by John Lindal.

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
	TestVarList(std::istream& input);

	virtual ~TestVarList();

// implementation of JVariableList

	virtual const JString&	GetVariableName(const JIndex index) const override;
	virtual void			GetVariableName(const JIndex index, JString* name,
											JString* subscript) const override;

	virtual bool	IsArray(const JIndex index) const override;
	virtual bool	ArrayIndexValid(const JIndex variableIndex,
										const JIndex elementIndex) const override;

	virtual bool	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex, JFloat* value) const override;
	virtual bool	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex, JComplex* value) const override;

	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JFloat value) override;
	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JComplex& value) override;

private:

	JPtrArray<JString>*		itsNumericNames;
	JArray<JFloat>*			itsNumericValues;
	JPtrArray<TVLNArray>*	itsNumericArrays;

private:

	void	TestVarListX();
};

#endif
