%define _unpackaged_files_terminate_build 0

Summary: Code Medic is a graphical debugging environment for UNIX.
Name: Code_Medic
Version: %pkg_version
Release: 1
License: Copyright New Planet Software, Inc.
Group: Development/Tools/Debuggers
Source: Code_Medic-%pkg_version.tar
Requires: lldb, editorconfig-libs, libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre

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

./install "$RPM_BUILD_ROOT" nozip

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
