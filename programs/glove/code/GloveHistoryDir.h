/******************************************************************************
 GloveHistoryDir.h

	Interface for the GloveHistoryDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_GloveHistoryDir
#define _H_GloveHistoryDir

#include <JXDocument.h>
#include <JXPM.h>

class GloveHistory;
class JXTextMenu;
class JXMenuBar;

class GloveHistoryDir : public JXDocument
{
public:

	GloveHistoryDir(JXDirector* supervisor);

	virtual ~GloveHistoryDir();
	void	AppendText(const JCharacter* text, const JBoolean show = kJTrue);
	void	Print();
	void	WriteData(std::ostream& os);
	void	ReadData(std::istream& is);
	virtual JBoolean	NeedsSave() const;
	virtual void		SafetySave(const JXDocumentManager::SafetySaveReason reason);
	virtual JXPM		GetMenuIcon() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual JBoolean	OKToClose();
	virtual JBoolean	OKToRevert();
	virtual JBoolean	CanRevert();
	virtual void		DiscardChanges();

private:

	GloveHistory*	 		itsHistory;
	JXTextMenu*				itsFileMenu;
	JXMenuBar*				itsMenuBar;

private:

	void BuildWindow();
	void UpdateFileMenu();
	void HandleFileMenu(const JIndex index);

	// not allowed

	GloveHistoryDir(const GloveHistoryDir& source);
	const GloveHistoryDir& operator=(const GloveHistoryDir& source);

public:

	static const JCharacter* kSessionChanged;
	
	class SessionChanged : public JBroadcaster::Message
	{
	public:
	
		SessionChanged()
			:
			JBroadcaster::Message(kSessionChanged)
			{ };
	};
	
};

#endif
