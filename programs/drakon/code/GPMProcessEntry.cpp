/******************************************************************************
 GPMProcessEntry.cpp

	BASE CLASS = JNamedTreeNode

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include "GPMProcessEntry.h"
#include "gpmGlobals.h"

#include <JTree.h>

#ifdef _J_HAS_PROC
#include <JDirEntry.h>
#endif

#include <JStringIterator.h>
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
	JNamedTreeNode(tree, JString::empty, false),
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
	JNamedTreeNode(tree, JString::empty, false),
	itsLastUTime(0),
	itsLastSTime(0)
{
	itsUID = entry.kp_eproc.e_pcred.p_ruid;
	itsPID = entry.kp_proc.p_pid;

	passwd* info = getpwuid(itsUID);
	if (info != nullptr)
		{
		itsUser = info->pw_name;
		}
}

#endif

// search target

GPMProcessEntry::GPMProcessEntry
	(
	JTree*			tree,
	const JString&	prefix
	)
	:
	JNamedTreeNode(tree, prefix, false)
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

#ifdef _J_HAS_PROC
	try
	{
		ReadStat();
		ReadStatM();

		JSize mem;
		if (GPMGetSystemMemory(&mem))
			{
			itsPercentMemory = JFloat(itsResident * 100) / mem;
			}

		// shared across #if
		ReadCmdline();	// not in ctor, to make ctor faster
	}
	catch (...)
	{
		itsState = kZombie;
//		std::cerr << "failed to update: " << itsPID << std::endl;
	}
#elif defined _J_HAS_SYSCTL
	{
	int mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, itsPID };

	kinfo_proc entry;
	size_t len = sizeof(entry);
	int result = sysctl(mib, 4, &entry, &len, nullptr, 0);
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

	// shared across #if
	ReadCmdline();	// not in ctor, to make ctor faster
	}
#endif

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

	JString str = JCombinePathAndName(itsProcPath, JString("stat", false));
	std::ifstream is(str.GetBytes());
	if (is.good())
		{
		is >> itsPID;
		is >> std::ws;
		itsCommand = JReadUntilws(is);
		if (itsCommand.GetCharacterCount() > 2)
			{
			JStringIterator iter(&itsCommand);
			iter.RemoveNext();
			iter.MoveTo(kJIteratorStartAtEnd, 0);
			iter.RemovePrev();
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
		is >> std::ws;
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

	JString str = JCombinePathAndName(itsProcPath, JString("statm", false));
	std::ifstream is(str.GetBytes());
	if (is.good())
		{
		is >> itsSize >> itsResident >> itsShare;
		itsSize	    *= 4;
		itsResident *= 4;
		itsShare    *= 4;
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

	JString str = JCombinePathAndName(itsProcPath, JString("cmdline", false));
	std::ifstream is(str.GetBytes());
	if (is.good())
		{
		JString cmdline;
		JReadAll(is, &cmdline);
		cmdline.TrimWhitespace();
		if (cmdline.IsEmpty())
			{
			return;
			}

		JStringIterator iter(&cmdline);
		JUtf8Character c;
		while (iter.Next(&c))
			{
			if (c == '\0')
				{
				iter.SetPrev(JUtf8Character(' '), kJIteratorStay);
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

	if (sysctl(mib, 4, nullptr, &len, nullptr, 0) == 0)
		{
		void* buf = malloc(len);
		assert( buf != nullptr );

		result = sysctl(mib, 4, buf, &len, nullptr, 0);
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

	int mib[] = { CTL_KERN, KERN_ARGMAX, 0 };

	int argmax;
	size_t len = sizeof(argmax);
	if (sysctl(mib, 2, &argmax, &len, nullptr, 0) == 0)
		{
		void* buf = malloc(argmax);
		assert( buf != nullptr );

		mib[0] = CTL_KERN;
		mib[1] = KERN_PROCARGS2;
		mib[2] = itsPID;

		len = argmax;
		if (sysctl(mib, 3, buf, &len, nullptr, 0) == 0)
			{
			int argc = * (int*) buf;
			buf      = ((char*) buf) + sizeof(argc);

			int offset = 0;
			for (int i=0; i<argc; i++)
				{
				itsFullCommand += ((char*) buf) + offset;
				itsFullCommand.Append(" ");

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
		std::cout << task_info.resident_size;
		}
*/
#endif
}

#endif

/******************************************************************************
 List comparison (static)

 ******************************************************************************/

JListT::CompareResult
GPMProcessEntry::CompareListPID
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsPID > e2->itsPID)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsPID < e2->itsPID)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
		}
}

JListT::CompareResult
GPMProcessEntry::CompareListUser
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	JListT::CompareResult	result =
		JCompareStringsCaseInsensitive(&(e1->itsUser), &(e2->itsUser));

	if (result == JListT::kFirstEqualSecond)
		{
		return CompareListPID(e1, e2);
		}
	else
		{
		return result;
		}
}

JListT::CompareResult
GPMProcessEntry::CompareListNice
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsNice > e2->itsNice)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsNice < e2->itsNice)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return CompareListPID(e1, e2);
		}
}

JListT::CompareResult
GPMProcessEntry::CompareListSize
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsSize > e2->itsSize)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsSize < e2->itsSize)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return CompareListPID(e1, e2);
		}
}

JListT::CompareResult
GPMProcessEntry::CompareListPercentMemory
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsPercentMemory > e2->itsPercentMemory)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsPercentMemory < e2->itsPercentMemory)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return CompareListPID(e1, e2);
		}
}

JListT::CompareResult
GPMProcessEntry::CompareListPercentCPU
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsPercentCPU > e2->itsPercentCPU)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsPercentCPU < e2->itsPercentCPU)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return CompareListPID(e1, e2);
		}
}

JListT::CompareResult
GPMProcessEntry::CompareListTime
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	if (e1->itsTime > e2->itsTime)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsTime < e2->itsTime)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return CompareListPID(e1, e2);
		}
}

JListT::CompareResult
GPMProcessEntry::CompareListCommand
	(
	GPMProcessEntry * const & e1,
	GPMProcessEntry * const & e2
	)
{
	JListT::CompareResult	result =
		JCompareStringsCaseInsensitive(&(e1->itsCommand), &(e2->itsCommand));

	if (result == JListT::kFirstEqualSecond)
		{
		return CompareListPID(e1, e2);
		}
	else
		{
		return result;
		}
}

JListT::CompareResult
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

JListT::CompareResult
GPMProcessEntry::CompareTreePID
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	auto * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	auto * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsPID > e2->itsPID)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsPID < e2->itsPID)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return JListT::kFirstEqualSecond;
		}
}

JListT::CompareResult
GPMProcessEntry::CompareTreeUser
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	auto * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	auto * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	JListT::CompareResult	result =
		JCompareStringsCaseInsensitive(&(e1->itsUser), &(e2->itsUser));

	if (result == JListT::kFirstEqualSecond)
		{
		return CompareTreePID(n1, n2);
		}
	else
		{
		return result;
		}
}

JListT::CompareResult
GPMProcessEntry::CompareTreeNice
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	auto * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	auto * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsNice > e2->itsNice)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsNice < e2->itsNice)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return CompareTreePID(n1, n2);
		}
}

JListT::CompareResult
GPMProcessEntry::CompareTreeSize
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	auto * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	auto * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsSize > e2->itsSize)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsSize < e2->itsSize)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return CompareTreePID(n1, n2);
		}
}

JListT::CompareResult
GPMProcessEntry::CompareTreePercentMemory
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	auto * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	auto * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsPercentMemory > e2->itsPercentMemory)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsPercentMemory < e2->itsPercentMemory)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return CompareTreePID(n1, n2);
		}
}

JListT::CompareResult
GPMProcessEntry::CompareTreePercentCPU
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	auto * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	auto * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsPercentCPU > e2->itsPercentCPU)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsPercentCPU < e2->itsPercentCPU)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return CompareTreePID(n1, n2);
		}
}

JListT::CompareResult
GPMProcessEntry::CompareTreeTime
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	auto * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	auto * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	if (e1->itsTime > e2->itsTime)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (e1->itsTime < e2->itsTime)
		{
		return JListT::kFirstLessSecond;
		}
	else
		{
		return CompareTreePID(n1, n2);
		}
}

JListT::CompareResult
GPMProcessEntry::CompareTreeCommand
	(
	JTreeNode * const & n1,
	JTreeNode * const & n2
	)
{
	auto * const e1 = dynamic_cast<GPMProcessEntry*const>(n1);
	auto * const e2 = dynamic_cast<GPMProcessEntry*const>(n2);

	JListT::CompareResult	result =
		JCompareStringsCaseInsensitive(&(e1->itsCommand), &(e2->itsCommand));

	if (result == JListT::kFirstEqualSecond)
		{
		return CompareTreePID(n1, n2);
		}
	else
		{
		return result;
		}
}
