/******************************************************************************
 CBCommandPathInput.h

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCommandPathInput
#define _H_CBCommandPathInput

#include <JXPathInput.h>

class CBCommandPathInput : public JXPathInput
{
public:

	CBCommandPathInput(JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~CBCommandPathInput();

	virtual JBoolean	InputValid();
	virtual JBoolean	GetPath(JString* path) const;
	virtual JString		GetTextForChoosePath() const;

	static JColorID	GetTextColor(const JString& path, const JString& base,
									 const JBoolean requireWrite);

protected:

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<JFont>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);

private:

	// not allowed

	CBCommandPathInput(const CBCommandPathInput& source);
	const CBCommandPathInput& operator=(const CBCommandPathInput& source);
};

#endif
