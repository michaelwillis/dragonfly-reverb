#  freeverb3 Makefile.am
#
#  Copyright (C) 2006-2013 Teru Kamogashira
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

if BUILD_STATIC
AM_LDFLAGS = -static
endif

noinst_LTLIBRARIES = libsamplerate2.la
libsamplerate2includedir = ${includedir}/libsamplerate2
libsamplerate2include_HEADERS = \
	samplerate2.h samplerate2_t.h samplerate2_common.h type_float.h
AM_CPPFLAGS = -I$(top_srcdir)
COEFF_HDRS = high_qual_coeffs.h mid_qual_coeffs.h fastest_coeffs.h \
slow_high_qual_coeffs.h slow_mid_qual_coeffs.h

noinst_HEADERS = common.h common_c.h float_cast.h $(COEFF_HDRS)
libsamplerate2_la_SOURCES = \
	samplerate_common.c src_common.c $(noinst_HEADERS)

if BUILD_SRCNEWCOEFFS
AM_CPPFLAGS += -DSRCNEWCOEFFS
endif
