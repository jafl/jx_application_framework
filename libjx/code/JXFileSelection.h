/******************************************************************************
 JXFileSelection.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFileSelection
#define _H_JXFileSelection

#include <JXSelectionManager.h>

class JXFileSelection : public JXSelectionData
{
public:

	JXFileSelection(JXDisplay* display, const JPtrArray<JString>& list);
	JXFileSelection(JXDisplay* display, JPtrArray<JString>* list);
	JXFileSelection(JXWidget* widget, const JUtf8Byte* id);

	virtual	~JXFileSelection();

	JBoolean	GetData(const JPtrArray<JString>** list) const;

	void	SetData(const JPtrArray<JString>& list);
	void	SetData(JPtrArray<JString>* list);

	static JBoolean	GetFileList(const Atom selectionName, const Time time,
								JXDisplay* display,
								JPtrArray<JString>* fileNameList,
								JPtrArray<JString>* urlList);
	static JBoolean	GetFileList(JXSelectionManager* selMgr,
								const Atom selectionName, const Time time,
								JPtrArray<JString>* fileNameList,
								JPtrArray<JString>* urlList);

protected:

	virtual void		AddTypes(const Atom selectionName);
	virtual JBoolean	ConvertData(const Atom requestType, Atom* returnType,
									unsigned char** data, JSize* dataLength,
									JSize* bitsPerBlock) const;

private:

	JPtrArray<JString>*	itsList;		// NULL until after Resolve()
	JString*			itsBuffer;

private:

	void	JXFileSelectionX();
	void	CreateBuffer() const;

	// not allowed

	JXFileSelection(const JXFileSelection& source);
	const JXFileSelection& operator=(const JXFileSelection& source);
};


/******************************************************************************
 GetData

 ******************************************************************************/

inline JBoolean
JXFileSelection::GetData
	(
	const JPtrArray<JString>** list
	)
	const
{
	*list = itsList;
	return JI2B( itsList != NULL );
}

#endif
