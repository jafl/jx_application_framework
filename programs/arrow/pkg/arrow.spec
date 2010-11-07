%define arrow_version 2.1.0

Summary: A mail reading program for the X Window System.
Name: Arrow
Version: %arrow_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: Productivity/Networking/Email/Clients
Source: Arrow_%arrow_version.tar

%description
Arrow is am e-mail program that was written to address the e-mail needs
of users migrating from Windows and the Macintosh to Linux. It currently
supports viewing, composing, and organizing of e-mail, including secure
e-mail via PGP, POP access, and a spell checker.
(http://www.newplanetsoftware.com/arrow/)

%prep
%setup

%install

%define arrow_doc_dir   /usr/share/doc/arrow
%define arrow_lib_dir   /usr/lib/arrow
%define gnome_app_path  /usr/share/applications
%define gnome_icon_path /usr/share/pixmaps

./install "" nozip

%post
gunzip %arrow_lib_dir/*.gz

%postun
rm -rf %arrow_lib_dir

%files

%docdir %arrow_doc_dir

/usr/bin/arrow
%arrow_lib_dir
%arrow_doc_dir

%gnome_app_path/arrow.desktop
%gnome_icon_path/arrow.xpm
