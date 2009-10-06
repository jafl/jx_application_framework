%define <Binary>_version <Version>

Summary: <Program> provides ...
Name: <Program>
Version: %<Binary>_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: ...
Group: ...
Source: <Binary>_%<Binary>_version.tar

%description
<Program> provides ...
(<URL>)

%prep
%setup

%install

%define <Binary>_doc_dir /usr/share/doc/<Binary>
%define <Binary>_lib_dir /usr/lib/<Binary>
%define gnome_app_path   /usr/share/applications
%define gnome_icon_path  /usr/share/pixmaps

./install "" nozip

%post
gunzip %<Binary>_lib_dir/*.gz

%postun
rm -rf %<Binary>_lib_dir

%files

%docdir %<Binary>_doc_dir

/usr/X11R6/bin/<Binary>
%<Binary>_lib_dir
%<Binary>_doc_dir

%gnome_app_path/<Binary>.desktop
%gnome_icon_path/<Binary>.xpm
