/******************************************************************************
 TestVarList.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestVarList
#define _H_TestVarList

#include <jx-af/jexpr/JVariableList.h>
#include <jx-af/jcore/JPtrArray.h>

class JString;

typedef JArray<JFloat>	TVLNArray;

class TestVarList : public JVariableList
{
public:

	TestVarList();
	TestVarList(std::istream& input);

	virtual ~TestVarList();

// implementation of JVariableList

	const JString&	GetVariableName(const JIndex index) const override;
	void			GetVariableName(const JIndex index, JString* name,
											JString* subscript) const override;

	bool	IsArray(const JIndex index) const override;
	bool	ArrayIndexValid(const JIndex variableIndex,
										const JIndex elementIndex) const override;

	bool	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex, JFloat* value) const override;
	bool	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex, JComplex* value) const override;

	void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JFloat value) override;
	void	SetNumericValue(const JIndex variableIndex,
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
