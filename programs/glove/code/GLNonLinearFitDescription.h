/******************************************************************************
 GLNonLinearFitDescription.h

	Interface for the GLNonLinearFitDescription class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GLNonLinearFitDescription
#define _H_GLNonLinearFitDescription

// Superclass Header
#include <GLFitDescription.h>

class GLNonLinearFitDescription : public GLFitDescription
{
public:

	GLNonLinearFitDescription(const JCharacter* name, 
							  const JCharacter* function, 
							  const JCharacter* fPrimed, 
							  const JPtrArray<JString>& vars);
	GLNonLinearFitDescription(std::istream& is);
								  
	virtual ~GLNonLinearFitDescription();

	JString		GetFunctionString() const;
	JString		GetFunctionPrimedString() const;

	virtual void	WriteSetup(std::ostream& os); // must call base class first!
	
protected:

private:

	JString		itsFunction;
	JString		itsFPrimed;
	
private:

	// not allowed

	GLNonLinearFitDescription(const GLNonLinearFitDescription& source);
	const GLNonLinearFitDescription& operator=(const GLNonLinearFitDescription& source);

};

/******************************************************************************
 GetFunctionString (public)

 ******************************************************************************/

inline JString
GLNonLinearFitDescription::GetFunctionString()
	const
{
	return itsFunction;
}

/******************************************************************************
 GetFunctionPrimedString (public)

 ******************************************************************************/

inline JString
GLNonLinearFitDescription::GetFunctionPrimedString()
	const
{
	return itsFPrimed;
}

#endif
