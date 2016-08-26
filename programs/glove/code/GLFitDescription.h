/******************************************************************************
 GLFitDescription.h

	Interface for the GLFitDescription class

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#ifndef _H_GLFitDescription
#define _H_GLFitDescription

#include "JFitBase.h"

#include <GVarList.h>

#include <JArray.h>
#include <JString.h>

#include <jTypes.h>

class GLFitDescription : public JFitBase
{
public:

	enum FitType
	{
		kPolynomial = 1,
		kNonLinear,
		kModule,
		kBLinear,
		kBQuadratic,
		kBExp,
		kBPower
	};

public:

	GLFitDescription(const FitType type, const JCharacter* form = "", const JCharacter* name = "Untitled");

	static JBoolean	Create(istream& is, GLFitDescription** fd);
	
	virtual ~GLFitDescription();

	FitType	GetType() const;
	void	SetType(const FitType type); 

	virtual JBoolean	GetParameterName(const JIndex index, JString* name) const;
	virtual JBoolean	GetParameter(const JIndex index, JFloat* value) const;


	virtual JString	GetFitFunctionString() const;
	void			SetFitFunctionString(const JCharacter* form);

	const JString&	GetFnName() const;

	JBoolean		RequiresStartValues() const;
	JBoolean		CanUseStartValues() const;

	GVarList*		GetVarList();

	virtual void	WriteSetup(ostream& os); // must call base class first!

	static JOrderedSetT::CompareResult
		CompareFits(GLFitDescription * const &, GLFitDescription * const &);

protected:

	void			SetFnName(const JCharacter* name);
	void			DoesRequireStartValues(const JBoolean require);
	void			SetCanUseStartValues(const JBoolean use);

private:

	GVarList*	itsVarList;
	FitType		itsType;
	JString		itsFnForm;
	JString		itsFnName;
	JBoolean	itsRequiresStartValues;
	JBoolean	itsCanUseStartValues;
	
private:

	// not allowed

	GLFitDescription(const GLFitDescription& source);
	const GLFitDescription& operator=(const GLFitDescription& source);

};

/******************************************************************************
 GetType (public)

 ******************************************************************************/

inline GLFitDescription::FitType
GLFitDescription::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 GetFitFunctionString (public)

 ******************************************************************************/

inline JString
GLFitDescription::GetFitFunctionString()
	const
{
	return itsFnForm;
}

/******************************************************************************
 GetFnName (public)

 ******************************************************************************/

inline const JString&
GLFitDescription::GetFnName()
	const
{
	return itsFnName;
}

/******************************************************************************
 RequiresStartValues (public)

 ******************************************************************************/

inline JBoolean
GLFitDescription::RequiresStartValues()
	const
{
	return itsRequiresStartValues;
}

/******************************************************************************
 CanUseStartValues (public)

 ******************************************************************************/

inline JBoolean
GLFitDescription::CanUseStartValues()
	const
{
	return itsCanUseStartValues;
}

/******************************************************************************
 GetVarList (public)

 ******************************************************************************/

inline GVarList*
GLFitDescription::GetVarList()
{
	return itsVarList;
}

#endif
