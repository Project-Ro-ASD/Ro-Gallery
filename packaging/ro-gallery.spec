%global debug_package %{nil}

Name:           ro-gallery
Version:        0.1.0
Release:        1%{?dist}
Summary:        Local photo library and viewer for Ro-ASD
License:        GPL-3.0-or-later
URL:            https://github.com/Project-Ro-ASD/Ro-Gallery
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  cmake
BuildRequires:  ninja-build
BuildRequires:  qt6-qtbase-devel
BuildRequires:  python3
BuildRequires:  desktop-file-utils
Requires:       qt6-qtimageformats

%description
Ro-Gallery is a native Qt 6 photo library and viewer. It supports local
folders, date grouping, filename search, favorites, zoom and pan.

%prep
%autosetup

%build
%cmake -G Ninja -DRO_GALLERY_VERSION=%{version}
%cmake_build

%install
%cmake_install

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/ro-gallery.desktop
%ctest

%files
%license LICENSE
%doc README.md
%{_bindir}/ro-gallery
%{_datadir}/applications/ro-gallery.desktop
%{_datadir}/icons/hicolor/scalable/apps/ro-gallery.svg
%{_datadir}/metainfo/org.roasd.RoGallery.metainfo.xml
