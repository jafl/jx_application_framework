%define jcc_version   6.3.0
%define ctags_version 5.8.0
%define mm_version    3.3.0

Summary: Code Crusader is a UNIX development environment for X.
Name: Code_Crusader
Version: %jcc_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: Copyright John Lindal
Group: Development/Tools/IDE
Source: Code_Crusader_%jcc_version.tar

%description
Code Crusader is a graphical development environment for UNIX.
(http://www.newplanetsoftware.com/jcc/)

%package -n ctags
Summary: This is the latest version of ctags, for use with Code Crusader.
Version: %ctags_version
Release: 3
License: GPL
Group: Development/Building

%description -n ctags
This is the latest version of ctags, for use with Code Crusader.

%prep
%setup 

%install

%define jcc_doc_dir     /usr/share/doc/code_crusader
%define mm_doc_dir      /usr/share/doc/makemake
%define ctags_doc_dir   /usr/share/doc/ctags
%define jcc_lib_dir     /usr/lib/jxcb
%define gnome_app_path  /usr/share/applications
%define gnome_icon_path /usr/share/pixmaps

./install "" nozip

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

%files -n ctags

%docdir %ctags_doc_dir

/usr/bin/ctags
/usr/man/man1/ctags.1
%ctags_doc_dir
