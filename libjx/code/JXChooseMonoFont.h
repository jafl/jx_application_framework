/******************************************************************************
 JXChooseMonoFont.h

	Interface for the JXChooseMonoFont class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXChooseMonoFont
#define _H_JXChooseMonoFont

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidgetSet.h>

class JString;
class JXXFontMenu;
class JXFontSizeMenu;
class JXFontCharSetMenu;

class JXChooseMonoFont : public JXWidgetSet
{
public:

	JXChooseMonoFont(JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual ~JXChooseMonoFont();

	void	GetFont(JString* name, JSize* size) const;
	void	SetFont(const JCharacter* name, const JSize size);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXXFontMenu*		itsFontMenu;
	JXFontSizeMenu*		itsSizeMenu;
	JXFontCharSetMenu*	itsCharSetMenu;

private:

	void	AppendOtherMonospaceFonts(JXXFontMenu* menu);
	void	UpdateMenus(const JBoolean updateSize);

	static JOrderedSetT::CompareResult
		CompareFontNames(JString * const &, JString * const &);

	// not allowed

	JXChooseMonoFont(const JXChooseMonoFont& source);
	const JXChooseMonoFont& operator=(const JXChooseMonoFont& source);

public:

	// JBroadcaster messages

	static const JCharacter* kFontChanged;

	class FontChanged : public JBroadcaster::Message
		{
		public:

			FontChanged()
				:
				JBroadcaster::Message(kFontChanged)
				{ };

		// call broadcaster's GetFont() to get fontName
		};
};

#endif
