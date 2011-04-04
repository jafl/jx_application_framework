/******************************************************************************
 JFSLibVersion.h

	Defines current version of JFS library

	Copyright © 1996-2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JFSLibVersion
#define _H_JFSLibVersion

// These have to be #defined so they can be used for conditional compilation.

#define CURRENT_JFS_MAJOR_VERSION	1
#define CURRENT_JFS_MINOR_VERSION	1
#define CURRENT_JFS_PATCH_VERSION	4

// This is mainly provided so programmers can see the official version number.

static const char* kCurrentJFSLibVersionStr = "1.1.4";

// version 1.1.4:
//	JXFSBindingManager:
//		Added Destroy().
//	JFSBinding:
//		Removed CreateContentRegex().
//		Supports file name patterns, not just suffixes.

// version 1.1.2:
//	*** All egcs thunks hacks have been removed.
//	JXFSDirMenu:
//		Added Create() to work around egcs thunks bug.
//		Added Will/ShouldDeleteBrokenLinks().
//	Changed default image viewer from xv to eog.
//	Added $qf and $uf to allow explicit use of full paths.
//		(Mozilla requires this for file: arguments.)
//	Changed default PDF viewer to xpdf.

// version 1.1.1:
//	JFSFileTreeNode:
//		Added second, optional argument "sort" to Rename().  This is useful because,
//			if the rename is done because the user clicked the mouse to initiate
//			another action, the files should not be sorted until the user releases
//			the mouse.
//	JXFSDirMenu:
//		Added SetFileIcon() and SetExecIcon().
//		Added SetEmptyMessage().

// version 1.1.0:
//	Initial release.  Minor version of 1 indicates that the API is not
//		guaranteed to be stable.
//	JXFSBindingManager is the only exported interface.

// pre-release changes:

// JFSFileTreeNodeBase:
//	Update() propagates call to children.
// JFSFileTreeNode:
//	Moved GetFSFileTree() and GetFSParent() to JFSFileTreeNodeBase.
//	Update() is now virtual.

//	Cleaned up object names.  Use lib/util/rename_symbol to update your code.
//	Removed alternate file binding.
//	Added option to run in shell or window.
//	Added flag to binding specifying whether or not it must be run for one
//		file at a time.  If not, cmd is executed once with all files.
//	Centralized work in JFSBindingList.
//	JFSBindingList is now maintained in memory.
//		It is reloaded if the file mod time changes.
//	JXFSDirMenu:
//		Call GetDirInfo() on top level menu, set any options you wish, and these
//			options will automatically propagate to submenus.
//	Eliminated need for JFSFileTree.
//	JFSFileTreeNode:
//		Completely redesigned interface.
//			Update() does everything automatically.
//			CreatChild() is the only function that needs to be overridden.
//		Added Rename() to safely rename a file/folder.
//		Requires parent node to derive from JFSFileTreeNodeBase.  This allows
//			root node of tree to not use its JUNIXDirInfo to build the children.
//			(The root node must still have a JUNIXDirInfo, however, because the
//			options are copied from there -- check the class docs.)
//	Created JFSFileTree to define DirectoryRenamed.

#endif
