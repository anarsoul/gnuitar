# Copyright 1999-2007 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header$

inherit eutils cvs autotools

DESCRIPTION="A GTK+ based guitar processor. Includes such effects as reverberator, sustain, distortion, phasor, wah-wah."
HOMEPAGE="gnuitar.com"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~amd64"
IUSE="jack alsa oss fftw3 sndfile float clipeverywhere"

RDEPEND="|| ( x11-libs/libX11 virtual/x11 )
	>=x11-libs/gtk+-2.4.0
	sys-libs/glibc"

DEPEND="${RDEPEND}
	sys-devel/autoconf
	>=sys-devel/gcc-3.4
	jack? ( media-sound/jack-audio-connection-kit )
	alsa? ( media-libs/alsa-lib )
	
	fftw3? ( sci-libs/fftw )
	sndfile? ( media-libs/libsndfile )
	"

ECVS_SERVER="gnuitar.cvs.sourceforge.net:/cvsroot/gnuitar"
ECVS_MODULE="gnuitar"
S="${WORKDIR}/gnuitar"

src_unpack() {
	cvs_src_unpack
	cd ${S}
	eautoreconf
}

src_compile() {
    econf --with-gtk2 \
	$(use_with jack jack) \
	$(use_with alsa alsa) \
        $(use_with oss oss) \
	$(use_with fftw3 fftw3) \
        $(use_with sndfile sndfile) \
        $(use_enable float float) \
	$(use_enable clipeverywhere clip-everywhere) || die "econf failed"													
    emake || die "emake failed"																									
}

src_install() {
    cd ${S}
    einstall || die "einstall failed"
}
