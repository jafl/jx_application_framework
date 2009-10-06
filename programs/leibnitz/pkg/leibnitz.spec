%define leibnitz_version 2.0.0

Summary: Leibnitz provides electronic paper for calculations and graphs.
Name: Leibnitz
Version: %leibnitz_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: Applications/Scientific
Source: Leibnitz_%leibnitz_version.tar

%description
Leibnitz provides electronic paper for calculations and graphs.
(http://www.newplanetsoftware.com/leibnitz/)

%prep
%setup

%install

%define leibnitz_doc_dir  /usr/share/doc/leibnitz
%define leibnitz_lib_dir  /usr/lib/leibnitz
%define gnome_app_path    /usr/share/applications
%define gnome_icon_path   /usr/share/pixmaps

./install "" nozip

%post
gunzip %leibnitz_lib_dir/*.gz

%postun
rm -rf %leibnitz_lib_dir

%files

%docdir %leibnitz_doc_dir

/usr/X11R6/bin/leibnitz
%leibnitz_lib_dir
%leibnitz_doc_dir

%gnome_app_path/leibnitz.desktop
%gnome_icon_path/leibnitz.xpm
