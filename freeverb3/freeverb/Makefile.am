#  freeverb3 Makefile.am
#
#  Copyright (C) 2006-2014 Teru Kamogashira
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

include $(top_srcdir)/freeverb/src.make

if BUILD_STATIC
AM_LDFLAGS = -static
endif

EXTRA_DIST = \
	irmodel3w.cpp \
	irmodel3w_t.hpp \
	irmodel3w.hpp

AM_CPPFLAGS = -DLIBFV3_DEFAULT -I$(top_srcdir) -I$(top_builddir)
libfreeverbincludedir = ${includedir}/libfreeverb3-3/freeverb
libfreeverbinclude_HEADERS = \
	allpass.hpp \
	allpass_t.hpp \
	biquad.hpp \
	biquad_t.hpp \
	blockDelay.hpp \
	blockDelay_t.hpp \
	comb.hpp \
	comb_t.hpp \
	compmodel.hpp \
	compmodel_t.hpp \
	delay.hpp \
	delay_t.hpp \
	delayline.hpp \
	delayline_t.hpp \
	dl_gardner.hpp \
	dl_gardner_t.hpp \
	earlyref.hpp \
	earlyref_t.hpp \
	efilter.hpp \
	efilter_t.hpp \
	fir3bandsplit.hpp \
	fir3bandsplit_t.hpp \
	firfilter.hpp \
	firfilter_t.hpp \
	firwindow.hpp \
	firwindow_t.hpp \
	frag.hpp \
	frag_t.hpp \
	fv3_ch_tool.hpp \
	fv3_defs.h \
	fv3_ns_end.h \
	fv3_ns_start.h \
	fv3_pthread_tool.hpp \
	fv3_type_float.h \
	irbase.hpp \
	irbase_t.hpp \
	irmodel.hpp \
	irmodel_t.hpp \
	irmodel2.hpp \
	irmodel2_t.hpp \
	irmodel2zl.hpp \
	irmodel2zl_t.hpp \
	irmodel3.hpp \
	irmodel3_t.hpp \
	irmodel3p.hpp \
	irmodel3p_t.hpp \
	irmodels.hpp \
	irmodels_t.hpp \
	limitmodel.hpp \
	limitmodel_t.hpp \
	mls.hpp \
	mls_t.hpp \
	revbase.hpp \
	revbase_t.hpp \
	nrev.hpp \
	nrev_t.hpp \
	nrevb.hpp \
	nrevb_t.hpp \
	progenitor.hpp \
	progenitor_t.hpp \
	progenitor2.hpp \
	progenitor2_t.hpp \
	revmodel.hpp \
	revmodel_t.hpp \
	rms.hpp \
	rms_t.hpp \
	scomp.hpp \
	scomp_t.hpp \
	sweep.hpp \
	sweep_t.hpp \
	slimit.hpp \
	slimit_t.hpp \
	slot.hpp \
	slot_t.hpp \
	src.hpp \
	src_t.hpp \
	stenh.hpp \
	stenh_t.hpp \
	strev.hpp \
	strev_t.hpp \
	utils.hpp \
	utils_t.hpp \
	zrev_t.hpp \
	zrev.hpp \
	zrev2_t.hpp \
	zrev2.hpp \
	$(top_builddir)/fv3_config.h

lib_LTLIBRARIES = libfreeverb3.la

libfreeverb3_la_LDFLAGS = -version-info @LIBTOOL_VERSION_INFO@

libfreeverb3_la_LIBADD = \
	../libsamplerate2/libsamplerate2.la \
	$(fftw3f_LIBS) \
	$(fftw3_LIBS) \
	$(fftw3l_LIBS)

if BUILD_FLOAT
libfreeverb3_la_LIBADD += ../float/libfv3_float.la
libfreeverb3_la_LIBADD += ../float/libsr2_float.la
endif

if BUILD_DOUBLE
libfreeverb3_la_LIBADD += ../double/libfv3_double.la
libfreeverb3_la_LIBADD += ../double/libsr2_double.la
endif

if BUILD_LDOUBLE
libfreeverb3_la_LIBADD += ../ldouble/libfv3_ldouble.la
libfreeverb3_la_LIBADD += ../ldouble/libsr2_ldouble.la
endif

libfreeverb3_la_SOURCES =
