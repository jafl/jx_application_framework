%define glove_version 2.0.0

Summary: Data acquisition, manipulation, and analysis program for X.
Name: Glove
Version: %glove_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: Applications/Scientific
Source: Glove_%glove_version.tar

%description 
Glove was designed as a flexible tool to address a number of issues. We
needed a generic plotting tool, an interface to data acquisition systems,
sophisticated (and correct) statistical analysis, and the ability to easily
and intuitively manipulate the data we had.
(http://www.newplanetsoftware.com/glove/)

%prep
%setup

%install

%define glove_doc_dir   /usr/share/doc/glove
%define glove_lib_dir   /usr/lib/glove
%define gnome_app_path  /usr/share/applications
%define gnome_icon_path /usr/share/pixmaps

./install "" nozip

%post
gunzip %glove_lib_dir/*.gz

%postun
rm -rf %glove_lib_dir

%files

%docdir %glove_doc_dir

/usr/bin/glove
%glove_lib_dir
%glove_doc_dir

%gnome_app_path/glove.desktop
%gnome_icon_path/glove.xpm
