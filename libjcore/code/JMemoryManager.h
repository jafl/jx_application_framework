#ifndef _H_JMemoryManager
#define _H_JMemoryManager

/******************************************************************************
 JMemoryManager.h

	Interface for the JMemoryManager class.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

// Superclass header
#include <JMMRecord.h>

#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Stream.h>
#include <JMessageProtocol.h>	// template; requires ace includes
#include <sstream>

#include <stdlib.h> // For size_t

	class JMMTable;
	class JMMErrorPrinter;
	class JMMDebugErrorStream;

const JFileVersion kJMemoryManagerDebugVersion = 0;

class JMemoryManager : virtual public JBroadcaster
{
	friend void JLocateDelete(const JCharacter* file, const JUInt32 line);
	friend void JMMHandleExit();
	friend void JMMHandleACEExit(void*, void*);
	friend class JMMTable;

public:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	DebugLink;

	enum
	{
		kRunningStatsMessage,
		kExitStatsMessage,
		kErrorMessage,
		kRecordsMessage
	};

	enum
	{
		kHistogramSlotCount = 30
	};

	struct RecordFilter
	{
		JBoolean	includeInternal;
		JSize		minSize;
		JString*	fileName;

		RecordFilter()
			:
			includeInternal(kJFalse), minSize(0), fileName(NULL)
			{ };

		~RecordFilter()
		{
			jdelete fileName;
		};

		JBoolean	Match(const JMMRecord& record) const;

		void	Read(istream& input);
		void	Write(ostream& output) const;
	};

	static const JCharacter* kUnknownFile;

public:

	virtual ~JMemoryManager();

	static JMemoryManager* Instance();

	static void* New(const size_t size, const JCharacter* file,
					 const JUInt32 line,  const JBoolean isArray);

	void  Delete(void* memory, JBoolean isArray);

// Record keeping

	JBoolean RecordingAllocated() const;
//	JBoolean RecordingDeallocated() const;

	void CancelRecordAllocated();
	void CancelRecordDeallocated();

// Allocation

//	JBoolean Initializing() const;
//	void CancelInitialize();

//	JBoolean Shredding() const;
//	void CancelShred();

	unsigned char GetAllocateGarbage() const;
	unsigned char GetDeallocateGarbage() const;

	// This function doesn't mean anything--yet.  Stay tuned
	size_t GetTagSize() const;

// Statistics reporting

	void PrintMemoryStats();

	JBoolean GetPrintExitStats() const;
	void     SetPrintExitStats(const JBoolean yesNo);

	JBoolean GetPrintInternalStats() const;
	void     SetPrintInternalStats(const JBoolean yesNo);

	void PrintAllocated() const;

// Error notification

	JBoolean GetBroadcastErrors() const;
	void     SetBroadcastErrors(const JBoolean broadcast);

	JBoolean GetCheckDoubleAllocation() const;
	void     SetCheckDoubleAllocation(const JBoolean yesNo);

	JBoolean GetDisallowDeleteNULL() const;
	void     SetDisallowDeleteNULL(const JBoolean yesNo);

	static JBoolean GetAbortUnknownAlloc();
	static void     SetAbortUnknownAlloc(const JBoolean yesNo);

	JBoolean GetPrintErrors() const;
	void     SetPrintErrors(const JBoolean print);

// Debug link

	static void SetProtocol(DebugLink* link);
	static void SendError(const JString& msg);

protected:

	JMemoryManager();

	virtual void	Receive(JBroadcaster* sender, const Message& message);

	// Message handlers

	void HandleObjectDeletedAsArray(const JMMRecord& record);
	void HandleArrayDeletedAsObject(const JMMRecord& record);

	void HandleUnallocatedDeletion(const JCharacter* file, const JUInt32 line,
								   const JBoolean isArray);
	void HandleMultipleDeletion(const JMMRecord& thisRecord, const JCharacter* file,
								const JUInt32 line, const JBoolean isArray);

	void HandleMultipleAllocation(const JMMRecord& thisRecord,
								  const JMMRecord& firstRecord);

	void HandleNULLDeleted(const JCharacter* file, const JUInt32 line,
						   const JBoolean isArray);

private:

	struct DeleteRequest
	{
		void*             address;
		const JCharacter* file;
		JUInt32           line;
		JBoolean          array;
	};

// Static data

	static JBoolean theConstructingFlag;
	static JBoolean theInternalFlag;

	static JMMRecord theAllocStack[];
	static JSize     theAllocStackSize;

	static DeleteRequest theDeallocStack[];
	static JSize         theDeallocStackSize;

	static JBoolean      theInitializeFlag;
	static unsigned char theAllocateGarbage;

	static JBoolean      theAbortUnknownAllocFlag;

// Member data

	JMMTable*				itsMemoryTable;
	JMMErrorPrinter*		itsErrorPrinter;
	DebugLink*				itsLink;
	JMMDebugErrorStream*	itsErrorStream;
	JString*				itsExitStatsFileName;
	mutable std::ofstream*	itsExitStatsStream;

	JSize itsRecursionDepth;

	const JCharacter* itsLastDeleteFile;
	JUInt32           itsLastDeleteLine;

	// Statistics

	JBoolean itsBroadcastErrorsFlag;

	JBoolean itsPrintExitStatsFlag;
	JBoolean itsPrintInternalStatsFlag;

	JBoolean      itsShredFlag;
	unsigned char itsDeallocateGarbage;

	size_t itsTagSize;

	// Error notification

	JBoolean itsCheckDoubleAllocationFlag;
	JBoolean itsDisallowDeleteNULLFlag;

private:

	void	ConnectToDebugger(const JCharacter* socketName);
	void	HandleDebugRequest() const;
	void	SendDebugMessage(std::ostringstream& data) const;
	void	SendRunningStats() const;
	void	WriteRunningStats(ostream& output) const;
	void	SendRecords(istream& input) const;
	void	WriteRecords(ostream& output, const RecordFilter& filter) const;
	void	SendExitStats() const;
	void	WriteExitStats() const;

	void AddNewRecord(const JMMRecord& record);
	void DeleteRecord(void* block, const JCharacter* file, const JUInt32 line, const JBoolean isArray);

	static void BeginRecursiveBlock();
	static void EndRecursiveBlock();

	void EmptyStacks();

	static JUInt32 GetNewID();

	void LocateDelete(const JCharacter* file, const JUInt32 line);

	void HandleExit();
	void HandleACEExit();

	void ReadValue(JBoolean* hasValue, unsigned char* value, const JCharacter* string);

	// not allowed

	JMemoryManager(const JMemoryManager& source);
	const JMemoryManager& operator=(const JMemoryManager& source);

public:

	// Broadcaster messages

	/******************************************************************************
	 ObjectDeletedAsArray

		Record is the JMMRecord recording the object's first deletion,
		file and line is the location of the repeated deletion.
	 *****************************************************************************/

	static const JCharacter* kObjectDeletedAsArray;
	class ObjectDeletedAsArray : public JBroadcaster::Message
		{
		public:

			ObjectDeletedAsArray(const JMMRecord& record)
				:
				Message(kObjectDeletedAsArray),
				itsRecord(record)
				{ };

			const JMMRecord&  GetRecord() const { return itsRecord; };

		private:

			const JMMRecord&  itsRecord;
		};

	/******************************************************************************
	 ArrayDeletedAsObject

		Record is the JMMRecord recording the object's first deletion,
		file and line is the location of the repeated deletion.
	 *****************************************************************************/

	static const JCharacter* kArrayDeletedAsObject;
	class ArrayDeletedAsObject : public JBroadcaster::Message
		{
		public:

			ArrayDeletedAsObject(const JMMRecord& record)
				:
				Message(kArrayDeletedAsObject),
				itsRecord(record)
				{ };

			const JMMRecord&  GetRecord() const { return itsRecord; };

		private:

			const JMMRecord&  itsRecord;
		};

	/******************************************************************************
	 UnallocatedDeletion

		Record is the JMMRecord recording the object's first deletion,
		file and line is the location of the repeated deletion.
	 *****************************************************************************/

	static const JCharacter* kUnallocatedDeletion;
	class UnallocatedDeletion : public JBroadcaster::Message
		{
		public:

			UnallocatedDeletion(const JCharacter* file, const JUInt32 line,
								const JBoolean isArray)
				:
				Message(kUnallocatedDeletion),
				itsFile(file),
				itsLine(line),
				itsArrayFlag(isArray)
				{ };

			const JCharacter* GetFile() const { return itsFile; };
			JUInt32           GetLine() const { return itsLine; };
			JBoolean          IsArray() const { return itsArrayFlag; };

		private:

			const JCharacter* itsFile;
			const JUInt32     itsLine;
			const JBoolean    itsArrayFlag;
		};

	/******************************************************************************
	 MultipleDeletion

		Record is the JMMRecord recording the object's first deletion,
		file and line is the location of the repeated deletion.
	 *****************************************************************************/

	static const JCharacter* kMultipleDeletion;
	class MultipleDeletion : public JBroadcaster::Message
		{
		public:

			MultipleDeletion(const JMMRecord& record, const JCharacter* file,
							 const JUInt32 line, const JBoolean isArray)
				:
				Message(kMultipleDeletion),
				itsRecord(record),
				itsFile(file),
				itsLine(line),
				itsArrayFlag(isArray)
				{ };

			const JMMRecord&  GetRecord() const { return itsRecord; };
			const JCharacter* GetFile() const { return itsFile; };
			JUInt32           GetLine() const { return itsLine; };
			JBoolean          IsArray() const { return itsArrayFlag; };

		private:

			const JMMRecord&  itsRecord;
			const JCharacter* itsFile;
			const JUInt32     itsLine;
			const JBoolean    itsArrayFlag;
		};

	/******************************************************************************
	 MultipleAllocation

		Record is the JMMRecord recording the object's first deletion,
		file and line is the location of the repeated deletion.
	 *****************************************************************************/

	static const JCharacter* kMultipleAllocation;
	class MultipleAllocation : public JBroadcaster::Message
		{
		public:

			MultipleAllocation(const JMMRecord& thisRecord,
							   const JMMRecord& firstRecord)
				:
				Message(kMultipleAllocation),
				itsThisRecord(thisRecord),
				itsFirstRecord(firstRecord)
				{ };

			const JMMRecord&  GetThisRecord() const { return itsThisRecord; };
			const JMMRecord&  GetFirstRecord() const { return itsFirstRecord; };

		private:

			const JMMRecord&  itsThisRecord;
			const JMMRecord&  itsFirstRecord;
		};

	/******************************************************************************
	 NULLDeleted

		Record is the JMMRecord recording the object's first deletion,
		file and line is the location of the repeated deletion.
	 *****************************************************************************/

	static const JCharacter* kNULLDeleted;
	class NULLDeleted : public JBroadcaster::Message
		{
		public:

			NULLDeleted(const JCharacter* file, const JUInt32 line, const JBoolean isArray)
				:
				Message(kNULLDeleted),
				itsFile(file),
				itsLine(line),
				itsIsArrayFlag(isArray)
				{ };

			const JCharacter* GetFile() const { return itsFile; };
			JUInt32           GetLine() const { return itsLine; };
			JBoolean          IsArray() const { return itsIsArrayFlag; };

		private:

			const JCharacter* itsFile;
			const JUInt32     itsLine;
			const JBoolean    itsIsArrayFlag;
		};
};

/******************************************************************************
 RecordingAllocated

 *****************************************************************************/

inline JBoolean
JMemoryManager::RecordingAllocated() const
{
	return JConvertToBoolean(itsMemoryTable != NULL);
}

/******************************************************************************
 GetPrintExitStats

 *****************************************************************************/

inline JBoolean
JMemoryManager::GetPrintExitStats() const
{
	return itsPrintExitStatsFlag;
}

/******************************************************************************
 SetPrintExitStats

	Sets whether dynamic memory usage stats will be printed when the program
	exists.  Values set by this function override that set with the environment
	variable J_PRINT_EXIT_STATS.

 *****************************************************************************/

inline void
JMemoryManager::SetPrintExitStats
	(
	const JBoolean yesNo
	)
{
	itsPrintExitStatsFlag = yesNo;
}

/******************************************************************************
 GetPrintInternalStats

 *****************************************************************************/

inline JBoolean
JMemoryManager::GetPrintInternalStats() const
{
	return itsPrintInternalStatsFlag;
}

/******************************************************************************
 SetPrintInternalStats

	Sets whether memory usage stats for the memory manager itself will be
	printed whenever regular memory allocation stats are printed (such as at
	program exit).  Values set by this function override that set with the
	environment variable J_PRINT_EXIT_STATS.

 *****************************************************************************/

inline void
JMemoryManager::SetPrintInternalStats
	(
	const JBoolean yesNo
	)
{
	itsPrintInternalStatsFlag = yesNo;
}

/******************************************************************************
 GetAllocateGarbage

 *****************************************************************************/

inline unsigned char
JMemoryManager::GetAllocateGarbage() const
{
	return theAllocateGarbage;
}

/******************************************************************************
 GetTagSize

 *****************************************************************************/

inline size_t
JMemoryManager::GetTagSize() const
{
	return itsTagSize;
}

/******************************************************************************
 GetDeallocateGarbage

 *****************************************************************************/

inline unsigned char
JMemoryManager::GetDeallocateGarbage() const
{
	return itsDeallocateGarbage;
}

/******************************************************************************
 GetBroadcastErrors

 *****************************************************************************/

inline JBoolean
JMemoryManager::GetBroadcastErrors() const
{
	return itsBroadcastErrorsFlag;
}

/******************************************************************************
 SetBroadcastErrors

	Sets whether error notifications will be broadcast.  Overrides the
	JMM_BROADCAST_ERRORS environment variable.

 *****************************************************************************/

inline void
JMemoryManager::SetBroadcastErrors
	(
	const JBoolean broadcast
	)
{
	itsBroadcastErrorsFlag = broadcast;
}

/******************************************************************************
 GetCheckDoubleAllocation

 *****************************************************************************/

inline JBoolean
JMemoryManager::GetCheckDoubleAllocation() const
{
	return itsCheckDoubleAllocationFlag;
}

/******************************************************************************
 SetCheckDoubleAllocation

	Sets whether deletion of a NULL pointer is considered an error (ANSI says
	no).  Overrides the J_ALLOW_DELETE_NULL environment variable.

 *****************************************************************************/

inline void
JMemoryManager::SetCheckDoubleAllocation
	(
	const JBoolean yesNo
	)
{
	itsCheckDoubleAllocationFlag = yesNo;
}

/******************************************************************************
 GetDisallowDeleteNULL

 *****************************************************************************/

inline JBoolean
JMemoryManager::GetDisallowDeleteNULL() const
{
	return itsDisallowDeleteNULLFlag;
}

/******************************************************************************
 SetDisallowDeleteNULL

	Sets whether deletion of a NULL pointer is considered an error (ANSI says
	no).  Overrides the JMM_ALLOW_DELETE_NULL environment variable.

 *****************************************************************************/

inline void
JMemoryManager::SetDisallowDeleteNULL
	(
	const JBoolean yesNo
	)
{
	itsDisallowDeleteNULLFlag = yesNo;
}

/******************************************************************************
 GetAbortUnknownAlloc (static)

 *****************************************************************************/

inline JBoolean
JMemoryManager::GetAbortUnknownAlloc()
{
	return theAbortUnknownAllocFlag;
}

/******************************************************************************
 SetAbortUnknownAlloc (static)

	Sets whether the program will abort if unknown code allocates memory.  If
	the memory manager cannot locate an allocation, examining the core dump
	resulting from such an abort is one way to figure out where the memory is
	being allocated.  This overrides the JMM_ABORT_UNKNOWN_ALLOC environment
	variable.

 *****************************************************************************/

inline void
JMemoryManager::SetAbortUnknownAlloc
	(
	const JBoolean yesNo
	)
{
	theAbortUnknownAllocFlag = yesNo;
}

#endif
