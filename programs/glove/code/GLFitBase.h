/*********************************************************************************
 GLFitBase.h
 
	Interface for the GLFitBase class.
 
	Copyright @ 1997 by Glenn W. Bach.

 ********************************************************************************/
 
#ifndef _H_GLFitBase
#define _H_GLFitBase

#include <jTypes.h>

class JString;

class GLFitBase 
{
public:
	
	GLFitBase();
	GLFitBase(const JSize paramCount, const JBoolean errors = kJFalse, const JBoolean gof = kJFalse);
	virtual ~GLFitBase();

	JSize 				GetParameterCount() const;
	virtual JBoolean	GetParameterName(const JIndex index, JString* name) const = 0;
	virtual JBoolean	GetParameter(const JIndex index, JFloat* value) const = 0;

	JBoolean 			HasParameterErrors() const;
	virtual JBoolean	GetParameterError(const JIndex index, JFloat* value) const;

	JBoolean			HasGoodnessOfFit() const;
	virtual JBoolean	GetGoodnessOfFitName(JString* name) const;
	virtual JBoolean	GetGoodnessOfFit(JFloat* value) const;
	
	virtual JString		GetFitFunctionString() const = 0;

protected:

	void 				SetHasParameterErrors(const JBoolean errors);
	void				SetParameterCount(const JSize count);
	void				SetHasGoodnessOfFit(const JBoolean gof);

private:

	JSize				itsParameterCount;
	JBoolean			itsHasParameterErrors;
	JBoolean			itsHasGOF;
};

/*********************************************************************************
 HasParameterErrors
  
 ********************************************************************************/

inline JBoolean
GLFitBase::HasParameterErrors()
	const	
{
	return itsHasParameterErrors;
}

/*********************************************************************************
 GetParameterError
  
 ********************************************************************************/

inline JBoolean
GLFitBase::GetParameterError
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	return kJFalse;
}


/*********************************************************************************
 HasGoodnessOfFit
  
 ********************************************************************************/

inline JBoolean
GLFitBase::HasGoodnessOfFit()
	const	
{
	return itsHasGOF;
}

/*********************************************************************************
 GetGoodnessOfFitName
  
 ********************************************************************************/

inline JBoolean
GLFitBase::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	return kJFalse;
}

/*********************************************************************************
 GetGoodnessOfFit
  
 ********************************************************************************/

inline JBoolean
GLFitBase::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	return kJFalse;
}

/*********************************************************************************
 GetParameterCount
  
 ********************************************************************************/

inline JSize
GLFitBase::GetParameterCount()
	const
{
	return itsParameterCount;
}

#endif
