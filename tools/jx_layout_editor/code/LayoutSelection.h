/******************************************************************************
 LayoutSelection.h

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutSelection
#define _H_LayoutSelection

#include <jx-af/jx/JXSelectionManager.h>

class LayoutContainer;
class BaseWidget;

class LayoutSelection : public JXSelectionData
{
public:

	LayoutSelection(LayoutContainer* layout, const JUtf8Byte* id);

	~LayoutSelection() override;

	void	SetData(const JPtrArray<BaseWidget>& list);

	static const JUtf8Byte*	GetXAtomName();

protected:

	void	AddTypes(const Atom selectionName) override;
	bool	ConvertData(const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						JSize* bitsPerBlock) const override;

private:

	LayoutContainer*	itsLayout;	// not owned; can be nullptr
	JString				itsData;

	Atom	itsXAtom;
};

#endif
