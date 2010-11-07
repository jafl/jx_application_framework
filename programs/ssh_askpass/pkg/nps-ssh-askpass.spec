%define askpass_version 1.0.3

Summary: nps-ssh-askpass provides a cross-platform user interface for SSH to ask for a password.
Name: nps-ssh-askpass
Version: %askpass_version
Release: 1
ExclusiveArch: i386
ExclusiveOS: Linux
License: GPL
Group: Utilities
Source: nps_ssh_askpass_%askpass_version.tar

%description
nps-ssh-askpass provides a cross-platform user interface for SSH
to ask for a password.
(http://www.newplanetsoftware.com/ssh_askpass/)

%prep
%setup

%install

%define askpass_doc_dir  /usr/share/doc/nps-ssh-askpass
%define askpass_lib_dir  /usr/lib/nps-ssh-askpass

./install "" nozip

%post
gunzip %askpass_lib_dir/*.gz

%postun
rm -rf %askpass_lib_dir

%files

%docdir %askpass_doc_dir

/usr/bin/nps-ssh-askpass
%askpass_lib_dir
%askpass_doc_dir
