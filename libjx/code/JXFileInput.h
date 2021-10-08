/******************************************************************************
 JXFileInput.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFileInput
#define _H_JXFileInput

#include "jx-af/jx/JXFSInputBase.h"

class JXFileInput : public JXFSInputBase
{
public:

	JXFileInput(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	~JXFileInput() override;

	bool				InputValid() override;
	virtual bool		GetFile(JString* fullName) const;
	virtual JString		GetTextForChooseFile() const;

	bool	WillAllowInvalidFile() const;
	void	ShouldAllowInvalidFile(const bool allow = true);

	bool	WillRequireReadable() const;
	void	ShouldRequireReadable(const bool require = true);

	bool	WillRequireWritable() const;
	void	ShouldRequireWritable(const bool require = true);

	bool	WillRequireExecutable() const;
	void	ShouldRequireExecutable(const bool require = true);

	bool	ChooseFile(const JString& prompt, const JString& instr = JString::empty);
	bool	SaveFile(const JString& prompt, const JString& instr = JString::empty);

	static JColorID		GetTextColor(const JString& fileName, const JString& basePath,
									 const bool requireRead, const bool requireWrite,
									 const bool requireExec);

protected:

	class StyledText : public JXFSInputBase::StyledText
	{
		public:

		StyledText(JXFileInput* field, JFontManager* fontManager)
			:
			JXFSInputBase::StyledText(field, fontManager)
		{ };

		protected:

		JSize	ComputeErrorLength(JXFSInputBase* field,
								   const JSize totalLength,
								   const JString& fullName) const override;
	};

protected:

	JXFileInput(StyledText* text, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

private:

	bool	itsAllowInvalidFileFlag;
	bool	itsRequireReadFlag;
	bool	itsRequireWriteFlag;
	bool	itsRequireExecFlag;

private:

	void JXFileInputX();
};


/******************************************************************************
 Allow invalid file

 ******************************************************************************/

inline bool
JXFileInput::WillAllowInvalidFile()
	const
{
	return itsAllowInvalidFileFlag;
}

inline void
JXFileInput::ShouldAllowInvalidFile
	(
	const bool allow
	)
{
	itsAllowInvalidFileFlag = allow;
}

/******************************************************************************
 Require readable file

 ******************************************************************************/

inline bool
JXFileInput::WillRequireReadable()
	const
{
	return itsRequireReadFlag;
}

inline void
JXFileInput::ShouldRequireReadable
	(
	const bool require
	)
{
	itsRequireReadFlag = require;
}

/******************************************************************************
 Require writable file

 ******************************************************************************/

inline bool
JXFileInput::WillRequireWritable()
	const
{
	return itsRequireWriteFlag;
}

inline void
JXFileInput::ShouldRequireWritable
	(
	const bool require
	)
{
	itsRequireWriteFlag = require;
}

/******************************************************************************
 Require executable file

 ******************************************************************************/

inline bool
JXFileInput::WillRequireExecutable()
	const
{
	return itsRequireExecFlag;
}

inline void
JXFileInput::ShouldRequireExecutable
	(
	const bool require
	)
{
	itsRequireExecFlag = require;
}

#endif
