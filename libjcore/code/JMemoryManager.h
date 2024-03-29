#ifndef _H_JMemoryManager
#define _H_JMemoryManager

/******************************************************************************
 JMemoryManager.h

	Interface for the JMemoryManager class.

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

// Superclass header
#include "JMMRecord.h"

#include <ace/UNIX_Addr.h>
#include <ace/LSOCK_Stream.h>
#include "JMessageProtocol.h"	// template; requires ace includes
#include <sstream>
#include <mutex>

#include <stdlib.h> // For size_t

	class JMMTable;
	class JMMErrorPrinter;
	class JMMDebugErrorStream;

const JFileVersion kJMemoryManagerDebugVersion = 1;

// version  1 adds includeBucket*, includeLibrary, includeUnknown to RecordFilter and filter to "running stats" message

class JMemoryManager : virtual public JBroadcaster
{
	friend void JLocateDelete(const JUtf8Byte* file, const JUInt32 line);
	friend void JMMHandleExit();
	friend void JMMHandleACEExit(void*, void*);
	friend class JMMTable;

public:

	using DebugLink = JMessageProtocol<ACE_LSOCK_STREAM>;

	enum
	{
		kRunningStatsMessage,
		kExitStatsMessage,
		kErrorMessage,
		kRecordsMessage,
		kSaveSnapshotMessage,
		kSnapshotDiffMessage
	};

	enum
	{
		kHistogramSlotCount = 30
	};

	struct RecordFilter
	{
		bool		includeInternal;
		bool		includeLibrary;
		bool		includeApp;
		bool		includeBucket1;
		bool		includeBucket2;
		bool		includeBucket3;
		bool		includeUnknown;
		JSize		minSize;
		JString*	fileName;

		RecordFilter()
			:
			includeInternal(false), includeLibrary(false),
			includeApp(false), includeBucket1(false),
			includeBucket2(false), includeBucket3(false),
			includeUnknown(false), minSize(0), fileName(nullptr)
			{ };

		~RecordFilter()
		{
			jdelete fileName;
		};

		bool	Match(const JMMRecord& record) const;

		void	Read(std::istream& input);
		void	Write(std::ostream& output) const;
	};

	static const JUtf8Byte* kUnknownFile;

public:

	~JMemoryManager() override;

	static JMemoryManager* Instance();

	static void* New(const size_t size, const JUtf8Byte* file,
					 const JUInt32 line, const int type,
					 const bool isArray, const bool assert);

	void  Delete(void* memory, bool isArray);

// Record keeping

	bool RecordingAllocated() const;

	unsigned char GetAllocateGarbage() const;
	unsigned char GetDeallocateGarbage() const;

// Statistics reporting

	void PrintMemoryStats();

	bool GetPrintExitStats() const;
	void SetPrintExitStats(const bool yesNo);

	bool GetPrintLibraryStats() const;
	void SetPrintLibraryStats(const bool yesNo);

	bool GetPrintInternalStats() const;
	void SetPrintInternalStats(const bool yesNo);

	void PrintAllocated() const;

// Error notification

	bool GetBroadcastErrors() const;
	void SetBroadcastErrors(const bool broadcast);

	bool GetCheckDoubleAllocation() const;
	void SetCheckDoubleAllocation(const bool yesNo);

	static bool GetAbortUnknownAlloc();
	static void SetAbortUnknownAlloc(const bool yesNo);

	bool GetPrintErrors() const;
	void SetPrintErrors(const bool print);

// Debug link

	static void SetProtocol(DebugLink* link);
	static void SendError(const JString& msg);

protected:

	JMemoryManager();

	void	Receive(JBroadcaster* sender, const Message& message) override;

	// Message handlers

	void HandleObjectDeletedAsArray(const JMMRecord& record);
	void HandleArrayDeletedAsObject(const JMMRecord& record);

	void HandleUnallocatedDeletion(const JUtf8Byte* file, const JUInt32 line,
								   const bool isArray);
	void HandleMultipleDeletion(const JMMRecord& thisRecord, const JUtf8Byte* file,
								const JUInt32 line, const bool isArray);

	void HandleMultipleAllocation(const JMMRecord& thisRecord,
								  const JMMRecord& firstRecord);

private:

	struct DeleteRequest
	{
		void*            address;
		const JUtf8Byte* file;
		JUInt32          line;
		bool             array;
	};

// Static data

	static bool theConstructingFlag;
	static bool theInternalFlag;

	static JMMRecord theAllocStack[];
	static JSize     theAllocStackSize;

	static DeleteRequest theDeallocStack[];
	static JSize         theDeallocStackSize;

	static bool          theInitializeFlag;
	static unsigned char theAllocateGarbage;

	static bool theAbortUnknownAllocFlag;

// Member data

	JMMTable*				itsMemoryTable;
	JMMErrorPrinter*		itsErrorPrinter;
	DebugLink*				itsLink;
	JMMDebugErrorStream*	itsErrorStream;
	JString*				itsExitStatsFileName;
	mutable std::ofstream*	itsExitStatsStream;
	mutable RecordFilter	itsRecordFilter;
	mutable JUInt32			itsSnapshotID;	// can be 0
	std::recursive_mutex*	itsMutex;

	// Statistics

	bool itsBroadcastErrorsFlag;
	bool itsPrintExitStatsFlag;

	// Error notification

	bool          itsShredFlag;
	unsigned char itsDeallocateGarbage;

	bool itsCheckDoubleAllocationFlag;

private:

	void ConnectToDebugger(const JUtf8Byte* socketName);
	void HandleDebugRequest() const;
	void SendDebugMessage(std::ostringstream& data) const;
	void SendRunningStats(std::istream& input) const;
	void WriteRunningStats(std::ostream& output) const;
	void SendRecords(std::istream& input) const;
	void WriteRecords(std::ostream& output) const;
	void SendSnapshotDiff(std::istream& input) const;
	void WriteSnapshotDiff(std::ostream& output) const;
	void SendExitStats() const;
	void WriteExitStats() const;

	void AddNewRecord(const JMMRecord& record);
	void DeleteRecord(void* block, const JUtf8Byte* file, const JUInt32 line, const bool isArray);

	static void BeginRecursiveBlock();
	static void EndRecursiveBlock();

	void EmptyStacks();

	static JUInt32 GetNewID();

	static void LocateDelete(const JUtf8Byte* file, const JUInt32 line);

	void HandleExit();
	void HandleACEExit();

	static void ReadValue(bool* hasValue, unsigned char* value, const JUtf8Byte* string);

	// not allowed

	JMemoryManager(const JMemoryManager&) = delete;
	JMemoryManager& operator=(const JMemoryManager&) = delete;

public:

	// Broadcaster messages

	/******************************************************************************
	 ObjectDeletedAsArray

		Record is the JMMRecord recording the object's first deletion,
		file and line is the location of the repeated deletion.
	 *****************************************************************************/

	static const JUtf8Byte* kObjectDeletedAsArray;
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

	static const JUtf8Byte* kArrayDeletedAsObject;
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

	static const JUtf8Byte* kUnallocatedDeletion;
	class UnallocatedDeletion : public JBroadcaster::Message
	{
	public:

		UnallocatedDeletion(const JUtf8Byte* file, const JUInt32 line,
							const bool isArray)
			:
			Message(kUnallocatedDeletion),
			itsFile(file),
			itsLine(line),
			itsArrayFlag(isArray)
			{ };

		const JUtf8Byte* GetFile() const { return itsFile; };
		JUInt32          GetLine() const { return itsLine; };
		bool         IsArray() const { return itsArrayFlag; };

	private:

		const JUtf8Byte* itsFile;
		const JUInt32    itsLine;
		const bool   itsArrayFlag;
	};

	/******************************************************************************
	 MultipleDeletion

		Record is the JMMRecord recording the object's first deletion,
		file and line is the location of the repeated deletion.
	 *****************************************************************************/

	static const JUtf8Byte* kMultipleDeletion;
	class MultipleDeletion : public JBroadcaster::Message
	{
	public:

		MultipleDeletion(const JMMRecord& record, const JUtf8Byte* file,
						 const JUInt32 line, const bool isArray)
			:
			Message(kMultipleDeletion),
			itsRecord(record),
			itsFile(file),
			itsLine(line),
			itsArrayFlag(isArray)
			{ };

		const JMMRecord& GetRecord() const { return itsRecord; };
		const JUtf8Byte* GetFile() const { return itsFile; };
		JUInt32          GetLine() const { return itsLine; };
		bool         IsArray() const { return itsArrayFlag; };

	private:

		const JMMRecord& itsRecord;
		const JUtf8Byte* itsFile;
		const JUInt32    itsLine;
		const bool   itsArrayFlag;
	};

	/******************************************************************************
	 MultipleAllocation

		Record is the JMMRecord recording the object's first deletion,
		file and line is the location of the repeated deletion.
	 *****************************************************************************/

	static const JUtf8Byte* kMultipleAllocation;
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
};

/******************************************************************************
 RecordingAllocated

 *****************************************************************************/

inline bool
JMemoryManager::RecordingAllocated() const
{
	return itsMemoryTable != nullptr;
}

/******************************************************************************
 GetPrintExitStats

 *****************************************************************************/

inline bool
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
	const bool yesNo
	)
{
	itsPrintExitStatsFlag = yesNo;
}

/******************************************************************************
 GetPrintLibraryStats

 *****************************************************************************/

inline bool
JMemoryManager::GetPrintLibraryStats() const
{
	return itsRecordFilter.includeLibrary;
}

/******************************************************************************
 SetPrintLibraryStats

	Sets whether memory usage stats for the libraries will be printed
	whenever regular memory allocation stats are printed (such as at
	program exit).  Values set by this function override that set with the
	environment variable JMM_PRINT_LIBRARY_STATS.

 *****************************************************************************/

inline void
JMemoryManager::SetPrintLibraryStats
	(
	const bool yesNo
	)
{
	itsRecordFilter.includeLibrary = yesNo;
}

/******************************************************************************
 GetPrintInternalStats

 *****************************************************************************/

inline bool
JMemoryManager::GetPrintInternalStats() const
{
	return itsRecordFilter.includeInternal;
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
	const bool yesNo
	)
{
	itsRecordFilter.includeInternal = yesNo;
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

inline bool
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
	const bool broadcast
	)
{
	itsBroadcastErrorsFlag = broadcast;
}

/******************************************************************************
 GetCheckDoubleAllocation

 *****************************************************************************/

inline bool
JMemoryManager::GetCheckDoubleAllocation() const
{
	return itsCheckDoubleAllocationFlag;
}

/******************************************************************************
 SetCheckDoubleAllocation

	Sets whether deletion of a nullptr pointer is considered an error (ANSI says
	no).  Overrides the J_ALLOW_DELETE_NULL environment variable.

 *****************************************************************************/

inline void
JMemoryManager::SetCheckDoubleAllocation
	(
	const bool yesNo
	)
{
	itsCheckDoubleAllocationFlag = yesNo;
}

/******************************************************************************
 GetAbortUnknownAlloc (static)

 *****************************************************************************/

inline bool
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
	const bool yesNo
	)
{
	theAbortUnknownAllocFlag = yesNo;
}

#endif
