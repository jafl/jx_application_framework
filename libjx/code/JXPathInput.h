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

	~JXPathInput() override;

	bool			InputValid() override;
	virtual bool	GetPath(JString* path) const;
	virtual JString	GetTextForChoosePath() const;

	bool	WillAllowInvalidPath() const;
	void	ShouldAllowInvalidPath(const bool allow = true);

	bool	WillRequireWritable() const;
	void	ShouldRequireWritable(const bool require = true);

	bool	ChoosePath(const JString& instr = JString::empty);

	static JColorID		GetTextColor(const JString& path, const JString& base,
									 const bool requireWrite);

protected:

	class StyledText : public JXFSInputBase::StyledText
	{
		public:

		StyledText(JXPathInput* field, JFontManager* fontManager)
			:
			JXFSInputBase::StyledText(field, fontManager)
		{ };

		protected:

		JSize	ComputeErrorLength(JXFSInputBase* field,
								   const JSize totalLength,
								   const JString& fullName) const override;
	};

protected:

	JXPathInput(StyledText* text, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

private:

	bool	itsAllowInvalidPathFlag;
	bool	itsRequireWriteFlag;

private:

	void JXPathInputX();
};


/******************************************************************************
 Allow invalid path

 ******************************************************************************/

inline bool
JXPathInput::WillAllowInvalidPath()
	const
{
	return itsAllowInvalidPathFlag;
}

inline void
JXPathInput::ShouldAllowInvalidPath
	(
	const bool allow
	)
{
	itsAllowInvalidPathFlag = allow;
}

/******************************************************************************
 Require writable directory

 ******************************************************************************/

inline bool
JXPathInput::WillRequireWritable()
	const
{
	return itsRequireWriteFlag;
}

inline void
JXPathInput::ShouldRequireWritable
	(
	const bool require
	)
{
	itsRequireWriteFlag = require;
}

#endif
