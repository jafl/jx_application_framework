/******************************************************************************
 GMMIMEParser.cc

 ******************************************************************************/

#include "GMMIMEParser.h"
#include "GMIMEHeader.h"
#include "MediaTypes.h"

#include <JDirInfo.h>
#include <JPtrArray-JString.h>
#include <JRegex.h>

#include <jFileUtil.h>
#include <jStreamUtil.h>
#include <jProcessUtil.h>

#include <ctype.h>
#include <jFStreamUtil.h>
#include <strstream>
#include <jAssert.h>

static JRegex gmime_header_regex("^[^[:space:]]+.*");//([[:space:]]+[^[:space:]]+.*\n)*");

static JRegex gmime_charset_regex("charset\\s*=\\s*ISO-8859-(.)");
static JRegex gmime_text_plain_regex("text/plain");

static JRegex gmime_image_jpeg_regex("image/jpeg");
static JRegex gmime_image_gif_regex("image/gif");

//static JRegex gmime_name_regex("name\\s*=\s*([^\\s])+");
static JRegex gmime_quoted_name_regex("name\\s*=\\s*\"([^\\s\"])+\"");

static JCharacter* kTextType		= "text";
static JCharacter* kMultipartType	= "multipart";
static JCharacter* kAppType			= "application";
static JCharacter* kImageType		= "image";
static JCharacter* kMessageType		= "message";

static JCharacter* kAttachDirPrefix	= "gmimeattach";
static JCharacter* kTextDirPrefix	= "gmimetext";

static JCharacter* kPlainType		= "plain";
static JCharacter* kHTMLType		= "html";

static JCharacter* kMixedType		= "mixed";
static JCharacter* kRelatedType		= "related";
static JCharacter* kAlternateType	= "alternative";

static JCharacter* kBase64Encoding	= "base64";
static JCharacter* kQPEncoding		= "quoted-printable";

/******************************************************************************
 Constructor

 ******************************************************************************/

GMMIMEParser::GMMIMEParser
	(
	JString*			data,
	const JCharacter*	rootDir
	)
	:
	itsTextSegments(NULL),
	itsTextInfo(NULL),
	itsAttachInfo(NULL),
	itsIsSuccessful(kJTrue),
	itsTextFileIndex(100),
	itsData(data)
{
	std::istrstream input(data->GetCString(), data->GetLength());

	JError err = JCreateTempDirectory(rootDir, kAttachDirPrefix, &itsAttachDir);
	if (!err.OK())
		{
		itsIsSuccessful	= kJFalse;
		err.ReportIfError();
		}

	err = JCreateTempDirectory(rootDir, kTextDirPrefix, &itsTextDir);
	if (!err.OK())
		{
		itsIsSuccessful	= kJFalse;
		err.ReportIfError();
		}

	if (itsIsSuccessful)
		{
		if (!JDirInfo::Create(itsTextDir, &itsTextInfo))
			{
			itsIsSuccessful	= kJFalse;
			err.ReportIfError();
			}
		if (!JDirInfo::Create(itsAttachDir, &itsAttachInfo))
			{
			itsIsSuccessful	= kJFalse;
			err.ReportIfError();
			}
		}

	if (itsIsSuccessful)
		{
		itsTextInfo->ShowHidden(kJTrue);
		itsAttachInfo->ShowHidden(kJTrue);
		Parse(input, itsData->GetLength());
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMMIMEParser::~GMMIMEParser()
{
	if (itsTextSegments != NULL)
		{
		itsTextSegments->DeleteAll();
		delete itsTextSegments;
		}

	if (itsTextInfo != NULL)
		{
		JKillDirectory(itsTextDir);
		delete itsTextInfo;
		}

	if (itsAttachInfo != NULL)
		{
		JKillDirectory(itsAttachDir);
		delete itsAttachInfo;
		}
}

/******************************************************************************
 Parse (private)

 ******************************************************************************/

void
GMMIMEParser::Parse
	(
	std::istream&		input,
	const JIndex	isEnd,
	const JBoolean	nested
	)
{
	GMIMEHeader* header = new GMIMEHeader();
	assert(header != NULL);

	if (input.good())
		{
		ParseMIMEHeader(input, header, nested);

		if (header->GetEncoding() == kBase64Encoding)
			{
			JString filename = header->GetFileName();
			if (filename.IsEmpty())
				{
				const JError err = JCreateTempFile(itsAttachDir, NULL, &filename);
				if (!err.OK())
					{
					itsIsSuccessful	= kJFalse;
					}
				}
			else
				{
				filename = JCombinePathAndName(itsAttachDir, filename);
				}
			if (!filename.IsEmpty())
				{
				AdjustAttachmentName(*header, &filename);
				std::ofstream os(filename);
				JDecodeBase64(input, os);
				}
			}
		else if ((header->GetType() != kMultipartType) &&
				 (!header->GetFileName().IsEmpty()))
			{
			JIndex startI = JTellg(input);
			const JCharacter* c = itsData->GetCString() + startI;
			JString filename	= header->GetFileName();
			if (filename.IsEmpty())
				{
				const JError err = JCreateTempFile(itsAttachDir, NULL, &filename);
				if (!err.OK())
					{
					itsIsSuccessful	= kJFalse;
					}
				}
			else
				{
				filename = JCombinePathAndName(itsAttachDir, filename);
				}
			if (!filename.IsEmpty())
				{
				AdjustAttachmentName(*header, &filename);
				std::ofstream os(filename);
				os.write(c, isEnd - startI);
				}
			}
		else
			{
			// parse message body. may be recursive
			ParseByType(input, *header, isEnd);
			}
		}
	else
		{
		itsIsSuccessful	= kJFalse;
		}

	if (itsTextInfo != NULL)
		{
		itsTextInfo->ForceUpdate();
		}
	if (itsAttachInfo != NULL)
		{
		itsAttachInfo->ForceUpdate();
		}
		
	delete header;
}

/******************************************************************************
 GetTextSegmentCount (public)

 ******************************************************************************/

JSize
GMMIMEParser::GetTextSegmentCount()
	const
{
	if (itsTextInfo == NULL)
		{
		return 0;
		}
	return itsTextInfo->GetEntryCount();
}

/******************************************************************************
 GetTextSegment (public)

 ******************************************************************************/

JBoolean
GMMIMEParser::GetTextSegment
	(
	const JIndex	index,
	JString*		text,
	TextFormat*		format,
	JString*		charset
	)
{
	assert(itsTextInfo != NULL);
	assert(index <= itsTextInfo->GetEntryCount());
	JString name = itsTextInfo->GetEntry(index).GetFullName();
	std::ifstream is(name);
	if (is.good())
		{
		JReadAll(is, text);
		if (name.EndsWith(kPlainType))
			{
			*format = kPlain;
			}
		else if (name.EndsWith(kHTMLType))
			{
			*format = kHTML;
			}
		JIndex findex;
		JBoolean ok = name.LocateLastSubstring(".", &findex);
		assert(ok);
		name.RemoveSubstring(findex, name.GetLength());
		ok = name.LocateLastSubstring(".", &findex);
		if (findex < name.GetLength())
			{
			*charset = name.GetSubstring(findex + 1, name.GetLength());
			}
		}
	else
		{
		itsIsSuccessful = kJFalse;
		}

	return itsIsSuccessful;
}

/******************************************************************************
 GetAttachmentCount (public)

 ******************************************************************************/

JSize
GMMIMEParser::GetAttachmentCount()
	const
{
	return (itsAttachInfo == NULL ? 0 : itsAttachInfo->GetEntryCount());
}

/******************************************************************************
 GetAttachmentName (public)

 ******************************************************************************/

JString
GMMIMEParser::GetAttachmentName
	(
	const JIndex index
	)
{
	assert(itsAttachInfo != NULL);
	assert(index <= itsAttachInfo->GetEntryCount());
	return itsAttachInfo->GetEntry(index).GetFullName();
}

/******************************************************************************
 ReadUntilBoundary (private)

  Returns true if we have reached the end boundary.

 ******************************************************************************/

JBoolean
GMMIMEParser::ReadUntilBoundary
	(
	std::istream&		input,
	const JString&	boundary,
	JIndex*		start,
	JIndex*		end
	)
{
	JString endBoundary = boundary + "--\n";

	JIndex startI = JTellg(input);
	JIndex findex = startI;

	JString boundaryNL	= boundary + "\n";

	if (itsData->LocateNextSubstring(boundaryNL, &findex))
		{
		// a '--' precedes the boundary, so we need to back up before
		// those.
		*start	= findex - 3;
		
		// the end is the character before the '\n'.
		*end	= findex + boundaryNL.GetLength() - 2;
		return kJFalse;
		}

	findex = startI;
	if (itsData->LocateNextSubstring(endBoundary, &findex))
		{
		// a '--' precedes the boundary, so we need to back up before
		// those.
		*start	= findex - 3;
		
		// the end is the character before the '\n'.
		*end	= findex + endBoundary.GetLength() - 2;
		return kJTrue;
		}
	*start	= itsData->GetLength();
	*end	= itsData->GetLength();
	return kJTrue;

// I found a situation where the following code didn't work. I hadn't
// thought of the case where nested boundaries where equal to the parent
// boundary plus something else, ie:
// 
// Content-Type: multipart/mixed;
//        boundary="=====================_40887633==_"
//
//--=====================_40887633==_
//Content-Type: multipart/related;
//        type="multipart/alternative";
//        boundary="=====================_40887633==_.REL"
//
//--=====================_40887633==_.REL
//Content-Type: multipart/alternative;
//        boundary="=====================_40887643==_.ALT"
//
//--=====================_40887643==_.ALT


/*	if (itsData->LocateNextSubstring(boundary, &findex))
		{
		*start	= findex - 3;
		JIndex eIndex	= findex;
		if (itsData->LocateNextSubstring("\n", &eIndex))
			{
			*end		= eIndex - 1;
			JString tBoundary = itsData->GetSubstring(findex, eIndex - 1);
			if (tBoundary == endBoundary)
				{
				return kJTrue;
				}
			return kJFalse;
			}
		}
	*start	= itsData->GetLength();
	*end	= itsData->GetLength();
	return kJTrue;
*/
}

/******************************************************************************
 ParseMIMEHeader (private)

  This function parses the header for specific MIME paramter:value pairs. The
  data gathered is returned in the GMIMEHeader object.

 ******************************************************************************/

void
GMMIMEParser::ParseMIMEHeader
	(
	std::istream&		input,
	GMIMEHeader*	header,
	const JBoolean	display
	)
{
	JString data;
	JCharacter c	= input.peek();
	if (c == '\n')
		{
//		input.get(c);
		}
//	input >> std::ws;

	// first we need to search for the first empty line. This line is the
	// end of the header.

	JString line;
	while (1)
		{
		JBoolean found;
		line = JReadLine(input, &found);
		if (line.IsEmpty())
			{
			break;
			}
		if (isspace(line.GetFirstCharacter()))
			{
			line.TrimWhitespace();
			if (line.IsEmpty())
				{
				break;
				}
			data.AppendCharacter(' ');
			}
		else if (!data.IsEmpty())
			{
			data.AppendCharacter('\n');
			}
		data += line;
		}
	data.AppendCharacter('\n');

	// we now need to search through the header for parameter:value pairs
	// using the gmime_header_regex defined above.

	JArray<JIndexRange> ranges;
	gmime_header_regex.MatchAll(data, &ranges);

	JSize count = ranges.GetElementCount();
	for (JSize i = 1; i <= count; i++)
		{
		JIndexRange range = ranges.GetElement(i);
		JString parmValPair = data.GetSubstring(range);
		JString parm;
		JString val;
		if (parmValPair.BeginsWith("MIME") ||
			parmValPair.BeginsWith("Mime") ||
			parmValPair.BeginsWith("Content"))
			{
			CleanParmValPair(parmValPair, &parm, &val);
			parm.ToLower();
			if (parm == "mime-Version")
				{
				val.TrimWhitespace();
				header->SetVersion(val);
				}
			else if (parm == "content-type")
				{
				ParseContentType(val, header);
				}
			else if (parm == "content-transfer-encoding")
				{
				val.TrimWhitespace();
				val.ToLower();
				header->SetEncoding(val);
				}
			else if (parm == "content-disposition")
				{
				ParseContentDisposition(val, header);
				}
			}
		}

	// this is a nested message, so some of the headers need to be displayed
	if (display)
		{
		JString text = "---------\n";
		JIndex findex	= 1;
		if (data.BeginsWith("From: ") || data.LocateSubstring("\nFrom: ", &findex))
			{
			if (findex > 1)
				{
				findex ++;
				}
			JIndex eindex	= findex;
			if (data.LocateNextSubstring("\n", &eindex) && (eindex > findex + 1))
				{
				text += data.GetSubstring(findex, eindex - 1);
				text += "\n";
				}
			}
		findex	= 1;
		if (data.BeginsWith("Date: ") || data.LocateSubstring("\nDate: ", &findex))
			{
			if (findex > 1)
				{
				findex ++;
				}
			JIndex eindex	= findex;
			if (data.LocateNextSubstring("\n", &eindex) && (eindex > findex + 1))
				{
				text += data.GetSubstring(findex, eindex - 1);
				text += "\n";
				}
			}
		findex	= 1;
		const JCharacter* kSubjectStr	= "Subject: ";
		if (data.BeginsWith("Subject: ") || data.LocateSubstring("\nSubject: ", &findex))
			{
			if (findex > 1)
				{
				findex ++;
				}
			JIndex eindex	= findex;
			if (data.LocateNextSubstring("\n", &eindex) && (eindex > findex + 1))
				{
				text += data.GetSubstring(findex, eindex - 1);
				text += "\n";
				}
			}
		WriteTextString(&text, GMIMEHeader());
		}
}

/******************************************************************************
 CleanParmValPair (private)

  This function converts newlines into spaces, and removes RFC822 type
  comments. The resultant string is broken into parameter/value pairs.

 ******************************************************************************/

void
GMMIMEParser::CleanParmValPair
	(
	const JString&	parmValPair,
	JString*		parm,
	JString*		val
	)
{
	JIndex findex;
	JBoolean ok	= parmValPair.LocateSubstring(":", &findex);
	JSize length	= parmValPair.GetLength();
	if (ok && (findex > 1) && (findex < length))
		{
		*parm	= parmValPair.GetSubstring(1, findex - 1);
		*val	= parmValPair.GetSubstring(findex + 1, length);

		JIndex index = 1;
		while (index <= val->GetLength())
			{
			JCharacter c = val->GetCharacter(index);
			if (c == '\"')
				{
				FindStringEnd(*val, &index);
				index++;
				}
			else if (c == '(')
				{
				Remove822Comment(val, index);
				}
			else
				{
				if (c == '\n')
					{
					val->SetCharacter(index, ' ');
					}
				index++;
				}
			}
		}
}

/******************************************************************************
 FindStringEnd (private)

 ******************************************************************************/

void
GMMIMEParser::FindStringEnd
	(
	const JString&	val,
	JIndex*		index
	)
{
	JSize length = val.GetLength();
	*index = *index + 1;
	while (*index <= length)
		{
		JCharacter c = val.GetCharacter(*index);
		if (c == '\"')
			{
			return;
			}
		else if ((c == '\\') && (*index < length))
			{
			*index = *index + 1;
			}
		*index = *index + 1;
		}
}

/******************************************************************************
 Remove822Comment (private)

 ******************************************************************************/

void
GMMIMEParser::Remove822Comment
	(
	JString*		val,
	const JIndex	index
	)
{
	JIndex findex = index + 1;
	while (findex <= val->GetLength())
		{
		JCharacter c = val->GetCharacter(findex);
		if (c == ')')
			{
			val->RemoveSubstring(index, findex);
			return;
			}
		findex++;
		}
}

/******************************************************************************
 ParseContentType (private)

  We've already split the parameter/value pairs, and determined that this
  one is the Content-Type header. This header value can contain many
  separate values separated by semi-colons. It may include the boundary
  which can have quotes, so we have to look for that.

 ******************************************************************************/

void
GMMIMEParser::ParseContentType
	(
	const JString&	val,
	GMIMEHeader*	header
	)
{
	// we first need to determine the type. this will be found before the
	// first '/' which will be followed by the subtype.
	JString tVal	= val;
	tVal.TrimWhitespace();
	JSize length	= tVal.GetLength();

	JString type;
	JIndex findex;
	if (tVal.LocateSubstring("/", &findex) && (findex > 1))
		{
		type = tVal.GetSubstring(1, findex - 1);
		tVal.RemoveSubstring(1, findex);
		tVal.TrimWhitespace();
		type.ToLower();
		header->SetType(type);
		}
	else
		{
		return;
		}

	// now we need to determine the subtype
	JString subType;
	length			= tVal.GetLength();
	JBoolean found	= kJFalse;
	JIndex index;
	if (tVal.LocateSubstring(";", &index))
		{
		subType = tVal.GetSubstring(1, index - 1);
		tVal.RemoveSubstring(1, index);
		}
	else
		{
		subType = tVal;
		tVal.Clear();
		}

	tVal.TrimWhitespace();
	subType.TrimWhitespace();
	subType.ToLower();
	header->SetSubType(subType);

	if (tVal.IsEmpty())
		{
		return;
		}

	JPtrArray<JString> strings(JPtrArrayT::kDeleteAll);
	ParseContentParameters(tVal, &strings);
	JSize count = strings.GetElementCount();
	for (JIndex i = 1; i <= count; i += 2)
		{
		JString* str	= strings.NthElement(i);
		if (type == kTextType)
			{
			if (*str == "charset")
				{
				if (strings.IndexValid(i+1))
					{
					str	= strings.NthElement(i+1);
					header->SetCharSet(*str);
					}
				}
			}
		else if (type == kMultipartType)
			{
			if (*str == "boundary")
				{
				if (strings.IndexValid(i+1))
					{
					str	= strings.NthElement(i+1);
					header->SetBoundary(*str);
					}
				}
			}
		if ((type == kAppType) ||
			(type == kImageType) ||
			(type == kTextType))
			{
			if (*str == "name")
				{
				if (strings.IndexValid(i+1))
					{
					str	= strings.NthElement(i+1);
					if (str->LocateLastSubstring("/", &findex))
						{
						str->RemoveSubstring(1, findex);
						}
					if (str->LocateLastSubstring("\\", &findex))
						{
						str->RemoveSubstring(1, findex);
						}
					header->SetFileName(*str);
					}
				}
			}
		}
}

/******************************************************************************
 ParseContentType (private)

  We've already split the parameter/value pairs, and determined that this
  one is the Content-Disposition header. This header value can contain many
  separate values separated by semi-colons.

 ******************************************************************************/

void
GMMIMEParser::ParseContentDisposition
	(
	const JString&	val,
	GMIMEHeader*	header
	)
{
	// we first need to strip the type. this will be found before the
	// first ';' which will be followed by the parameters.
	JString tVal	= val;
	tVal.TrimWhitespace();
	JSize length	= tVal.GetLength();

	JIndex findex;
	if (tVal.LocateSubstring(";", &findex) && (findex > 1))
		{
		tVal.RemoveSubstring(1, findex);
		tVal.TrimWhitespace();
		}
	else
		{
		return;
		}

	JPtrArray<JString> strings(JPtrArrayT::kDeleteAll);
	ParseContentParameters(tVal, &strings);
	JSize count = strings.GetElementCount();
	for (JIndex i = 1; i <= count; i += 2)
		{
		JString* str = strings.NthElement(i);
		if ((*str == "filename") &&
			(strings.IndexValid(i+1)))
			{
			str	= strings.NthElement(i+1);
			if (str->LocateLastSubstring("/", &findex))
				{
				str->RemoveSubstring(1, findex);
				}
			if (str->LocateLastSubstring("\\", &findex))
				{
				str->RemoveSubstring(1, findex);
				}
			header->SetFileName(*str);
			}
		}
}

/******************************************************************************
 ParseContentParameters (private)

  The text should contain parameter value pairs in the form:

    parameter1=value1; parameter2=value2

  The values may be quoted.

 ******************************************************************************/

void
GMMIMEParser::ParseContentParameters
	(
	const JString&		text,
	JPtrArray<JString>*	strings
	)
{
	JString tmp = text;
	JString* str;
	JIndex findex;
	while (tmp.LocateSubstring("=", &findex) && findex > 1)
		{
		str = new JString(tmp.GetSubstring(1, findex - 1));
		assert(str != NULL);
		str->TrimWhitespace();
		str->ToLower();
		strings->Append(str);

		tmp.RemoveSubstring(1, findex);
		tmp.TrimWhitespace();

		// now we need to get the corresponding value.
		// we need to check for quotes first.
		JIndex index = 1;
		if (tmp.GetLength() > 1 && tmp.GetCharacter(1) == '\"')
			{
			FindStringEnd(tmp, &index);
			str = new JString(tmp.GetSubstring(2, index-1));
			assert(str != NULL);
			if (tmp.LocateSubstring(";", &findex))
				{
				tmp.RemoveSubstring(1, findex);
				}
			}
		else if (tmp.LocateSubstring(";", &index))
			{
			str = new JString();
			assert(str != NULL);
			if (index > 1)
				{
				*str = tmp.GetSubstring(1, index-1);
				}
			tmp.RemoveSubstring(1, index);
			}
		else
			{
			str = new JString(tmp);
			assert(str != NULL);
			}

		str->TrimWhitespace();
		strings->Append(str);

		tmp.TrimWhitespace();
		}
}

/******************************************************************************
 WriteTextString (private)

 ******************************************************************************/

void
GMMIMEParser::WriteTextString
	(
	JString*			data,
	const GMIMEHeader&	header
	)
{
	JString filename =
		itsTextDir + JString(itsTextFileIndex) +
		"." + header.GetCharSet() + "." + header.GetSubType();
	itsTextFileIndex++;
	std::ofstream os(filename);
	if (header.GetEncoding() == kBase64Encoding)
		{
		std::istrstream is(data->GetCString(), data->GetLength());
		JDecodeBase64(is, os);
		return;
		}
		
	if (header.GetEncoding() == kQPEncoding)
		{
		ParseQuotedPrintable(data);
		}
	data->Print(os);
}

/******************************************************************************
 ParseQuotedPrintable (private)

 ******************************************************************************/

void
GMMIMEParser::ParseQuotedPrintable
	(
	JString* text
	)
{
	// read until '='
	// if next two characters are hex, then decode and replace
	// if next char is whitespace, then toss  = --> '\n'
	// if neither of the above, just toss the '=', this is an error

	JIndex findex	= 1;
	while (text->LocateNextSubstring("=", &findex))
		{
		JSize size	= text->GetLength();
		if (findex < size)
			{
			JCharacter c	= text->GetCharacter(findex + 1);
			if (isspace(c) || (c == '\n'))
				{
				JIndex rIndex	= findex;
				if (text->LocateNextSubstring("\n", &rIndex))
					{
					text->RemoveSubstring(findex, rIndex);
					}
				else
					{
					text->RemoveSubstring(findex, text->GetLength());
					}
				}
			else if (findex < size - 1)
				{
				JString hex	= text->GetSubstring(findex + 1, findex + 2);
				hex.Prepend("0x");
				JInteger hexVal;
				if (hex.ConvertToInteger(&hexVal))
					{
					text->RemoveSubstring(findex, findex + 2);
					c	= (JCharacter)hexVal;
					text->InsertCharacter(c, findex);
					findex++;
					}
				else
					{
					text->RemoveSubstring(findex, findex);
					}
				}
			else
				{
				text->RemoveSubstring(findex, text->GetLength());
				}
			}
		else
			{
			text->RemoveSubstring(findex, findex);
			}
		}
}

/******************************************************************************
 WriteAttachment (private)

 ******************************************************************************/

void
GMMIMEParser::WriteAttachment
	(
	const JString&		data,
	const GMIMEHeader&	header
	)
{
	JString filename = header.GetFileName();
	if (filename.IsEmpty())
		{
		const JError err = JCreateTempFile(itsAttachDir, NULL, &filename);
		if (!err.OK())
			{
			err.ReportIfError();
			return;
			}
		}
	else
		{
		filename = JCombinePathAndName(itsAttachDir, filename);
		}

	AdjustAttachmentName(header, &filename);
	std::ofstream os(filename);
	if (header.GetEncoding() == kBase64Encoding)
		{
		std::istrstream is(data.GetCString(), data.GetLength());
		JDecodeBase64(is, os);
		}
	else
		{
		data.Print(os);
		}
}

/******************************************************************************
 AdjustAttachmentName (private)

 ******************************************************************************/

void
GMMIMEParser::AdjustAttachmentName
	(
	const GMIMEHeader&	header,
	JString*			name
	)
{
	JString root, suffix;
	if (!JSplitRootAndSuffix(*name, &root, &suffix))
		{
		suffix = header.GetSubType();
		*name = JCombineRootAndSuffix(root, suffix);
		}
}

/******************************************************************************
 ParseMixed (private)

 ******************************************************************************/

void
GMMIMEParser::ParseMixed
	(
	std::istream&			input,
	const GMIMEHeader&	header
	)
{
	JString boundary	= "-" + header.GetBoundary();
	JString endBoundary = boundary + "--";

	// slurp the initial empty part
	JIndex bstart, bend;
	ReadUntilBoundary(input, boundary, &bstart, &bend);
	JIndex current = JTellg(input);
	JString data;
	data.Read(input, bend - current + 1);

	while (1)
		{
		GMIMEHeader child;
		ParseMIMEHeader(input, &child);

		// when the following function returns true, it has found the
		// last boundary
		data.Clear();
		JBoolean end;
		if (child.GetEncoding() == kBase64Encoding)
			{
			JString filename = child.GetFileName();
			if (filename.IsEmpty())
				{
				JCreateTempFile(itsAttachDir, NULL, &filename);
				}
			else
				{
				filename = JCombinePathAndName(itsAttachDir, filename);
				}
			if (!filename.IsEmpty())
				{
				AdjustAttachmentName(child, &filename);
				std::ofstream os(filename);
				JDecodeBase64(input, os);
				end = ReadUntilBoundary(input, boundary, &bstart, &bend);
	//			JBoolean found;
	//			JString line;
	//			while (line.IsEmpty() && !input.fail())
	//				{
	//				line = JReadLine(input, &found);
	//				}
	//			JString endBoundary = boundary + "--";
	//			if (line.Contains(endBoundary))
	//				{
	//				end = kJTrue;
	//				}
				}
			}
		else if ((child.GetType() != kMultipartType) &&
				 (!child.GetFileName().IsEmpty()))
			{
			JIndex startI = JTellg(input);
			JIndex findex = startI;
			if (itsData->LocateNextSubstring(boundary, &findex))
				{
				const JCharacter* c = itsData->GetCString() + startI;
				JString filename	= child.GetFileName();
				if (filename.IsEmpty())
					{
					JCreateTempFile(itsAttachDir, NULL, &filename);
					}
				else
					{
					filename = JCombinePathAndName(itsAttachDir, filename);
					}
				if (!filename.IsEmpty())
					{
					AdjustAttachmentName(child, &filename);
					std::ofstream os(filename);
					JSeekg(input, findex - 1);
					if (child.GetEncoding() == kQPEncoding)
						{
						JString data	= itsData->GetSubstring(startI, findex - 3);
						ParseQuotedPrintable(&data);
						data.Print(os);
						}
					else
						{
						os.write(c, findex - startI - 3);
						}				
					JBoolean found;
					JString line = JReadLine(input, &found);
					if (line.BeginsWith(endBoundary))
						{
						end = kJTrue;
						}
					}
				}
			}
		else
			{
			end = ReadUntilBoundary(input, boundary, &bstart, &bend);
			ParseByType(input, child, bstart - 1);

			current	= JTellg(input);
			data.Read(input, bend - current + 1);
			}
		if (end)
			{
			break;
			}
		}

	if (itsTextInfo != NULL)
		{
		itsTextInfo->ForceUpdate();
		}
	if (itsAttachInfo != NULL)
		{
		itsAttachInfo->ForceUpdate();
		}
}

/******************************************************************************
 ParseAlternate (private)

 ******************************************************************************/

void
GMMIMEParser::ParseAlternate
	(
	std::istream&			input,
	const GMIMEHeader&	header
	)
{
	JString boundary	= "-" + header.GetBoundary();
	JString data;
	JString junk;

	// slurp the initial empty part
	JIndex bstart, bend;
	ReadUntilBoundary(input, boundary, &bstart, &bend);
	JIndex current = JTellg(input);
	data.Read(input, bend - current + 1);

	JString text;
	JString charset;
	JString subType;

	GMIMEHeader shownHeader;

	while (1)
		{
		GMIMEHeader child;
		ParseMIMEHeader(input, &child);

		if (child.GetSubType() != kPlainType && child.GetSubType() != kHTMLType)
			{
			text = data;
			while (!ReadUntilBoundary(input, boundary, &bstart, &bend))
				{
				// slurp up the rest, because it is useless to us.
				current	= JTellg(input);
				data.Read(input, bend - current + 1);
				}
			current	= JTellg(input);
			data.Read(input, bend - current + 1);
			data.Clear();
			break;
			}
		else
			{
			shownHeader	= child;
			}

		// when the following function returns true, it has found the
		// last boundary
		data.Clear();
		JBoolean end = ReadUntilBoundary(input, boundary, &bstart, &bend);
		current	= JTellg(input);
		data.Read(input, bstart - current);
		current = JTellg(input);
		junk.Read(input, bend - current + 1);

		if (end)
			{
			text = data;
			data.Clear();
			break;
			}
		}

	WriteTextString(&text, shownHeader);

	if (itsTextInfo != NULL)
		{
		itsTextInfo->ForceUpdate();
		}
	if (itsAttachInfo != NULL)
		{
		itsAttachInfo->ForceUpdate();
		}
}

/******************************************************************************
 ParseByType (private)

 ******************************************************************************/

void
GMMIMEParser::ParseByType
	(
	std::istream&			input,
	const GMIMEHeader&	header,
	const JIndex		isEnd
	)
{
	 if (header.GetType()    == kMessageType &&
		 header.GetSubType() == "rfc822")
		{
		Parse(input, isEnd, kJTrue);
		}
	else if (TextIsReadable(header))
		{
		JString data;
		JIndex current	= JTellg(input);
		data.Read(input, isEnd - current + 1);
		WriteTextString(&data, header);
		}
	else if (header.GetType() == kMultipartType)
		{
		if (header.GetSubType() == kMixedType ||
			header.GetSubType() == kRelatedType)
			{
			ParseMixed(input, header);
			}
		else if (header.GetSubType() == kAlternateType)
			{
			ParseAlternate(input, header);
			}
		else
			{
			ParseMixed(input, header);
			}
		}
	else
		{
		JString data;
		JIndex current	= JTellg(input);
		data.Read(input, isEnd - current + 1);
		WriteAttachment(data, header);
		}
}

/******************************************************************************
 TextIsReadable (private)

 ******************************************************************************/

JBoolean
GMMIMEParser::TextIsReadable
	(
	const GMIMEHeader& header
	)
	const
{
	if (header.GetType()      != kTextType &&
		!(header.GetType()    == kMessageType &&
		  header.GetSubType() == "delivery-status"))
		{
		return kJFalse;
		}
	if (!header.GetFileName().IsEmpty())
		{
		return kJFalse;
		}
	if (header.GetEncoding() == kBase64Encoding)
		{
		return kJFalse;
		}

	return kJTrue;

	// should I test? ...
	JString subType	= header.GetSubType();
	if ((subType == kPlainType) || (subType == kHTMLType))
		{
		return kJTrue;
		}
	return kJFalse;
}
