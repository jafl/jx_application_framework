/******************************************************************************
 JExprRenderer.h

	Interface for the JExprRenderer class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JExprRenderer
#define _H_JExprRenderer

#include <JRect.h>

class JString;
class JUtf8Character;

class JExprRenderer
{
public:

	JExprRenderer();

	virtual ~JExprRenderer();

	virtual JSize	GetInitialFontSize() const = 0;
	virtual JSize	GetSuperSubFontSize(const JSize baseFontSize) const = 0;
	virtual JSize	GetSpaceWidth(const JSize fontSize) const = 0;
	virtual JSize	GetStringWidth(const JSize fontSize, const JString& str) const = 0;
	virtual JSize	GetLineHeight(const JSize fontSize) const = 0;
	virtual void	DrawString(const JCoordinate left, const JCoordinate midline,
							   const JSize fontSize, const JString& str) const = 0;

	virtual JSize	GetHorizBarHeight() const = 0;
	virtual void	DrawHorizBar(const JCoordinate left, const JCoordinate v,
								 const JSize length) const = 0;

	virtual JSize	GetVertBarWidth() const = 0;
	virtual void	DrawVertBar(const JCoordinate h, const JCoordinate top,
								const JSize length) const = 0;

	virtual JSize	GetSuperscriptHeight(const JCoordinate baseHeight) const = 0;
	virtual JSize	GetSubscriptDepth(const JCoordinate baseHeight) const = 0;

	virtual JSize	GetParenthesisWidth(const JCoordinate argHeight) const = 0;
	virtual void	DrawParentheses(const JRect& argRect) const = 0;

	virtual JSize	GetSquareBracketWidth(const JCoordinate argHeight) const = 0;
	virtual void	DrawSquareBrackets(const JRect& argRect) const = 0;

	virtual JSize	GetSquareRootLeadingWidth(const JCoordinate argHeight) const = 0;
	virtual JSize	GetSquareRootTrailingWidth(const JCoordinate argHeight) const = 0;
	virtual JSize	GetSquareRootExtraHeight() const = 0;
	virtual void	DrawSquareRoot(const JRect& enclosure) const = 0;

private:

	// not allowed

	JExprRenderer(const JExprRenderer&) = delete;
	JExprRenderer& operator=(const JExprRenderer&) = delete;
};

#endif
