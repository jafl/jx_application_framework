#include <iostream>
#include "lldb/lldb-public.h"
#include "lldb/API/LLDB.h"

extern char **environ;

// http://lldb.llvm.org/cpp_reference/html/classes.html

using namespace std;
using namespace lldb;

bool bpCallback
	(
	void*						baton,
	SBProcess&					process,
	SBThread&					thread,
	lldb::SBBreakpointLocation&	location
	)
{
	cout << "breakpoint " << *(int*)baton << " hit" << endl;
	return true;
}

int main()
{
	cout << "Hello World!" << endl;

	SBDebugger::Initialize();

	SBDebugger dbg(SBDebugger::Create());
	cout << dbg.GetVersionString() << endl;
	cout << "async: " << dbg.GetAsync() << endl;
	dbg.SetAsync(false);

	cout << "debugger: " << dbg.IsValid() << endl;
	cout << dbg.GetPrompt() << endl;

	SBTarget t = dbg.CreateTarget("../var_tree/vartree");
	cout << "target: " << t.IsValid() << endl;
	cout << t.GetNumModules() << " modules" << endl;
	{
	SBFileSpec f = t.GetExecutable();
	cout << "dir: " << f.GetDirectory() << endl;
	cout << "file: " << f.GetFilename() << endl;

	char path[1024];
	f.GetPath(path, 1024);
	cout << "path: " << path << endl;
	}
	cout << "target listeners: " << t.GetBroadcaster().EventTypeHasListeners(255) << endl;

	cout << "-----" << endl;
	{
	SBSymbolContextList list = t.FindFunctions("main");
	cout << "count: " << list.GetSize() << endl;
	SBSymbolContext sym = list.GetContextAtIndex(0);
	cout << "symbol ctx: " << sym.IsValid() << endl;
	cout << sym.GetSymbol().GetName() << endl;
	cout << sym.GetSymbol().GetStartAddress().GetLoadAddress(t) << endl;
	cout << sym.GetModule().GetTriple() << endl;
 
	cout << "block: " << sym.GetBlock().GetInlinedCallSiteLine() << endl;

	cout << "entry: " << sym.GetLineEntry().IsValid() << endl;

	SBFileSpec f = sym.GetModule().GetFileSpec();
	cout << "filespec: " << f.IsValid() << endl;
	cout << f.GetDirectory() << '/' << f.GetFilename() << endl;

	cout << "compile unit file: " << sym.GetCompileUnit().GetFileSpec().GetFilename() << endl;

	int count = sym.GetCompileUnit().GetNumLineEntries();
	cout << "compile unit entries: " << count << endl;
	SBLineEntry e = sym.GetCompileUnit().GetLineEntryAtIndex(0);
	cout << "entry: " << e.IsValid() << endl;
	cout << "line: " << e.GetLine() << endl;
	f = e.GetFileSpec();
	cout << "entry filespec: " << f.IsValid() << endl;
	cout << "entry filename: " << f.GetFilename() << endl;
	cout << "entry path: " << f.GetDirectory() << endl;

	addr_t mainAddr = sym.GetSymbol().GetStartAddress().GetFileAddress();
	for (int i=0; i<count; i++)
		{
		SBLineEntry e = sym.GetCompileUnit().GetLineEntryAtIndex(i);
		if (e.GetStartAddress().GetFileAddress() == mainAddr)
			{
			cout << "found main: " << e.GetLine() << endl;
			}
		}

	SBFunction func = sym.GetFunction();
	cout << "function: " << func.IsValid() << endl;
	cout << func.GetName() << endl;
	}
	cout << "-----" << endl;

	SBBreakpoint b = t.BreakpointCreateByLocation("vartree.cc", 345);
	cout <<"breakpoint:" << b.IsValid() << endl;
	cout <<"enabled:" << b.IsEnabled() << endl;
	cout <<"locations:" << b.GetNumLocations() << endl;
	cout <<"resolved:" << b.GetNumResolvedLocations() << endl;

	SBBreakpointLocation loc = b.GetLocationAtIndex(0);
	cout <<"bkpt location:" << loc.IsValid() << endl;
	SBAddress addr = loc.GetAddress();
	cout <<"location address:" << addr.IsValid() << endl;
	SBLineEntry e = addr.GetLineEntry();
	cout <<"line entry:" << e.IsValid() << endl;
	cout <<"file:" << e.GetFileSpec().GetFilename() << endl;
	cout <<"line:" << e.GetLine() << endl;

	int baton = 5;
	b.SetCallback(bpCallback, &baton);

	cout << "-----" << endl;
	{
	SBSymbolContextList list = t.FindFunctions("foo");
	SBSymbolContext sym = list.GetContextAtIndex(0);
	cout << "symbol: " << sym.IsValid() << endl;
	cout << sym.GetSymbol().GetMangledName() << endl;
	cout << sym.GetModule().GetTriple() << endl;

	SBFileSpec f = sym.GetModule().GetFileSpec();
	cout << "filespec: " << f.IsValid() << endl;
	cout << f.GetDirectory() << '/' << f.GetFilename() << endl;

	SBFunction func = sym.GetFunction();
	cout << "function: " << func.IsValid() << endl;
	cout << func.GetMangledName() << endl;
	}
	cout << "-----" << endl;

	cout <<"bkpt count:" << t.GetNumBreakpoints() << endl;

	SBProcess p = t.LaunchSimple(NULL, (const char**) environ, ".");
	cout << "process: " << p.IsValid() << endl;
	cout << "pid: " << p.GetProcessID() << endl;
	cout << "plugin: " << p.GetPluginName() << endl;
	cout << "state: " << p.GetState() << endl;

	cout << "target listeners: " << t.GetBroadcaster().EventTypeHasListeners(255) << endl;
	cout << "process listeners: " << p.GetBroadcaster().EventTypeHasListeners(255) << endl;
//	cout << "thread listeners: " << p.GetSelectedThread().EventTypeHasListeners(255) << endl;

	cout << "-----" << endl;
	{
	SBFrame f = p.GetSelectedThread().GetSelectedFrame();
	cout << "frame: " << f.IsValid() << endl;

	SBValue v = f.FindVariable("b");
	cout << "variable: " << v.IsValid() << endl;
	cout << "children: " << v.GetNumChildren() << endl;

	SBValue v1 = v.GetChildAtIndex(0);
	cout << v1.GetName() << ": " << v1.IsValid() << endl;
	cout << "value: " << v1.GetValueAsSigned() << endl;

	v1 = v.GetChildAtIndex(1);
	cout << v1.GetName() << ": " << v1.IsValid() << endl;
	cout << "value: " << v1.GetValueAsSigned() << endl;

	SBValue v2 = f.EvaluateExpression("b.a+b.b");
	cout << "variable: " << v2.IsValid() << endl;
	cout << "value: " << v2.GetValueAsSigned() << endl;
	}
	cout << "-----" << endl;

	char c;
	cin >> c;
	p.Continue();

	char buf[1024];
	size_t count = p.GetSTDOUT(buf, 1024);
	cout << "output: ";
	cout.write(buf, count);

	cout << "exit status: " << p.GetExitStatus() << endl;
	cout << "bkpt hit count: " << b.GetHitCount() << endl;

	SBDebugger::Terminate();

	return 0;
}

