#include "JStdError.h"
#include <boost/any.hpp>

class ItemSelected : public JBroadcaster::Message
{
public:

	ItemSelected(const JIndex index, const bool fromShortcut)
		:
		JBroadcaster::Message("item"),
		itsIndex(index), itsFromShortcutFlag(fromShortcut)
		{ };

	JIndex
	GetIndex() const
	{
		return itsIndex;
	};

	bool
	IsFromShortcut() const
	{
		return itsFromShortcutFlag;
	};

private:

	const JIndex	itsIndex;
	const bool		itsFromShortcutFlag;
};

template <class T>
void
make_call
	(
	const boost::any& f,
	const T& msg
	)
{
	(*boost::any_cast<std::function<void(const T&)>*>(f))(msg);
}

template <class T>
boost::any*
create_call
	(
	const std::function<void(const T&)>& f
	)
{
	return new boost::any(new std::function(f));
}

int
main
	(
	int argc,
	char** argv
	)
{
	auto* f = create_call(std::function([](const ItemSelected& msg)
	{
		std::cout << msg.GetIndex() << ' ' << msg.IsFromShortcut() << std::endl;
	}));

	make_call(*f, ItemSelected(3,false));
	return 0;
}
