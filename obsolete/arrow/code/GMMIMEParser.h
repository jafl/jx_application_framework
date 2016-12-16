/******************************************************************************
 GMMIMEParser.h

 ******************************************************************************/

#ifndef _H_GMMIMEParser
#define _H_GMMIMEParser

#include <JPtrArray.h>
#include <JString.h>

class GMIMEHeader;
class JDirInfo;

class GMMIMEParser
{
public:

	enum TextFormat
	{
		kPlain = 1,
		kHTML
	};

public:

	GMMIMEParser(JString* data, const JCharacter* rootDir);
	virtual ~GMMIMEParser();

	JSize			GetTextSegmentCount() const;
	JBoolean		GetTextSegment(const JIndex index,
								   JString* text, TextFormat* format,
								   JString* charset);

	JSize			GetAttachmentCount() const;
	JString			GetAttachmentName(const JIndex index);

	JBoolean		Successful() const;

private:

	JString				itsAttachDir;
	JString				itsTextDir;
	JPtrArray<JString>*	itsTextSegments;
	JDirInfo*			itsTextInfo;
	JDirInfo*			itsAttachInfo;
	JBoolean			itsIsSuccessful;
	JIndex				itsTextFileIndex;
	JString*			itsData;

private:

	void		Parse(std::istream& input, const JIndex isEnd, const JBoolean nested = kJFalse);
	void		ParseMIMEHeader(std::istream& input, GMIMEHeader* header, const JBoolean display = kJFalse);
	void		CleanParmValPair(const JString& parmValPair, JString* parm,
								 JString* val);
	void		FindStringEnd(const JString& val, JIndex* index);
	void		Remove822Comment(JString* val, const JIndex index);
	void		ParseContentType(const JString& val, GMIMEHeader* header);
	void		ParseContentDisposition(const JString& val, GMIMEHeader* header);
	JBoolean	ReadUntilBoundary(std::istream& input, const JString& boundary,
								  JIndex* start, JIndex* end);
	void		ParseContentParameters(const JString& text,
									   JPtrArray<JString>* strings);
	void		WriteTextString(JString* data, const GMIMEHeader& header);
	void		WriteAttachment(const JString& data, const GMIMEHeader& header);
	void		ParseMixed(std::istream& input, const GMIMEHeader& header);
	void		ParseAlternate(std::istream& input, const GMIMEHeader& header);
	void		ParseByType(std::istream& input, const GMIMEHeader& header, const JIndex isEnd);
	JBoolean	TextIsReadable(const GMIMEHeader& header) const;
	void		ParseQuotedPrintable(JString* text);
	void		AdjustAttachmentName(const GMIMEHeader& header, JString* name);

	// not allowed

	GMMIMEParser(const GMMIMEParser& source);
	const GMMIMEParser& operator=(const GMMIMEParser& source);
};

/******************************************************************************
 Success (public)

 ******************************************************************************/

inline JBoolean
GMMIMEParser::Successful()
	const
{
	return itsIsSuccessful;
}


#endif
