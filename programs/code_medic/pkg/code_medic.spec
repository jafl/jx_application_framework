%define medic_version 3.3.0

Summary: Code Medic is a graphical debugging environment for UNIX.
Name: Code_Medic
Version: %medic_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: Copyright New Planet Software, Inc.
Group: Development/Tools/Debuggers
Source: Code_Medic-%medic_version.tar

%description
Code Medic is a graphical debugging environment for UNIX.
It supports gdb and Xdebug.
(http://www.newplanetsoftware.com/medic/)

%prep
%setup

%install

%define medic_doc_dir   /usr/share/doc/code_medic
%define medic_lib_dir   /usr/lib/medic
%define gnome_app_path  /usr/share/applications
%define gnome_icon_path /usr/share/pixmaps

./install "" nozip

%post
gunzip %medic_lib_dir/*.gz

%postun
rm -rf %medic_lib_dir

%files

%docdir %medic_doc_dir

/usr/bin/medic
%medic_doc_dir
%medic_lib_dir

%gnome_app_path/code_medic.desktop
%gnome_icon_path/code_medic.xpm
