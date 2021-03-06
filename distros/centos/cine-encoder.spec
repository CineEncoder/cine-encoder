Summary:    Video encoder
Name:       cine-encoder
Version:    3.2
Release:    1%{?dist}
License:    GPLv3
URL:        https://github.com/CineEncoder/%{name}
Source0:    %{name}-%{version}.tar.xz

BuildRequires: gcc-c++
BuildRequires: libmediainfo-devel
BuildRequires: qt5-qtbase-devel
BuildRequires: qt5-qtmultimedia-devel
Requires: ffmpeg, mkvtoolnix

%description
Cine Encoder is an application that allows you to convert media
files while preserving HDR metadata.NVENC hardware encoding for 
NVIDIA video cards is also supported for H265 and H264 modes. 
The following encoding modes are implemented:

 - H265 NVENC (8, 10 bit)
 - H265 (8, 10 bit)
 - VP9 (8, 10 bit)
 - H264 NVENC (8 bit)
 - H264 (8 bit)
 - DNxHR (8, 10 bit)
 - ProRes (10 bit)

Supported operating system: Linux x86_64

Licence: GNU GPL v.3


%prep
%autosetup

%build
# Regenerate the Makefile with the Fedora qmake name
qmake-qt5 -o Makefile app/cine_encoder.pro -spec linux-g++ \
  CONFIG+=qtquickcompiler

%make_build

%install
%make_install INSTALL_ROOT=%{buildroot}
mkdir -p %{buildroot}/%{_datadir}/icons/hicolor/64x64/apps
mkdir -p %{buildroot}/%{_datadir}/applications
mkdir -p %{buildroot}/%{_datadir}/sounds
install -m 0644 share/%{name}.desktop %{buildroot}/%{_datadir}/applications
install -m 0644 share/%{name}.png %{buildroot}/%{_datadir}/icons/hicolor/64x64/apps
install -m 0644 share/%{name}.wav %{buildroot}/%{_datadir}/sounds

%files
%doc share/ABOUT
%license LICENSE
%{_bindir}/cine_encoder
%{_datadir}/applications/cine-encoder.desktop
%{_datadir}/icons/hicolor/64x64/apps/cine-encoder.png
%{_datadir}/sounds/cine-encoder.wav

%changelog
* Wed Mar 17 2021 Cine Encoder <depositmail@rambler.ru>
- Initial package for CentOS.
