/******************************************************************************
 test_jWebUtil.cpp

	Program to test jWebUtil.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/jWebUtil.h>
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(URL)
{
	JAssertTrue(JIsURL(JString("http://google.com", JString::kNoCopy)));
	JAssertTrue(JIsURL(JString("https://google.com", JString::kNoCopy)));
	JAssertTrue(JIsURL(JString("http://google.com/test", JString::kNoCopy)));
	JAssertFalse(JIsURL(JString("/usr/local", JString::kNoCopy)));

	JString protocol, host, path;
	JIndex port;
	JAssertTrue(JParseURL(
		JString("http://google.com", JString::kNoCopy),
		&protocol, &host, &port, &path));
	JAssertStringsEqual("http", protocol);
	JAssertStringsEqual("google.com", host);
	JAssertEqual(0, port);
	JAssertStringsEqual("/", path);

	JAssertTrue(JParseURL(
		JString("https://google.com:8443/foo/bar", JString::kNoCopy),
		&protocol, &host, &port, &path));
	JAssertStringsEqual("https", protocol);
	JAssertStringsEqual("google.com", host);
	JAssertEqual(8443, port);
	JAssertStringsEqual("/foo/bar", path);

	JAssertFalse(JParseURL(
		JString("/usr/local", JString::kNoCopy),
		&protocol, &host, &port, &path));
}
