/******************************************************************************
 JXPathInput.h

	Interface for the JXPathInput class

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPathInput
#define _H_JXPathInput

#include "JXFSInputBase.h"

class JXPathInput : public JXFSInputBase
{
public:

	JXPathInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXPathInput();

	virtual JBoolean	InputValid() override;
	virtual JBoolean	GetPath(JString* path) const;
	virtual JString		GetTextForChoosePath() const;

	JBoolean	WillAllowInvalidPath() const;
	void		ShouldAllowInvalidPath(const JBoolean allow = kJTrue);

	JBoolean	WillRequireWritable() const;
	void		ShouldRequireWritable(const JBoolean require = kJTrue);

	JBoolean	ChoosePath(const JString& prompt, const JString& instr = JString::empty);

	static JColorID		GetTextColor(const JString& path, const JString& base,
									 const JBoolean requireWrite);

private:

	class StyledText : public JXFSInputBase::StyledText
	{
		public:

		StyledText(JXPathInput* field, JFontManager* fontManager)
			:
			JXFSInputBase::StyledText(field, fontManager)
		{ };

		protected:

		virtual JSize	ComputeErrorLength(JXFSInputBase* field,
										   const JSize totalLength,
										   const JString& fullName) const;
	};

private:

	JBoolean	itsAllowInvalidPathFlag;
	JBoolean	itsRequireWriteFlag;
	JBoolean	itsExpectURLDropFlag;

private:

	// not allowed

	JXPathInput(const JXPathInput& source);
	const JXPathInput& operator=(const JXPathInput& source);
};


/******************************************************************************
 Allow invalid path

 ******************************************************************************/

inline JBoolean
JXPathInput::WillAllowInvalidPath()
	const
{
	return itsAllowInvalidPathFlag;
}

inline void
JXPathInput::ShouldAllowInvalidPath
	(
	const JBoolean allow
	)
{
	itsAllowInvalidPathFlag = allow;
}

/******************************************************************************
 Require writable directory

 ******************************************************************************/

inline JBoolean
JXPathInput::WillRequireWritable()
	const
{
	return itsRequireWriteFlag;
}

inline void
JXPathInput::ShouldRequireWritable
	(
	const JBoolean require
	)
{
	itsRequireWriteFlag = require;
}

#endif
