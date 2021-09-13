%define mm_version 3.4.1

Summary: Code Crusader is a UNIX development environment for X.
Name: Code_Crusader
Version: %pkg_version
Release: 1
License: Copyright John Lindal
Group: Development/Tools/IDE
Source: Code_Crusader_%pkg_version.tar
Requires: editorconfig-libs, libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre

%description
Code Crusader is a graphical development environment for UNIX.
(http://www.newplanetsoftware.com/jcc/)

%prep
%setup 

%install

%define jcc_doc_dir     /usr/share/doc/code_crusader
%define mm_doc_dir      /usr/share/doc/makemake
%define jcc_lib_dir     /usr/lib/jxcb
%define gnome_app_path  /usr/share/applications
%define gnome_icon_path /usr/share/pixmaps

./install "$RPM_BUILD_ROOT" nozip

%post
gunzip %jcc_lib_dir/*.gz

%postun
rm -rf %jcc_lib_dir

%files

%docdir %jcc_doc_dir

/usr/bin/code_crusader
/usr/bin/jcc
/usr/bin/dirdiff
/usr/bin/xml-auto-close
/usr/bin/html-auto-close
%jcc_lib_dir
%jcc_doc_dir

%gnome_app_path/code_crusader.desktop
%gnome_icon_path/code_crusader.xpm

%docdir %mm_doc_dir

/usr/bin/makemake
/usr/bin/makecheck
/usr/bin/maketouch
%mm_doc_dir
