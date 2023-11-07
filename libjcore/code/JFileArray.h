/******************************************************************************
 JFileArray.h

	Interface for the Array-in-a-File Class

	Copyright (C) 1993-98 John Lindal.

 ******************************************************************************/

#ifndef _H_JFileArray
#define _H_JFileArray

#include "JCollection.h"
#include "JFAIndex.h"
#include "JFAID.h"
#include "JError.h"
#include "jFStreamUtil.h"
#include <sstream>		// template
#include <string>		// template

class JFileArrayIndex;

class JFileArray : public JCollection
{
	friend class JFileArrayIndex;	// needs access to ReplaceStream

public:

	enum CreateAction
	{
		kFailIfOpen,
		kIgnoreIfOpen,			// read the warnings!
		kDeleteIfOpen,			// returns error from JRemoveFile()
		kTryWaitUntilCanOpen,	// fails if timeout
		kDeleteIfWaitTimeout	// returns error from JRemoveFile()
	};

	static const JFileVersion kInitialVersion;	// = 0

	enum
	{
		kUnsignedLongLength = 4
	};

public:

	static JError	Create(const JString& fileName,
						   const JUtf8Byte* fileSignature, JFileArray** obj,
						   const CreateAction action = kFailIfOpen);
	static JError	OKToCreateBase(const JString& fileName,
								   const JUtf8Byte* fileSignature,
								   const CreateAction action = kFailIfOpen);

	static JError	Create(JFileArray* theEnclosingFile,
						   const JFAID& enclosureItemID, JFileArray** obj);
	static JError	OKToCreateEmbedded(JFileArray* theEnclosingFile,
									   const JFAID& enclosureItemID);

	~JFileArray() override;

	const JString&	GetFileName() const;

	JFileVersion	GetVersion() const;
	void			SetVersion(const JFileVersion newVersion);

	void	GetItem(const JFAIndex& index, std::string* data) const;
	void	GetItem(const JFAID&    id,    std::string* data) const;

	void	SetItem(const JFAIndex& index, const JString& data);
	void	SetItem(const JFAID&    id,    const JString& data);

	void	InsertItemAtIndex(const JFAIndex& index, const JString& data);
	void	PrependItem(const JString& data);
	void	AppendItem(const JString& data);

	void	SetItem(const JFAIndex& index, std::ostringstream& dataStream);
	void	SetItem(const JFAID&    id,    std::ostringstream& dataStream);

	void	InsertItemAtIndex(const JFAIndex& index, std::ostringstream& dataStream);
	void	PrependItem(std::ostringstream& dataStream);
	void	AppendItem(std::ostringstream& dataStream);

	void	RemoveItem(const JFAIndex& index);
	void	RemoveItem(const JFAID&    id);

	void	MoveItemToIndex(const JFAIndex& currentIndex, const JFAIndex& newIndex);
	void	SwapItems(const JFAIndex& index1, const JFAIndex& index2);

	bool	IndexToID(const JFAIndex& index, JFAID*    id   ) const;
	bool	IDToIndex(const JFAID&    id,    JFAIndex* index) const;

	bool	IndexValid(const JFAIndex& index) const;
	bool	IDValid(const JFAID& id) const;

	bool	WillFlushChanges() const;
	void	ShouldFlushChanges(const bool write);
	void	FlushChanges();

protected:

	JFileArray(const JString& fileName, const JUtf8Byte* fileSignature,
			   const CreateAction action);
	JFileArray(JFileArray* theEnclosingFile, const JFAID& enclosureItemID);

	JFileArrayIndex*	GetFileArrayIndex();

private:

	// Allowable modes for SetReadWriteMark
	//
	// We don't need relative positioning because we don't know the initial point
	// (since everyone shares the same stream) so we must always perform an
	// absolute positioning first anyway.

	enum SetMarkMode
	{
		kFromFileStart = std::ios::beg,
		kFromFileEnd   = std::ios::end
	};

private:

	JString*		itsFileName;				// name of file (for JSetFStreamLength) - nullptr if embedded
	std::fstream*	itsStream;					// stream for accessing file
	bool			itsIsOpenFlag;				// true => set high bit of item count
	bool			itsFlushChangesFlag;		// true => write index after every change

	JFileVersion	itsVersion;					// version of file
	JUnsignedOffset	itsFileSignatureByteCount;	// space reserved at front for file signaure (string)

	JUnsignedOffset		itsIndexOffset;			// location in file where index is stored
	JFileArrayIndex*	itsFileIndex;			// index of items in file

	JFileArray*		itsEnclosingFile;			// file enclosing us
	const JFAID		itsEnclosureItemID;			// id of our item in enclosing file

private:

	void	FileArrayX(const bool isNew, const JUtf8Byte* fileSignature);

	std::fstream*	OpenEmbeddedFile(JFileArray* theEmbeddedFile,
								 const JFAID& id, bool* isNew);
	void	EmbeddedFileClosed(const JFAID& id);

	JSize	GetItemSize(const JFAIndex& index) const;
	void	SetItemSize(const JFAIndex& index, const JSize newSize);
	void	ExpandData(const JUnsignedOffset offset, const JSize spaceNeeded);
	void	CompactData(const JUnsignedOffset offset, const JSize blankSize);

	void	GoToItem(const JFAIndex& index) const;
	JSize	ReadItemSize() const;
	void	WriteItemSize(const JSize itemSize);

	void	ReadVersion();
	void	WriteVersion();

	void	ReadItemCount();
	void	WriteItemCount();

	void	ReadIndexOffset();
	void	WriteIndexOffset();

	void	ReadIndex(const JSize itemCount) const;
	void	WriteIndex();

	JUnsignedOffset	GetStartOfFile() const;
	JSize			GetFileLength() const;
	void			SetFileLength(const JSize newLength);
	void			ReplaceStream(std::fstream* newStream);

	JUnsignedOffset	GetReadWriteMark() const;
	void			SetReadWriteMark(const JSignedOffset howFar,
									 const SetMarkMode fromWhere) const;

	static unsigned long	ReadUnsignedLong(std::istream& input);
	static void				WriteUnsignedLong(std::ostream& output,
											  const unsigned long value);

	static bool	FileIsOpen(std::ifstream& file, const JSize sigLength);

	// not allowed

	JFileArray(const JFileArray& source);
	JFileArray& operator=(const JFileArray& source);

private:

	// base class for JBroadcaster messages

	class ItemMessage : public JBroadcaster::Message
	{
	public:

		ItemMessage(const JUtf8Byte* type, const JFAIndex& index)
			:
			JBroadcaster::Message(type),
			itsIndex(index)
			{ };

		JFAIndex
		GetIndex() const
		{
			return itsIndex;
		};

	private:

		JFAIndex	itsIndex;
	};

public:

	// JBroadcaster messages

	static const JUtf8Byte* kItemInserted;
	static const JUtf8Byte* kItemRemoved;
	static const JUtf8Byte* kItemMoved;
	static const JUtf8Byte* kItemsSwapped;
	static const JUtf8Byte* kItemChanged;

	class ItemInserted : public ItemMessage
	{
	public:

		ItemInserted(const JFAIndex& index)
			:
			ItemMessage(kItemInserted, index)
			{ };

		void	AdjustIndex(JFAIndex* index) const;
	};

	class ItemRemoved : public ItemMessage
	{
	public:

		ItemRemoved(const JFAIndex& index)
			:
			ItemMessage(kItemRemoved, index)
			{ };

		bool	AdjustIndex(JFAIndex* index) const;
	};

	class ItemMoved : public JBroadcaster::Message
	{
	public:

		ItemMoved(const JFAIndex& origIndex, const JFAIndex& newIndex)
			:
			JBroadcaster::Message(kItemMoved),
			itsOrigIndex(origIndex),
			itsNewIndex(newIndex)
			{ };

		void	AdjustIndex(JFAIndex* index) const;

		JFAIndex
		GetOrigIndex() const
		{
			return itsOrigIndex;
		};

		JFAIndex
		GetNewIndex() const
		{
			return itsNewIndex;
		};

	private:

		JFAIndex	itsOrigIndex;
		JFAIndex	itsNewIndex;
	};

	class ItemsSwapped : public JBroadcaster::Message
	{
	public:

		ItemsSwapped(const JFAIndex index1, const JFAIndex index2)
			:
			JBroadcaster::Message(kItemsSwapped),
			itsIndex1(index1),
			itsIndex2(index2)
			{ };

		void	AdjustIndex(JFAIndex* index) const;

		JFAIndex
		GetIndex1() const
		{
			return itsIndex1;
		};

		JFAIndex
		GetIndex2() const
		{
			return itsIndex2;
		};

	private:

		JFAIndex	itsIndex1;
		JFAIndex	itsIndex2;
	};

	class ItemChanged : public ItemMessage
	{
	public:

		ItemChanged(const JFAIndex& index)
			:
			ItemMessage(kItemChanged, index)
			{ };
	};

public:

	// JError classes

	static const JUtf8Byte* kFileNotWritable;
	static const JUtf8Byte* kFileAlreadyOpen;
	static const JUtf8Byte* kWrongSignature;
	static const JUtf8Byte* kNotEmbeddedFile;

	class FileNotWritable : public JError
		{
		public:

			FileNotWritable(const JString& fileName);
		};

	class FileAlreadyOpen : public JError
		{
		public:

			FileAlreadyOpen(const JString& fileName);
		};

	class WrongSignature : public JError
		{
		public:

			WrongSignature(const JString& fileName);
		};

	class NotEmbeddedFile : public JError
		{
		public:

			NotEmbeddedFile(const JIndex index);
		};
};


/******************************************************************************
 PrependItem

 ******************************************************************************/

inline void
JFileArray::PrependItem
	(
	std::ostringstream& dataStream
	)
{
	InsertItemAtIndex(1, dataStream);
}

inline void
JFileArray::PrependItem
	(
	const JString& data
	)
{
	InsertItemAtIndex(1, data);
}

/******************************************************************************
 AppendItem

 ******************************************************************************/

inline void
JFileArray::AppendItem
	(
	std::ostringstream& dataStream
	)
{
	InsertItemAtIndex(GetItemCount() + 1, dataStream);
}

inline void
JFileArray::AppendItem
	(
	const JString& data
	)
{
	InsertItemAtIndex(GetItemCount() + 1, data);
}

/******************************************************************************
 Version

 ******************************************************************************/

inline JFileVersion
JFileArray::GetVersion()
	const
{
	return itsVersion;
}

inline void
JFileArray::SetVersion
	(
	const JFileVersion newVersion
	)
{
	itsVersion = newVersion;

	// make sure all the data in the file is correct

	WriteVersion();
}

/******************************************************************************
 GetFileArrayIndex (protected)

 ******************************************************************************/

inline JFileArrayIndex*
JFileArray::GetFileArrayIndex()
{
	return itsFileIndex;
}

/******************************************************************************
 IndexValid

 ******************************************************************************/

inline bool
JFileArray::IndexValid
	(
	const JFAIndex& index
	)
	const
{
	return JCollection::IndexValid(index.GetIndex());
}

/******************************************************************************
 IDValid

 ******************************************************************************/

inline bool
JFileArray::IDValid
	(
	const JFAID& id
	)
	const
{
	JFAIndex index;
	return IDToIndex(id, &index);
}

/******************************************************************************
 Flushing changes

	When this flag is set, the file's index will be written out after
	every change.  The default is not to do this, since it slows us
	down dramatically.  If you need to protect against data loss due to
	crashes, you should turn this option on.

 ******************************************************************************/

inline bool
JFileArray::WillFlushChanges()
	const
{
	return itsFlushChangesFlag;
}

inline void
JFileArray::ShouldFlushChanges
	(
	const bool write
	)
{
	itsFlushChangesFlag = write;
}

#endif
