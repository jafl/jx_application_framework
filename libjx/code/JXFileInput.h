/******************************************************************************
 JXFileInput.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFileInput
#define _H_JXFileInput

#include "JXFSInputBase.h"

class JXFileInput : public JXFSInputBase
{
public:

	JXFileInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~JXFileInput();

	virtual JBoolean	InputValid() override;
	virtual JBoolean	GetFile(JString* fullName) const;
	virtual JString		GetTextForChooseFile() const;

	JBoolean	WillAllowInvalidFile() const;
	void		ShouldAllowInvalidFile(const JBoolean allow = kJTrue);

	JBoolean	WillRequireReadable() const;
	void		ShouldRequireReadable(const JBoolean require = kJTrue);

	JBoolean	WillRequireWritable() const;
	void		ShouldRequireWritable(const JBoolean require = kJTrue);

	JBoolean	WillRequireExecutable() const;
	void		ShouldRequireExecutable(const JBoolean require = kJTrue);

	JBoolean	ChooseFile(const JString& prompt, const JString& instr = JString::empty);
	JBoolean	SaveFile(const JString& prompt, const JString& instr = JString::empty);

	static JColorID		GetTextColor(const JString& fileName, const JString& basePath,
									 const JBoolean requireRead, const JBoolean requireWrite,
									 const JBoolean requireExec);

private:

	class StyledText : public JXFSInputBase::StyledText
	{
		public:

		StyledText(JXFileInput* field, JFontManager* fontManager)
			:
			JXFSInputBase::StyledText(field, fontManager)
		{ };

		protected:

		virtual JSize	ComputeErrorLength(JXFSInputBase* field,
										   const JSize totalLength,
										   const JString& fullName) const;
	};

private:

	JBoolean	itsAllowInvalidFileFlag;
	JBoolean	itsRequireReadFlag;
	JBoolean	itsRequireWriteFlag;
	JBoolean	itsRequireExecFlag;

private:

	// not allowed

	JXFileInput(const JXFileInput& source);
	const JXFileInput& operator=(const JXFileInput& source);
};


/******************************************************************************
 Allow invalid file

 ******************************************************************************/

inline JBoolean
JXFileInput::WillAllowInvalidFile()
	const
{
	return itsAllowInvalidFileFlag;
}

inline void
JXFileInput::ShouldAllowInvalidFile
	(
	const JBoolean allow
	)
{
	itsAllowInvalidFileFlag = allow;
}

/******************************************************************************
 Require readable file

 ******************************************************************************/

inline JBoolean
JXFileInput::WillRequireReadable()
	const
{
	return itsRequireReadFlag;
}

inline void
JXFileInput::ShouldRequireReadable
	(
	const JBoolean require
	)
{
	itsRequireReadFlag = require;
}

/******************************************************************************
 Require writable file

 ******************************************************************************/

inline JBoolean
JXFileInput::WillRequireWritable()
	const
{
	return itsRequireWriteFlag;
}

inline void
JXFileInput::ShouldRequireWritable
	(
	const JBoolean require
	)
{
	itsRequireWriteFlag = require;
}

/******************************************************************************
 Require executable file

 ******************************************************************************/

inline JBoolean
JXFileInput::WillRequireExecutable()
	const
{
	return itsRequireExecFlag;
}

inline void
JXFileInput::ShouldRequireExecutable
	(
	const JBoolean require
	)
{
	itsRequireExecFlag = require;
}

#endif
