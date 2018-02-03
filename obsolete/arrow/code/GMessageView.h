/******************************************************************************
 GMessageView.h

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GMessageView
#define _H_GMessageView

#include <JXLinkText.h>

class JLatentPG;

class GMessageView : public JXLinkText
{
public:

	GMessageView(JXMenuBar* menuBar, JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);
	GMessageView(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual	~GMessageView();

	void	ParseURLs(const JBoolean deleteFirst = kJTrue);
	void	RemoveLinks();

	virtual void	HandleShortcut(const int key,					// must call inherited
								   const JXKeyModifiers& modifiers) override;

protected:

	virtual JSize		GetLinkCount() const;
	virtual JIndexRange	GetLinkRange(const JIndex index) const;
	virtual void		LinkClicked(const JIndex index);

	virtual void	HandleHTMLTag(const JString& name, const JStringPtrMap<JString>& attr);
	virtual void	HandleHTMLError(const JCharacter* errStr);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

public:

	enum LinkType
	{
		kFullURL = 1,
		kEMail,
		kWeb
	};

	struct LinkInfo
	{
		JIndexRange	range;
		JString*	url;
		LinkType	type;

		LinkInfo()
			:
			range(), url(NULL), type(kFullURL)
		{ };

		LinkInfo(const JIndexRange& r, JString* s, const LinkType t)
			:
			range(r), url(s), type(t)
		{ };
	};

private:

	JArray<LinkInfo>*	itsLinks;

	// used during parsing

	JString		itsLinkUrl;
	JIndexRange	itsLinkRange;

private:

	void GMessageViewX(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
						const HSizingOption hSizing, const VSizingOption vSizing,
						const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h);

	JBoolean	ParseForURLs(const JIndex index, JLatentPG* pg,
							 const JSize length, JIndex* lastindex);
	void		AddLink(const JIndexRange range, const JCharacter* url);
	void		RemoveLinks(const JIndex startIndex);

	// not allowed

	GMessageView(const GMessageView& source);
	const GMessageView& operator=(const GMessageView& source);

public:

	static const JCharacter* kUnderBarShortcutCaught;

	class UnderBarShortcutCaught : public JBroadcaster::Message
		{
		public:

			UnderBarShortcutCaught()
				:
				JBroadcaster::Message(kUnderBarShortcutCaught)
				{ };
		};
};

#endif
