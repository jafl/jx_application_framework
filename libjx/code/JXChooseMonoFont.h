/******************************************************************************
 JXChooseMonoFont.h

	Interface for the JXChooseMonoFont class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXChooseMonoFont
#define _H_JXChooseMonoFont

#include "JXWidgetSet.h"

class JXXFontMenu;
class JXFontSizeMenu;

class JXChooseMonoFont : public JXWidgetSet
{
public:

	JXChooseMonoFont(JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	~JXChooseMonoFont() override;

	void	GetFont(JString* name, JSize* size) const;
	void	SetFont(const JString& name, const JSize size);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXXFontMenu*		itsFontMenu;
	JXFontSizeMenu*		itsSizeMenu;

private:

	void	PrependOtherMonospaceFonts(JXXFontMenu* menu);
	void	UpdateMenus(const bool updateSize);

	static std::weak_ordering
		CompareFontNames(JString * const &, JString * const &);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFontChanged;

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
