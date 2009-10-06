%define mondrian_version 1.0.0

Summary: Mondrian provides ...
Name: Mondrian
Version: %mondrian_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: Development/Tools/GUI Builders
Source: mondrian_%mondrian_version.tar

%description
Mondrian provides ...
(http://www.newplanetsoftware.com/)

%prep
%setup

%install

%define mondrian_doc_dir /usr/share/doc/mondrian
%define mondrian_lib_dir /usr/lib/mondrian
%define gnome_app_path   /usr/share/applications
%define gnome_icon_path  /usr/share/pixmaps

./install "" nozip

%post
gunzip %mondrian_lib_dir/*.gz

%postun
rm -rf %mondrian_lib_dir

%files

%docdir %mondrian_doc_dir

/usr/X11R6/bin/mondrian
%mondrian_lib_dir
%mondrian_doc_dir

%gnome_app_path/mondrian.desktop
%gnome_icon_path/mondrian.xpm
