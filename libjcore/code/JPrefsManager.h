/******************************************************************************
 JPrefsManager.h

	Interface for the JPrefsManager class

	Copyright © 1997 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPrefsManager
#define _H_JPrefsManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JContainer.h>
#include <JPrefsFile.h>		// need definition of JPrefID
#include <JArray.h>
#include <JError.h>

class JString;
class JPrefsFile;
class JPrefObject;

class JPrefsManager : public JContainer
{
	friend class JPrefObject;

public:

	JPrefsManager(const JCharacter* fileName, const JFileVersion currentVersion,
				  const JBoolean eraseFileIfOpen);

	virtual ~JPrefsManager();

	JError	SaveToDisk() const;

	JFileVersion	GetPrevPrefsVersion() const;

protected:

	JBoolean	IDValid(const JPrefID& id) const;
	JBoolean	GetData(const JPrefID& id, std::string* data) const;

	void	SetData(const JPrefID& id, std::ostringstream& data);
	void	SetData(const JPrefID& id, std::string& data);
	void	SetData(const JPrefID& id, const JCharacter* data);

	void	RemoveData(const JPrefID& id);

	JBoolean		UpgradeData(const JBoolean reportError = kJTrue);
	virtual void	UpgradeData(const JBoolean isNew, const JFileVersion currentVersion) = 0;

private:

	struct PrefItem
	{
		JIndex		id;
		JString*	data;

		PrefItem()
			:
			id(0), data(NULL)
		{ };

		PrefItem(const JIndex i, JString* d)
			:
			id(i), data(d)
		{ };
	};

private:

	JString*			itsFileName;
	const JFileVersion	itsCurrentFileVersion;
	const JBoolean		itsEraseFileIfOpenFlag;		// kJTrue => delete file if somebody left it open
	JFileVersion		itsPrevFileVersion;

	JArray<PrefItem>*	itsData;

private:

	JError	Open(JPrefsFile** file, const JBoolean allowPrevVers = kJFalse) const;
	void	LoadData(JPrefsFile* file);
	JError	DeletePrefsFile(const JCharacter* fileName) const;

	static JOrderedSetT::CompareResult
		ComparePrefIDs(const PrefItem& p1, const PrefItem& p2);

	// not allowed

	JPrefsManager(const JPrefsManager& source);
	JPrefsManager& operator=(const JPrefsManager& source);

protected:

	// base class for JBroadcaster messages

	class DataMessage : public JBroadcaster::Message
		{
		public:

			DataMessage(const JCharacter* type, const JPrefID& id)
				:
				JBroadcaster::Message(type),
				itsID(id.GetID())
			{ };

			JIndex
			GetID() const
			{
				return itsID;
			};

		private:

			JIndex itsID;
		};

public:

	// JBroadcaster messages

	static const JCharacter* kDataChanged;
	static const JCharacter* kDataRemoved;

	class DataChanged : public DataMessage
		{
		public:

			DataChanged(const JPrefID& id)
				:
				DataMessage(kDataChanged, id)
			{ };
		};

	class DataRemoved : public DataMessage
		{
		public:

			DataRemoved(const JPrefID& id)
				:
				DataMessage(kDataRemoved, id)
			{ };
		};

public:

	// JError classes

	static const JCharacter* kWrongVersion;

	class WrongVersion : public JError
		{
		public:

			WrongVersion()
				:
				JError(kWrongVersion)
			{ };
		};
};


/******************************************************************************
 GetPrevPrefsVersion

 ******************************************************************************/

inline JFileVersion
JPrefsManager::GetPrevPrefsVersion()
	const
{
	return itsPrevFileVersion;
}

/******************************************************************************
 IDValid (protected)

 ******************************************************************************/

inline JBoolean
JPrefsManager::IDValid
	(
	const JPrefID& id
	)
	const
{
	PrefItem target(id.GetID(), (JString*) NULL);
	JIndex index;
	return itsData->SearchSorted(target, JOrderedSetT::kAnyMatch, &index);
}

/******************************************************************************
 SetData (protected)

	This creates the item if it doesn't already exist.

 ******************************************************************************/

inline void
JPrefsManager::SetData
	(
	const JPrefID&		id,
	std::ostringstream&	data
	)
{
	const std::string s = data.str();
	SetData(id, s.c_str());
}

inline void
JPrefsManager::SetData
	(
	const JPrefID&	id,
	std::string&	data
	)
{
	SetData(id, data.c_str());
}

#endif
