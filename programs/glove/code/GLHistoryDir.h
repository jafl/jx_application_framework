/******************************************************************************
 GLHistoryDir.h

	Interface for the GLHistoryDir class

	Written by Glenn Bach - 1997.

 ******************************************************************************/

#ifndef _H_GLHistoryDir
#define _H_GLHistoryDir

#include <JXDocument.h>
#include <JXPM.h>

class GLHistory;
class JXTextMenu;
class JXMenuBar;

class GLHistoryDir : public JXDocument
{
public:

	GLHistoryDir(JXDirector* supervisor);

	virtual ~GLHistoryDir();
	void	AppendText(const JString& text, const JBoolean show = kJTrue);
	void	Print();
	void	WriteData(std::ostream& os);
	void	ReadData(std::istream& is);
	virtual JBoolean	NeedsSave() const override;
	virtual void		SafetySave(const JXDocumentManager::SafetySaveReason reason) override;
	virtual JBoolean	GetMenuIcon(const JXImage** icon) const override;

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;
	virtual JBoolean	OKToClose() override;
	virtual JBoolean	OKToRevert() override;
	virtual JBoolean	CanRevert() override;
	virtual void		DiscardChanges() override;

private:

	GLHistory* 	itsHistory;
	JXTextMenu*	itsFileMenu;
	JXMenuBar*	itsMenuBar;

private:

	void BuildWindow();
	void UpdateFileMenu();
	void HandleFileMenu(const JIndex index);

	// not allowed

	GLHistoryDir(const GLHistoryDir& source);
	const GLHistoryDir& operator=(const GLHistoryDir& source);

public:

	static const JUtf8Byte* kSessionChanged;
	
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
