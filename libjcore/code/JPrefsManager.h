/******************************************************************************
 JPrefsManager.h

	Interface for the JPrefsManager class

	Copyright (C) 1997 John Lindal.

 ******************************************************************************/

#ifndef _H_JPrefsManager
#define _H_JPrefsManager

#include "jx-af/jcore/JContainer.h"
#include "jx-af/jcore/JPrefsFile.h"		// need definition of JPrefID
#include "jx-af/jcore/JArray.h"
#include "jx-af/jcore/JError.h"

class JString;
class JPrefsFile;
class JPrefObject;

class JPrefsManager : public JContainer
{
	friend class JPrefObject;

public:

	JPrefsManager(const JString& fileName, const JFileVersion currentVersion,
				  const bool eraseFileIfOpen);

	virtual ~JPrefsManager();

	JError	SaveToDisk() const;

	JFileVersion	GetPrevPrefsVersion() const;

protected:

	bool	IDValid(const JPrefID& id) const;
	bool	GetData(const JPrefID& id, std::string* data) const;

	void	SetData(const JPrefID& id, std::ostringstream& data);
	void	SetData(const JPrefID& id, std::string& data);
	void	SetData(const JPrefID& id, const JString& data);
	void	SetData(const JPrefID& id, const JUtf8Byte* data);

	void	RemoveData(const JPrefID& id);

	bool			UpgradeData(const bool reportError = true);
	virtual void	UpgradeData(const bool isNew, const JFileVersion currentVersion) = 0;

private:

	struct PrefItem
	{
		JIndex		id;
		JString*	data;

		PrefItem()
			:
			id(0), data(nullptr)
		{ };

		PrefItem(const JIndex i, JString* d)
			:
			id(i), data(d)
		{ };
	};

private:

	JString*			itsFileName;
	const JFileVersion	itsCurrentFileVersion;
	const bool			itsEraseFileIfOpenFlag;		// true => delete file if somebody left it open
	JFileVersion		itsPrevFileVersion;

	JArray<PrefItem>*	itsData;

private:

	JError	Open(JPrefsFile** file, const bool allowPrevVers = false) const;
	void	LoadData(JPrefsFile* file);
	JError	DeletePrefsFile(const JString& fileName) const;

	static JListT::CompareResult
		ComparePrefIDs(const PrefItem& p1, const PrefItem& p2);

	// not allowed

	JPrefsManager(const JPrefsManager&) = delete;
	JPrefsManager& operator=(const JPrefsManager&) = delete;

protected:

	// base class for JBroadcaster messages

	class DataMessage : public JBroadcaster::Message
		{
		public:

			DataMessage(const JUtf8Byte* type, const JPrefID& id)
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

	static const JUtf8Byte* kDataChanged;
	static const JUtf8Byte* kDataRemoved;

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

	static const JUtf8Byte* kWrongVersion;

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

inline bool
JPrefsManager::IDValid
	(
	const JPrefID& id
	)
	const
{
	PrefItem target(id.GetID(), (JString*) nullptr);
	JIndex index;
	return itsData->SearchSorted(target, JListT::kAnyMatch, &index);
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
