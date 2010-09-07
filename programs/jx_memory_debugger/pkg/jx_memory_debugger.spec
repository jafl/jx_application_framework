%define jx_memory_debugger_version 1.0.0

Summary: JX Memory Debugger provides ...
Name: JX Memory Debugger
Version: %jx_memory_debugger_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: ...
Group: ...
Source: jx_memory_debugger_%jx_memory_debugger_version.tar

%description
JX Memory Debugger provides ...
(http://newplanetsoftware.com/jx)

%prep
%setup

%install

%define jx_memory_debugger_doc_dir /usr/share/doc/jx_memory_debugger
%define jx_memory_debugger_lib_dir /usr/lib/jx_memory_debugger
%define gnome_app_path   /usr/share/applications
%define gnome_icon_path  /usr/share/pixmaps

./install "" nozip

%post
gunzip %jx_memory_debugger_lib_dir/*.gz

%postun
rm -rf %jx_memory_debugger_lib_dir

%files

%docdir %jx_memory_debugger_doc_dir

/usr/bin/jx_memory_debugger
%jx_memory_debugger_lib_dir
%jx_memory_debugger_doc_dir

%gnome_app_path/jx_memory_debugger.desktop
%gnome_icon_path/jx_memory_debugger.xpm
