/******************************************************************************
 JXFileSelection.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFileSelection
#define _H_JXFileSelection

#include "jx-af/jx/JXSelectionManager.h"

class JXFileSelection : public JXSelectionData
{
public:

	JXFileSelection(JXDisplay* display, const JPtrArray<JString>& list);
	JXFileSelection(JXDisplay* display, JPtrArray<JString>* list);
	JXFileSelection(JXWidget* widget, const JUtf8Byte* id);

	~JXFileSelection() override;

	bool	GetData(const JPtrArray<JString>** list) const;

	void	SetData(const JPtrArray<JString>& list);
	void	SetData(JPtrArray<JString>* list);

	static bool	GetFileList(const Atom selectionName, const Time time,
							JXDisplay* display,
							JPtrArray<JString>* fileNameList,
							JPtrArray<JString>* urlList);
	static bool	GetFileList(JXSelectionManager* selMgr,
							const Atom selectionName, const Time time,
							JPtrArray<JString>* fileNameList,
							JPtrArray<JString>* urlList);

protected:

	void	AddTypes(const Atom selectionName) override;
	bool	ConvertData(const Atom requestType, Atom* returnType,
						unsigned char** data, JSize* dataLength,
						JSize* bitsPerBlock) const override;

private:

	JPtrArray<JString>*	itsList;		// nullptr until after Resolve()
	JString*			itsBuffer;

private:

	void	JXFileSelectionX();
	void	CreateBuffer() const;
};


/******************************************************************************
 GetData

 ******************************************************************************/

inline bool
JXFileSelection::GetData
	(
	const JPtrArray<JString>** list
	)
	const
{
	*list = itsList;
	return itsList != nullptr;
}

#endif
