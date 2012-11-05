/******************************************************************************
 CBJavaStyler.h

	Copyright © 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBJavaStyler
#define _H_CBJavaStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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

	virtual void	Scan(istream& input, const TokenExtra& initData);
	virtual void	UpgradeTypeList(const JFileVersion vers,
									JArray<JFontStyle>* typeStyles);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	static CBJavaStyler*	itsSelf;

private:

	// not allowed

	CBJavaStyler(const CBJavaStyler& source);
	const CBJavaStyler& operator=(const CBJavaStyler& source);
};

#endif
