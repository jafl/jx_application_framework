/******************************************************************************
 JXTextSelection.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextSelection
#define _H_JXTextSelection

#include "JXSelectionManager.h"
#include <JTextEditor.h>

class JXTextSelection : public JXSelectionData
{
public:

	JXTextSelection(JXDisplay* display, const JString& text,
					const JRunArray<JFont>* style = nullptr);
	JXTextSelection(JXDisplay* display, JString* text,
					JRunArray<JFont>* style = nullptr);
	JXTextSelection(JXDisplay* display, const JPtrArray<JString>& list);
	JXTextSelection(JXWidget* widget, const JUtf8Byte* id);

	virtual	~JXTextSelection();

	JBoolean	GetText(const JString** text) const;
	JBoolean	GetStyle(const JRunArray<JFont>** style) const;
	JBoolean	GetTextEditor(JTextEditor** te) const;

	void	SetData(const JString& text, const JRunArray<JFont>* style = nullptr);
	void	SetData(JString* text, JRunArray<JFont>* style);
	void	SetData(const JPtrArray<JString>& list);

	void	SetTextEditor(JTextEditor* te, const JCharacterRange& selection);

	static const JUtf8Byte*	GetStyledText0XAtomName();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

private:

	JString*			itsText;		// nullptr until Resolve()
	JRunArray<JFont>*	itsStyle;		// can be nullptr
	JTextEditor*		itsTE;			// not owned; can be nullptr
	JCharacterRange		itsSelection;

	Atom	itsStyledText0XAtom;

private:

	void	JXTextSelectionX();

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
	return JI2B( itsText != nullptr );
}

/******************************************************************************
 GetStyle

 ******************************************************************************/

inline JBoolean
JXTextSelection::GetStyle
	(
	const JRunArray<JFont>** style
	)
	const
{
	*style = itsStyle;
	return JI2B( itsStyle != nullptr );
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
	return JI2B( itsTE != nullptr );
}

#endif
