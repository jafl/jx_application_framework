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

#		JMM_INITIALIZE         If this environment variable is set but given no
#		                       value the manager will initialize new memory blocks
#		                       to the default AllocateGarbage value.  If it is
#		                       given a numerical value, that will be the value
#		                       used for initialization.  If the manager cannot
#		                       understand the value given, the default is used
#		                       (setting it to "default" is the recommended way
#		                       to do this intentionally).  If it does not exist
#		                       or is set to "no" memory blocks will not be
#		                       initialized.

#		JMM_SHRED              If this environment variable is set but given no
#		                       value the manager will set deleted blocks to the
#		                       default DeallocateGarbage value.  If it is given
#		                       a numerical value, that will be the value the
#		                       blocks are set to.  If the manager cannot
#		                       understand the value given, the default is used
#		                       (setting it to "default" is the recommended way
#		                       to do this intentionally).  If it does not exist
#		                       or is set to "no" memory blocks will not be
#		                       initialized.

#		                       This variable is meaningless if allocation records
#		                       are not being kept, because in that case the
#		                       manager cannot know the size of the block being
#		                       freed.

#		                       Keep in mind that jdelete (via free()) generally
#		                       will store information at the beginning of each
#		                       deallocated block.  This information will overwrite
#		                       the JMM garbage values at those locations, so
#		                       don't expect the first few bytes to have the
#		                       JMM_SHRED value.

#		                       Generally, you'll want to use different initialize
#		                       and shred values so you can look at a piece of
#		                       memory and see at a glance whether the manager
#		                       thinks it is allocated or deallocated.

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

#		JMM_DISALLOW_DELETE_NULL If this environment variable is set to "yes" the
#		                       manager will consider deletion of a null pointer to
#		                       be an error, in spite of ANSI.  Most people delete
#		                       NULL frequently, so the default is to allow it, but
#		                       those with a particularly clean (not to say
#		                       obsessive) style may find this useful.  Overridden
#		                       by the SetDisllowDeleteNULL method.

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

#		MALLOC_CHECK_          Setting this environment variable turns on checking
#		                       in libc.  1 => print error  2 => abort

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
	better than arrays. For building the tree deallocation didn't cost much
	extra with arrays, though if the program would have kept running it
	eventually would have.

	BASE CLASS = virtual public JBroadcaster

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

//Class Header
#include <JMemoryManager.h>

#include <ctype.h>
#include <stdlib.h>
#include <new>

#include <JMMArrayTable.h>
#include <JMMHashTable.h>
#include <JMMErrorPrinter.h>

#include <ace/Connector.h>
#include <ace/LSOCK_Connector.h>
#include <JMMDebugErrorStream.h>
#include <jFileUtil.h>
#include <fstream>

#include <jErrno.h>
#include <jAssert.h>
#include <jNew.h>

typedef ACE_Connector<JMemoryManager::DebugLink, ACE_LSOCK_CONNECTOR>	DebugLinkConnector;

static const JCharacter* kDisconnectStr = "JMemoryManager::DebugLinkDisconnect";
const JSize kDisconnectStrLength        = strlen(kDisconnectStr);

// Broadcaster message types

	const JCharacter* JMemoryManager::kObjectDeletedAsArray = "ObjectDeletedAsArray::JMemoryManager";
	const JCharacter* JMemoryManager::kArrayDeletedAsObject = "ArrayDeletedAsObject::JMemoryManager";

	const JCharacter* JMemoryManager::kUnallocatedDeletion = "UnallocatedDeletion::JMemoryManager";
	const JCharacter* JMemoryManager::kMultipleDeletion = "MultipleDeletion::JMemoryManager";

	const JCharacter* JMemoryManager::kMultipleAllocation = "MultipleAllocation::JMemoryManager";

	const JCharacter* JMemoryManager::kNULLDeleted = "NULLDeleted::JMemoryManager";

// Constants

	const unsigned char defaultAllocateGarbage = 0xA7;   // A for Allocate :-)
	const unsigned char defaultDeallocateGarbage = 0xD7; // D for Delete   :-)

// Static member data
	static const JSize theStackMax = 15;

	JBoolean  JMemoryManager::theConstructingFlag = kJFalse;
	JMMRecord JMemoryManager::theAllocStack[theStackMax];
	JSize     JMemoryManager::theAllocStackSize = 0;

	JBoolean JMemoryManager::theInternalFlag = kJFalse;

	JMemoryManager::DeleteRequest JMemoryManager::theDeallocStack[theStackMax];
	JSize        JMemoryManager::theDeallocStackSize = 0;

	JBoolean      JMemoryManager::theInitializeFlag = kJFalse;
	unsigned char JMemoryManager::theAllocateGarbage = defaultAllocateGarbage;

	JBoolean      JMemoryManager::theAbortUnknownAllocFlag = kJFalse;

	const JCharacter* JMemoryManager::kUnknownFile = "<UNKNOWN>";

// static functions

	void JMMHandleExit();
	void JMMHandleACEExit(void*, void*);

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

JMemoryManager::JMemoryManager()
	:
	itsMemoryTable(NULL),
	itsErrorPrinter(NULL),
	itsLink(NULL),
	itsErrorStream(NULL),
	itsExitStatsFileName(NULL),
	itsExitStatsStream(NULL),
	itsRecursionDepth(0),
	itsLastDeleteFile(kUnknownFile),
	itsLastDeleteLine(0),
	itsBroadcastErrorsFlag(kJFalse),
	itsPrintExitStatsFlag(kJFalse),
	itsPrintInternalStatsFlag(kJFalse),
	itsShredFlag(kJFalse), // Dummy initialization
	itsDeallocateGarbage(defaultDeallocateGarbage),
	itsTagSize(8),
	itsCheckDoubleAllocationFlag(kJFalse),
	itsDisallowDeleteNULLFlag(kJFalse)
{
	// Instance() must set the flag
	assert(theConstructingFlag == kJTrue);

	const JCharacter* abortUnknownAlloc = getenv("JMM_ABORT_UNKNOWN_ALLOC");
	if (abortUnknownAlloc != NULL && JStringCompare(abortUnknownAlloc, "yes", kJFalse) == 0)
		{
		theAbortUnknownAllocFlag = kJTrue;
		}

	const JCharacter* broadcastErrors = getenv("JMM_BROADCAST_ERRORS");
	if (broadcastErrors != NULL && JStringCompare(broadcastErrors, "yes", kJFalse) == 0)
		{
		itsBroadcastErrorsFlag = kJTrue;
		}

	const JCharacter* printExitStats = getenv("JMM_PRINT_EXIT_STATS");
	if (printExitStats != NULL && JStringCompare(printExitStats, "yes", kJFalse) == 0)
		{
		itsPrintExitStatsFlag = kJTrue;
		}

	const JCharacter* printInternalStats = getenv("JMM_PRINT_INTERNAL_STATS");
	if (printInternalStats != NULL && JStringCompare(printInternalStats, "yes", kJFalse) == 0)
		{
		itsPrintInternalStatsFlag = kJTrue;
		}

	ReadValue(&theInitializeFlag, &theAllocateGarbage, getenv("JMM_INITIALIZE"));
	ReadValue(&itsShredFlag, &itsDeallocateGarbage, getenv("JMM_SHRED"));
	const JCharacter* checkDoubleAllocation = getenv("JMM_CHECK_DOUBLE_ALLOCATION");
	if (checkDoubleAllocation != NULL && JStringCompare(checkDoubleAllocation, "yes", kJFalse) == 0)
		{
		itsCheckDoubleAllocationFlag = kJTrue;
		}

	const JCharacter* disallowDeleteNULL = getenv("JMM_DISALLOW_DELETE_NULL");
	if (disallowDeleteNULL != NULL && JStringCompare(disallowDeleteNULL, "yes", kJFalse) == 0)
		{
		itsDisallowDeleteNULLFlag = kJTrue;
		}

	const JCharacter* recordAllocated = getenv("JMM_RECORD_ALLOCATED");
	if (recordAllocated != NULL && JStringCompare(recordAllocated, "yes", kJFalse) == 0)
		{
		const JCharacter* tableType = getenv("JMM_TABLE_TYPE");
		JCharacter* recordDeallocated = getenv("JMM_RECORD_DEALLOCATED");
		JBoolean recordDeallocatedFlag = kJFalse;
		if (recordDeallocated != NULL && JStringCompare(recordDeallocated, "yes", kJFalse) == 0)
			{
			recordDeallocatedFlag = kJTrue;
			}

		if (tableType != NULL && JStringCompare(tableType, "array", kJFalse) == 0)
			{
			itsMemoryTable = new JMMArrayTable(this, recordDeallocatedFlag);
			}
		else
			{
			itsMemoryTable = new JMMHashTable(this, recordDeallocatedFlag);
			}

		assert(itsMemoryTable != NULL);
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

	// For form's sake, write it correctly

	if (itsMemoryTable != NULL)
		{
		delete itsMemoryTable;
		itsMemoryTable = NULL;
		}

	delete itsErrorPrinter;
	// ACE deletes the link on exit
	delete itsErrorStream;
	delete itsExitStatsFileName;
	delete itsExitStatsStream;
}

/******************************************************************************
 Instance (static)

 *****************************************************************************/

JMemoryManager*
JMemoryManager::Instance()
{
	// Guarantees access is only through this function
	static JMemoryManager* manager = NULL;

	if (manager == NULL)
		{
		theConstructingFlag = kJTrue;
		manager = new JMemoryManager;
		assert(manager != NULL);

		// Create the error printer proxy to do the printing work.
		// Construction of the error printer must take place here, after
		// the manager is fully constructed; the recursive call to Instance
		// is harmless.
		manager->itsErrorPrinter = new JMMErrorPrinter;
		assert(manager->itsErrorPrinter != NULL);

		const JCharacter* pipeName = getenv("JMM_PIPE");
		if (!JStringEmpty(pipeName))
			{
			manager->itsErrorStream = new JMMDebugErrorStream;
			assert(manager->itsErrorStream != NULL);
			}

		theConstructingFlag = kJFalse;
		manager->EmptyStacks();

		// do it here since it calls delete as well as new

		if (!JStringEmpty(pipeName))
			{
			manager->ConnectToDebugger(pipeName);
			ACE_Object_Manager::at_exit(NULL, ::JMMHandleACEExit, NULL);

			// If we create the file when we actually need it, it
			// re-constructs JStringManager.

			JString fileName;
			const JError err = JCreateTempFile(&fileName);
			if (err.OK())
				{
				theInternalFlag = kJTrue;

				manager->itsExitStatsFileName = new JString(fileName);
				assert( manager->itsExitStatsFileName != NULL );

				theInternalFlag = kJFalse;
				}
			else
				{
				cerr << "Failed to create exit stats file:" << endl;
				cerr << err.GetMessage() << endl;
				}
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
	const size_t      size,
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	if (theAbortUnknownAllocFlag && line == 0)
		{
		cout << "Memory allocated by unknown code, aborting...\n" << endl;
		abort();
		}

	const size_t trueSize = size ? size : 1;
	void* newBlock = malloc(trueSize);

	if (newBlock == NULL)
		{
		cout << "failed to allocate block of size " << trueSize << endl;
		}

	assert(newBlock != NULL);
	if (theInitializeFlag)
		{
		memset(newBlock, theAllocateGarbage, trueSize);
		}

	const JBoolean isManager = JI2B(theConstructingFlag || Instance()->itsRecursionDepth > 0 || theInternalFlag);
	JMMRecord newRecord(GetNewID(), newBlock, trueSize, file, line, isArray, isManager);

	if (theConstructingFlag || Instance()->itsRecursionDepth > 0)
		{
		assert(theAllocStackSize < theStackMax);
		theAllocStack[theAllocStackSize] = newRecord;
		theAllocStackSize++;
		}
	else
		{
		Instance()->AddNewRecord(newRecord);
		}

	return newBlock;
}

/******************************************************************************
 Delete

 *****************************************************************************/

void
JMemoryManager::Delete
	(
	void*          block,
	const JBoolean isArray
	)
{
	if (theConstructingFlag || Instance()->itsRecursionDepth > 0)
		{
		DeleteRequest thisRequest;
		thisRequest.address = block;
		thisRequest.file    = itsLastDeleteFile;
		thisRequest.line    = itsLastDeleteLine;
		thisRequest.array   = isArray;

		assert(theDeallocStackSize < theStackMax);
		theDeallocStack[theDeallocStackSize] = thisRequest;
		theDeallocStackSize++;

		// Don't delete while on the request stack so that the MemoryTable entry
		// doesn't lie; is there a better way?
		}
	else
		{
		DeleteRecord(block, itsLastDeleteFile, itsLastDeleteLine, isArray);
		}

	itsLastDeleteFile = kUnknownFile;
	itsLastDeleteLine = 0;
}

/******************************************************************************
 PrintMemoryStats

 *****************************************************************************/

void
JMemoryManager::PrintMemoryStats()
{
	if (itsMemoryTable != NULL)
		{
		cout << "\n   Dynamic memory usage:\n";
#if 0
		cout << "\n";
		cout << "      Tag memory per block: " << 2*itsTagSize << " bytes" << "\n";
		cout << "\n";
#endif
		JSize count = itsMemoryTable->GetAllocatedCount();
		cout << "      Blocks currently allocated: " << count << "\n";
		count = itsMemoryTable->GetDeletedCount();
		cout << "                  Deleted blocks: " << count << "\n";
		count = itsMemoryTable->GetTotalCount();
		cout << "                    Total blocks: " << count << "\n";
		cout << "\n";
#if 0
		JSize currentMemory = 0;
		for (JSize i=1;i<=blockCount;i++)
			{
			currentMemory += itsActiveRecordTable->GetElement(i).GetSize();
			}
		cout << "      Current memory allocated: " << currentMemory << " bytes\n";
		cout << "        Total memory allocated: " << itsTotalUserMemory << " bytes\n";
		cout << "    Total tag memory allocated: " << itsTotalBlocks*2*itsTagSize << " bytes\n";
		cout << "        Total memory allocated: "
			 << itsTotalUserMemory + itsTotalBlocks*2*itsTagSize << " bytes\n";
		cout << "\n";
		size_t managerMemory = sizeof(*this);
		managerMemory += sizeof(*itsActiveRecordTable);
		// Need space for array's internal list?
		cout << "      Additional memory used by memory manager: " << managerMemory << " bytes\n";
#endif
		cout << endl;
		}
}

/******************************************************************************
 CancelRecordAllocated

	Discards (permanently) all allocation records.  Cancels JMM_RECORD_ALLOCATED.
	Implies CancelRecordDeallocated.

 *****************************************************************************/

void
JMemoryManager::CancelRecordAllocated()
{
	if (itsMemoryTable != NULL)
		{
		BeginRecursiveBlock();
		delete itsMemoryTable;
		itsMemoryTable = NULL;
		EndRecursiveBlock();
		}
}

/******************************************************************************
 CancelRecordDeallocated

	Discards (permanently) all deletion records.  Cancels JMM_RECORD_DEALLOCATED.

 *****************************************************************************/

void
JMemoryManager::CancelRecordDeallocated()
{
	if (itsMemoryTable != NULL)
		{
		itsMemoryTable->CancelRecordDeallocated();
		}
}

/******************************************************************************
 PrintAllocated

 *****************************************************************************/

void
JMemoryManager::PrintAllocated() const
{
	if (itsMemoryTable != NULL)
		{
		itsMemoryTable->PrintAllocated(itsPrintInternalStatsFlag);
		}
}

/******************************************************************************
 GetPrintErrors

 *****************************************************************************/

JBoolean
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
	const JBoolean print
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
	if (itsMemoryTable != NULL)
		{
		itsMemoryTable->AddNewRecord(record, itsCheckDoubleAllocationFlag);
		}
}

/******************************************************************************
 DeleteRecord (private)

 *****************************************************************************/

void
JMemoryManager::DeleteRecord
	(
	void*             block,
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	if (block == NULL)
		{
		HandleNULLDeleted(file, line, isArray);
		}
	else
		{
		JBoolean wasAllocated;
		if (itsMemoryTable != NULL)
			{
			JMMRecord record;
			wasAllocated = itsMemoryTable->SetRecordDeleted(&record, block,
															file, line, isArray);
			// Can't do this unless we're keeping records
			if (itsShredFlag && wasAllocated)
				{
				assert(record.GetAddress() == block);
				memset(block, itsDeallocateGarbage, record.GetSize() );
				}
			}
		else
			{
			wasAllocated = kJTrue; // Have to trust the client
			}

		// Try to avoid a seg fault so the program can continue
		if (wasAllocated)
			{
			free(block);
			}
		}
}

/******************************************************************************
 BeginRecursiveBlock (private static)

 *****************************************************************************/

void
JMemoryManager::BeginRecursiveBlock()
{
	Instance()->itsRecursionDepth++;
	if (Instance()->itsRecursionDepth > 1)
	{
		cout << "Unusual (but probably safe) memory manager behavior: JMM recursing at depth "
			 << Instance()->itsRecursionDepth << ".  Please notify the author." << endl;
	}
}

/******************************************************************************
 EndRecursiveBlock (private static)

 *****************************************************************************/

void
JMemoryManager::EndRecursiveBlock()
{
	JMemoryManager* manager = Instance();
	manager->itsRecursionDepth--;
	manager->EmptyStacks();
}

/******************************************************************************
 EmptyStacks (private)

 *****************************************************************************/

void
JMemoryManager::EmptyStacks()
{
	if (itsMemoryTable != NULL && itsRecursionDepth == 0)
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
 GetNewID (private)

 *****************************************************************************/

JUInt32
JMemoryManager::GetNewID()
{
	// Guarantees access is only through this function
	static JUInt32 nextID = 0;

	return nextID++;
}

/******************************************************************************
 LocateDelete (private)

 *****************************************************************************/

void
JMemoryManager::LocateDelete
	(
	const JCharacter* file,
	const JUInt32     line
	)
{
	itsLastDeleteFile = file;
	itsLastDeleteLine = line;
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
	if (itsLink != NULL)
		{
		SendExitStats();
		itsLink->SendDisconnect();
		itsLink->Flush();
		itsLink = NULL;
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
	if (itsLink != NULL)
		{
		HandleACEExit();
		}

	if (itsExitStatsStream != NULL)
		{
		WriteExitStats();
		}

	if (itsPrintExitStatsFlag)
		{
		cout << "\n*******************************************************************" << endl;
		cout << "\n\nJCore Exit Statistics: anything unallocated is probably garbage" << "\n";
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
		HandleDebugRequest();
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
	const JCharacter* socketName
	)
{
	itsLink = new DebugLink;
	assert( itsLink != NULL );

	DebugLinkConnector* connector = new DebugLinkConnector;
	assert( connector != NULL );

	ACE_UNIX_Addr addr(socketName);
	if (connector->connect(itsLink, addr, ACE_Synch_Options::asynch) == -1 &&
		jerrno() != EAGAIN)
		{
		cerr << "error trying to connect JMemoryManager::DebugLink: " << jerrno() << endl;
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
	assert( itsLink != NULL );

	JString text;
	const JBoolean ok = itsLink->GetNextMessage(&text);
	assert( ok );

	std::string s(text, text.GetLength());
	std::istringstream input(s);

	JFileVersion vers;
	input >> vers;
	if (vers != kJMemoryManagerDebugVersion)
		{
		cerr << "JMemoryManager::HandleDebugRequest received version (" << vers;
		cerr << ") different than expected (" << kJMemoryManagerDebugVersion << ")" << endl;
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
	ostream& output
	)
	const
{
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
	istream& input
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
	ostream&			output,
	const RecordFilter&	filter
	)
	const
{
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
	if (!itsExitStatsFileName->IsEmpty())
		{
		std::ostringstream output;
		output << kJMemoryManagerDebugVersion;
		output << ' ' << kExitStatsMessage;
		output << ' ' << *itsExitStatsFileName;

		SendDebugMessage(output);

		theInternalFlag = kJTrue;

		itsExitStatsStream = new std::ofstream(*itsExitStatsFileName);
		assert( itsExitStatsStream != NULL );

		theInternalFlag = kJFalse;
		}
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
	*itsExitStatsStream << ' ';
	WriteRunningStats(*itsExitStatsStream);

	RecordFilter filter;
	*itsExitStatsStream << ' ';
	WriteRecords(*itsExitStatsStream, filter);

	delete itsExitStatsStream;
	itsExitStatsStream = NULL;
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
	if (mgr->itsLink != NULL)
		{
		std::ostringstream output;
		output << kJMemoryManagerDebugVersion;
		output << ' ' << kErrorMessage;
		output << ' ' << msg;

		mgr->SendDebugMessage(output);
		}
	else if (mgr->itsExitStatsStream != NULL)
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
	JString s1 = data.str();
	JIndex i   = 1;
	while (s1.LocateNextSubstring(kDisconnectStr, &i))
		{
		s1.ReplaceSubstring(i, i+kDisconnectStrLength-1, "<ixnay on the disconnect string!>");
		}

	std::string s2 = s1.GetCString();
	itsLink->SendMessage(s2.c_str(), s2.length());
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
	link->SetProtocol("\0", 1, kDisconnectStr, kDisconnectStrLength);
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

 *****************************************************************************/

void
JMemoryManager::HandleUnallocatedDeletion
	(
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	if (itsBroadcastErrorsFlag)
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
	const JMMRecord&  thisRecord,
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
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
 HandleNULLDeleted (protected)

 *****************************************************************************/

void
JMemoryManager::HandleNULLDeleted
	(
	const JCharacter* file,
	const JUInt32     line,
	const JBoolean    isArray
	)
{
	if (itsDisallowDeleteNULLFlag && itsBroadcastErrorsFlag)
		{
		Broadcast( NULLDeleted(file, line, isArray) );
		}
}

/******************************************************************************
 ReadValue (private)

 *****************************************************************************/

void
JMemoryManager::ReadValue
	(
	JBoolean*         hasValue,
	unsigned char*    value,
	const JCharacter* string
	)
{
	*hasValue = JConvertToBoolean(string != NULL);

	if (*hasValue && JStringCompare(string, "no", kJFalse) != 0)
		{
		const JCharacter* start = string;
		while ( isspace(*start) )
			{
			++start;
			}
		if (*start != '\0') // The string has a non-space value, so try to read as a number
			{
			JCharacter* end;
			unsigned char theValue = (unsigned char) strtol(start, &end, 0);
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

JBoolean
JMemoryManager::RecordFilter::Match
	(
	const JMMRecord& record
	)
	const
{
	JBoolean match = kJTrue;

	if (!includeInternal && record.IsManagerMemory())
		{
		match = kJFalse;
		}

	if (record.GetSize() < minSize)
		{
		match = kJFalse;
		}

	const JSize newFileLength = strlen(record.GetNewFile());
	if (match && fileName != NULL && newFileLength == fileName->GetLength())
		{
		if (record.GetNewFile() != *fileName)
			{
			match = kJFalse;
			}
		}
	else if (match && fileName != NULL)
		{
		const JCharacter *s1, *s2;
		JSize l1, l2;
		if (newFileLength > fileName->GetLength())
			{
			s1 = record.GetNewFile();
			l1 = newFileLength;
			s2 = *fileName;
			l2 = fileName->GetLength();
			}
		else
			{
			s1 = *fileName;
			l1 = fileName->GetLength();
			s2 = record.GetNewFile();
			l2 = newFileLength;
			}

		if (*(s1 + l1 - l2 - 1) != ACE_DIRECTORY_SEPARATOR_CHAR ||
			strcmp(s1 + l1 - l2, s2) != 0)
			{
			match = kJFalse;
			}
		}

	return match;
}

void
JMemoryManager::RecordFilter::Read
	(
	istream& input
	)
{
	input >> includeInternal;
	input >> minSize;

	JBoolean hasFile;
	input >> hasFile;

	if (hasFile)
		{
		if (fileName == NULL)
			{
			fileName = new JString;
			assert( fileName != NULL );
			}

		input >> *fileName;
		}
}

void
JMemoryManager::RecordFilter::Write
	(
	ostream& output
	)
	const
{
	output << includeInternal;
	output << ' ' << minSize;

	const JBoolean hasFile = JI2B(fileName != NULL && !fileName->IsEmpty());
	output << ' ' << hasFile;
	if (hasFile)
		{
		output << ' ' << *fileName;
		}
}
