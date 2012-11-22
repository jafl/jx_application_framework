/******************************************************************************
 CBHTMLStyler.h

	Copyright © 2001-2007 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBHTMLStyler
#define _H_CBHTMLStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBStylerBase.h"
#include "CBHTMLScanner.h"

class CBHTMLStyler : public CBStylerBase, public CBHTMLScanner
{
public:

	static CBStylerBase*	Instance();
	static void				Shutdown();

	virtual ~CBHTMLStyler();

protected:

	CBHTMLStyler();

	virtual void		Scan(istream& input, const TokenExtra& initData);
	virtual TokenExtra	GetFirstTokenExtraData() const;
	virtual void		UpgradeTypeList(const JFileVersion vers,
										JArray<JFontStyle>* typeStyles);

	virtual const JString&	GetScannedText() const;

private:

	JString	itsLatestTagName;

	static CBHTMLStyler*	itsSelf;

private:

	void		InitMustacheTypeStyles();
	void		InitPHPTypeStyles();
	void		InitJSPTypeStyles();
	void		InitJavaScriptTypeStyles();
	void		ExtendCheckRangeForString(const JIndexRange& tokenRange);
	void		ExtendCheckRangeForPHPStartEnd(const JIndexRange& tokenRange);
	void		ExtendCheckRangeForJSPStartEnd(const JIndexRange& tokenRange);
	JFontStyle	GetTagStyle(const JIndexRange& tokenRange, const JIndex typeIndex);
	JBoolean	GetXMLStyle(const JString& tagName, JFontStyle* style);
	void		StyleEmbeddedVariables(const Token& token);

	// not allowed

	CBHTMLStyler(const CBHTMLStyler& source);
	const CBHTMLStyler& operator=(const CBHTMLStyler& source);
};

#endif
