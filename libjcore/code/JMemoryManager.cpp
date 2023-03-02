/******************************************************************************
 JMemoryManager.cpp

	JMemoryManager controls all memory allocation in a JCore/JX program,
	normally through the JCore operator jnew and jdelete replacements.  It is
	a singleton class (it can only be instantiated once) so that memory
	allocation is consistent throughout a single program.  The unique
	instance is accessed through the static member function Instance.

	JMemoryManager and the JCore operator jnew present a paradox: like all
	JX objects created at run time, JMemoryManager components are allocated
	with the global operator jnew.  However, operator jnew uses
	JMemoryManager to determine what its behavior should be; thus
	JMemoryManager must exist before it is allocated!  It is not necessary
	for JMemoryManager to have its own class operator jnew and jdelete to
	solve this problem; JMemoryManager uses static buffering to allow
	recursive calls to jnew and jdelete, and this also works when the
	manager itself is created.

	Most programs will interact with the memory manager indirectly through
	operator jnew and jdelete and will not be aware of its existence.
	However, JMemoryManager can supply a number of services for programs
	which take the trouble to interact with it directly.  Currently this
	ability is limited to requesting simple statistics reporting, but the
	capabilities will expand in the future.

	JMemoryManager cannot be used in conjunction with objects that define
	their own versions of new and delete.

	JMemoryManager's consistency checks depend on it controlling every jnew
	and jdelete, so that if it is to record information it must begin doing
	so at the moment of construction.  Because the memory manager is almost
	certain to be created before execution of main even begins (any static
	object whose constructor calls operator jnew will guarantee this on any
	system, for example), the memory manager uses lots of environment
	variables to control its operation (for all variables case is
	irrelevant):

#		JMM_TABLE_TYPE         If this environment variable is set to "array"
#		                       (case is irrelevant) the manager uses a stack
#		                       implementation for its memory block table, and
#		                       if its value is "hashtable" a hash table
#		                       implementation is used.  If it does not exist,
#		                       or its value is unrecognized, the default
#		                       (currently hashtable) is used without warning.

#		                       This is mostly useful for debugging the tables
#		                       themselves; otherwise, there is little reason
#		                       to use the grossly inefficient array
#		                       implementation.

#		JMM_RECORD_ALLOCATED   If this environment variable is set to "yes" the
#		                       manager will keep a record of all currently
#		                       allocated memory.

#		                       CancelRecordAllocated() can stop the recording
#		                       of initializations, but once canceled it cannot
#		                       be restarted.

#		JMM_RECORD_DEALLOCATED If this environment variable is set to "yes" the
#		                       manager will keep a record of all memory which
#		                       has been deallocated.  It is useless unless
#		                       JMM_RECORD_ALLOCATED is also set.  Can be canceled
#		                       by CancelRecordDeallocated.

#		JMM_CHECK_DOUBLE_ALLOCATION  If this environment variable is set to "yes"
#		                       the manager will complain if memory is allocated at
#		                       a location previously allocated (this indicates an
#		                       error in the system malloc() and so is only useful
#		                       for debugging the C memory manager itself, or more
#		                       likely for detecting a logic error in JMM).  It is
#		                       useless unless JMM_RECORD_ALLOCATED is also set.
#		                       The SetCheckDoubleAllocation method overrides this
#		                       variable.

#		JMM_BROADCAST_ERRORS   If this environment variable is set to "yes" the
#		                       manager will broadcast when errors occur.  It
#		                       is overridden by the SetBroadcastErrors method.

#		JMM_PRINT_EXIT_STATS   If this environment variable is set to "yes" the
#		                       manager will print dynamic memory usage stats
#		                       when the program finishes.  The SetPrintExitStats
#		                       method overrides the environment variable setting.

#		JMM_PRINT_INTERNAL_STATS  If this environment variable is set to "yes"
#		                       then whenever allocation stats are printed (such as
#		                       at program exit) the manager will also print stats
#		                       on its internal state, such as the memory it has
#		                       allocated for its own use.  The SetInternalExitStats
#		                       method overrides the environment variable setting.

#		JMM_ABORT_UNKNOWN_ALLOC If set to "yes", the process will abort if memory
#		                       is allocated by code that the memory manager cannot
#		                       locate.  One reason to do this is to examine the
#		                       resulting core dump for the location of the offending
#		                       code.  Overridden by SetAbortUnknownAlloc().

#		JMM_NO_PRINT_ERRORS    Setting this environment variable to "yes" suppresses
#		                       all printing (the default is to print because
#		                       JMemoryManager's messages are already off by default
#		                       and if you turn them on you likely want to print error
#		                       messages).

#		JMM_PIPE               UNIX pipe on which to receive requests.  Also used
#		                       to send exit stats.

#		MALLOC_PERTURB_        Setting this environment variable turns on
#		                       initialization and shredding of memory in libc.

	(JMM_NO_PRINT_ERRORS is actually read in the JMMErrorPrinter proxy object.)

	Setting up so many variables can be a pain, and frequently writing a
	few handy shell scripts is the best solution.  After you do this you'll
	forget exactly what the variables do when you go to change them!  The
	descriptions are commented out with the unix script comment symbol,
	'#', to make it easy to paste them directly into a shell script;
	self-documentation is much easier than having to find this file again
	to modify the script.  The jmmenv script provides an example of this as
	well as a handy summary of all the variables and their effects.

	To get file/line information you must include <jNew.h>, because it must
	define jnew and jdelete.  The recommended way to do this is to #include
	jAssert.h, since it #include's jNew.h.

	Performance:

	A test involving starting and immediately quitting jcc on a project
	that displays the jcore+jx inheritance tree with various JMM
	environment settings produced the following results (in CPU seconds):

		baseline: 1.97
		+init: 2.05
		+hash: 3.99
		+broadcast: 4.05
		+shred: 3.94
		+dealloc: 6.32
		+double alloc: 6.54
		+array-dealloc: 616.77
		+dealloc: 611.67

	Repeated runs showed the accuracy was no better than a few percent.
	This suggests that there is no noticable penalty for initialization or
	shredding. Clearly hash tables were at least two orders of magnitude
	better than arrays. For building the tree, deallocation didn't cost much
	extra with arrays, though if the program would have kept running it
	eventually would have.

	BASE CLASS = virtual public JBroadcaster

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

//Class Header
#include "JMemoryManager.h"

#include <stdlib.h>
#include <new>
#include <atomic>

#include "JMMArrayTable.h"
#include "JMMHashTable.h"
#include "JMMErrorPrinter.h"

#include <ace/Connector.h>
#include <ace/LSOCK_Connector.h>
#include "JMMDebugErrorStream.h"
#include "JStringIterator.h"
#include "jFileUtil.h"
#include <fstream>

#include "jErrno.h"
#include "jAssert.h"
#include "jNew.h"

using DebugLinkConnector = ACE_Connector<JMemoryManager::DebugLink, ACE_LSOCK_CONNECTOR>;

static const JUtf8Byte* kDisconnectStr = "JMemoryManager::DebugLinkDisconnect";
const JSize kDisconnectStrLength       = strlen(kDisconnectStr);

// Broadcaster message types

	const JUtf8Byte* JMemoryManager::kObjectDeletedAsArray = "ObjectDeletedAsArray::JMemoryManager";
	const JUtf8Byte* JMemoryManager::kArrayDeletedAsObject = "ArrayDeletedAsObject::JMemoryManager";

	const JUtf8Byte* JMemoryManager::kUnallocatedDeletion = "UnallocatedDeletion::JMemoryManager";
	const JUtf8Byte* JMemoryManager::kMultipleDeletion = "MultipleDeletion::JMemoryManager";

	const JUtf8Byte* JMemoryManager::kMultipleAllocation = "MultipleAllocation::JMemoryManager";

// Static member data

	static const JSize theStackMax = 50;

	bool      JMemoryManager::theConstructingFlag = false;
	JMMRecord JMemoryManager::theAllocStack[theStackMax];
	JSize     JMemoryManager::theAllocStackSize = 0;

	bool JMemoryManager::theInternalFlag = false;

	JMemoryManager::DeleteRequest JMemoryManager::theDeallocStack[theStackMax];
	JSize                         JMemoryManager::theDeallocStackSize = 0;

	bool JMemoryManager::theAbortUnknownAllocFlag = false;

	const JUtf8Byte* JMemoryManager::kUnknownFile = "<UNKNOWN>";

	static JSize theRecursionDepth = 0;		// only modified inside mutex lock

// clang linker error if these are declared as member data

	static thread_local const JUtf8Byte* theLastDeleteFile = JMemoryManager::kUnknownFile;
	static thread_local JUInt32          theLastDeleteLine = 0;

// static functions

	void JMMHandleExit();
	void JMMHandleACEExit(void*, void*);

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

JMemoryManager::JMemoryManager()
	:
	itsMemoryTable(nullptr),
	itsErrorPrinter(nullptr),
	itsLink(nullptr),
	itsErrorStream(nullptr),
	itsExitStatsFileName(nullptr),
	itsExitStatsStream(nullptr),
	itsBroadcastErrorsFlag(false),
	itsPrintExitStatsFlag(false),
	itsPrintInternalStatsFlag(false),
	itsCheckDoubleAllocationFlag(false)
{
	itsMutex = new std::recursive_mutex;
	assert( itsMutex != nullptr );

	// Instance() must set the flag
	assert(theConstructingFlag == true);

	const JUtf8Byte* abortUnknownAlloc = getenv("JMM_ABORT_UNKNOWN_ALLOC");
	if (abortUnknownAlloc != nullptr && JString::Compare(abortUnknownAlloc, "yes", JString::kIgnoreCase) == 0)
	{
		theAbortUnknownAllocFlag = true;
	}

	const JUtf8Byte* broadcastErrors = getenv("JMM_BROADCAST_ERRORS");
	if (broadcastErrors != nullptr && JString::Compare(broadcastErrors, "yes", JString::kIgnoreCase) == 0)
	{
		itsBroadcastErrorsFlag = true;
	}

	const JUtf8Byte* printExitStats = getenv("JMM_PRINT_EXIT_STATS");
	if (printExitStats != nullptr && JString::Compare(printExitStats, "yes", JString::kIgnoreCase) == 0)
	{
		itsPrintExitStatsFlag = true;
	}

	const JUtf8Byte* printInternalStats = getenv("JMM_PRINT_INTERNAL_STATS");
	if (printInternalStats != nullptr && JString::Compare(printInternalStats, "yes", JString::kIgnoreCase) == 0)
	{
		itsPrintInternalStatsFlag = true;
	}

	const JUtf8Byte* checkDoubleAllocation = getenv("JMM_CHECK_DOUBLE_ALLOCATION");
	if (checkDoubleAllocation != nullptr && JString::Compare(checkDoubleAllocation, "yes", JString::kIgnoreCase) == 0)
	{
		itsCheckDoubleAllocationFlag = true;
	}

	const JUtf8Byte* recordAllocated = getenv("JMM_RECORD_ALLOCATED");
	if (recordAllocated != nullptr && JString::Compare(recordAllocated, "yes", JString::kIgnoreCase) == 0)
	{
		const JUtf8Byte* tableType = getenv("JMM_TABLE_TYPE");
		const JUtf8Byte* recordDeallocated = getenv("JMM_RECORD_DEALLOCATED");
		bool recordDeallocatedFlag = false;
		if (recordDeallocated != nullptr && JString::Compare(recordDeallocated, "yes", JString::kIgnoreCase) == 0)
		{
			recordDeallocatedFlag = true;
		}

		if (tableType != nullptr && JString::Compare(tableType, "array", JString::kIgnoreCase) == 0)
		{
			itsMemoryTable = new JMMArrayTable(this, recordDeallocatedFlag);
		}
		else
		{
			itsMemoryTable = new JMMHashTable(this, recordDeallocatedFlag);
		}
		assert(itsMemoryTable != nullptr);
	}

	// JMM_PIPE done in Instance()

	atexit(::JMMHandleExit);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JMemoryManager::~JMemoryManager()
{
	assert(0); // Impossible to call, except maliciously.  So don't do that.
}

/******************************************************************************
 Instance (static)

 *****************************************************************************/

JMemoryManager*
JMemoryManager::Instance()
{
	// Guarantees access is only through this function
	static JMemoryManager* manager = nullptr;

	if (manager == nullptr)
	{
		theConstructingFlag = true;
		manager = new JMemoryManager;
		assert(manager != nullptr);

		// Create the error printer proxy to do the printing work.
		// Construction of the error printer must take place here, after
		// the manager is fully constructed; the recursive call to Instance
		// is harmless.
		manager->itsErrorPrinter = new JMMErrorPrinter;
		assert(manager->itsErrorPrinter != nullptr);

		const JUtf8Byte* pipeName = getenv("JMM_PIPE");
		if (!JString::IsEmpty(pipeName))
		{
			manager->itsErrorStream = new JMMDebugErrorStream;
			assert(manager->itsErrorStream != nullptr);
		}

		theConstructingFlag = false;
		manager->EmptyStacks();

		// do it here since it calls delete as well as new

		if (!JString::IsEmpty(pipeName))
		{
			theInternalFlag = true;

			manager->ConnectToDebugger(pipeName);
			ACE_Object_Manager::at_exit(nullptr, ::JMMHandleACEExit, nullptr);

			// If we create the file when we actually need it, it
			// re-constructs JStringManager.

			JString fileName;
			const JError err = JCreateTempFile(&fileName);
			if (err.OK())
			{
				manager->itsExitStatsFileName = new JString(fileName);
				assert( manager->itsExitStatsFileName != nullptr );
			}
			else
			{
				std::cerr << "Failed to create exit stats file:" << std::endl;
				std::cerr << err.GetMessage() << std::endl;
			}

			theInternalFlag = false;
		}
	}

	return manager;
}

/******************************************************************************
 New (static)

 *****************************************************************************/

void*
JMemoryManager::New
	(
	const size_t     size,
	const JUtf8Byte* file,
	const JUInt32    line,
	const bool       isArray
	)
{
	if (theAbortUnknownAllocFlag && line == 0)
	{
		std::cerr << "Memory allocated by unknown code, aborting..." << std::endl;
		abort();
	}

	const size_t trueSize = size ? size : 1;
	void* newBlock = malloc(trueSize);

	if (newBlock == nullptr)
	{
		std::cerr << "Failed to allocate block of size " << trueSize << std::endl;
		return nullptr;
	}

	if (!theConstructingFlag)
	{
		Instance()->itsMutex->lock();
	}

	const bool useStack  = theConstructingFlag || theRecursionDepth > 0;
	const bool isManager = useStack || theInternalFlag;
	JMMRecord newRecord(GetNewID(), newBlock, trueSize, file, line, isArray, isManager);

	if (useStack)
	{
		assert(theAllocStackSize < theStackMax);
		theAllocStack[theAllocStackSize] = newRecord;
		theAllocStackSize++;
	}
	else
	{
		Instance()->AddNewRecord(newRecord);
	}

	if (!theConstructingFlag)
	{
		Instance()->itsMutex->unlock();
	}

	return newBlock;
}

/******************************************************************************
 Delete

 *****************************************************************************/

void
JMemoryManager::Delete
	(
	void*      block,
	const bool isArray
	)
{
	std::lock_guard lock(*itsMutex);

	if (theConstructingFlag || theRecursionDepth > 0)
	{
		DeleteRequest thisRequest;
		thisRequest.address = block;
		thisRequest.file    = theLastDeleteFile;
		thisRequest.line    = theLastDeleteLine;
		thisRequest.array   = isArray;

		assert(theDeallocStackSize < theStackMax);
		theDeallocStack[theDeallocStackSize] = thisRequest;
		theDeallocStackSize++;

		// Don't delete while on the request stack so that the MemoryTable entry
		// doesn't lie; is there a better way?
	}
	else
	{
		DeleteRecord(block, theLastDeleteFile, theLastDeleteLine, isArray);
	}

	theLastDeleteFile = kUnknownFile;
	theLastDeleteLine = 0;
}

/******************************************************************************
 PrintMemoryStats

 *****************************************************************************/

void
JMemoryManager::PrintMemoryStats()
{
	if (itsMemoryTable != nullptr)
	{
		std::lock_guard lock(*itsMutex);

		std::cout << "\n";
		std::cout << "   Dynamic memory usage:\n";
		std::cout << "      Blocks currently allocated: " << itsMemoryTable->GetAllocatedCount() << "\n";
		std::cout << "       Bytes currently allocated: " << itsMemoryTable->GetAllocatedBytes() << "\n";
		std::cout << "                  Deleted blocks: " << itsMemoryTable->GetDeletedCount() << "\n";
		std::cout << "                    Total blocks: " << itsMemoryTable->GetTotalCount() << "\n";
		std::cout << "\n";

		std::cout << std::endl;
	}
}

/******************************************************************************
 PrintAllocated

 *****************************************************************************/

void
JMemoryManager::PrintAllocated() const
{
	if (itsMemoryTable != nullptr)
	{
		std::lock_guard lock(*itsMutex);
		itsMemoryTable->PrintAllocated(itsPrintInternalStatsFlag);
	}
}

/******************************************************************************
 GetPrintErrors

 *****************************************************************************/

bool
JMemoryManager::GetPrintErrors() const
{
	return itsErrorPrinter->GetPrintErrors();
}

/******************************************************************************
 SetPrintErrors

	Sets whether error notifications will be printed.  Overrides the
	JMM_NO_PRINT_ERRORS environment variable.

 *****************************************************************************/

void
JMemoryManager::SetPrintErrors
	(
	const bool print
	)
{
	itsErrorPrinter->SetPrintErrors(print);
}

/******************************************************************************
 AddNewRecord (private)

 *****************************************************************************/

void
JMemoryManager::AddNewRecord
	(
	const JMMRecord& record
	)
{
	if (itsMemoryTable != nullptr)
	{
		std::lock_guard lock(*itsMutex);
		itsMemoryTable->AddNewRecord(record, itsCheckDoubleAllocationFlag);
	}
}

/******************************************************************************
 DeleteRecord (private)

 *****************************************************************************/

void
JMemoryManager::DeleteRecord
	(
	void*            block,
	const JUtf8Byte* file,
	const JUInt32    line,
	const bool       isArray
	)
{
	if (block == nullptr)
	{
		// should never happen, since C++14 specifies it that way
		return;
	}

	JMMRecord record;
	const bool wasAllocated = itsMemoryTable == nullptr ||	// Have to trust the client
		itsMemoryTable->SetRecordDeleted(&record, block, file, line, isArray);

	// Try to avoid a seg fault so the program can continue
	if (wasAllocated)
	{
		free(block);
	}
}

/******************************************************************************
 BeginRecursiveBlock (private static)

 *****************************************************************************/

void
JMemoryManager::BeginRecursiveBlock()
{
	theRecursionDepth++;
	if (theRecursionDepth > 1)
{
		std::cout << "Unusual (but probably safe) memory manager behavior: JMM recursing at depth "
			 << theRecursionDepth << ".  Please notify the author." << std::endl;
}
}

/******************************************************************************
 EndRecursiveBlock (private static)

 *****************************************************************************/

void
JMemoryManager::EndRecursiveBlock()
{
	JMemoryManager* manager = Instance();
	assert( theRecursionDepth > 0 );
	theRecursionDepth--;
	assert( theRecursionDepth >= 0 );	// for case with multiple threads
	manager->EmptyStacks();
}

/******************************************************************************
 EmptyStacks (private)

 *****************************************************************************/

void
JMemoryManager::EmptyStacks()
{
	if (itsMemoryTable != nullptr && theRecursionDepth == 0)
	{
		// Do alloc stack first so dealloc's never have to search through the
		// Alloc stack
		while (theAllocStackSize > 0)
		{
			theAllocStackSize--;
			JMMRecord thisRecord = theAllocStack[theAllocStackSize];

			AddNewRecord(thisRecord);
		}

		while (theDeallocStackSize > 0)
		{
			theDeallocStackSize--;
			DeleteRequest thisRequest = theDeallocStack[theDeallocStackSize];

			DeleteRecord(thisRequest.address, thisRequest.file,
						 thisRequest.line, thisRequest.array);
		}
	}
}

/******************************************************************************
 GetNewID (static private)

 *****************************************************************************/

JUInt32
JMemoryManager::GetNewID()
{
	// Guarantees access is only through this function
	static std::atomic_ulong nextID = 0;

	return nextID++;
}

/******************************************************************************
 LocateDelete (static private)

 *****************************************************************************/

void
JMemoryManager::LocateDelete
	(
	const JUtf8Byte* file,
	const JUInt32    line
	)
{
	theLastDeleteFile = file;
	theLastDeleteLine = line;
}

/******************************************************************************
 JMMHandleACEExit (ordinary)

 *****************************************************************************/

void
JMMHandleACEExit(void*, void*)
{
	JMemoryManager::Instance()->HandleACEExit();
}

/******************************************************************************
 HandleACEExit (private)

 *****************************************************************************/

void
JMemoryManager::HandleACEExit()
{
	if (itsLink != nullptr)
	{
		theInternalFlag = true;

		SendExitStats();
		itsLink->SendDisconnect();
		itsLink->Flush();
		itsLink = nullptr;

		theInternalFlag = false;
	}
}

/******************************************************************************
 JMMHandleExit (ordinary)

 *****************************************************************************/

void
JMMHandleExit()
{
	JMemoryManager::Instance()->HandleExit();
}

/******************************************************************************
 HandleExit (private)

 *****************************************************************************/

void
JMemoryManager::HandleExit()
{
	HandleACEExit();
	WriteExitStats();

	if (itsPrintExitStatsFlag)
	{
		std::cout << "\n*******************************************************************" << std::endl;
		std::cout << "\n\nJCore Exit Statistics: anything unallocated is probably garbage" << "\n";
		PrintMemoryStats();

		PrintAllocated();
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JMemoryManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(JMessageProtocolT::kMessageReady))
	{
		theInternalFlag = true;
		HandleDebugRequest();
		theInternalFlag = false;
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 ConnectToDebugger (private)

	Connects to the specified named UNIX socket.

 ******************************************************************************/

void
JMemoryManager::ConnectToDebugger
	(
	const JUtf8Byte* socketName
	)
{
	itsLink = new DebugLink;
	assert( itsLink != nullptr );

	auto* connector = new DebugLinkConnector;
	assert( connector != nullptr );

	ACE_UNIX_Addr addr(socketName);
	if (connector->connect(itsLink, addr, ACE_Synch_Options::asynch) == -1 &&
		jerrno() != EAGAIN)
	{
		std::cerr << "error trying to connect JMemoryManager::DebugLink: " << jerrno() << std::endl;
	}
	else
	{
		SetProtocol(itsLink);
		ListenTo(itsLink);
		ClearWhenGoingAway(itsLink, &itsLink);
	}
}

/******************************************************************************
 HandleDebugRequest (private)

 ******************************************************************************/

void
JMemoryManager::HandleDebugRequest()
	const
{
	assert( itsLink != nullptr );

	JString text;
	const bool ok = itsLink->GetNextMessage(&text);
	assert( ok );

	std::string s(text.GetBytes(), text.GetByteCount());
	std::istringstream input(s);

	JFileVersion vers;
	input >> vers;
	if (vers != kJMemoryManagerDebugVersion)
	{
		std::cerr << "JMemoryManager::HandleDebugRequest received version (" << vers;
		std::cerr << ") different than expected (" << kJMemoryManagerDebugVersion << ")" << std::endl;
		return;
	}

	long type;
	input >> type;

	if (type == kRunningStatsMessage)
	{
		SendRunningStats();
	}
	else if (type == kRecordsMessage)
	{
		SendRecords(input);
	}
}

/******************************************************************************
 SendRunningStats (private)

 ******************************************************************************/

void
JMemoryManager::SendRunningStats()
	const
{
	std::ostringstream output;
	output << kJMemoryManagerDebugVersion;
	output << ' ';
	WriteRunningStats(output);

	SendDebugMessage(output);
}

/******************************************************************************
 WriteRunningStats (private)

 ******************************************************************************/

void
JMemoryManager::WriteRunningStats
	(
	std::ostream& output
	)
	const
{
	std::lock_guard lock(*itsMutex);

	output << kRunningStatsMessage;
	output << ' ' << itsMemoryTable->GetAllocatedCount();
	output << ' ' << itsMemoryTable->GetAllocatedBytes();
	output << ' ' << itsMemoryTable->GetDeletedCount();
	output << ' ';
	itsMemoryTable->StreamAllocationSizeHistogram(output);
}

/******************************************************************************
 SendRecords (private)

 ******************************************************************************/

void
JMemoryManager::SendRecords
	(
	std::istream& input
	)
	const
{
	RecordFilter filter;
	filter.Read(input);

	std::ostringstream output;
	output << kJMemoryManagerDebugVersion;
	output << ' ';
	WriteRecords(output, filter);

	SendDebugMessage(output);
}

/******************************************************************************
 WriteRecords (private)

 ******************************************************************************/

void
JMemoryManager::WriteRecords
	(
	std::ostream&		output,
	const RecordFilter&	filter
	)
	const
{
	std::lock_guard lock(*itsMutex);

	output << kRecordsMessage;
	output << ' ';
	itsMemoryTable->StreamAllocatedForDebug(output, filter);
}

/******************************************************************************
 SendExitStats (private)

	We can't write useful exit stats when ACE is shutting down, so we
	simply tell jx_memory_debugger where to find the information after we
	are gone.

 ******************************************************************************/

void
JMemoryManager::SendExitStats()
	const
{
	if (itsExitStatsFileName == nullptr || itsExitStatsFileName->IsEmpty())
	{
		return;
	}

	std::ostringstream output;
	output << kJMemoryManagerDebugVersion;
	output << ' ' << kExitStatsMessage;
	output << ' ' << *itsExitStatsFileName;

	SendDebugMessage(output);

	itsExitStatsStream = new std::ofstream(itsExitStatsFileName->GetBytes());
	assert( itsExitStatsStream != nullptr );
}

/******************************************************************************
 WriteExitStats (private)

	We don't need to include a version number because we already sent
	itsExitStatsFileName.

 ******************************************************************************/

void
JMemoryManager::WriteExitStats()
	const
{
	if (itsExitStatsStream == nullptr)
	{
		return;
	}

	*itsExitStatsStream << ' ';
	WriteRunningStats(*itsExitStatsStream);

	RecordFilter filter;
	*itsExitStatsStream << ' ';
	WriteRecords(*itsExitStatsStream, filter);

	delete itsExitStatsStream;
	itsExitStatsStream = nullptr;
}

/******************************************************************************
 SendError (static)

	called by JMMDebugErrorStream.

 ******************************************************************************/

void
JMemoryManager::SendError
	(
	const JString& msg
	)
{
	JMemoryManager* mgr = JMemoryManager::Instance();
	if (mgr->itsLink != nullptr)
	{
		std::ostringstream output;
		output << kJMemoryManagerDebugVersion;
		output << ' ' << kErrorMessage;
		output << ' ' << msg;

		mgr->SendDebugMessage(output);
	}
	else if (mgr->itsExitStatsStream != nullptr)
	{
		*(mgr->itsExitStatsStream) << ' ' << kErrorMessage;
		*(mgr->itsExitStatsStream) << ' ' << msg;
	}
}

/******************************************************************************
 SendDebugMessage (private)

 ******************************************************************************/

void
JMemoryManager::SendDebugMessage
	(
	std::ostringstream& data
	)
	const
{
	std::string s = data.str();
	while (true)
	{
		// we don't want to allocate a JStringMatch
		const long i = s.find(kDisconnectStr);
		if (i < 0)
		{
			break;
		}

		// replace with slightly shorter string, so reallocation is not required
		s.replace(i, kDisconnectStrLength, "<ixnay on the disconnect string!>");
	}

	std::lock_guard lock(*itsMutex);
	itsLink->SendMessage(JString(s.c_str(), s.length(), JString::kNoCopy));
}

/******************************************************************************
 SetProtocol (static)

 ******************************************************************************/

void
JMemoryManager::SetProtocol
	(
	DebugLink* link
	)
{
	link->SetProtocol("\1", 1, kDisconnectStr, kDisconnectStrLength);
}

/******************************************************************************
 Message Handlers

 *****************************************************************************/

/******************************************************************************
 HandleObjectDeletedAsArray (protected)

 *****************************************************************************/

void
JMemoryManager::HandleObjectDeletedAsArray
	(
	const JMMRecord& record
	)
{
	if (itsBroadcastErrorsFlag)
	{
		Broadcast( ObjectDeletedAsArray(record) );
	}
}

/******************************************************************************
 HandleArrayDeletedAsObject (protected)

 *****************************************************************************/

void
JMemoryManager::HandleArrayDeletedAsObject
	(
	const JMMRecord& record
	)
{
	if (itsBroadcastErrorsFlag)
	{
		Broadcast( ArrayDeletedAsObject(record) );
	}
}

/******************************************************************************
 HandleUnallocatedDeletion (protected)

	It frequently happens that we don't catch the alloc for system code.

 *****************************************************************************/

void
JMemoryManager::HandleUnallocatedDeletion
	(
	const JUtf8Byte* file,
	const JUInt32    line,
	const bool   isArray
	)
{
	if (itsBroadcastErrorsFlag && line != 0)
	{
		Broadcast( UnallocatedDeletion(file, line, isArray) );
	}
}

/******************************************************************************
 HandleMultipleDeletion (protected)

 *****************************************************************************/

void
JMemoryManager::HandleMultipleDeletion
	(
	const JMMRecord& thisRecord,
	const JUtf8Byte* file,
	const JUInt32    line,
	const bool   isArray
	)
{
	if (itsBroadcastErrorsFlag)
	{
		Broadcast( MultipleDeletion(thisRecord, file, line, isArray) );
	}
}

/******************************************************************************
 HandleMultipleAllocation (protected)

 *****************************************************************************/

void
JMemoryManager::HandleMultipleAllocation
	(
	const JMMRecord& thisRecord,
	const JMMRecord& firstRecord
	)
{
	if (itsBroadcastErrorsFlag)
	{
		Broadcast( MultipleAllocation(thisRecord, firstRecord) );
	}
}

/******************************************************************************
 ReadValue (static private)

 *****************************************************************************/

void
JMemoryManager::ReadValue
	(
	bool*        hasValue,
	unsigned char*   value,
	const JUtf8Byte* string
	)
{
	*hasValue = string != nullptr && JString::Compare(string, "no", JString::kIgnoreCase) != 0;

	if (*hasValue)
	{
		const JUtf8Byte* start = string;
		while ( isspace(*start) )
		{
			++start;
		}
		if (*start != '\0') // The string has a non-space value, so try to read as a number
		{
			JUtf8Byte* end;
			auto theValue = (unsigned char) strtol(start, &end, 0);
			if (*end == '\0') // We read the entire string, so the value must be good
			{
				*value = theValue;
			}
		}
	}
}

/******************************************************************************
 JMemoryManager::RecordFilter

 *****************************************************************************/

bool
JMemoryManager::RecordFilter::Match
	(
	const JMMRecord& record
	)
	const
{
	bool match = true;

	if (!includeInternal && record.IsManagerMemory())
	{
		match = false;
	}

	if (record.GetSize() < minSize)
	{
		match = false;
	}

	const JSize newFileLength = strlen(record.GetNewFile());
	if (match && fileName != nullptr && newFileLength == fileName->GetByteCount())
	{
		if (record.GetNewFile() != *fileName)
		{
			match = false;
		}
	}
	else if (match && fileName != nullptr)
	{
		const JUtf8Byte *s1, *s2;
		JSize l1, l2;
		if (newFileLength > fileName->GetByteCount())
		{
			s1 = record.GetNewFile();
			l1 = newFileLength;
			s2 = fileName->GetBytes();
			l2 = fileName->GetByteCount();
		}
		else
		{
			s1 = fileName->GetBytes();
			l1 = fileName->GetByteCount();
			s2 = record.GetNewFile();
			l2 = newFileLength;
		}

		if (*(s1 + l1 - l2 - 1) != ACE_DIRECTORY_SEPARATOR_CHAR ||
			strcmp(s1 + l1 - l2, s2) != 0)
		{
			match = false;
		}
	}

	return match;
}

void
JMemoryManager::RecordFilter::Read
	(
	std::istream& input
	)
{
	input >> JBoolFromString(includeInternal);
	input >> minSize;

	bool hasFile;
	input >> JBoolFromString(hasFile);

	if (hasFile)
	{
		if (fileName == nullptr)
		{
			fileName = new JString;
			assert( fileName != nullptr );
		}

		input >> *fileName;
	}
}

void
JMemoryManager::RecordFilter::Write
	(
	std::ostream& output
	)
	const
{
	output << JBoolToString(includeInternal);
	output << ' ' << minSize;

	const bool hasFile = fileName != nullptr && !fileName->IsEmpty();
	output << ' ' << JBoolToString(hasFile);
	if (hasFile)
	{
		output << ' ' << *fileName;
	}
}
