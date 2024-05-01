/******************************************************************************
 JUpdateChecker.cpp

	Retrieves the program's version number from the server.

	BASE CLASS = ACE_Svc_Handler, JPrefObject

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "JUpdateChecker.h"
#include "JProcess.h"
#include "jStreamUtil.h"
#include "jGlobals.h"
#include "jAssert.h"

const time_t kServerCheckInterval     = 7*24*3600;		// 1 week (seconds)
const time_t kInitialReminderInterval = 30*24*3600;		// 1 month (seconds)
const time_t kDefaultReminderInterval = 6*30*24*3600;	// 6 months (seconds)

const JFileVersion kCurrentStateVersion = 1;

	// version  1 stores the next time to contact the version server
	// version  0 stores version from server + sorted list of reminder times

/******************************************************************************
 Constructor

 ******************************************************************************/

JUpdateChecker::JUpdateChecker
	(
	JPrefsManager*	prefsMgr,
	const JPrefID&	prefID
	)
	:
	JPrefObject(prefsMgr, prefID),
	itsNextServerTime(0)
{
	itsReminderList = jnew JArray<time_t>;

	JPrefObject::ReadPrefs();

	const JString vers = JGetString("VERSION");
	if (itsVersion.IsEmpty())
	{
		itsVersion = vers;
	}
	else if (itsVersion == vers)
	{
		itsReminderList->RemoveAll();
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JUpdateChecker::~JUpdateChecker()
{
	jdelete itsReminderList;
}

/******************************************************************************
 CheckForNewerVersion

	Retrieve the latest version from the server.

 ******************************************************************************/

void
JUpdateChecker::CheckForNewerVersion()
{
	const time_t now  = time(nullptr);
	if (now < itsNextServerTime)
	{
		return;
	}

	const JUtf8Byte* argv[] = { "curl", "-L", JGetString("VERSION_URL").GetBytes(), nullptr };

	JProcess* p;
	int fd;
	const JError err =
		JProcess::Create(&p, argv, sizeof(argv),
						 kJIgnoreConnection, nullptr,
						 kJCreatePipe, &fd,
						 kJTossOutput);
	if (!err.OK())
	{
		std::cerr << err.GetMessage() << std::endl;
		return;
	}

	JString vers;
	if (!JReadAll(fd, &vers))
	{
		std::cerr << "Failed to read latest version from curl" << std::endl;
		return;
	}
	vers.TrimWhitespace();

	itsNextServerTime = now + kServerCheckInterval;

	if (vers == JGetString("VERSION"))
	{
		itsReminderList->RemoveAll();
	}
	else if (vers != itsVersion)
	{
		const time_t delta = kInitialReminderInterval;

		itsReminderList->RemoveAll();
		itsReminderList->AppendItem(now);		// remind next time we start
		itsReminderList->AppendItem(now + delta);
		itsReminderList->AppendItem(now + 2*delta);
		itsReminderList->AppendItem(now + 3*delta);
	}

	itsVersion = vers;
	JPrefObject::WritePrefs();
}

/******************************************************************************
 TimeToRemind

 ******************************************************************************/

bool
JUpdateChecker::TimeToRemind()
{
	bool remind = false;

	const time_t now = time(nullptr);
	while (!itsReminderList->IsEmpty())
	{
		const time_t t = itsReminderList->GetFirstItem();
		if (now >= t)
		{
			itsReminderList->RemoveItem(1);
			remind = true;
		}
		else
		{
			break;
		}
	}

	if (remind)
	{
		if (itsReminderList->IsEmpty())
		{
			itsReminderList->AppendItem(now + kDefaultReminderInterval);
		}

		JPrefObject::WritePrefs();
	}

	return remind;
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
JUpdateChecker::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentStateVersion)
	{
		return;
	}

	input >> itsVersion;

	if (vers >= 1)
	{
		input >> itsNextServerTime;
	}

	JSize count;
	input >> count;

	for (JIndex i=1; i<=count; i++)
	{
		time_t t;
		input >> t;
		itsReminderList->AppendItem(t);
	}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
JUpdateChecker::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentStateVersion;
	output << ' ' << itsVersion;
	output << ' ' << itsNextServerTime;

	output << ' ' << itsReminderList->GetItemCount();

	for (auto t : *itsReminderList)
	{
		output << ' ' << t;
	}
}
