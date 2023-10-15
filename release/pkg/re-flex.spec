Summary: Package to support JX Application Framework ci.
Name: re-flex
Version: %pkg_version
Release: 1
License: GPL
Group: System/Monitoring

%description
Package to support JX Application Framework ci.

%prep
cp -R %{_sourcedir}/* %{buildroot}

%files

/usr/local/lib
/usr/local/include
/usr/local/bin
