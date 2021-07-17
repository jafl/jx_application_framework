/******************************************************************************
 CMDebuggerProgramInput.h

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMDebuggerProgramInput
#define _H_CMDebuggerProgramInput

#include <JXFileInput.h>

class CMDebuggerProgramInput : public JXFileInput
{
public:

	CMDebuggerProgramInput(JXContainer* enclosure,
						   const HSizingOption hSizing, const VSizingOption vSizing,
						   const JCoordinate x, const JCoordinate y,
						   const JCoordinate w, const JCoordinate h);

	virtual ~CMDebuggerProgramInput();

	virtual bool	InputValid() override;
	virtual bool	GetFile(JString* fullName) const override;

protected:

	class StyledText : public JXFileInput::StyledText
	{
		public:

		StyledText(CMDebuggerProgramInput* field, JFontManager* fontManager)
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

	CMDebuggerProgramInput(const CMDebuggerProgramInput& source);
	const CMDebuggerProgramInput& operator=(const CMDebuggerProgramInput& source);
};

#endif
