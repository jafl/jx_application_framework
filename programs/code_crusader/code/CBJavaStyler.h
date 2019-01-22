/******************************************************************************
 CBJavaStyler.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBJavaStyler
#define _H_CBJavaStyler

#include "CBStylerBase.h"
#include "CBJavaScanner.h"

class CBJavaStyler : public CBStylerBase, public CBJavaScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBJavaStyler();

protected:

	CBJavaStyler();

	virtual void	Scan(std::istream& input, const TokenExtra& initData) override;
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	static CBJavaStyler*	itsSelf;

private:

	// not allowed

	CBJavaStyler(const CBJavaStyler& source);
	const CBJavaStyler& operator=(const CBJavaStyler& source);
};

#endif
