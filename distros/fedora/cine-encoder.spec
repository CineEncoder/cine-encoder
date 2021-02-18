Summary:    Video encoder
Name:       cine-encoder
Version:    3.1
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
 - VP9 (10 bit)
 - AV1 (10 bit)
 - H264 NVENC (8 bit)
 - H264 (8 bit)
 - DNxHR HQX 4:2:2 (10 bit)
 - ProRes HQ 4:2:2 (10 bit)
 - ProRes 4444 4:4:4 (10 bit)

Supported operating system: Linux x86_64

Licence: GNU GPL v.3


%prep
%autosetup

%build
# Regenerate the Makefile with the Fedora qmake name
qmake-qt5 -o Makefile app/cine_encoder.pro -spec linux-g++ \
  CONFIG+=qtquickcompiler

# Modify the Makefile to use Fedora flags.  This would not be necessary if the
# Makefile allowed overriding variable settings (e.g., with ?=).
sed -e 's|-pipe -O2 -Wall -Wextra|%{build_cflags}|' \
    -e 's|-pipe -O2 -std=gnu++11 -Wall -Wextra|%{build_cxxflags}|' \
    -e 's|-Wl,-O1|%{build_ldflags}|' \
    -e 's|= strip|=|' \
    -e 's|/opt/cine_encoder|%{_prefix}|' \
    -i Makefile

%make_build

%install
%make_install INSTALL_ROOT=%{buildroot}
mkdir -p %{buildroot}/%{_datadir}/icons/hicolor/64x64/apps
mkdir -p %{buildroot}/%{_datadir}/applications
mkdir -p %{buildroot}/%{_datadir}/sounds
install -m 0644 %{name}.png %{buildroot}/%{_datadir}/icons/hicolor/64x64/apps
install -m 0644 %{name}.desktop %{buildroot}/%{_datadir}/applications
install -m 0644 %{name}.wav %{buildroot}/%{_datadir}/sounds

%files
%doc ABOUT README.md
%license LICENSE
%{_bindir}/cine_encoder
%{_datadir}/icons/hicolor/64x64/apps/cine-encoder.png
%{_datadir}/applications/cine-encoder.desktop
%{_datadir}/sounds/cine-encoder.wav

%changelog
* Thu Jan 31 2021 Cine Encoder <depositmail@rambler.ru>
- Initial package for Fedora.
