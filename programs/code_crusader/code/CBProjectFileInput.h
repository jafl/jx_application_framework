/******************************************************************************
 CBProjectFileInput.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBProjectFileInput
#define _H_CBProjectFileInput

#include <JXFileInput.h>

class CBProjectFileInput : public JXFileInput
{
public:

	CBProjectFileInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~CBProjectFileInput();

protected:

	class StyledText : public JXFileInput::StyledText
	{
		public:

		StyledText(CBProjectFileInput* field, JFontManager* fontManager)
			:
			JXFileInput::StyledText(field, fontManager)
		{ };

		protected:

		virtual JSize	ComputeErrorLength(JXFSInputBase* field,
										   const JSize totalLength,
										   const JString& fullName) const override;
	};

private:

	// not allowed

	CBProjectFileInput(const CBProjectFileInput& source);
	const CBProjectFileInput& operator=(const CBProjectFileInput& source);
};

#endif
