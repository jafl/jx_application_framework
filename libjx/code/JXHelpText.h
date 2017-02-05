/******************************************************************************
 JXHelpText.h

	Interface for the JXHelpText class

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXHelpText
#define _H_JXHelpText

#include <JXLinkText.h>

class JXHelpText : public JXLinkText
{
public:

	JXHelpText(const JString& title, const JString& text,
			   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JXHelpText();

	void	ShowSubsection(const JUtf8Byte* name);

protected:

	virtual void	PrepareToPasteHTML();
	virtual void	PrepareToReadHTML();
	virtual void	ReadHTMLFinished();
	virtual void	HandleHTMLTag(const JString& name, const JStringPtrMap<JString>& attr);

	virtual JSize			GetLinkCount() const;
	virtual JCharacterRange	GetLinkRange(const JIndex index) const;
	virtual void			LinkClicked(const JIndex index);

	virtual JCoordinate	GetPrintHeaderHeight(JPagePrinter& p) const;
	virtual void		DrawPrintHeader(JPagePrinter& p, const JCoordinate footerHeight);

private:

	struct MarkInfo
	{
		JString*	name;
		JIndex		charIndex;
		JIndex		byteIndex;

		MarkInfo()
			:
			name(NULL), byteIndex(0)
		{ };

		MarkInfo(JString* s, const JIndex ch, const JIndex byte)
			:
			name(s), charIndex(ch), byteIndex(byte)
		{ };
	};

	struct LinkInfo
	{
		JCharacterRange	charRange;
		JUtf8ByteRange	byteRange;
		JString*		url;

		LinkInfo()
			:
			url(NULL)
		{ };

		LinkInfo(const JCharacterRange& cr, const JUtf8ByteRange& br, JString* s)
			:
			charRange(cr), byteRange(br), url(s)
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

	JCharacterRange	itsAnchorCharRange;
	JUtf8ByteRange	itsAnchorByteRange;
	JString*		itsAnchorText;		// NULL when not reading HTML

private:

	void	BeginAnchor(const JStringPtrMap<JString>& attr);
	void	EndAnchor();
	void	ClearAnchorInfo();

	static JListT::CompareResult
		CompareMarkNames(const MarkInfo& m1, const MarkInfo& m2);

	// not allowed

	JXHelpText(const JXHelpText& source);
	const JXHelpText& operator=(const JXHelpText& source);
};

#endif
