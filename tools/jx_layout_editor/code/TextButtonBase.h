/******************************************************************************
 TextButtonBase.h

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_TextButtonBase
#define _H_TextButtonBase

#include "CoreWidget.h"

class TextButtonBase : public CoreWidget
{
public:

	TextButtonBase(LayoutContainer* layout,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);
	TextButtonBase(std::istream& input, const JFileVersion vers, LayoutContainer* layout,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	~TextButtonBase() override;

protected:

	bool	SharedPrintConfiguration(std::ostream& output,
									 const JString& indent,
									 const JString& varName,
									 const JString& shortcuts,
									 JStringManager* stringdb) const;
};

#endif
