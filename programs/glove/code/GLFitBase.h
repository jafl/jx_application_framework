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
	GLFitBase(const JSize paramCount, const bool errors = false, const bool gof = false);
	virtual ~GLFitBase();

	JSize 				GetParameterCount() const;
	virtual bool	GetParameterName(const JIndex index, JString* name) const = 0;
	virtual bool	GetParameter(const JIndex index, JFloat* value) const = 0;

	bool 			HasParameterErrors() const;
	virtual bool	GetParameterError(const JIndex index, JFloat* value) const;

	bool			HasGoodnessOfFit() const;
	virtual bool	GetGoodnessOfFitName(JString* name) const;
	virtual bool	GetGoodnessOfFit(JFloat* value) const;
	
	virtual JString		GetFitFunctionString() const = 0;

protected:

	void 				SetHasParameterErrors(const bool errors);
	void				SetParameterCount(const JSize count);
	void				SetHasGoodnessOfFit(const bool gof);

private:

	JSize				itsParameterCount;
	bool			itsHasParameterErrors;
	bool			itsHasGOF;
};

/*********************************************************************************
 HasParameterErrors
  
 ********************************************************************************/

inline bool
GLFitBase::HasParameterErrors()
	const	
{
	return itsHasParameterErrors;
}

/*********************************************************************************
 GetParameterError
  
 ********************************************************************************/

inline bool
GLFitBase::GetParameterError
	(
	const JIndex index, 
	JFloat* value
	)
	const
{
	return false;
}


/*********************************************************************************
 HasGoodnessOfFit
  
 ********************************************************************************/

inline bool
GLFitBase::HasGoodnessOfFit()
	const	
{
	return itsHasGOF;
}

/*********************************************************************************
 GetGoodnessOfFitName
  
 ********************************************************************************/

inline bool
GLFitBase::GetGoodnessOfFitName
	(
	JString* name
	)
	const
{
	return false;
}

/*********************************************************************************
 GetGoodnessOfFit
  
 ********************************************************************************/

inline bool
GLFitBase::GetGoodnessOfFit
	(
	JFloat* value
	)
	const
{
	return false;
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
