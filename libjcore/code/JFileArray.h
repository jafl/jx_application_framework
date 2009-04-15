/******************************************************************************
 JFileArray.h

	Interface for the Array-in-a-File Class

	Copyright © 1993-98 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFileArray
#define _H_JFileArray

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JCollection.h>
#include <JFAIndex.h>
#include <JFAID.h>
#include <JError.h>
#include <jFStreamUtil.h>
#include <sstream>		// template
#include <string>		// template

class JString;
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

	static JError	Create(const JCharacter* fileName,
						   const JCharacter* fileSignature, JFileArray** obj,
						   const CreateAction action = kFailIfOpen);
	static JError	OKToCreateBase(const JCharacter* fileName,
								   const JCharacter* fileSignature,
								   const CreateAction action = kFailIfOpen);

	static JError	Create(JFileArray* theEnclosingFile,
						   const JFAID& enclosureElementID, JFileArray** obj);
	static JError	OKToCreateEmbedded(JFileArray* theEnclosingFile,
									   const JFAID& enclosureElementID);

	virtual ~JFileArray();

	const JString&	GetFileName() const;

	JFileVersion	GetVersion() const;
	void			SetVersion(const JFileVersion newVersion);

	void	GetElement(const JFAIndex& index, std::string* elementData) const;
	void	GetElement(const JFAID&    id,    std::string* elementData) const;

	void	SetElement(const JFAIndex& index, const JCharacter* data);
	void	SetElement(const JFAID&    id,    const JCharacter* data);

	void	InsertElementAtIndex(const JFAIndex& index, const JCharacter* data);
	void	PrependElement(const JCharacter* data);
	void	AppendElement(const JCharacter* data);

	void	SetElement(const JFAIndex& index, std::ostringstream& dataStream);
	void	SetElement(const JFAID&    id,    std::ostringstream& dataStream);

	void	InsertElementAtIndex(const JFAIndex& index, std::ostringstream& dataStream);
	void	PrependElement(std::ostringstream& dataStream);
	void	AppendElement(std::ostringstream& dataStream);

	void	RemoveElement(const JFAIndex& index);
	void	RemoveElement(const JFAID&    id);

	void	MoveElementToIndex(const JFAIndex& currentIndex, const JFAIndex& newIndex);
	void	SwapElements(const JFAIndex& index1, const JFAIndex& index2);

	JBoolean	IndexToID(const JFAIndex& index, JFAID*    id   ) const;
	JBoolean	IDToIndex(const JFAID&    id,    JFAIndex* index) const;

	JBoolean	IndexValid(const JFAIndex& index) const;
	JBoolean	IDValid(const JFAID& id) const;

	JBoolean	WillFlushChanges() const;
	void		ShouldFlushChanges(const JBoolean write);
	void		FlushChanges();

protected:

	JFileArray(const JCharacter* fileName, const JCharacter* fileSignature,
			   const CreateAction action);
	JFileArray(JFileArray* theEnclosingFile, const JFAID& enclosureElementID);

	JFileArrayIndex*	GetFileArrayIndex();

private:

	// Allowable modes for SetReadWriteMark
	//
	// We don't need relative positioning because we don't know the initial point
	// (since everyone shares the same stream) so we must always perform an
	// absolute positioning first anyway.

	enum SetMarkMode
		{
		kFromFileStart = ios::beg,
		kFromFileEnd   = ios::end
		};

private:

	JString*		itsFileName;			// name of file (for JSetFStreamLength)
	fstream*		itsStream;				// stream for accessing file
	JBoolean		itsIsOpenFlag;			// kJTrue => set high bit of element count
	JBoolean		itsFlushChangesFlag;	// kJTrue => write index after every change

	JFileVersion	itsVersion;				// version of file
	JUnsignedOffset	itsFileSignatureLength;	// space reserved at front for file signaure (string)

	JUnsignedOffset		itsIndexOffset;		// location in file where index is stored
	JFileArrayIndex*	itsFileIndex;		// index of elements in file

	JFileArray*		itsEnclosingFile;		// file enclosing us
	const JFAID		itsEnclosureElementID;	// id of our element in enclosing file

private:

	void	FileArrayX(const JBoolean isNew, const JCharacter* fileSignature);

	fstream*	OpenEmbeddedFile(JFileArray* theEmbeddedFile,
								 const JFAID& id, JBoolean* isNew);
	void	EmbeddedFileClosed(const JFAID& id);

	JSize	GetElementSize(const JFAIndex& index) const;
	void	SetElementSize(const JFAIndex& index, const JSize newSize);
	void	ExpandData(const JUnsignedOffset offset, const JSize spaceNeeded);
	void	CompactData(const JUnsignedOffset offset, const JSize blankSize);

	void	GoToElement(const JFAIndex& index) const;
	JSize	ReadElementSize() const;
	void	WriteElementSize(const JSize elementSize);

	void	ReadVersion();
	void	WriteVersion();

	void	ReadElementCount();
	void	WriteElementCount();

	void	ReadIndexOffset();
	void	WriteIndexOffset();

	void	ReadIndex(const JSize elementCount) const;
	void	WriteIndex();

	JUnsignedOffset	GetStartOfFile() const;
	JSize			GetFileLength() const;
	void			SetFileLength(const JSize newLength);
	void			ReplaceStream(fstream* newStream);

	JUnsignedOffset	GetReadWriteMark() const;
	void			SetReadWriteMark(const JSignedOffset howFar,
									 const SetMarkMode fromWhere) const;

	static unsigned long	ReadUnsignedLong(istream& input);
	static void				WriteUnsignedLong(ostream& output,
											  const unsigned long value);

	static JBoolean	FileIsOpen(ifstream& file, const JSize sigLength);

	// not allowed

	JFileArray(const JFileArray& source);
	JFileArray& operator=(const JFileArray& source);

private:

	// base class for JBroadcaster messages

	class ElementMessage : public JBroadcaster::Message
		{
		public:

			ElementMessage(const JCharacter* type, const JFAIndex& index)
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

	static const JCharacter* kElementInserted;
	static const JCharacter* kElementRemoved;
	static const JCharacter* kElementMoved;
	static const JCharacter* kElementsSwapped;
	static const JCharacter* kElementChanged;

	class ElementInserted : public ElementMessage
		{
		public:

			ElementInserted(const JFAIndex& index)
				:
				ElementMessage(kElementInserted, index)
				{ };

			void	AdjustIndex(JFAIndex* index) const;
		};

	class ElementRemoved : public ElementMessage
		{
		public:

			ElementRemoved(const JFAIndex& index)
				:
				ElementMessage(kElementRemoved, index)
				{ };

			JBoolean	AdjustIndex(JFAIndex* index) const;
		};

	class ElementMoved : public JBroadcaster::Message
		{
		public:

			ElementMoved(const JFAIndex& origIndex, const JFAIndex& newIndex)
				:
				JBroadcaster::Message(kElementMoved),
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

	class ElementsSwapped : public JBroadcaster::Message
		{
		public:

			ElementsSwapped(const JFAIndex index1, const JFAIndex index2)
				:
				JBroadcaster::Message(kElementsSwapped),
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

	class ElementChanged : public ElementMessage
		{
		public:

			ElementChanged(const JFAIndex& index)
				:
				ElementMessage(kElementChanged, index)
				{ };
		};

public:

	// JError classes

	static const JCharacter* kFileNotWritable;
	static const JCharacter* kFileAlreadyOpen;
	static const JCharacter* kWrongSignature;
	static const JCharacter* kNotEmbeddedFile;

	class FileNotWritable : public JError
		{
		public:

			FileNotWritable()
				:
				JError(kFileNotWritable)
			{ };
		};

	class FileAlreadyOpen : public JError
		{
		public:

			FileAlreadyOpen()
				:
				JError(kFileAlreadyOpen)
			{ };
		};

	class WrongSignature : public JError
		{
		public:

			WrongSignature()
				:
				JError(kWrongSignature)
			{ };
		};

	class NotEmbeddedFile : public JError
		{
		public:

			NotEmbeddedFile()
				:
				JError(kNotEmbeddedFile)
			{ };
		};
};


/******************************************************************************
 PrependElement

 ******************************************************************************/

inline void
JFileArray::PrependElement
	(
	std::ostringstream& dataStream
	)
{
	InsertElementAtIndex(1, dataStream);
}

inline void
JFileArray::PrependElement
	(
	const JCharacter* data
	)
{
	InsertElementAtIndex(1, data);
}

/******************************************************************************
 AppendElement

 ******************************************************************************/

inline void
JFileArray::AppendElement
	(
	std::ostringstream& dataStream
	)
{
	InsertElementAtIndex(GetElementCount() + 1, dataStream);
}

inline void
JFileArray::AppendElement
	(
	const JCharacter* data
	)
{
	InsertElementAtIndex(GetElementCount() + 1, data);
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

inline JBoolean
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

inline JBoolean
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

inline JBoolean
JFileArray::WillFlushChanges()
	const
{
	return itsFlushChangesFlag;
}

inline void
JFileArray::ShouldFlushChanges
	(
	const JBoolean write
	)
{
	itsFlushChangesFlag = write;
}

#endif
