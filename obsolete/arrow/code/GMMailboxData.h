/******************************************************************************
 GMMailboxData.h

	Interface for the GMMailboxData class

	Copyright (C) 2003 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GMMailboxData
#define _H_GMMailboxData

#include <JBroadcaster.h>
#include <JPtrArray.h>
#include <JString.h>

#include <GMessageHeader.h>

class JProgressDisplay;
class JDirEntry;

class GMessageDataUpdateTask;
class GMessageFrom;
class GMessageHeaderList;
class GMessageTable;

class GMMailboxData : public JBroadcaster
{
public:

	enum kSortType
		{
		kNone	= 1,
		kFrom,
		kSubject,
		kDate,
		kSize
		};

public:

	GMMailboxData(const JCharacter* mailfile, GMessageHeaderList* list, JProgressDisplay* pg = NULL);
	GMMailboxData(GMMailboxData* parent, GMessageHeaderList* list);
	
	virtual ~GMMailboxData();

	const JString&	GetMailFile();

	JSize			GetHeaderCount() const;
	GMessageHeader*	GetHeader(const JIndex index);

	JBoolean		IsPostponingUpdate() const;
	void			ShouldBePostponingUpdate(const JBoolean postpone);

	JBoolean		Includes(const GMessageHeader* header, JIndex* index = NULL);

	JBoolean		Update(const JBoolean unlock = kJTrue, const JBoolean alreadyLocked = kJFalse);
	void			Save();
	void			Flush();

	void			HandleMessageTransfer(const JCharacter* dest, const JBoolean move, GMessageHeaderList* list);
	void			SaveSelectedMessages(std::ostream& os, const JBoolean headers, GMessageHeaderList* list);

	void			DropHeader(GMessageHeader* header, JIndex* index);
	void			DropHeaders(const JCharacter* srcMBox, const JIndex afterIndex, GMessageHeaderList* headers);

	void			ChangeMessageStatus(GMessageHeader* header, const GMessageHeader::MessageStatus status);

	JBoolean		NeedsSave() const;
	JBoolean		HasNewMail() const;
	void			ShouldHaveNewMail(const JBoolean newMail);

	void			BroadcastDataModified();
	void			BroadcastDataReverted();

	const JDirEntry&	GetEntry() const;

	kSortType		GetSortType() const;
	JBoolean		SortIsAscending() const;
	void			ToggleSortType(const kSortType type);

	JBoolean		CanReorganize() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JString						itsMailFile;
	JPtrArray<GMessageFrom>*	itsFromList;
	GMessageHeaderList*			itsList;
	GMessageHeaderList*			itsListByDate;
	GMessageHeaderList*			itsListBySize;
	GMessageHeaderList*			itsListBySubject;
	GMessageHeaderList*			itsListByFrom;
	JDirEntry*					itsEntry;
	GMessageDataUpdateTask*		itsIdleTask;
	JBoolean					itsNeedsUpdate;
	JBoolean					itsNeedsSave;
	JString						itsInternalMessage;
	JBoolean					itsHasNewMail;
	JBoolean					itsPostponeUpdate;
	JProgressDisplay*			itsPG;
	GMMailboxData*				itsParent;
	kSortType					itsSortType;
	JBoolean					itsSortIsAscending;

private:

	void		GMMailboxDataX();

	void		AdjustList(GMessageHeaderList* list);
	void		SaveHeader(std::istream&	is, std::ostream& os, GMessageHeader* header, const JBoolean headers);
	void		GenerateFromList();

	void		RemoveHeader(const GMessageHeader* header);
	void		RemoveHeaderAtIndex(const JIndex index);
	void		HeaderAppended(GMessageHeader* header);

	void		SyncLists();


	GMMailboxData(const GMMailboxData& source);
	const GMMailboxData& operator=(const GMMailboxData& source);

public:

	static const JCharacter* kStartingUpdate;
	static const JCharacter* kUpdateFinished;
	static const JCharacter* kNewMail;
	static const JCharacter* kMBoxState;
	static const JCharacter* kHeaderRemoved;
	static const JCharacter* kHeaderAdded;
	static const JCharacter* kDataModified;
	static const JCharacter* kDataReverted;
	static const JCharacter* kHeaderMarkedDeleted;
	static const JCharacter* kSortChanged;
	static const JCharacter* kHeaderStatusChanged;

	class StartingUpdate : public JBroadcaster::Message
		{
		public:

			StartingUpdate()
				:
				JBroadcaster::Message(kStartingUpdate)
				{ };
		};

	class UpdateFinished : public JBroadcaster::Message
		{
		public:

			UpdateFinished()
				:
				JBroadcaster::Message(kUpdateFinished)
				{ };
		};

	class NewMail : public JBroadcaster::Message
		{
		public:

			NewMail(const JBoolean modified)
				:
				JBroadcaster::Message(kNewMail),
				itsIsModified(modified)
				{ };

			JBoolean
			IsModified()
				const
				{
				return itsIsModified;
				};

		private:

			JBoolean	itsIsModified;
		};


	class MBoxState : public JBroadcaster::Message
		{
		public:

			MBoxState(const JBoolean exists)
				:
				JBroadcaster::Message(kMBoxState),
				itsExists(exists)
				{ };

			JBoolean
			Exists()
				const
				{
				return itsExists;
				};
				
		private:

			JBoolean	itsExists;
		};


	class HeaderRemoved : public JBroadcaster::Message
		{
		public:

			HeaderRemoved(const GMessageHeader* header)
				:
				JBroadcaster::Message(kHeaderRemoved),
				itsHeader(header)
				{ };

			const GMessageHeader*
			GetHeader()
				const
				{
				return itsHeader;
				};

		private:

			const GMessageHeader* itsHeader;
		};

	class HeaderAdded : public JBroadcaster::Message
		{
		public:

			HeaderAdded(const GMessageHeader* header)
				:
				JBroadcaster::Message(kHeaderAdded),
				itsHeader(header)
				{ };

			const GMessageHeader*
			GetHeader()
				const
				{
				return itsHeader;
				};

		private:

			const GMessageHeader* itsHeader;
		};


	class DataModified : public JBroadcaster::Message
		{
		public:

			DataModified()
				:
				JBroadcaster::Message(kDataModified)
				{ };
		};
		

	class DataReverted : public JBroadcaster::Message
		{
		public:

			DataReverted()
				:
				JBroadcaster::Message(kDataReverted)
				{ };
		};

	class HeaderMarkedDeleted : public JBroadcaster::Message
		{
		public:

			HeaderMarkedDeleted(const GMessageHeader* header)
				:
				JBroadcaster::Message(kHeaderMarkedDeleted),
				itsHeader(header)
				{ };

			const GMessageHeader*
			GetHeader()
				const
				{
				return itsHeader;
				};

		private:

			const GMessageHeader* itsHeader;
		};

	class HeaderStatusChanged : public JBroadcaster::Message
		{
		public:

			HeaderStatusChanged
				(
				const GMessageHeader* header, 
				const GMessageHeader::MessageStatus	status
				)
				:
				JBroadcaster::Message(kHeaderStatusChanged),
				itsHeader(header),
				itsStatus(status)
				{ };

			const GMessageHeader*
			GetHeader()
				const
				{
				return itsHeader;
				};

			GMessageHeader::MessageStatus
			GetStatus()
				const
				{
				return itsStatus;
				};

		private:

			const GMessageHeader*			itsHeader;
			GMessageHeader::MessageStatus	itsStatus;
		};	

	class SortChanged : public JBroadcaster::Message
		{
		public:

			SortChanged()
				:
				JBroadcaster::Message(kSortChanged)
				{ };
		};
};

#endif
