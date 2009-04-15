/******************************************************************************
 JXHelpText.h

	Interface for the JXHelpText class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXHelpText
#define _H_JXHelpText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXLinkText.h>

class JXHelpText : public JXLinkText
{
public:

	JXHelpText(const JCharacter* title, const JCharacter* text,
			   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JXHelpText();

	void	ShowSubsection(const JCharacter* name);

protected:

	virtual void	PrepareToPasteHTML();
	virtual void	PrepareToReadHTML();
	virtual void	ReadHTMLFinished();
	virtual void	HandleHTMLTag(const JString& name, const JStringPtrMap<JString>& attr);

	virtual JSize		GetLinkCount() const;
	virtual JIndexRange	GetLinkRange(const JIndex index) const;
	virtual void		LinkClicked(const JIndex index);

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate footerHeight);

private:

	struct MarkInfo
	{
		JString*	name;
		JIndex		index;

		MarkInfo()
			:
			name(NULL), index(0)
		{ };

		MarkInfo(JString* s, const JIndex i)
			:
			name(s), index(i)
		{ };
	};

	struct LinkInfo
	{
		JIndexRange	range;
		JString*	url;

		LinkInfo()
			:
			range(), url(NULL)
		{ };

		LinkInfo(const JIndexRange& r, JString* s)
			:
			range(r), url(s)
		{ };
	};

private:

	JString				itsTitle;
	JArray<MarkInfo>*	itsMarks;
	JArray<LinkInfo>*	itsLinks;

	JCursorIndex	itsLinkCursor;

	// used while dragging

	JIndex	itsMouseDownIndex;

	// used while reading HTML

	JIndexRange	itsAnchorRange;
	JString*	itsAnchorText;		// NULL when not reading HTML

private:

	void	BeginAnchor(const JStringPtrMap<JString>& attr);
	void	EndAnchor();
	void	ClearAnchorInfo();

	static JOrderedSetT::CompareResult
		CompareMarkNames(const MarkInfo& m1, const MarkInfo& m2);

	// not allowed

	JXHelpText(const JXHelpText& source);
	const JXHelpText& operator=(const JXHelpText& source);
};

#endif
