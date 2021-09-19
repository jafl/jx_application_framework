/******************************************************************************
 CBPythonStyler.h

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPythonStyler
#define _H_CBPythonStyler

#include "CBStylerBase.h"
#include "CBPythonScanner.h"

class CBPythonStyler : public CBStylerBase, public CB::Text::Python::Scanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBPythonStyler();

protected:

	CBPythonStyler();

	virtual void	Scan(const JStyledText::TextIndex& startIndex,
						 std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

private:

	static CBPythonStyler*	itsSelf;

private:

	// not allowed

	CBPythonStyler(const CBPythonStyler& source);
	const CBPythonStyler& operator=(const CBPythonStyler& source);
};

#endif
