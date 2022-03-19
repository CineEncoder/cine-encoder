Summary:    Video encoder
Name:       cine-encoder
Version:    3.5
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
Cine Encoder is an application, uses the FFmpeg, MKVToolNix and MediaInfo
utilities, that allows to convert media files while preserving HDR metadata.
Supported hardware encoding NVENC for codecs H265, H264 and Intel QSV
(for Windows and experimental for Linux) for codecs H264, MPEG-2.
The following encoding modes are implemented: H265, H264, VP9, MPEG-2,
XDCAM, DNxHR, ProRes.
Licence: GNU GPL v.3

%prep
%autosetup

%build
# Regenerate the Makefile with the Fedora qmake name
qmake-qt5 -o builddir/Makefile app/cine_encoder.pro -spec linux-g++ \
  CONFIG+=qtquickcompiler
cd builddir

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
cd builddir
%make_install INSTALL_ROOT=%{buildroot}
cd -
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
* Sat Mar 19 2022 Cine Encoder <depositmail@rambler.ru>
- Initial package for Fedora.
