/*
**                    dfttest for Avisynth+
**
**   2D/3D frequency domain denoiser.
**
**   Copyright (C) 2007-2010 Kevin Stone, 2017 (C) DJATOM
**             (C) 2020, 2026 pinterf
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
#include <immintrin.h>

void proc0_uint8_to_float_AVX2_8pixels(const unsigned char* s0, const float* s1, float* d,
  const int p0, const int p1, const int /*offset_lsb*/);

template<int bits_per_pixel>
void proc0_uint16_to_float_AVX2_8pixels(const unsigned char* s0, const float* s1, float* d,
  const int p0, const int p1, const int /*offset_lsb*/);

void proc0_float_to_float_AVX2_8pixels(const unsigned char* s0, const float* s1, float* d,
  const int p0, const int p1, const int /*offset_lsb*/);

void proc1_AVX2_8(const float* s0, const float* s1, float* d,
  const int p0, const int p1);

void filter_0_AVX2_8(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);

void filter_1_AVX2_8(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);

void filter_2_AVX2_8(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);

void filter_3_AVX2_8(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);

void filter_4_AVX2_8(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);

void filter_5_AVX2_8(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);

void filter_6_AVX2_8(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);


