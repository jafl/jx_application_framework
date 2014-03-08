/******************************************************************************
 GPMProcessEntry.cc

	BASE CLASS = JNamedTreeNode

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include "GPMProcessEntry.h"
#include "gpmGlobals.h"

#include <JTree.h>

#ifdef _J_HAS_PROC
#include <JDirEntry.h>
#endif

#include <jDirUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

#ifdef _J_HAS_PROC

GPMProcessEntry::GPMProcessEntry
	(
	JTree*				tree,
	const JDirEntry&	entry
	)
	:
	JNamedTreeNode(tree, "", kJFalse),
	itsLastUTime(0),
	itsLastSTime(0)
{
	itsProcPath = entry.GetFullName();
	itsUID      = entry.GetUserID();
	itsUser     = entry.GetUserName();
	
	JUInt value;
	entry.GetName().ConvertToUInt(&value);
	itsPID = value;
}

#elif defined _J_HAS_SYSCTL

GPMProcessEntry::GPMProcessEntry
	(
	JTree*				tree,
	const kinfo_proc&	entry
	)
	:
	JNamedTreeNode(tree, "", kJFalse),
	itsLastUTime(0),
	itsLastSTime(0)
{
	itsUID = entry.kp_eproc.e_pcred.p_ruid;
	itsPID = entry.kp_proc.p_pid;

	passwd* info = getpwuid(itsUID);
	if (info != NULL)
		{
		itsUser = info->pw_name;
		}
}

#endif

// search target

GPMProcessEntry::GPMProcessEntry
	(
	JTree*				tree,
	const JCharacter*	prefix
	)
	:
	JNamedTreeNode(tree, prefix, kJFalse)
{
	itsCommand = prefix;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GPMProcessEntry::~GPMProcessEntry()
{
}

/******************************************************************************
 Update

 ******************************************************************************/

#if defined KERN_PROCARGS2
#include <mach/mach.h>
#include <mach/task.h>
kern_return_t task_for_pid(task_port_t task, pid_t pid, task_port_t *target);
#endif

void
GPMProcessEntry::Update
	(
	const JFloat elapsedTime
	)
{
	itsPercentMemory = 0;

	try
	{
#ifdef _J_HAS_PROC
		{
		ReadStat();
		ReadStatM();

		JSize mem;
		if (GPMGetSystemMemory(&mem))
			{
			itsPercentMemory = JFloat(itsResident * 100) / mem;
			}
		}
#elif defined _J_HAS_SYSCTL
		{
		int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, itsPID };

		kinfo_proc entry;
		size_t len = sizeof(entry);
		int result = sysctl(mib, 4, &entry, &len, NULL, 0);
		if (result != 0)
			{
			itsState = kZombie;
			}
		else
			{
			itsCommand  = entry.kp_proc.p_comm;
			itsPPID     = entry.kp_eproc.e_ppid;
			itsPriority = entry.kp_proc.p_priority;
			itsNice     = entry.kp_proc.p_nice;
			itsSize     = 0;
			itsResident = 0;
			itsShare    = 0;
			itsUTime    = entry.kp_proc.p_uticks;
			itsSTime    = entry.kp_proc.p_sticks;

			if (entry.kp_proc.p_stat == SSLEEP)
				{
				itsState = kSleep;
				}
			else if (entry.kp_proc.p_stat == SSTOP)
				{
				itsState = kStopped;
				}
			else if (entry.kp_proc.p_stat == SZOMB)
				{
				itsState = kZombie;
				}
			else
				{
				itsState = kRun;
				}

			JSize mem;
			if (GPMGetSystemMemory(&mem))
				{
				itsPercentMemory = JFloat(itsResident) / mem;
				}
			}
		}
#endif

		ReadCmdline();	// not in ctor, to make ctor faster
	}
	catch (...)
	{
		itsState = kZombie;
//		cerr << "failed to update: " << itsPID << endl;
	}

	SetName(itsCommand);
	ShouldBeOpenable(HasChildren());

	itsTime	        = (itsUTime + itsSTime) / sysconf(_SC_CLK_TCK);
	JSize totalTime	= (itsLastUTime == 0 || itsLastSTime == 0) ? 0 : (itsUTime - itsLastUTime) + (itsSTime - itsLastSTime);
	itsLastUTime	= itsUTime;
	itsLastSTime	= itsSTime;
	itsPercentCPU	= elapsedTime == 0 || itsState == kZombie ? 0 : JFloat(totalTime * 1000 / sysconf(_SC_CLK_TCK)) / (10 * elapsedTime);
}

#ifdef _J_HAS_PROC

/******************************************************************************
 ReadStat (private)

 ******************************************************************************/

void
GPMProcessEntry::ReadStat()
{
	const JSize uTime = itsUTime, sTime = itsSTime;

	JString str = JCombinePathAndName(itsProcPath, "stat");
	ifstream is(str);
	if (is.good())
		{
		is >> itsPID;
		is >> ws;
		itsCommand = JReadUntilws(is);
		if (itsCommand.GetLength() > 2)
			{
			itsCommand = itsCommand.GetSubstring(2, itsCommand.GetLength() - 1);
			}
		JString state = JReadUntilws(is);
		if (state.Contains("S"))
			{
			itsState = kSleep;
			}
		else if (state.Contains("D"))
			{
			itsState = kUnIntSleep;
			}
		else if (state.Contains("Z"))
			{
			itsState = kZombie;
			}
		else if (state.Contains("T"))
			{
			itsState = kStopped;
			}
		else
			{
			itsState = kRun;
			}
		is >> itsPPID;
		is >> ws;
		int toss;
		is >> toss;
		is >> toss;
		is >> toss;
		is >> toss;
		is >> toss;
		is >> toss;
		is >> toss;
		is >> toss;
		is >> toss;
		is >> itsUTime;
		is >> itsSTime;
		is >> toss;
		is >> toss;
		is >> itsPriority;
		is >> itsNice;
		}

	if (!is.good())
		{
		itsState = kZombie;
		itsUTime = uTime;
		itsSTime = sTime;
		itsPriority = itsNice = 0;
		}
}

/******************************************************************************
 ReadStatM (private)

 ******************************************************************************/

void
GPMProcessEntry::ReadStatM()
{

	JString str = JCombinePathAndName(itsProcPath, "statm");
	ifstream is(str);
	if (is.good())
		{
		is >> itsSize;
		itsSize	*= 4;
		is >> itsResident;
		itsResident	*= 4;
		is >> itsShare;
		itsShare	*= 4;
		}

	if (!is.good())
		{
		itsState = kZombie;
		itsSize = itsResident = itsShare = 0;
		}
}

/******************************************************************************
 ReadCmdline (private)

 ******************************************************************************/

void
GPMProcessEntry::ReadCmdline()
{
	if (!itsFullCommand.IsEmpty())
		{
		return;
		}

	JString str = JCombinePathAndName(itsProcPath, "cmdline");
	ifstream is(str);
	if (is.good())
		{
		JString cmdline;
		JReadAll(is, &cmdline);
		cmdline.TrimWhitespace();
		if (cmdline.IsEmpty())
			{
			return;
			}
		const JSize count = cmdline.GetLength();
		for (JIndex i = 1; i <= count; i++)
			{
			if (cmdline.GetCharacter(i) == '\0')
				{
				cmdline.SetCharacter(i, ' ');
				}
			}
		itsFullCommand = cmdline;
		}
}

#elif defined _J_HAS_SYSCTL

/******************************************************************************
 ReadCmdline (private)

 ******************************************************************************/

void
GPMProcessEntry::ReadCmdline()
{
	if (!itsFullCommand.IsEmpty())
		{
		return;
		}

#ifdef KERN_PROC_ARGS

	int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_ARGS, itsPID };

	if (sysctl(mib, 4, NULL, &len, NULL, 0) == 0)
		{
		void* buf = malloc(len);
		assert( buf != NULL );

		result = sysctl(mib, 4, buf, &len, NULL, 0);
		if (result == 0)
			{
			JIndex i = 0;
			while (i < len)
				{
				itsFullCommand += ((char*) buf) + i;
				itsFullCommand.AppendCharacter(' ');

				i += strlen(((char*) buf) + i) + 1;
				}
			}

		free(buf);
		}

#elif defined KERN_PROCARGS2

	int mib[] = { CTL_KERN, KERN_ARGMAX };

	int argmax;
	size_t len = sizeof(argmax);
	if (sysctl(mib, 2, &argmax, &len, NULL, 0) == 0)
		{
		void* buf = malloc(argmax);
		assert( buf != NULL );

		mib[0] = CTL_KERN;
		mib[1] = KERN_PROCARGS2;
		mib[2] = itsPID;

		len = argmax;
		if (sysctl(mib, 3, buf, &len, NULL, 0) == 0)
			{
			int argc = * (int*) buf;
			buf      = ((char*) buf) + sizeof(argc);

			int offset = 0;
			for (int i=0; i<argc; i++)
				{
				itsFullCommand += ((char*) buf) + offset;
				itsFullCommand.AppendCharacter(' ');

				offset += strlen(((char*) buf) + offset) + 1;
				}
			}
		}
/*
	task_port_t task;
	task_basic_info task_info;
	unsigned int info_count = TASK_BASIC_INFO_COUNT;
	if (task_for_pid(mach_task_self(), itsPID, &task) == KERN_SUCCESS &&
		task_info(task, TASK_BASIC_INFO, &task_info, &info_count) == KERN_SUCCESS)
		{
		cout << task_info.resident_size;
		}
*/
#endif
}

#endif

/******************************************************************************
 List comparison (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
GPMProcessEntry::CompareListPID
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsPID > e2->itsPID)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsPID < e2->itsPID)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareListUser
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	JOrderedSetT::CompareResult	result =
		JCompareStringsCaseInsensitive(&(e1->itsUser), &(e2->itsUser));

	if (result == JOrderedSetT::kFirstEqualSecond)
		{
		return CompareListPID(e1, e2);
		}
	else
		{
		return result;
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareListNice
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsNice > e2->itsNice)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsNice < e2->itsNice)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareListPID(e1, e2);
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareListSize
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsSize > e2->itsSize)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsSize < e2->itsSize)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareListPID(e1, e2);
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareListPercentMemory
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsPercentMemory > e2->itsPercentMemory)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsPercentMemory < e2->itsPercentMemory)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareListPID(e1, e2);
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareListPercentCPU
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsPercentCPU > e2->itsPercentCPU)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsPercentCPU < e2->itsPercentCPU)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareListPID(e1, e2);
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareListTime
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsTime > e2->itsTime)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsTime < e2->itsTime)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareListPID(e1, e2);
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareListCommand
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	JOrderedSetT::CompareResult	result =
		JCompareStringsCaseInsensitive(&(e1->itsCommand), &(e2->itsCommand));

	if (result == JOrderedSetT::kFirstEqualSecond)
		{
		return CompareListPID(e1, e2);
		}
	else
		{
		return result;
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareListCommandForIncrSearch
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	return JCompareStringsCaseInsensitive(&(e1->itsCommand), &(e2->itsCommand));
}

/******************************************************************************
 Tree comparison (static)

 ******************************************************************************/

JOrderedSetT::CompareResult
GPMProcessEntry::CompareTreePID
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	GPMProcessEntry * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	GPMProcessEntry * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsPID > e2->itsPID)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsPID < e2->itsPID)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareTreeUser
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	GPMProcessEntry * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	GPMProcessEntry * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	JOrderedSetT::CompareResult	result =
		JCompareStringsCaseInsensitive(&(e1->itsUser), &(e2->itsUser));

	if (result == JOrderedSetT::kFirstEqualSecond)
		{
		return CompareTreePID(n1, n2);
		}
	else
		{
		return result;
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareTreeNice
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	GPMProcessEntry * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	GPMProcessEntry * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsNice > e2->itsNice)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsNice < e2->itsNice)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareTreePID(n1, n2);
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareTreeSize
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	GPMProcessEntry * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	GPMProcessEntry * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsSize > e2->itsSize)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsSize < e2->itsSize)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareTreePID(n1, n2);
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareTreePercentMemory
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	GPMProcessEntry * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	GPMProcessEntry * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsPercentMemory > e2->itsPercentMemory)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsPercentMemory < e2->itsPercentMemory)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareTreePID(n1, n2);
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareTreePercentCPU
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	GPMProcessEntry * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	GPMProcessEntry * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsPercentCPU > e2->itsPercentCPU)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsPercentCPU < e2->itsPercentCPU)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareTreePID(n1, n2);
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareTreeTime
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	GPMProcessEntry * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	GPMProcessEntry * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsTime > e2->itsTime)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (e1->itsTime < e2->itsTime)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else
		{
		return CompareTreePID(n1, n2);
		}
}

JOrderedSetT::CompareResult
GPMProcessEntry::CompareTreeCommand
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	GPMProcessEntry * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	GPMProcessEntry * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	JOrderedSetT::CompareResult	result =
		JCompareStringsCaseInsensitive(&(e1->itsCommand), &(e2->itsCommand));

	if (result == JOrderedSetT::kFirstEqualSecond)
		{
		return CompareTreePID(n1, n2);
		}
	else
		{
		return result;
		}
}
