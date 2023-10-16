Summary: The ADAPTIVE Communication Environment (ACE)
Name:    ace
Version: %pkg_version
Release: 1.0
Group:   Development/Libraries/C and C++
URL:     http://www.cs.wustl.edu/~schmidt/ACE.html
License: DOC License

%description
Package to support JX Application Framework ci.

%prep
cp -R %{_sourcedir} %{buildroot}

%files

/usr/local/lib
/usr/local/include
