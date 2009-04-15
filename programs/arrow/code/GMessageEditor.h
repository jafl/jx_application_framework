/******************************************************************************
 GMessageEditor.h

	Interface for the GMessageEditor class

	Copyright � 1997 by Glenn W. Bach.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#ifndef _H_GMessageEditor
#define _H_GMessageEditor

#include <JXTEBase.h>

class JXTextMenu;
class GMessageEditDir;

class GMessageEditor : public JXTEBase
{
public:

	GMessageEditor(GMessageEditDir* dir, JXMenuBar* menuBar,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	virtual ~GMessageEditor();

	JBoolean GetPasteText(JString* text);

protected:

	virtual void		Receive(JBroadcaster* sender,
								const JBroadcaster::Message& message);
	virtual JBoolean	CRMIsPrefixChar(const JCharacter c) const;
	virtual JSize		GetPlainTextTabWidth(const JIndex charIndex, const JIndex textColumn) const;
	virtual JBoolean	IsCharacterInWord(const JString& text,
										  const JIndex charIndex) const;

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList,
									   Atom* action,const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDEnter();
	virtual void		HandleDNDHere(const JPoint& pt, const JXWidget* source);
	virtual void		HandleDNDLeave();
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);
private:

	JIndex				itsStartEditCmd;

	GMessageEditDir*	itsDir;

	JBoolean			itsUsingBaseClassDND;

private:

	void		UpdateEditMenu();
	void		HandleEditMenu(const JIndex index);

	// not allowed

	GMessageEditor(const GMessageEditor& source);
	const GMessageEditor& operator=(const GMessageEditor& source);

};
#endif
