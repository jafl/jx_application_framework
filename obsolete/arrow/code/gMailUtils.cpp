/******************************************************************************
 gMailUtils.cc

	Copyright © 1997 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <gMailUtils.h>
#include "GMessageHeaderList.h"
#include "md5.h"
#include <GMGlobals.h>

#include <JOutPipeStream.h>
#include <JRegex.h>
#include <JUserNotification.h>
#include <JPtrArray-JString.h>
#include <GAddressScanner.h>
#include <GMessageHeader.h>
#include <GMAttachmentTable.h>

#include <JRegex.h>
#include <JTextEditor.h>
#include <JDirEntry.h>

#include <jProcessUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jFStreamUtil.h>
#include <jMissingProto.h>

#include <time.h>
#include <stdio.h>
#include <sstream>
#include <jAssert.h>

static JCharacter* kMIMENotice	=
	"This message is in MIME format. Since your mail reader does not understand"
	"\r\nthis format, some or all of this message may not be legible.\r\n\r\n";

static const JRegex smtpPeriodStuff("^(\\.)+");

void	GMPrepareStringForSMTP(JString* text);

/*****************************************************************************
 ParseNameLists

 *****************************************************************************/

void GParseNameLists
	(
	const JString& to,
	const JString& cc,
	const JString& bcc,
	JPtrArray<JString>& tos,
	JPtrArray<JString>& ccs,
	JPtrArray<JString>& bccs
	)
{
	GParseNameList(to, tos);
	GParseNameList(cc, ccs);
	GParseNameList(bcc, bccs);
}

/*****************************************************************************
 ParseNameLists

 *****************************************************************************/

void GParseNameLists
	(
	const JString& to,
	const JString& cc,
	const JString& bcc,
	JPtrArray<JString>& names
	)
{
	GParseNameList(to, names);
	GParseNameList(cc, names);
	GParseNameList(bcc, names);
}


/*****************************************************************************
 GParseNameList

 *****************************************************************************/

void GParseNameList
	(
	const JString&		list,
	JPtrArray<JString>& names,
	const JBoolean		forName
	)
{
	JString temp(list);
	temp.TrimWhitespace();
	if (temp.IsEmpty())
		{
		return;
		}

	GAddressScanner scanner(list, &names);
	scanner.yylex();
	for (JSize i = 1; i <= names.GetElementCount(); i++)
		{
		JString* str = names.NthElement(i);
		if (forName)
			{
			*str = GGetName(*str);
			str->TrimWhitespace();
			}
		else
			{
			*str = GGetAddress(*str);
			str->TrimWhitespace();
			}
		}

/*
	jistrstream(is, temp, temp.GetLength());
	istrstream is(temp);
	names.SetCompareFunction(JCompareStringsCaseInsensitive);
	JString* name;

	JString* buffer = new JString();
	if (list.Contains(","))
		{
		JString parse;
		JCharacter c;
		JBoolean ok = JReadUntil(is, 3, ",(\"", &parse, &c);
		while (ok)
			{
			if (ok && c == '\"')
				{
				buffer->Append(parse);
				buffer->Append("\"");
				GReadQuote(is, buffer);
				}
			else if (ok && c == '(')
				{
				buffer->Append(parse);
				buffer->Append("(");
				GReadComment(is, buffer);
				}
			else if (ok && c == '<')
				{
				buffer->Append(parse);
				buffer->Append("<");
				GReadBracket(is, buffer);
				}
			else
				{
				buffer->Append(parse);
				if (forName)
					{
					name = new JString(GGetName(*buffer));
					}
				else
					{
					name = new JString(GGetAddress(*buffer));
					}
				assert(name != NULL);
				name->TrimWhitespace();
				names.InsertSorted(name, kJFalse);
				buffer->Clear();
				}
			ok = JReadUntil(is, 4, ",(<\"", &parse, &c);
			}

		buffer->Append(parse);
		}
	else
		{
		*buffer = list;
		}
	if (forName)
		{
		name = new JString(GGetName(*buffer));
		}
	else
		{
		name = new JString(GGetAddress(*buffer));
		}
	assert(name != NULL);
	name->TrimWhitespace();
	names.InsertSorted(name, kJFalse);
	delete buffer;
*/
}

/*****************************************************************************
 GGetRealNames

 *****************************************************************************/

void GGetRealNames
	(
	const JString& list,
	JPtrArray<JString>& names
	)
{
	GParseNameList(list, names, kJTrue);
}

/*****************************************************************************
 GVerifyPGPNames

 *****************************************************************************/

JBoolean GVerifyPGPNames
	(
	const JPtrArray<JString>& names
	)
{
	for (JSize i = 1; i <= names.GetElementCount(); i++)
		{
		JString name = *(names.NthElement(i));
		JString sysCmd = "pgp -kv " + name;
		int inFD;
		JError err =
			JExecute(sysCmd, NULL,
				kJIgnoreConnection, NULL,
				kJCreatePipe, &inFD,
				kJTossOutput, NULL);
		if (err.OK())
			{
			JString buffer;
			JReadAll(inFD, &buffer);
			JRegex regex;
			JBoolean matched;
			JArray<JIndexRange>* subList = new JArray<JIndexRange>;
			assert(subList != NULL);
			err = regex.SetPattern("([0-9]+) matching key[s]* found");
			assert(err.OK());
			matched = regex.Match(buffer, subList);
			if (matched)
				{
				JIndexRange sRange = subList->GetElement(2);
				JString number = buffer.GetSubstring(sRange);
				if (number == "0")
					{
					JString notice = "No key found for " + name + ".";
					JGetUserNotification()->ReportError(notice);
					return kJFalse;
					}
				}
			else
				{
				JGetUserNotification()->ReportError("Error checking pgp key.");
				return kJFalse;
				}
			}
		else
			{
			JGetUserNotification()->ReportError("Error checking pgp key.");
			return kJFalse;
			}
		}
	return kJTrue;
}

/*****************************************************************************
 GVerifyPGPNames

 *****************************************************************************/

JBoolean GVerifyGPGNames
	(
	const JPtrArray<JString>& names
	)
{
	for (JSize i = 1; i <= names.GetElementCount(); i++)
		{
		JString name = *(names.NthElement(i));
		JString sysCmd = "gpg --list-key --no-secmem-warning " + name;
		int errFD;
		JError err =
			JExecute(sysCmd,		NULL,
				kJIgnoreConnection, NULL,
				kJTossOutput,		NULL,
				kJCreatePipe,		&errFD);
		if (err.OK())
			{
			JString buffer;
			JReadAll(errFD, &buffer);
			if (buffer.Contains("public key not found"))
				{
				JString notice = "No key found for " + name + ".";
				JGetUserNotification()->ReportError(notice);
				return kJFalse;
				}
			}
		else
			{
			JGetUserNotification()->ReportError("Error checking gog key.");
			return kJFalse;
			}
		}
	return kJTrue;
}

/******************************************************************************
 GGetAddress

 *****************************************************************************/

JString
GGetAddress
	(
	const JString& fullname
	)
{
	JString address;
	JString name;
	GSplitAddressAndName(fullname, &address, &name);
	return address;
}

/******************************************************************************
 GGetName

 *****************************************************************************/

JString
GGetName
	(
	const JString& fullname
	)
{
	JString address;
	JString name;
	GSplitAddressAndName(fullname, &address, &name);
	return name;
}

/******************************************************************************
 GSplitAddressAndName

 *****************************************************************************/

void
GSplitAddressAndName
	(
	const JString&	fullname,
	JString*		address,
	JString*		name
	)
{
	JString test = fullname;
	test.TrimWhitespace();
	JRegex regex;
	JBoolean matched;
	JArray<JIndexRange>* subList = new JArray<JIndexRange>;
	assert(subList != NULL);
	JError err = regex.SetPattern("^[\"]?([^\"<>]+)[\"]?[[:space:]]*<([^<>]*)>");
	assert(err.OK());
	matched = regex.Match(test, subList);
	if (matched)
		{
		JIndexRange sRange = subList->GetElement(3);
		if (!sRange.IsEmpty())
			{
			*address = test.GetSubstring(sRange);
			}
		else
			{
			*address = test;
			}
		sRange = subList->GetElement(2);
		if (!sRange.IsEmpty())
			{
			*name = test.GetSubstring(sRange);
			}
		else
			{
			*name = *address;
			}
		delete subList;
		return;
		}

	err = regex.SetPattern("^<(.*@.*)>");
	assert(err.OK());
	matched = regex.Match(test, subList);
	if (matched)
		{
		JIndexRange sRange = subList->GetElement(2);
		if (!sRange.IsEmpty())
			{
			*address = test.GetSubstring(sRange);
			*name = *address;
			}
		else
			{
			*address = test;
			*name = *address;
			}
		delete subList;
		return;
		}

	err = regex.SetPattern("^[\"](.*)[\"][[:space:]]*<(.*)>");
	assert(err.OK());
	matched = regex.Match(test, subList);
	if (matched)
		{
		JIndexRange sRange = subList->GetElement(3);
		if (!sRange.IsEmpty())
			{
			*address = test.GetSubstring(sRange);
			}
		else
			{
			*address = test;
			}
		sRange = subList->GetElement(2);
		if (!sRange.IsEmpty())
			{
			*name = test.GetSubstring(sRange);
			}
		else
			{
			*name = *address;
			}
		delete subList;
		return;
		}

	err = regex.SetPattern("(.*@.*)[[:space:]]*\\((.*)\\)");
	assert(err.OK());
	matched = regex.Match(test, subList);
	if (matched)
		{
		JIndexRange sRange = subList->GetElement(2);
		if (!sRange.IsEmpty())
			{
			*address = test.GetSubstring(sRange);
			}
		else
			{
			*address = test;
			}
		sRange = subList->GetElement(3);
		if (!sRange.IsEmpty())
			{
			*name = test.GetSubstring(sRange);
			}
		else
			{
			*name = *address;
			}
		}
	else
		{
		*address = test;
		*name = test;
		}

	delete subList;
}

/******************************************************************************
 GFixHeaderForReply


 ******************************************************************************/

void
GFixHeaderForReply
	(
	GMessageHeader::ReplyStatus status,
	JString* sub
	)
{
	JRegex regex;
	JString prepStr;
	if (status == GMessageHeader::kForwarded)
		{
		prepStr	= "Fwd: ";
		JError err = regex.SetPattern("^[fF][wW][dD][.:]");
		assert(err.OK());
		}
	else if (status == GMessageHeader::kRedirected)
		{
		prepStr	= "FYI: ";
		JError err = regex.SetPattern("^[fF][yY][iI][.:]");
		assert(err.OK());
		}
	else
		{
		prepStr	= "Re: ";
		JError err = regex.SetPattern("^[rR][eE][.:]");
		assert(err.OK());
		}
	JBoolean matched;
	JArray<JIndexRange>* subList = new JArray<JIndexRange>;
	assert(subList != NULL);
	matched = regex.Match(*sub, subList);
	if (!matched)
		{
		sub->Prepend(prepStr);
		}
}

/******************************************************************************
 GCompressWhitespace

 *****************************************************************************/

void
GCompressWhitespace
	(
	JString* str
	)
{
	str->TrimWhitespace();
	if (!str->IsEmpty())
		{
		JIndex findex;
		while (str->LocateSubstring("\n", &findex))
			{
			str->ReplaceSubstring(findex, findex, " ");
			}
		while (str->LocateSubstring("\t", &findex))
			{
			str->ReplaceSubstring(findex, findex, " ");
			}
		findex = 1;
		while(str->LocateNextSubstring(" ",&findex))
			{
			findex++;
			while (  str->IndexValid(findex) &&
					(str->GetCharacter(findex) == ' '))
				{
				str->RemoveSubstring(findex, findex);
				}
			}
		}
}

/******************************************************************************
 GReadQuote


 ******************************************************************************/

void
GReadQuote
	(
	istream& is,
	JString* str
	)
{
	JCharacter c;
	JString parse;
	JBoolean ok = JReadUntil(is, 2, "\"\\", &parse, &c);
	while (ok)
		{
		if (ok && c == '\"')
			{
			str->Append(parse);
			str->Append("\"");
			return;
			}
		if (ok && c == '\\')
			{
			str->Append(parse);
			str->Append("\\");
			is.get(c);
			if (is.fail())
				{
				return;
				}
			str->AppendCharacter(c);
			}
		ok = JReadUntil(is, 2, "\"\\", &parse, &c);
		}
	str->Append(parse);
}

/******************************************************************************
 GReadComment


 ******************************************************************************/

void
GReadComment
	(
	istream& is,
	JString* str
	)
{
	JCharacter c;
	JString parse;
	JBoolean ok = JReadUntil(is, 4, ")(\"\\", &parse, &c);
	while (ok)
		{
		if (ok && c == ')')
			{
			str->Append(parse);
			str->Append(")");
			return;
			}
		else if (ok && c == '\\')
			{
			str->Append(parse);
			str->Append("\\");
			is.get(c);
			if (is.fail())
				{
				return;
				}
			str->AppendCharacter(c);
			}
		else if (ok && c == '(')
			{
			str->Append(parse);
			str->Append("(");
			GReadComment(is, str);
			}
		else if (ok && c == '\"')
			{
			str->Append(parse);
			str->Append("\"");
			GReadQuote(is, str);
			}
		ok = JReadUntil(is, 4, ")(\"\\", &parse, &c);
		}
	str->Append(parse);
}

/******************************************************************************
 GReadBracket


 ******************************************************************************/

void
GReadBracket
	(
	istream& is,
	JString* str
	)
{
	JCharacter c;
	JString parse;
	JBoolean ok = JReadUntil(is, 4, "><\"\\", &parse, &c);
	while (ok)
		{
		if (ok && c == '>')
			{
			str->Append(parse);
			str->Append(">");
			return;
			}
		else if (ok && c == '\\')
			{
			str->Append(parse);
			str->Append("\\");
			is.get(c);
			if (is.fail())
				{
				return;
				}
			str->AppendCharacter(c);
			}
		else if (ok && c == '<')
			{
			str->Append(parse);
			str->Append("<");
			GReadBracket(is, str);
			}
		else if (ok && c == '\"')
			{
			str->Append(parse);
			str->Append("\"");
			GReadQuote(is, str);
			}
		ok = JReadUntil(is, 4, "><\"\\", &parse, &c);
		}
	str->Append(parse);
}

/******************************************************************************
 SaveMessage

 ******************************************************************************/

void
GSaveMessage
	(
	istream& is,
	ostream& os,
	GMessageHeader* header,
	const JBoolean update,
	const JBoolean flush
	)
{
	JSize headerStart = JTellp(os);
	if ((header->GetMessageStatus() != GMessageHeader::kDelete) || !flush)
		{
		JSeekg(is, header->GetHeaderStart());
		JString line;
		JBoolean foundG = kJFalse;
		JBoolean foundS = kJFalse;
		JBoolean foundR = kJFalse;
		JBoolean foundX = kJFalse;
		while ((line = JReadLine(is)) != "")
			{
			if (line.BeginsWith("Status:"))
				{
				foundS = kJTrue;
				line = "Status: ";
				if (header->GetMessageStatus() != GMessageHeader::kNew)
					{
					line += "RO";
					line.Print(os);
					os << "\n";
					}
				}
			else if (line.BeginsWith("X-Status:"))
				{
				foundX = kJTrue;
				line = "X-Status: ";
				if (header->GetReplyStatus() >= GMessageHeader::kReplied &&
					header->GetReplyStatus() <= GMessageHeader::kRepliedAll)
					{
					line += "A";
					line.Print(os);
					os << "\n";
					}
				}
			else if (line.BeginsWith("GMStatus:"))
				{
				foundG = kJTrue;
				line = "GMStatus: ";
				if (header->GetMessageStatus() == GMessageHeader::kUrgent)
					{
					line += "U";
					line.Print(os);
					os << "\n";
					}
				else if (header->GetMessageStatus() == GMessageHeader::kImportant)
					{
					line += "I";
					line.Print(os);
					os << "\n";
					}
				}
			else if (line.BeginsWith("X-Arrow-Reply-Status:"))
				{
				foundR = kJTrue;
				line = "X-Arrow-Reply-Status: ";
				if (header->GetReplyStatus() == GMessageHeader::kReplied)
					{
					line += "R";
					line.Print(os);
					os << "\n";
					}
				else if (header->GetReplyStatus() == GMessageHeader::kRepliedSender)
					{
					line += "RS";
					line.Print(os);
					os << "\n";
					}
				else if (header->GetReplyStatus() == GMessageHeader::kRepliedAll)
					{
					line += "RA";
					line.Print(os);
					os << "\n";
					}
				else if (header->GetReplyStatus() == GMessageHeader::kForwarded)
					{
					line += "F";
					line.Print(os);
					os << "\n";
					}
				else if (header->GetReplyStatus() == GMessageHeader::kRedirected)
					{
					line += "RD";
					line.Print(os);
					os << "\n";
					}
				}
			else
				{
				line.Print(os);
				os << "\n";
				}
			}
		if (!foundG)
			{
			line = "GMStatus: ";
			if (header->GetMessageStatus() == GMessageHeader::kUrgent)
				{
				line += "U";
				line.Print(os);
				os << "\n";
				}
			else if (header->GetMessageStatus() == GMessageHeader::kImportant)
				{
				line += "I";
				line.Print(os);
				os << "\n";
				}
			}
		if (!foundS)
			{
			line = "Status: ";
			if (header->GetMessageStatus() != GMessageHeader::kNew)
				{
				line += "RO";
				line.Print(os);
				os << "\n";
				}
			}
		if (!foundR)
			{
			line = "X-Arrow-Reply-Status: ";
			if (header->GetReplyStatus() == GMessageHeader::kReplied)
				{
				line += "R";
				line.Print(os);
				os << "\n";
				}
			else if (header->GetReplyStatus() == GMessageHeader::kRepliedSender)
				{
				line += "RS";
				line.Print(os);
				os << "\n";
				}
			else if (header->GetReplyStatus() == GMessageHeader::kRepliedAll)
				{
				line += "RA";
				line.Print(os);
				os << "\n";
				}
			else if (header->GetReplyStatus() == GMessageHeader::kForwarded)
				{
				line += "F";
				line.Print(os);
				os << "\n";
				}
			else if (header->GetReplyStatus() == GMessageHeader::kRedirected)
				{
				line += "RD";
				line.Print(os);
				os << "\n";
				}
			}
		if (!foundX)
			{
			line = "X-Status: ";
			if (header->GetReplyStatus() >= GMessageHeader::kReplied &&
				header->GetReplyStatus() <= GMessageHeader::kRepliedAll)
				{
				line += "A";
				line.Print(os);
				os << "\n";
				}
			}
		line = "\n";
		line.Print(os);
		JSize headerEnd = JTellp(os);
		JString body = JRead(is, header->GetMessageEnd() - header->GetHeaderEnd());
		if (!body.EndsWith("\n\n"))
			{
			body.Append("\n\n");
			}
		body.Print(os);
		JSize messageEnd = JTellp(os);
		if (update)
			{
			header->SetHeaderStart(headerStart);
			header->SetHeaderEnd(headerEnd);
			header->SetMessageEnd(messageEnd);
			}
		}
}

/******************************************************************************
 GAppendMessage

 ******************************************************************************/

JBoolean
GAppendMessage
	(
	const JCharacter*	srcMBox,
	const JCharacter*	destMBox,
	GMessageHeaderList*	list
	)
{
	JString mailfile(destMBox);

	JBoolean okToMove = kJTrue;

	JString path, name;
	JSplitPathAndName(mailfile, &path, &name);
	JString tempname;
	JError err = JCreateTempFile(path, NULL, &tempname);
	if (!err.OK())
		{
		okToMove = kJFalse;
		err      = JCreateTempFile(&tempname);
		if (!err.OK())
			{
			err.ReportIfError();
			return kJFalse;
			}
		}

	JString mbox(srcMBox);
	if (!GLockFile(mailfile))
		{
		JRemoveFile(tempname);
		return kJFalse;
		}
	mode_t perms, perms2;

	err = JGetPermissions(mailfile, &perms);
	if (!err.OK())
		{
		perms = 0600;
		}
	err = JGetPermissions(mbox, &perms2);
	if (!err.OK())
		{
		perms2 = 0600;
		}
	ifstream is(mailfile);
	ifstream mboxis(mbox);
	ofstream os(tempname);

	JSize lcount = list->GetElementCount();
	JSize index;
	JString temp;
	JDirEntry entry(mailfile);
	JSize size = entry.GetSize();
	temp.Read(is, size);

	if ((size != 0) && !temp.EndsWith("\n\n"))
		{
		temp.Append("\n\n");
		}
	temp.Print(os);
	for (index = 1; index <= lcount; index++)
		{
		GMessageHeader* header = list->NthElement(index);
		GSaveMessage(mboxis, os, header);
		}
	is.close();
	os.close();
	mboxis.close();

	JString cmd;
	if (okToMove)
		{
		cmd = "mv -f \"" + tempname + "\" \"" + mailfile + "\"";
		}
	else
		{
		cmd = "cp -f \"" + tempname + "\" \"" + mailfile + "\"";
		}
	JExecute(cmd, NULL);
	if (!okToMove)
		{
		JRemoveFile(tempname);
		}
	GUnlockFile(mailfile);
	GUnlockFile(mbox);
	return kJTrue;
}

/******************************************************************************
 GEncryptPGP2_6_2

 ******************************************************************************/

JBoolean
GEncryptPGP2_6_2
	(
	const JString&		text,
	const JString&		passwd,
	JPtrArray<JString>*	names,
	JString*			newText
	)
{
	JString file_out;
	JError err = JCreateTempFile(&file_out);
	if (!err.OK())
		{
		JGetUserNotification()->ReportError("Error encrypting file.");
		return kJFalse;
		}

	JString file_in;
	err = JCreateTempFile(&file_in);
	if (!err.OK())
		{
		JGetUserNotification()->ReportError("Error encrypting file.");
		JRemoveFile(file_out);
		return kJFalse;
		}

	ofstream os(file_out);
	if (!os.good())
		{
		JGetUserNotification()->ReportError("Error encrypting file.");
		os.close();
		JRemoveFile(file_out);
		JRemoveFile(file_in);
		return kJFalse;
		}
	text.Print(os);
	os.close();
	JString sysCmd = "pgp -east " + file_out + " -o " + file_in + " ";
	if (GVerifyPGPNames(*names))
		{
		for (JSize i = 1; i <= names->GetElementCount(); i++)
			{
			sysCmd += *(names->NthElement(i)) + " ";
			}
		setenv("PGPPASS", passwd, 1);
		int errFD;
		JError err =
			JExecute(sysCmd, NULL,
				kJIgnoreConnection, NULL,
				kJTossOutput, NULL,
				kJCreatePipe, &errFD);
		JRemoveFile(file_out);
		if (err.OK())
			{
			file_in += ".asc";
			JReadFile(file_in, newText);
			}
		else
			{
			JGetUserNotification()->ReportError("Error encrypting file.");
			JRemoveFile(file_in);
			return kJFalse;
			}
		setenv("PGPPASS", "", 1);
		}

	JRemoveFile(file_out);
	JRemoveFile(file_in);
	return kJTrue;
}

/******************************************************************************
 GEncryptPGP2_6_2

 ******************************************************************************/

JBoolean
GEncryptGPG1_0
	(
	const JString&		text,
	JPtrArray<JString>*	names,
	JString*			newText
	)
{
	JString sysCmd = "gpg -ea --batch --no-secmem-warning";
	if (GVerifyGPGNames(*names))
		{
		for (JSize i = 1; i <= names->GetElementCount(); i++)
			{
			sysCmd += " -r " + *(names->NthElement(i));
			}
		int errFD, toFD, fromFD;
		pid_t pid;
		JError err =
			JExecute(sysCmd, &pid,
				kJCreatePipe, &toFD,
				kJCreatePipe, &fromFD,
				kJCreatePipe, &errFD);
		if (err.OK())
			{
			JOutPipeStream output(toFD, kJTrue);
			text.Print(output);
			output.close();
			JReadAll(fromFD, newText);
			JString errStr;
			JReadAll(errFD, &errStr);
			if (!errStr.IsEmpty())
				{
				JGetUserNotification()->ReportError(errStr);
				}
			}
		else
			{
			JGetUserNotification()->ReportError("Error encrypting file.");
			return kJFalse;
			}
		}
	return kJTrue;
}

/******************************************************************************
 GDecryptPGP2_6_2

 ******************************************************************************/

JBoolean
GDecryptPGP2_6_2
	(
	const JString&		text,
	const JString&		passwd,
	JString*			newText
	)
{
	JString file_out;
	JError err =  JCreateTempFile(&file_out);
	if (!err.OK())
		{
		JGetUserNotification()->ReportError("Error decrypting file.");
		return kJFalse;
		}

	JString file_in;
	err =  JCreateTempFile(&file_in);
	if (!err.OK())
		{
		JGetUserNotification()->ReportError("Error decrypting file.");
		JRemoveFile(file_out);
		return kJFalse;
		}

	ofstream os(file_out);
	if (!os.good())
		{
		JGetUserNotification()->ReportError("Error decrypting file.");
		os.close();
		JRemoveFile(file_out);
		JRemoveFile(file_in);
		return kJFalse;
		}
	text.Print(os);
	os.close();
	JString sysCmd = "pgp " + file_out + " -o " + file_in;
	setenv("PGPPASS", passwd, 1);
	err =
		JExecute(sysCmd,		NULL,
			kJIgnoreConnection, NULL,
			kJTossOutput,		NULL,
			kJTossOutput,		NULL);
	setenv("PGPPASS", "", 1);
	JRemoveFile(file_out);
	if (err.OK())
		{
		JReadFile(file_in, newText);
		}
	else
		{
		JGetUserNotification()->ReportError("Error decrypting file.");
		JRemoveFile(file_in);
		return kJFalse;
		}

	JRemoveFile(file_in);
	return kJTrue;
}

/******************************************************************************
 GDecryptGPG1_0

 ******************************************************************************/

JBoolean
GDecryptGPG1_0
	(
	const JString&		text,
	const JString&		passwd,
	JString*			newText
	)
{
	JString sysCmd = "gpg -d --passphrase-fd 0 --batch --no-secmem-warning";
	int errFD, toFD, fromFD;
	pid_t pid;
	JError err =
		JExecute(sysCmd, &pid,
			kJCreatePipe, &toFD,
			kJCreatePipe, &fromFD,
			kJCreatePipe, &errFD);
	if (err.OK())
		{
		JOutPipeStream output(toFD, kJTrue);
		passwd.Print(output);
		output << endl;
		text.Print(output);
		output.close();
		JReadAll(fromFD, newText);
		JString errStr;
		JReadAll(errFD, &errStr);
		if (!errStr.IsEmpty())
			{
			JGetUserNotification()->ReportError(errStr);
			}
		}
	else
		{
		JGetUserNotification()->ReportError("Error encrypting file.");
		return kJFalse;
		}
	return kJTrue;

}

/******************************************************************************
 GMGetMIMESource

 ******************************************************************************/

void
GMGetMIMESource
	(
	GMAttachmentTable*	table,
	JTextEditor*		editor,
	JString*			body,
	JString*			header
	)
{
	// currently I don't use MIME if there are no attachments.
	// eventually, this will support alternate text forms.
	JString text(editor->GetText());
	GMPrepareStringForSMTP(&text);
	
	if ((table == NULL) ||
		(table->GetAttachmentCount() == 0))
		{
		*body	= text;
		return;
		}

	const JSize count	= table->GetAttachmentCount();

	header->Append("MIME-Version: 1.0\r\n");

	JString boundary("--_=_NextPart_");
	time_t now;
	time(&now);
	boundary += GMMD5Encrypt(ctime(&now));

	// generate boundary here
	header->Append("Content-Type: multipart/mixed;\r\n");
	header->Append("\tboundary=\"");
	header->Append(boundary);
	header->Append("\"\r\n");

	boundary.Prepend("--");

	body->Append(kMIMENotice);

	// write out the text and then all of the attachments
	body->Append(boundary);
	body->AppendCharacter('\r');
	body->AppendCharacter('\n');

	body->Append("Content-Type: text/plain;\r\n");
	body->Append("\tcharset=\"iso-8859-1\"\r\n\r\n");
	body->Append(text);

	for (JIndex i = 1; i <= count; i++)
		{
		JBoolean binary;
		JString filename	= table->GetAttachmentName(i, &binary);
		if (!JFileExists(filename) ||
			!JFileReadable(filename))
			{
			continue;
			}
		JString path;
		JString name;
		JSplitPathAndName(filename, &path, &name);

		body->Append("\r\n");
		body->Append(boundary);
		body->Append("\r\n");

		if (binary)
			{
			body->Append("Content-Type: application/octet-stream;\r\n");
			body->Append("\tname=\"");
			body->Append(name);
			body->Append("\"\r\n");
			body->Append("Content-Transfer-Encoding: base64\r\n");
			body->Append("Content-Disposition: attachment;\r\n");
			body->Append("\tfilename=\"");
			body->Append(name);
			body->Append("\"\r\n\r\n");
			std::ifstream is(filename);
			assert(is.good());
			std::ostringstream os;
			JEncodeBase64(is, os);
			body->Append(os.str());
			}
		else
			{
			body->Append("Content-Type: text/plain;\r\n");
			body->Append("\tname=\"");
			body->Append(name);
			body->Append("\"\r\n");
			body->Append("Content-Transfer-Encoding: 7bit\r\n");
			body->Append("Content-Disposition: attachment;\r\n");
			body->Append("\tfilename=\"");
			body->Append(name);
			body->Append("\"\r\n\r\n");
			JString textfile;
			JReadFile(filename, &textfile);
			GMPrepareStringForSMTP(&textfile);
			body->Append(textfile);
			}
		}

	body->Append("\r\n");
	body->Append(boundary);
	body->Append("--\r\n");
}

/******************************************************************************
 GMMD5Encrypt

 ******************************************************************************/

JString
GMMD5Encrypt
	(
	const JCharacter* text
	)
{
	MD5_CTX mdContext;
	unsigned char digest[16];

	char* charbuffer = strdup(text);

	MD5Init(&mdContext);
	MD5Update(&mdContext, (unsigned char *)charbuffer, strlen(charbuffer));
	MD5Final(digest, &mdContext);

	free(charbuffer);

	JString encrypted;
	char charchunk[5];
	for (int i = 0 ; i < 16; i++)
		{
		sprintf(charchunk, "%02x", digest[i]);
		encrypted += charchunk;
		}

	return encrypted;
}

/******************************************************************************
 GGetRelativeToHome

 ******************************************************************************/

JString
GGetRelativeToHome
	(
	const JString& filename
	)
{
	JString temphome;
	JString home;
	JGetHomeDirectory(&temphome);
	JGetTrueName(temphome, &home);
	JAppendDirSeparator(&home);
	JString fullname	= filename;
	JString name		= filename;
	JGetTrueName(fullname, &name);
	if (name.BeginsWith(home))
		{
		name.RemoveSubstring(1, home.GetLength());
		fullname		= name;
		}
	return fullname;
}

/******************************************************************************
 GGetAbsoluteFromHome

 ******************************************************************************/

JBoolean
GGetAbsoluteFromHome
	(
	const JString&	filename,
	JString*		file
	)
{
	*file	= filename;
	if (!file->BeginsWith("/"))
		{
		JString home;
		JBoolean ok = JGetHomeDirectory(&home);
		if (!ok)
			{
			JGetUserNotification()->ReportError("You do not appear to have a home directory.");
			return kJFalse;
			}
		JAppendDirSeparator(&home);
		*file	= home + *file;
		}
	return kJTrue;
}

/******************************************************************************
 GMPrepareStringForSMTP

 ******************************************************************************/

void	
GMPrepareStringForSMTP
	(
	JString* text
	)
{
	JIndexRange range(1,0);
	while (smtpPeriodStuff.MatchAfter(*text, range, &range))
		{
		text->InsertSubstring(".", range.last + 1);
		range += 1;
		}

	JIndex findex = 1;
	while (text->IndexValid(findex) && text->LocateNextSubstring("\n", &findex))
		{
		text->ReplaceSubstring(findex, findex, "\r\n");
		findex += 2;
		}

}
