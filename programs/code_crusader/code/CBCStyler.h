/******************************************************************************
 CBCStyler.h

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCStyler
#define _H_CBCStyler

#include "CBStylerBase.h"
#include "CBCScanner.h"

class CBCStyler : public CBStylerBase, public CBCScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBCStyler();

protected:

	CBCStyler();

	virtual void	Scan(std::istream& input, const TokenExtra& initData);
	virtual void	PreexpandCheckRange(const JString& text,
										const JRunArray<JFont>& styles,
										const JIndexRange& modifiedRange,
										const JBoolean deletion,
										JIndexRange* checkRange);

	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CBCStyler*	itsSelf;

private:

	JBoolean	SlurpPPComment(JIndexRange* totalRange);

	// not allowed

	CBCStyler(const CBCStyler& source);
	const CBCStyler& operator=(const CBCStyler& source);
};

#endif
