/******************************************************************************
 LayoutSelection.h

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_LayoutSelection
#define _H_LayoutSelection

#include <jx-af/jx/JXSelectionManager.h>

class JPoint;
class JRect;
class LayoutContainer;
class BaseWidget;

class LayoutSelection : public JXSelectionData
{
public:

	LayoutSelection(LayoutContainer* layout, const JPoint& ptG);

	~LayoutSelection() override;

	static const JUtf8Byte*	GetDataXAtomName();
	static const JUtf8Byte*	GetMetaXAtomName();

	static void	ReadMetaData(std::istream& input,
							 JPoint* boundsOffset, JRect* bounds,
							 JPoint* mouseOffset, JArray<JRect>* rectList);

protected:

	void	AddTypes(const Atom selectionName) override;
	bool	ConvertData(const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						JSize* bitsPerBlock) const override;

private:

	LayoutContainer*	itsLayout;	// not owned; can be nullptr
	JString				itsData;
	JString				itsMetaData;

	Atom	itsDataXAtom;
	Atom	itsMetaXAtom;
};

#endif
