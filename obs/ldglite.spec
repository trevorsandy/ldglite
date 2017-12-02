%if 0%{?suse_version}
%define dist .openSUSE%(echo %{suse_version} | sed 's/0$//')
%endif

%if 0%{?sles_version}
%define dist .SUSE%(echo %{sles_version} | sed 's/0$//')
%endif

%if "%{vendor}" == "obs://build.opensuse.org/home:pbartfai"
%define opensuse_bs 1
%endif

%if "%{vendor}" == "obs://private/home:pbartfai"
%define opensuse_bs 1
%endif

%if 0%{?centos_ver}
%define centos_version %{centos_ver}00
%endif

%if 0%{?fedora} || 0%{?centos_version} || 0%{?scientificlinux_version}
BuildRequires: mesa-libOSMesa-devel
%endif

%if 0%{?mageia}
%define dist .mga%{mgaversion}
%define distsuffix .mga%{mgaversion}
%ifarch x86_64
BuildRequires: lib64osmesa-devel
%else
BuildRequires: libosmesa-devel
%endif
%endif

%if 0%{?suse_version} > 1300
BuildRequires: Mesa-devel
%endif

%if 0%{?rhel_version}
%define without_osmesa 1
%endif

%if 0%{?sles_version}
# SLE 11 SP3 has no libOSMesa.so
%define osmesa_found %(test -f /usr/lib/libOSMesa.so -o -f /usr/lib64/libOSMesa.so && echo 1 || echo 0)
%if "%{osmesa_found}" != "1"
%define without_osmesa 1
%endif
%endif

%if 0%{?opensuse_bs}
%if 0%{?fedora_version} == 25
BuildRequires: llvm-libs
%endif
%endif

%if 0%{?mdkversion}
%define without_osmesa 1
%endif

Summary: 3D Viewer for LDraw models
Name: ldglite
%if 0%{?suse_version} || 0%{?sles_version}
Group: Productivity/Graphics/Viewers
%endif
%if 0%{?mdkversion} || 0%{?rhel_version}
Group: Graphics
%endif
%if 0%{?fedora} || 0%{?centos_version}
Group: Amusements/Graphics
%endif
Version: 1.3.1
Release: 1%{?dist}
%if 0%{?mdkversion} || 0%{?rhel_version} || 0%{?fedora} || 0%{?centos_version} || 0%{?scientificlinux_version} || 0%{?mageia}
License: GPLv2+
%endif
%if 0%{?suse_version} || 0%{?sles_version}
License: GPL-2.0+
BuildRequires: fdupes
%endif
URL: http://ldglite.sourceforge.net
Vendor: Don Heyse <dheyse@hotmail.com>
Packager: Peter Bartfai <pbartfai@stardust.hu>
BuildRoot: %{_builddir}/%{name}

BuildRequires: make, gcc, gcc-c++, freeglut-devel, libpng-devel
Requires: nawk

%if 0%{?rhel_version} || 0%{?centos_version}
BuildRequires: libXext-devel
%endif

Source0: ldglite.tar.gz
Patch0: ldglite.patch

%description
iLdglite is a program that lets you view and edit Lego brick models stored in LDRAW format. It was created by connecting the LDLite DAT file parser to an OpenGL rendering engine, making it portable to other operating systems. Along the way the L3 parser from L3P and a few nifty features from other DAT file viewers such as L3Lab were added or emulated. Most recently, an LEdit emulation mode was added. This gives you the ability to create and edit models in ldglite in addition to the viewing capabilities. The LEdit mode has several extensions including a hose maker that can also be used to generate minifig chains. Another new feature is the ability to function as a cheesy scene modeler for l3p and POV. Several internet sites use ldglite as a scripted offscreen renderer, generating many pictures of ldraw parts for inventory lists and such. Possibly the most important feature of ldglite is that all of the source code is available.

%prep
cd $RPM_SOURCE_DIR
if [ -d ldglite ] ; then rm -rf ldglite ; fi
tar zxf %{SOURCE0}
patch -p0 < %{PATCH0}

%build
cd $RPM_SOURCE_DIR/ldglite
%if "%{without_osmesa}" != "1"
make -f makefile.linux ENABLE_OFFSCREEN_RENDERING=Yes
%else
make -f makefile.linux ENABLE_OFFSCREEN_RENDERING=No
%endif


%install
cd $RPM_SOURCE_DIR/ldglite
install -d $RPM_BUILD_ROOT%{_bindir}
install -d $RPM_BUILD_ROOT%{_datadir}/ldglite
install -d $RPM_BUILD_ROOT%{_mandir}/man1
install -m 755 ldglite $RPM_BUILD_ROOT%{_bindir}/ldglite
install -m 644 readme.txt $RPM_BUILD_ROOT%{_datadir}/ldglite/readme.txt
install -m 644 todo.txt $RPM_BUILD_ROOT%{_datadir}/ldglite/todo.txt
install -m 644 doc/LDGLITE.TXT $RPM_BUILD_ROOT%{_datadir}/ldglite/ldglite.txt
install -m 644 doc/LICENCE $RPM_BUILD_ROOT%{_datadir}/ldglite/licence
install -m 644 ldglite.1 $RPM_BUILD_ROOT%{_mandir}/man1/ldglite.1
gzip -f $RPM_BUILD_ROOT%{_mandir}/man1/ldglite.1

%files
%if 0%{?sles_version} || 0%{?suse_version}
%defattr(-,root,root)
%endif
%{_bindir}/ldglite
%dir %{_datadir}/ldglite
%doc %{_datadir}/ldglite/readme.txt
%doc %{_datadir}/ldglite/todo.txt
%doc %{_datadir}/ldglite/ldglite.txt
%doc %{_datadir}/ldglite/licence
%if 0%{?mdkversion} || 0%{?mageia}
%{_mandir}/man1/ldglite.1.xz
%else
%{_mandir}/man1/ldglite.1.gz
%endif

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Tue Jul 15 2014 - pbartfai (at) stardust.hu 1.2.6-1
- Initial version
