/*
**                    dfttest for Avisynth+
**
**   2D/3D frequency domain denoiser.
**
**   Copyright (C) 2007-2010 Kevin Stone, 2017 (C) DJATOM
**             (C) 2020 pinterf
**
**   This program is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   This program is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**   GNU General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with this program; if not, write to the Free Software
**   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "avs/config.h"
#ifdef INTEL_INTRINSICS
#include <immintrin.h>
#endif

void removeMean_AVX(float* dftc, const float* dftgc, const int ccnt, float* dftc2);
void addMean_AVX(float* dftc, const int ccnt, const float* dftc2);
void filter_0_AVX_8(float* dftc, const float* sigmas, const int ccnt, const float* pmin, const float* pmax, const float* sigmas2);

