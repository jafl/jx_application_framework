/******************************************************************************
 JFunctionWithArgs.h

	Interface for JFunctionWithArgs class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JFunctionWithArgs
#define _H_JFunctionWithArgs

#include "jx-af/jexpr/JFunction.h"
#include <jx-af/jcore/JString.h>

class JFunctionWithArgs : public JFunction
{
public:

	JFunctionWithArgs(const JUtf8Byte* name);
	JFunctionWithArgs(const JFunctionWithArgs& source);

	~JFunctionWithArgs();

	const JString&	GetName() const;
	void	Print(std::ostream& output) const override;
	JIndex	Layout(const JExprRenderer& renderer,
				   const JPoint& upperLeft, const JSize fontSize,
				   JExprRectList* rectList) override;
	void	Render(const JExprRenderer& renderer,
				   const JExprRectList& rectList) const override;

	virtual JSize				GetArgCount() const = 0;
	virtual const JFunction*	GetArg(const JIndex index) const = 0;
	virtual JFunction*			GetArg(const JIndex index) = 0;
	virtual void				SetArg(const JIndex index, JFunction* arg) = 0;

	bool	ReplaceArg(const JFunction* origArg, JFunction* newArg);

private:

	const JString	itsName;

private:

	void	PrintArgs(std::ostream& output) const;
};


/******************************************************************************
 GetName

 ******************************************************************************/

inline const JString&
JFunctionWithArgs::GetName()
	const
{
	return itsName;
}

#endif
