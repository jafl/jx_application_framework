/******************************************************************************
 JPrefsFile.h

	Copyright (C) 1995-2001 John Lindal.

 ******************************************************************************/

#ifndef _H_JPrefsFile
#define _H_JPrefsFile

#include "JFileArray.h"

using JPrefID = JFAID;

class JPrefsFile : public JFileArray
{
public:

	static JError	Create(const JString& fileNameStem, JPrefsFile** obj,
						   const CreateAction action = kFailIfOpen);
	static JError	OKToCreate(const JString& fileNameStem, JString* fullName,
							   const CreateAction action = kFailIfOpen);

	~JPrefsFile() override;

	void	GetData(const JPrefID& id, std::string* data) const;

	void	SetData(const JPrefID& id, std::ostringstream& data);
	void	SetData(const JPrefID& id, const std::string& data);
	void	SetData(const JPrefID& id, const JString& data);

	void	RemoveData(const JPrefID& id);

	static JError	GetFullName(const JString& fileNameStem,
								JString* fullName);

protected:

	JPrefsFile(const JString& fileName, const CreateAction action);

private:

	// not allowed

	JPrefsFile(const JPrefsFile& source);
	JPrefsFile& operator=(const JPrefsFile& source);

public:

	// JError classes

	static const JUtf8Byte* kNoHomeDirectory;

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
	GetItem(id, data);
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
		RemoveItem(id);
		}
}

#endif
