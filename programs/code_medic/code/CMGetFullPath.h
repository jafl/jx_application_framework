/******************************************************************************
 CMGetFullPath.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CMGetFullPath
#define _H_CMGetFullPath

#include "CMCommand.h"
#include <JBroadcaster.h>

class CMGetFullPath : public CMCommand, virtual public JBroadcaster
{
public:

	CMGetFullPath(const JString& cmd,
				  const JString& fileName, const JIndex lineIndex);

	virtual	~CMGetFullPath();

	const JString&	GetFileName() const;
	JIndex			GetLineIndex() const;

private:

	const JString	itsFileName;
	const JIndex	itsLineIndex;

private:

	// not allowed

	CMGetFullPath(const CMGetFullPath& source);
	const CMGetFullPath& operator=(const CMGetFullPath& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kFileFound;
	static const JUtf8Byte* kFileNotFound;
	static const JUtf8Byte* kNewCommand;

	class FileFound : public JBroadcaster::Message
		{
		public:

			FileFound(const JString& fullName, const JIndex lineIndex)
				:
				JBroadcaster::Message(kFileFound),
				itsFullName(fullName), itsLineIndex(lineIndex)
				{ };

			const JString&
			GetFullName() const
			{
				return itsFullName;
			}

			JIndex
			GetLineIndex() const
			{
				return itsLineIndex;
			}

		private:

			const JString&	itsFullName;
			const JIndex	itsLineIndex;
		};

	class FileNotFound : public JBroadcaster::Message
		{
		public:

			FileNotFound(const JString& fileName)
				:
				JBroadcaster::Message(kFileNotFound),
				itsFileName(fileName)
				{ };

			const JString&
			GetFileName() const
			{
				return itsFileName;
			}

		private:

			const JString& itsFileName;
		};

	class NewCommand : public JBroadcaster::Message
		{
		public:

			NewCommand(CMGetFullPath* cmd)
				:
				JBroadcaster::Message(kNewCommand),
				itsCmd(cmd)
				{ };

			CMGetFullPath*
			GetNewCommand() const
			{
				return itsCmd;
			}

		private:

			CMGetFullPath*	itsCmd;
		};
};


/******************************************************************************
 GetFileName

 ******************************************************************************/

inline const JString&
CMGetFullPath::GetFileName()
	const
{
	return itsFileName;
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

inline JIndex
CMGetFullPath::GetLineIndex()
	const
{
	return itsLineIndex;
}

#endif
