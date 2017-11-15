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

FV3_SRC = \
	../freeverb/allpass.cpp \
	../freeverb/allpass.hpp \
	../freeverb/allpass_t.hpp \
	../freeverb/biquad.cpp \
	../freeverb/biquad.hpp \
	../freeverb/biquad_t.hpp \
	../freeverb/blockDelay.cpp \
	../freeverb/blockDelay.hpp \
	../freeverb/blockDelay_t.hpp \
	../freeverb/comb.cpp \
	../freeverb/comb.hpp \
	../freeverb/comb_t.hpp \
	../freeverb/compmodel.cpp \
	../freeverb/compmodel.hpp \
	../freeverb/compmodel_t.hpp \
	../freeverb/delay.cpp \
	../freeverb/delay.hpp \
	../freeverb/delay_t.hpp \
	../freeverb/delayline.cpp \
	../freeverb/delayline.hpp \
	../freeverb/delayline_t.hpp \
	../freeverb/dl_gardner.cpp \
	../freeverb/dl_gardner.hpp \
	../freeverb/dl_gardner_t.hpp \
	../freeverb/earlyref.cpp \
	../freeverb/earlyref.hpp \
	../freeverb/earlyref_t.hpp \
	../freeverb/efilter.cpp \
	../freeverb/efilter.hpp \
	../freeverb/efilter_t.hpp \
	../freeverb/fir3bandsplit.cpp \
	../freeverb/fir3bandsplit.hpp \
	../freeverb/fir3bandsplit_t.hpp \
	../freeverb/firfilter.cpp \
	../freeverb/firfilter.hpp \
	../freeverb/firfilter_t.hpp \
	../freeverb/firwindow.cpp \
	../freeverb/firwindow.hpp \
	../freeverb/firwindow_t.hpp \
	../freeverb/frag.cpp \
	../freeverb/frag.hpp \
	../freeverb/frag_t.hpp \
	../freeverb/irbase.cpp \
	../freeverb/irbase.hpp \
	../freeverb/irbase_t.hpp \
	../freeverb/irmodel.cpp \
	../freeverb/irmodel.hpp \
	../freeverb/irmodel_t.hpp \
	../freeverb/irmodel2.cpp \
	../freeverb/irmodel2.hpp \
	../freeverb/irmodel2_t.hpp \
	../freeverb/irmodel2zl.cpp \
	../freeverb/irmodel2zl.hpp \
	../freeverb/irmodel2zl_t.hpp \
	../freeverb/irmodel3.cpp \
	../freeverb/irmodel3.hpp \
	../freeverb/irmodel3_t.hpp \
	../freeverb/irmodels.cpp \
	../freeverb/irmodels.hpp \
	../freeverb/irmodels_t.hpp \
	../freeverb/limitmodel.cpp \
	../freeverb/limitmodel.hpp \
	../freeverb/limitmodel_t.hpp \
	../freeverb/mls.cpp \
	../freeverb/mls.hpp \
	../freeverb/mls_t.hpp \
	../freeverb/revbase.cpp \
	../freeverb/revbase.hpp \
	../freeverb/revbase_t.hpp \
	../freeverb/nrev.cpp \
	../freeverb/nrev.hpp \
	../freeverb/nrev_t.hpp \
	../freeverb/nrevb.cpp \
	../freeverb/nrevb.hpp \
	../freeverb/nrevb_t.hpp \
	../freeverb/progenitor.cpp \
	../freeverb/progenitor.hpp \
	../freeverb/progenitor_t.hpp \
	../freeverb/progenitor2.cpp \
	../freeverb/progenitor2.hpp \
	../freeverb/progenitor2_t.hpp \
	../freeverb/revmodel.cpp \
	../freeverb/revmodel.hpp \
	../freeverb/revmodel_t.hpp \
	../freeverb/rms.cpp \
	../freeverb/rms.hpp \
	../freeverb/rms_t.hpp \
	../freeverb/scomp.cpp \
	../freeverb/scomp.hpp \
	../freeverb/scomp_t.hpp \
	../freeverb/sweep.cpp \
	../freeverb/sweep.hpp \
	../freeverb/sweep_t.hpp \
	../freeverb/slimit.cpp \
	../freeverb/slimit.hpp \
	../freeverb/slimit_t.hpp \
	../freeverb/slot.cpp \
	../freeverb/slot.hpp \
	../freeverb/slot_t.hpp \
	../freeverb/src.cpp \
	../freeverb/src.hpp \
	../freeverb/src_t.hpp \
	../freeverb/stenh.cpp \
	../freeverb/stenh.hpp \
	../freeverb/stenh_t.hpp \
	../freeverb/strev.cpp \
	../freeverb/strev.hpp \
	../freeverb/strev_t.hpp \
	../freeverb/utils.cpp \
	../freeverb/utils.hpp \
	../freeverb/utils_t.hpp \
	../freeverb/zrev.cpp \
	../freeverb/zrev.hpp \
	../freeverb/zrev_t.hpp \
	../freeverb/zrev2.cpp \
	../freeverb/zrev2.hpp \
	../freeverb/zrev2_t.hpp

if BUILD_PTHREAD
FV3_SRC += \
	../freeverb/irmodel3p.cpp \
	../freeverb/irmodel3p.hpp \
	../freeverb/irmodel3p_t.hpp
endif