/******************************************************************************
 JXTextSelection.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTextSelection
#define _H_JXTextSelection

#include "jx-af/jx/JXSelectionManager.h"
#include <jx-af/jcore/JTextEditor.h>

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

	bool	GetText(const JString** text) const;
	bool	GetStyle(const JRunArray<JFont>** style) const;
	bool	GetTextEditor(JTextEditor** te) const;

	void	SetData(const JString& text, const JRunArray<JFont>* style = nullptr);
	void	SetData(JString* text, JRunArray<JFont>* style);
	void	SetData(const JPtrArray<JString>& list);

	void	SetTextEditor(JTextEditor* te, const JCharacterRange& selection);

	static const JUtf8Byte*	GetStyledText0XAtomName();

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual bool	ConvertData(const Atom requestType, Atom* returnType,
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
};


/******************************************************************************
 GetText

 ******************************************************************************/

inline bool
JXTextSelection::GetText
	(
	const JString** text
	)
	const
{
	*text = itsText;
	return itsText != nullptr;
}

/******************************************************************************
 GetStyle

 ******************************************************************************/

inline bool
JXTextSelection::GetStyle
	(
	const JRunArray<JFont>** style
	)
	const
{
	*style = itsStyle;
	return itsStyle != nullptr;
}

/******************************************************************************
 GetTextEditor

 ******************************************************************************/

inline bool
JXTextSelection::GetTextEditor
	(
	JTextEditor** te
	)
	const
{
	*te = itsTE;
	return itsTE != nullptr;
}

#endif
