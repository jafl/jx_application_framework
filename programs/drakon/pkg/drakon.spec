%define drakon_version 1.2.4

Summary: Drakon provides a simple way to manage UNIX processes.
Name: Drakon
Version: %drakon_version
Release: 1
License: GPL
Group: System/Monitoring
Source: Drakon_%drakon_version.tar

%description
Drakon provides a simple way to manage UNIX processes.
(http://www.newplanetsoftware.com/drakon/)

%prep
%setup

%install

%define drakon_doc_dir  /usr/share/doc/drakon
%define drakon_lib_dir  /usr/lib/drakon
%define gnome_app_path  /usr/share/applications
%define gnome_icon_path /usr/share/pixmaps

./install "" nozip

%post
gunzip %drakon_lib_dir/*.gz

%postun
rm -rf %drakon_lib_dir

%files

%docdir %drakon_doc_dir

/usr/bin/drakon
%drakon_lib_dir
%drakon_doc_dir

%gnome_app_path/drakon.desktop
%gnome_icon_path/drakon.xpm
