Summary: An electronic notebook and to-do list program for X-Windows.
Name: buzzer
Version: @VERSION@
Release: @RELEASE@
ExclusiveArch: i386
ExclusiveOS: Linux
Copyright: Commercial
Group: Applications/Utilities
BuildRoot: %{_tmppath}/%{name}-root

%description
The Buzzer Electronic Notebook is a program for storing information and
tasks. Items are stored in a hierarchical list of folders. Items and groups
can be rearranged via drag-and-drag. To-do list items can be marked as
finished and as urgent. They can also have notes assosiated with them.

%files
%defattr(-,root,root)
%{_bindir}/*
/usr/share/applications/*
/usr/share/icons/mini/*
/etc/X11/applnk/Utilities/*
