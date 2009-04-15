/******************************************************************************
 GMIMEHeader.cc

 ******************************************************************************/

#include <GMIMEHeader.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GMIMEHeader::GMIMEHeader()
{
	itsType	= "text";
	itsSubType	= "plain";
}

GMIMEHeader::GMIMEHeader
	(
	const GMIMEHeader& source
	)
{
	Set(source);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMIMEHeader::~GMIMEHeader()
{
}

/******************************************************************************
 Type

 ******************************************************************************/

void
GMIMEHeader::SetType
	(
	const JString& type
	)
{
	itsType = type;
}

const JString&
GMIMEHeader::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 SubType

 ******************************************************************************/

void
GMIMEHeader::SetSubType
	(
	const JString& subType
	)
{
	itsSubType = subType;
}

const JString&
GMIMEHeader::GetSubType()
	const
{
	return itsSubType;
}

/******************************************************************************
 CharSet

 ******************************************************************************/

void
GMIMEHeader::SetCharSet
	(
	const JString& charset
	)
{
	itsCharSet = charset;
}

const JString&
GMIMEHeader::GetCharSet()
	const
{
	return itsCharSet;
}

/******************************************************************************
 Boundary

 ******************************************************************************/

void
GMIMEHeader::SetBoundary
	(
	const JString& boundary
	)
{
	itsBoundary = boundary;
}

const JString&
GMIMEHeader::GetBoundary()
	const
{
	return itsBoundary;
}

/******************************************************************************
 FileName

 ******************************************************************************/

void
GMIMEHeader::SetFileName
	(
	const JString& filename
	)
{
	itsFileName = filename;
}

const JString&
GMIMEHeader::GetFileName()
	const
{
	return itsFileName;
}

/******************************************************************************
 DisplayType

 ******************************************************************************/

void
GMIMEHeader::SetDisplayType
	(
	const JString& displayType
	)
{
	itsDisplayType = displayType;
}

const JString&
GMIMEHeader::GetDisplayType()
	const
{
	return itsDisplayType;
}

/******************************************************************************
 Encoding (public)

 ******************************************************************************/

void
GMIMEHeader::SetEncoding
	(
	const JString& encoding
	)
{
	itsEncoding = encoding;
}

const JString&
GMIMEHeader::GetEncoding()
	const
{
	return itsEncoding;
}

/******************************************************************************
 Version (public)

 ******************************************************************************/

void
GMIMEHeader::SetVersion
	(
	const JString& version
	)
{
	itsVersion = version;
}

const JString&
GMIMEHeader::GetVersion()
	const
{
	return itsVersion;
}

/******************************************************************************
 Set (public)

 ******************************************************************************/

void
GMIMEHeader::Set
	(
	const GMIMEHeader& header
	)
{
	this->itsType			= header.itsType;
	this->itsSubType		= header.itsSubType;
	this->itsCharSet		= header.itsCharSet;
	this->itsBoundary		= header.itsBoundary;
	this->itsFileName		= header.itsFileName;
	this->itsDisplayType	= header.itsDisplayType;
	this->itsEncoding		= header.itsEncoding;
	this->itsVersion		= header.itsVersion;
}
