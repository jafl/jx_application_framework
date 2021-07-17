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
	bool	SetVariableName(const JIndex index, const JString& name);

	const JFunction*	GetFunction(const JIndex index) const;
	bool			SetFunction(const JIndex index, const JString& expr);

	bool	OKToRemoveFunction(const JIndex index) const;
	void		RemoveFunction(const JIndex index);

// implementation of JVariableList

	virtual const JString&	GetVariableName(const JIndex index) const override;
	virtual void			GetVariableName(const JIndex index, JString* name,
											JString* subscript) const override;

	virtual bool	IsArray(const JIndex index) const override;
	virtual bool	ArrayIndexValid(const JIndex variableIndex,
										const JIndex elementIndex) const override;

	virtual bool	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex,
										JFloat* value) const override;
	virtual bool	GetNumericValue(const JIndex variableIndex,
										const JIndex elementIndex,
										JComplex* value) const override;

	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JFloat value) override;
	virtual void	SetNumericValue(const JIndex variableIndex,
									const JIndex elementIndex,
									const JComplex& value) override;

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

inline bool
THXVarList::OKToRemoveFunction
	(
	const JIndex index
	)
	const
{
	return index > kUserFnOffset &&
				 OKToRemoveVariable(index);
}

#endif
