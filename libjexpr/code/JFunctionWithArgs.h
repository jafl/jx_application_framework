/******************************************************************************
 JFunctionWithArgs.h

	Interface for JFunctionWithArgs class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFunctionWithArgs
#define _H_JFunctionWithArgs

#include <JFunction.h>

class JFunctionWithArgs : public JFunction
{
public:

	JFunctionWithArgs(const JFnNameIndex nameIndex, const JFunctionType type);
	JFunctionWithArgs(const JFunctionWithArgs& source);

	virtual ~JFunctionWithArgs();

	const JCharacter*	GetName() const;
	const JCharacter*	GetMathematicaName() const;
	virtual void		Print(ostream& output) const;
	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

	virtual JSize				GetArgCount() const = 0;
	virtual const JFunction*	GetArg(const JIndex index) const = 0;
	virtual JFunction*			GetArg(const JIndex index) = 0;
	virtual void				SetArg(const JIndex index, JFunction* arg) = 0;

	JBoolean	ReplaceArg(const JFunction* origArg, JFunction* newArg);

	// provides safe downcasting

	virtual JFunctionWithArgs*			CastToJFunctionWithArgs();
	virtual const JFunctionWithArgs*	CastToJFunctionWithArgs() const;

private:

	const JFnNameIndex	itsNameIndex;

private:

	void	PrintArgs(ostream& output) const;

	// not allowed

	const JFunctionWithArgs& operator=(const JFunctionWithArgs& source);
};

#endif
