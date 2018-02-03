/******************************************************************************
 GARMBoxHeaderList.cc

	BASE CLASS = public JPtrArray <GMessageHeader>

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

//Class Header
#include <GARMBoxHeaderList.h>
#include <GMScanner.h>
#include <GPrefsMgr.h>
#include <GMGlobals.h>

#include <JProgressDisplay.h>
#include <JString.h>
#include <JUserNotification.h>
#include <JCreateProgressDisplay.h>
#include <JRegex.h>

#include <jXGlobals.h>

#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>


/******************************************************************************
 Static OkToCreate function

 *****************************************************************************/

JBoolean
GARMBoxHeaderList::OkToCreate
	(
	const JString&			mailfile
	)
{
	if (!JFileExists(mailfile))
		{
		JString smbox	= GGetPrefsMgr()->GetDefaultInbox();
		if (mailfile == smbox)
			{
			return kJTrue;
			}
		JString notice	= "The file \"" + mailfile + "\" does not exist.";
		JGetUserNotification()->ReportError(notice);
		return kJFalse;
		}
	if (!JFileReadable(mailfile))
		{
		JString notice = "The file \"" + mailfile + "\" is not readable.";
		JGetUserNotification()->ReportError(notice);
		return kJFalse;
		}
	mode_t perms;
	JError err = JGetPermissions(mailfile, &perms);
	if (!err.OK())
		{
		perms = 0600;
		}
	std::ifstream is(mailfile);
	is >> std::ws;
	JString line1 = JReadLine(is);
	is.close();
	JBoolean matched;
	JArray<JIndexRange>* subList = new JArray<JIndexRange>;
	assert(subList != NULL);
//	err = regex.SetPattern("^From[[:space:]]+.*.{3}[[:space:]]+.{3}[[:space:]]+[[:digit:]]+[[:space:]]+[[:digit:]]+:[[:digit:]]+:[[:digit:]]+[[:space:]]+[[:digit:]]{4}");
//	err = regex.SetPattern("^From[[:space:]]+.*.{3}[[:space:]]+.{3}[[:space:]]+[[:digit:]]+[[:space:]]+([[:digit:]]+:)+[[:digit:]]+[[:space:]]+(...[[:space:]])?[[:digit:]]{4}");
//	assert(err.OK());
	matched = GMGetMailRegex().Match(line1, subList);
	delete subList;
	if (matched || line1 == "")
		{
		return kJTrue;
		}
	else
		{
		JString notice = "The file \"" + mailfile + "\" is not a mail file.";
		JGetUserNotification()->ReportError(notice);
		return kJFalse;
		}
	return kJFalse;
}

/******************************************************************************
 Static Create function

 *****************************************************************************/

JBoolean
GARMBoxHeaderList::Create
	(
	const JString&			mailfile,
	GARMBoxHeaderList**	list,
	JProgressDisplay*		pg
	)
{
	JBoolean ok = OkToCreate(mailfile);
	if (ok)
		{
		JString bakfile = mailfile + "~";
		if (!JFileExists(bakfile))
			{
			JString cmd = "cp \"" + mailfile + "\" \"" + bakfile + "\"";
//			JExecute(cmd, NULL);
			}
		*list = new GARMBoxHeaderList(mailfile, pg);
		assert(*list != NULL);
		}
	else
		{
		*list = NULL;
		}
	return ok;
}

/******************************************************************************
 Constructor

 *****************************************************************************/

GARMBoxHeaderList::GARMBoxHeaderList
	(
	const JString&		mailfile,
	JProgressDisplay*	pg
	)
	:
	JPtrArray<GMessageHeader>(JPtrArrayT::kForgetAll)
{
	JSize size;
	const JError sizeErr = JGetFileLength(mailfile, &size);
//	assert(sizeErr.OK());
	JSize pgcount = 0;
	pg->FixedLengthProcessBeginning(size, "Reading mail file.", kJFalse, kJFalse);
	itsMailFile = mailfile;
	itsIsWritable = JFileWritable(mailfile);
	mode_t perms;
	JError err = JGetPermissions(mailfile, &perms);
	if (!err.OK())
		{
		perms = 0600;
		}
//	JString data;
//	JReadFile(mailfile, &data);
//	jistrstream(is, data.GetCString(), data.GetLength());
	std::ifstream is(mailfile);
	GMScanner scanner(&is);
	JIndex token;
	GMessageHeader* header = NULL;
	while ((token = scanner.yylex()) != 0)
		{
		if (token == kHeaderStart)
			{
			if (header != NULL)
				{
				JSize current = scanner.GetCurrentHeaderStart();
				header->SetMessageEnd(current);
//				current = current * 20/size;
//				while ((pgcount <= current) && (pgcount <= 20))
				if (current < size)
					{
//					std::cout << pgcount << std::endl;
					pg->IncrementProgress(current - pg->GetCurrentStepCount());
//					pgcount++;
					}
				}
			header = new GMessageHeader();
			assert(header != NULL);
			Append(header);
			header->SetHeader(scanner.YYText());
			header->SetHeaderStart(scanner.GetCurrentHeaderStart());
			}
		else if (token == kHeaderEnd)
			{
			assert(header != NULL);
			header->SetHeaderEnd(scanner.GetCurrentPosition());
			}
		else if (token == kFrom)
			{
			assert(header != NULL);
			header->SetFrom(scanner.YYText());
			}
		else if (token == kTo)
			{
			assert(header != NULL);
			header->SetTo(scanner.YYText());
			}
		else if (token == kSubject)
			{
			assert(header != NULL);
			header->SetSubject(scanner.YYText());
			}
		else if (token == kDate)
			{
			assert(header != NULL);
			header->SetDate(scanner.YYText());
			}
		else if (token == kCC)
			{
			assert(header != NULL);
			header->SetCC(scanner.YYText());
			}
		else if (token == kReplyTo)
			{
			assert(header != NULL);
			header->SetReplyTo(scanner.YYText());
			}
		else if (token == kStatus)
			{
			assert(header != NULL);
			JString status(scanner.YYText());
			if (status.Contains("R") &&
				(header->GetMessageStatus() == GMessageHeader::kNew))
				{
				header->SetMessageStatus(GMessageHeader::kRead);
				}
			}
		else if (token == kMIME)
			{
			assert(header != NULL);
			header->SetIsMIME(kJTrue);
			}
		else if (token == kContentType)
			{
			assert(header != NULL);
			header->SetIsMIME(kJTrue);
			JString str(scanner.YYText());
			str.ToLower();
			if (str.Contains("multipart/"))
				{
				if (!str.Contains("multipart/alternative"))
					{
					header->SetHasAttachment(kJTrue);
					}
				}
			else if (str.Contains("name"))
				{
				header->SetHasAttachment(kJTrue);
				}
			}
		else if (token == kContentDisposition)
			{
			assert(header != NULL);
			JString str(scanner.YYText());
			str.ToLower();
			if (str.Contains("filename"))
				{
				header->SetHasAttachment(kJTrue);
				}
			}
		else if (token == kContentEncoding)
			{
			assert(header != NULL);
			JString str(scanner.YYText());
			str.ToLower();
			if (str.Contains("base64"))
				{
				header->SetHasAttachment(kJTrue);
				}
			}
		else if (token == kReplyStatus)
			{
			assert(header != NULL);
			JString str(scanner.YYText());
			str.TrimWhitespace();
			if (str == "R")
				{
				header->SetReplyStatus(GMessageHeader::kReplied);
				}
			else if (str == "RS")
				{
				header->SetReplyStatus(GMessageHeader::kRepliedSender);
				}
			else if (str == "RA")
				{
				header->SetReplyStatus(GMessageHeader::kRepliedAll);
				}
			else if (str == "F")
				{
				header->SetReplyStatus(GMessageHeader::kForwarded);
				}
			else if (str == "RD")
				{
				header->SetReplyStatus(GMessageHeader::kRedirected);
				}
			}
		else if (token == kAccount)
			{
			assert(header != NULL);
			header->SetAccount(scanner.YYText());
			}
		else if (token == kXReplyStatus)
			{
			assert(header != NULL);
			JString str(scanner.YYText());
			str.TrimWhitespace();
			if (str == "A" &&
				header->GetReplyStatus() == GMessageHeader::kNoReply)
				{
				header->SetReplyStatus(GMessageHeader::kReplied);
				}
			}
		}
	if (header != NULL)
		{
		JSize current = scanner.GetCurrentPosition();
		header->SetMessageEnd(current);
//		current = current * 20/size;
//		while (pgcount <= current)
//			{
//			std::cout << "Last: " << pgcount << std::endl;
//			pg->IncrementProgress();
//			pgcount++;
//			}
		}
	is.close();
	pg->ProcessFinished();
}

GARMBoxHeaderList::GARMBoxHeaderList()
	:
	JPtrArray<GMessageHeader>(JPtrArrayT::kForgetAll)
{

}

/******************************************************************************
 Destructor

 *****************************************************************************/

GARMBoxHeaderList::~GARMBoxHeaderList()
{
}
