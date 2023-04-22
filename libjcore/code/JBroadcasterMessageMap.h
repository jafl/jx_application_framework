#ifndef _H_JBroadcasterMessageMap
#define _H_JBroadcasterMessageMap

/******************************************************************************
 JBroadcasterMessageMap.h

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include "JHashTable.h"
#include <boost/any.hpp>
#include <typeinfo>

class JBroadcaster;
class JBroadcasterMessageTarget;
struct JBroadcasterMessageTuple;
template <class T> class JArray;

class JBroadcasterMessageMap : public JHashTable<JBroadcasterMessageTarget>
{
public:

	JBroadcasterMessageMap();
	~JBroadcasterMessageMap() override;

	bool Contains(const std::type_info& key) const;
	bool GetElement(const std::type_info& key, const JBroadcaster* obj, boost::any** f) const;
	void SetElement(const std::type_info& key, JBroadcaster* obj, boost::any* f);

	bool GetList(const std::type_info& key, JArray<JBroadcasterMessageTuple>** f) const;

	void RemoveTuple(const std::type_info& key, const JBroadcaster* obj);
	void RemoveType(const std::type_info& key);
	void RemoveAll();
};

#endif
