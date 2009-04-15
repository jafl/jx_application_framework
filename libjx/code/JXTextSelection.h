/******************************************************************************
 JXTextSelection.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXTextSelection
#define _H_JXTextSelection

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXSelectionManager.h>
#include <JTextEditor.h>

class JXColormap;

class JXTextSelection : public JXSelectionData
{
public:

	JXTextSelection(JXDisplay* display, const JCharacter* text,
					const JXColormap* colormap = NULL,
					const JRunArray<JTextEditor::Font>* style = NULL);
	JXTextSelection(JXDisplay* display, JString* text,
					const JXColormap* colormap = NULL,
					JRunArray<JTextEditor::Font>* style = NULL);
	JXTextSelection(JXDisplay* display, const JPtrArray<JString>& list);
	JXTextSelection(JXWidget* widget, const JCharacter* id);

	virtual	~JXTextSelection();

	JBoolean	GetText(const JString** text) const;
	JBoolean	GetStyle(const JRunArray<JTextEditor::Font>** style) const;
	JBoolean	GetColormap(const JXColormap** colormap) const;
	JBoolean	GetTextEditor(JTextEditor** te) const;

	void	SetData(const JCharacter* text,
					const JXColormap* colormap = NULL,
					const JRunArray<JTextEditor::Font>* style = NULL);
	void	SetData(JString* text, const JXColormap* colormap,
					JRunArray<JTextEditor::Font>* style);
	void	SetData(const JPtrArray<JString>& list);

	void	SetTextEditor(JTextEditor* te, const JIndexRange& selection);

	static const JCharacter*	GetStyledText0XAtomName();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	JString*						itsText;		// NULL until Resolve()
	JRunArray<JTextEditor::Font>*	itsStyle;		// can be NULL
	const JXColormap*				itsColormap;	// not owned; can be NULL
	JTextEditor*					itsTE;			// not owned; can be NULL
	JIndexRange						itsSelection;

	Atom	itsStyledText0XAtom;

private:

	void	JXTextSelectionX();
	void	SetColormap(const JXColormap* colormap);

	// not allowed

	JXTextSelection(const JXTextSelection& source);
	const JXTextSelection& operator=(const JXTextSelection& source);
};


/******************************************************************************
 GetText

 ******************************************************************************/

inline JBoolean
JXTextSelection::GetText
	(
	const JString** text
	)
	const
{
	*text = itsText;
	return JI2B( itsText != NULL );
}

/******************************************************************************
 GetStyle

 ******************************************************************************/

inline JBoolean
JXTextSelection::GetStyle
	(
	const JRunArray<JTextEditor::Font>** style
	)
	const
{
	*style = itsStyle;
	return JI2B( itsStyle != NULL );
}

/******************************************************************************
 GetColormap

 ******************************************************************************/

inline JBoolean
JXTextSelection::GetColormap
	(
	const JXColormap** colormap
	)
	const
{
	*colormap = itsColormap;
	return JI2B( itsColormap != NULL );
}

/******************************************************************************
 GetTextEditor

 ******************************************************************************/

inline JBoolean
JXTextSelection::GetTextEditor
	(
	JTextEditor** te
	)
	const
{
	*te = itsTE;
	return JI2B( itsTE != NULL );
}

#endif
