/******************************************************************************
 GMIMEHeader.h

 ******************************************************************************/

#ifndef _H_GMIMEHeader
#define _H_GMIMEHeader

#include <JString.h>

class GMIMEHeader
{
public:

	GMIMEHeader();
	GMIMEHeader(const GMIMEHeader& source);

	virtual ~GMIMEHeader();

	void			SetType(const JString& type);
	const JString&	GetType() const;

	void			SetSubType(const JString& subType);
	const JString&	GetSubType() const;

	void			SetCharSet(const JString& charset);
	const JString&	GetCharSet() const;

	void			SetBoundary(const JString& boundary);
	const JString&	GetBoundary() const;

	void			SetFileName(const JString& filename);
	const JString&	GetFileName() const;

	void			SetDisplayType(const JString& displayType);
	const JString&	GetDisplayType() const;

	void			SetEncoding(const JString& encoding);
	const JString&	GetEncoding() const;

	void			SetVersion(const JString& version);
	const JString&	GetVersion() const;

	void			Set(const GMIMEHeader& header);

	const GMIMEHeader& operator=(const GMIMEHeader& header);

private:

	JString		itsType;
	JString		itsSubType;
	JString		itsCharSet;
	JString		itsBoundary;
	JString		itsFileName;
	JString		itsDisplayType;
	JString		itsEncoding;
	JString		itsVersion;

};

/******************************************************************************
 Assignment operator


 ******************************************************************************/

inline const GMIMEHeader&
GMIMEHeader::operator=
	(
	const GMIMEHeader& source
	)
{
	Set(source);
	return *this;
}


#endif
