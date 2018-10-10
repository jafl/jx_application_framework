/******************************************************************************
 GPMProcessEntry.h

	Interface for the GPMProcessEntry class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GPMProcessEntry
#define _H_GPMProcessEntry

#include <JNamedTreeNode.h>
#include <sys/types.h>

#ifdef _J_HAS_SYSCTL
#include <sys/sysctl.h>
#endif

class JDirEntry;

class GPMProcessEntry : public JNamedTreeNode
{
public:

	enum State
	{
		kSleep	= 1,
		kUnIntSleep,
		kRun,
		kZombie,
		kStopped
	};

public:

	#ifdef _J_HAS_PROC
	GPMProcessEntry(JTree* tree, const JDirEntry& entry);
	#elif defined _J_HAS_SYSCTL
	GPMProcessEntry(JTree* tree, const kinfo_proc& entry);
	#endif

	GPMProcessEntry(JTree* tree, const JString& prefix);

	virtual ~GPMProcessEntry();

	void	Update(const JFloat elapsedTime);

	State			GetState() const;
	uid_t			GetUID() const;
	const JString&	GetUser() const;
	pid_t			GetPID() const;
	const JString&	GetCommand() const;
	const JString&	GetFullCommand() const;
	pid_t			GetPPID() const;
	JInteger		GetPriority() const;
	JInteger		GetNice() const;
	JSize			GetSize() const;
	JSize			GetResident() const;
	JSize			GetShare() const;
	JSize			GetTime() const;
	JFloat			GetPercentMemory() const;
	JFloat			GetPercentCPU() const;

	static JListT::CompareResult
		CompareListPID(GPMProcessEntry * const & e1, GPMProcessEntry * const & e2);
	static JListT::CompareResult
		CompareListUser(GPMProcessEntry * const & e1, GPMProcessEntry * const & e2);
	static JListT::CompareResult
		CompareListNice(GPMProcessEntry * const & e1, GPMProcessEntry * const & e2);
	static JListT::CompareResult
		CompareListSize(GPMProcessEntry * const & e1, GPMProcessEntry * const & e2);
	static JListT::CompareResult
		CompareListPercentMemory(GPMProcessEntry * const & e1, GPMProcessEntry * const & e2);
	static JListT::CompareResult
		CompareListPercentCPU(GPMProcessEntry * const & e1, GPMProcessEntry * const & e2);
	static JListT::CompareResult
		CompareListTime(GPMProcessEntry * const & e1, GPMProcessEntry * const & e2);
	static JListT::CompareResult
		CompareListCommand(GPMProcessEntry * const & e1, GPMProcessEntry * const & e2);
	static JListT::CompareResult
		CompareListCommandForIncrSearch(GPMProcessEntry * const & e1, GPMProcessEntry * const & e2);

	static JListT::CompareResult
		CompareTreePID(JTreeNode * const & e1, JTreeNode * const & e2);
	static JListT::CompareResult
		CompareTreeUser(JTreeNode * const & e1, JTreeNode * const & e2);
	static JListT::CompareResult
		CompareTreeNice(JTreeNode * const & e1, JTreeNode * const & e2);
	static JListT::CompareResult
		CompareTreeSize(JTreeNode * const & e1, JTreeNode * const & e2);
	static JListT::CompareResult
		CompareTreePercentMemory(JTreeNode * const & e1, JTreeNode * const & e2);
	static JListT::CompareResult
		CompareTreePercentCPU(JTreeNode * const & e1, JTreeNode * const & e2);
	static JListT::CompareResult
		CompareTreeTime(JTreeNode * const & e1, JTreeNode * const & e2);
	static JListT::CompareResult
		CompareTreeCommand(JTreeNode * const & e1, JTreeNode * const & e2);

private:

	#ifdef _J_HAS_PROC
	JString			itsProcPath;
	#endif

	uid_t			itsUID;
	JString			itsUser;
	pid_t			itsPID;
	JString			itsCommand;
	JString			itsFullCommand;
	State			itsState;
	pid_t			itsPPID;
	JInteger		itsPriority;
	JInteger		itsNice;
	JSize			itsSize;
	JSize			itsResident;
	JSize			itsShare;
	JSize			itsTime;
	JFloat			itsPercentMemory;
	JFloat			itsPercentCPU;
	JSize			itsUTime;
	JSize			itsSTime;
	JSize			itsLastUTime;
	JSize			itsLastSTime;

private:

	void	ReadCmdline();

	#ifdef _J_HAS_PROC
	void	ReadStat();
	void	ReadStatM();
	#endif

	// not allowed

	GPMProcessEntry(const GPMProcessEntry& source);
	const GPMProcessEntry& operator=(const GPMProcessEntry& source);
};


/******************************************************************************
 GetState

 ******************************************************************************/

inline GPMProcessEntry::State
GPMProcessEntry::GetState()
	const
{
	return itsState;
}

/******************************************************************************
 GetUID

 ******************************************************************************/

inline uid_t
GPMProcessEntry::GetUID()
	const
{
	return itsUID;
}

/******************************************************************************
 GetUser

 ******************************************************************************/

inline const JString&
GPMProcessEntry::GetUser()
	const
{
	return itsUser;
}

/******************************************************************************
 GetPID

 ******************************************************************************/

inline pid_t
GPMProcessEntry::GetPID()
	const
{
	return itsPID;
}

/******************************************************************************
 GetCommand

 ******************************************************************************/

inline const JString&
GPMProcessEntry::GetCommand()
	const
{
	return itsCommand;
}

/******************************************************************************
 GetFullCommand

 ******************************************************************************/

inline const JString&
GPMProcessEntry::GetFullCommand()
	const
{
	return itsFullCommand;
}

/******************************************************************************
 GetPPID

 ******************************************************************************/

inline pid_t
GPMProcessEntry::GetPPID()
	const
{
	return itsPPID;
}

/******************************************************************************
 GetPriority

 ******************************************************************************/

inline JInteger
GPMProcessEntry::GetPriority()
	const
{
	return itsPriority;
}

/******************************************************************************
 GetNice

 ******************************************************************************/

inline JInteger
GPMProcessEntry::GetNice()
	const
{
	return itsNice;
}

/******************************************************************************
 GetSize

 ******************************************************************************/

inline JSize
GPMProcessEntry::GetSize()
	const
{
	return itsSize;
}

/******************************************************************************
 GetResident

 ******************************************************************************/

inline JSize
GPMProcessEntry::GetResident()
	const
{
	return itsResident;
}

/******************************************************************************
 GetShare

 ******************************************************************************/

inline JSize
GPMProcessEntry::GetShare()
	const
{
	return itsShare;
}

/******************************************************************************
 GetTime

 ******************************************************************************/

inline JSize
GPMProcessEntry::GetTime()
	const
{
	return itsTime;
}

/******************************************************************************
 GetPercentMemory

 ******************************************************************************/

inline JFloat
GPMProcessEntry::GetPercentMemory()
	const
{
	return itsPercentMemory;
}

/******************************************************************************
 GetPercentCPU

 ******************************************************************************/

inline JFloat
GPMProcessEntry::GetPercentCPU()
	const
{
	return itsPercentCPU;
}

#endif
