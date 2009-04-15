/******************************************************************************
 JPrefsFile.h

	Copyright © 1995-2001 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPrefsFile
#define _H_JPrefsFile

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFileArray.h>

typedef JFAID	JPrefID;

class JPrefsFile : public JFileArray
{
public:

	static JError	Create(const JCharacter* fileNameStem, JPrefsFile** obj,
						   const CreateAction action = kFailIfOpen);
	static JError	OKToCreate(const JCharacter* fileNameStem, JString* fullName,
							   const CreateAction action = kFailIfOpen);

	virtual ~JPrefsFile();

	void	GetData(const JPrefID& id, std::string* data) const;

	void	SetData(const JPrefID& id, std::ostringstream& data);
	void	SetData(const JPrefID& id, const std::string& data);
	void	SetData(const JPrefID& id, const JCharacter* data);

	void	RemoveData(const JPrefID& id);

	static JError	GetFullName(const JCharacter* fileNameStem,
								JString* fullName);

protected:

	JPrefsFile(const JCharacter* fileName, const CreateAction action);

private:

	// not allowed

	JPrefsFile(const JPrefsFile& source);
	JPrefsFile& operator=(const JPrefsFile& source);

public:

	// JError classes

	static const JCharacter* kNoHomeDirectory;

	class NoHomeDirectory : public JError
		{
		public:

			NoHomeDirectory()
				:
				JError(kNoHomeDirectory)
			{ };
		};
};


/******************************************************************************
 GetData

 ******************************************************************************/

inline void
JPrefsFile::GetData
	(
	const JPrefID&	id,
	std::string*	data
	)
	const
{
	GetElement(id, data);
}

/******************************************************************************
 RemoveData

 ******************************************************************************/

inline void
JPrefsFile::RemoveData
	(
	const JPrefID& id
	)
{
	if (IDValid(id))
		{
		RemoveElement(id);
		}
}

#endif
