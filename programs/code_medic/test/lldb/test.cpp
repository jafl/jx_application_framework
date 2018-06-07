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
	std::cout << "breakpoint " << *(int*)baton << " hit" << std::endl;
	return true;
}

int main()
{
	std::cout << "Hello World!" << std::endl;

	SBDebugger::Initialize();

	SBDebugger dbg(SBDebugger::Create());
	std::cout << dbg.GetVersionString() << std::endl;
	std::cout << "async: " << dbg.GetAsync() << std::endl;
	dbg.SetAsync(false);

	std::cout << "debugger: " << dbg.IsValid() << std::endl;
	std::cout << dbg.GetPrompt() << std::endl;

	SBTarget t = dbg.CreateTarget("../var_tree/vartree");
	std::cout << "target: " << t.IsValid() << std::endl;
	std::cout << t.GetNumModules() << " modules" << std::endl;
	{
	SBFileSpec f = t.GetExecutable();
	std::cout << "dir: " << f.GetDirectory() << std::endl;
	std::cout << "file: " << f.GetFilename() << std::endl;

	char path[1024];
	f.GetPath(path, 1024);
	std::cout << "path: " << path << std::endl;
	}
	std::cout << "target listeners: " << t.GetBroadcaster().EventTypeHasListeners(255) << std::endl;

	std::cout << "-----" << std::endl;
	{
	SBSymbolContextList list = t.FindFunctions("main");
	std::cout << "count: " << list.GetSize() << std::endl;
	SBSymbolContext sym = list.GetContextAtIndex(0);
	std::cout << "symbol ctx: " << sym.IsValid() << std::endl;
	std::cout << sym.GetSymbol().GetName() << std::endl;
	std::cout << sym.GetSymbol().GetStartAddress().GetLoadAddress(t) << std::endl;
	std::cout << sym.GetModule().GetTriple() << std::endl;
 
	std::cout << "block: " << sym.GetBlock().GetInlinedCallSiteLine() << std::endl;

	std::cout << "entry: " << sym.GetLineEntry().IsValid() << std::endl;

	SBFileSpec f = sym.GetModule().GetFileSpec();
	std::cout << "filespec: " << f.IsValid() << std::endl;
	std::cout << f.GetDirectory() << '/' << f.GetFilename() << std::endl;

	std::cout << "compile unit file: " << sym.GetCompileUnit().GetFileSpec().GetFilename() << std::endl;

	int count = sym.GetCompileUnit().GetNumLineEntries();
	std::cout << "compile unit entries: " << count << std::endl;
	SBLineEntry e = sym.GetCompileUnit().GetLineEntryAtIndex(0);
	std::cout << "entry: " << e.IsValid() << std::endl;
	std::cout << "line: " << e.GetLine() << std::endl;
	f = e.GetFileSpec();
	std::cout << "entry filespec: " << f.IsValid() << std::endl;
	std::cout << "entry filename: " << f.GetFilename() << std::endl;
	std::cout << "entry path: " << f.GetDirectory() << std::endl;

	addr_t mainAddr = sym.GetSymbol().GetStartAddress().GetFileAddress();
	for (int i=0; i<count; i++)
		{
		SBLineEntry e = sym.GetCompileUnit().GetLineEntryAtIndex(i);
		if (e.GetStartAddress().GetFileAddress() == mainAddr)
			{
			std::cout << "found main: " << e.GetLine() << std::endl;
			}
		}

	SBFunction func = sym.GetFunction();
	std::cout << "function: " << func.IsValid() << std::endl;
	std::cout << func.GetName() << std::endl;
	}
	std::cout << "-----" << std::endl;

	SBBreakpoint b = t.BreakpointCreateByLocation("vartree.cc", 345);
	std::cout <<"breakpoint:" << b.IsValid() << std::endl;
	std::cout <<"enabled:" << b.IsEnabled() << std::endl;
	std::cout <<"locations:" << b.GetNumLocations() << std::endl;
	std::cout <<"resolved:" << b.GetNumResolvedLocations() << std::endl;

	SBBreakpointLocation loc = b.GetLocationAtIndex(0);
	std::cout <<"bkpt location:" << loc.IsValid() << std::endl;
	SBAddress addr = loc.GetAddress();
	std::cout <<"location address:" << addr.IsValid() << std::endl;
	SBLineEntry e = addr.GetLineEntry();
	std::cout <<"line entry:" << e.IsValid() << std::endl;
	std::cout <<"file:" << e.GetFileSpec().GetFilename() << std::endl;
	std::cout <<"line:" << e.GetLine() << std::endl;

	int baton = 5;
	b.SetCallback(bpCallback, &baton);

	std::cout << "-----" << std::endl;
	{
	SBSymbolContextList list = t.FindFunctions("foo");
	SBSymbolContext sym = list.GetContextAtIndex(0);
	std::cout << "symbol: " << sym.IsValid() << std::endl;
	std::cout << sym.GetSymbol().GetMangledName() << std::endl;
	std::cout << sym.GetModule().GetTriple() << std::endl;

	SBFileSpec f = sym.GetModule().GetFileSpec();
	std::cout << "filespec: " << f.IsValid() << std::endl;
	std::cout << f.GetDirectory() << '/' << f.GetFilename() << std::endl;

	SBFunction func = sym.GetFunction();
	std::cout << "function: " << func.IsValid() << std::endl;
	std::cout << func.GetMangledName() << std::endl;
	}
	std::cout << "-----" << std::endl;

	std::cout <<"bkpt count:" << t.GetNumBreakpoints() << std::endl;

	SBProcess p = t.LaunchSimple(nullptr, (const char**) environ, ".");
	std::cout << "process: " << p.IsValid() << std::endl;
	std::cout << "pid: " << p.GetProcessID() << std::endl;
	std::cout << "plugin: " << p.GetPluginName() << std::endl;
	std::cout << "state: " << p.GetState() << std::endl;

	std::cout << "target listeners: " << t.GetBroadcaster().EventTypeHasListeners(255) << std::endl;
	std::cout << "process listeners: " << p.GetBroadcaster().EventTypeHasListeners(255) << std::endl;
//	std::cout << "thread listeners: " << p.GetSelectedThread().EventTypeHasListeners(255) << std::endl;

	std::cout << "-----" << std::endl;
	{
	SBFrame f = p.GetSelectedThread().GetSelectedFrame();
	std::cout << "frame: " << f.IsValid() << std::endl;

	SBValue v = f.FindVariable("b");
	std::cout << "variable: " << v.IsValid() << std::endl;
	std::cout << "children: " << v.GetNumChildren() << std::endl;

	SBValue v1 = v.GetChildAtIndex(0);
	std::cout << v1.GetName() << ": " << v1.IsValid() << std::endl;
	std::cout << "value: " << v1.GetValueAsSigned() << std::endl;

	v1 = v.GetChildAtIndex(1);
	std::cout << v1.GetName() << ": " << v1.IsValid() << std::endl;
	std::cout << "value: " << v1.GetValueAsSigned() << std::endl;

	SBValue v2 = f.EvaluateExpression("b.a+b.b");
	std::cout << "variable: " << v2.IsValid() << std::endl;
	std::cout << "value: " << v2.GetValueAsSigned() << std::endl;
	}
	std::cout << "-----" << std::endl;

	char c;
	std::cin >> c;
	p.Continue();

	char buf[1024];
	size_t count = p.GetSTDOUT(buf, 1024);
	std::cout << "output: ";
	std::cout.write(buf, count);

	std::cout << "exit status: " << p.GetExitStatus() << std::endl;
	std::cout << "bkpt hit count: " << b.GetHitCount() << std::endl;

	SBDebugger::Terminate();

	return 0;
}

