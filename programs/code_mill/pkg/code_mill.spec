%define code_mill_version 1.0.5

Summary: Code Mill is a Code Crusader plug-in for creating C++ derived classes.
Name: Code_Mill
Version: %code_mill_version
Release: 1
License: Copyright John Lindal
Group: Development/Code Generators
Source: Code_Mill_%code_mill_version.tar

%description
Code Mill is a Code Crusader plug-in for creating C++ derived classes.
(http://www.newplanetsoftware.com/code_mill/)

%prep
%setup 

%install

%define code_mill_doc_dir /usr/share/doc/code_mill
%define code_mill_lib_dir /usr/lib/code_mill
%define gnome_app_path    /usr/share/applications
%define gnome_icon_path   /usr/share/pixmaps

./install "$RPM_BUILD_ROOT" nozip

%post
gunzip %code_mill_lib_dir/*.gz

%postun
rm -rf %code_mill_lib_dir

%files

%docdir %code_mill_doc_dir

/usr/bin/code_mill
%code_mill_lib_dir
%code_mill_doc_dir

%gnome_app_path/code_mill.desktop
%gnome_icon_path/code_mill.xpm
