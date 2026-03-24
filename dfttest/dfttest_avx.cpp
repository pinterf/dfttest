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

#include "dfttest_avx.h"
#include "avs/config.h"

#include <immintrin.h>

#if defined(GCC) || defined(CLANG)
__attribute__((__target__("avx")))
#endif
void removeMean_AVX(float* dftc, const float* dftgc, const int ccnt, float* dftc2)
{
  const float gf = dftc[0] / dftgc[0];
  auto gf_asm = _mm256_broadcast_ss(reinterpret_cast<const float*>(&gf));

  for (int h = 0; h < ccnt; h += 8)
  {
    auto dftc_loop = _mm256_loadu_ps(dftc + h);
    auto dftgc_loop = _mm256_loadu_ps(dftgc + h);
    auto dftc2_loop = _mm256_loadu_ps(dftc2 + h);
    auto dftc2_result = _mm256_mul_ps(gf_asm, dftgc_loop);
    _mm256_storeu_ps(dftc2 + h, dftc2_result);
    auto dftc_result = _mm256_sub_ps(dftc_loop, dftc2_result);
    _mm256_storeu_ps(dftc + h, dftc_result);
  }
}

#if defined(GCC) || defined(CLANG)
__attribute__((__target__("avx")))
#endif
void addMean_AVX(float* dftc, const int ccnt, const float* dftc2)
{
  for (int h = 0; h < ccnt; h += 8)
  {
    auto dftc_loop = _mm256_loadu_ps(dftc + h);
    auto dftc2_loop = _mm256_loadu_ps(dftc2 + h);
    auto dftc_result = _mm256_add_ps(dftc2_loop, dftc_loop);
    _mm256_storeu_ps(dftc + h, dftc_result);
  }
}

#if defined(GCC) || defined(CLANG)
__attribute__((__target__("avx")))
#endif
void filter_0_AVX_8(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2)
{
  auto zero = _mm256_setzero_ps();
  auto sse_1em15 = _mm256_set1_ps(1e-15f);
  for (int h = 0; h < ccnt; h += 8)
  {
    auto dftc_loop = _mm256_loadu_ps(dftc + h); // R I r i
    auto sigmas_loop = _mm256_loadu_ps(sigmas + h); // S S s s // FIXME check: adjacent Sigma values should be the same!
    auto squares = _mm256_mul_ps(dftc_loop, dftc_loop); //R*R I*I r*r i*i

    auto sumofsquares = _mm256_add_ps(squares, _mm256_shuffle_ps(squares, squares, _MM_SHUFFLE(2, 3, 0, 1)));
    // R*R+I*I R*R+I*I r*r+i*i r*r+i*i from now SQ SQ sq sq

    auto diff = _mm256_sub_ps(sumofsquares, sigmas_loop); // SQ-S SQ-S sq-s sq-s // . is undefined
    // 1/x approximation, this part is giving a bit different result as C code
    // _mm256_rcp14_ps is not usable here it's AVX512
    auto div_as_rcp_of_ss = _mm256_rcp_ps(_mm256_add_ps(sumofsquares, sse_1em15)); // 1 / (sq+1e-15)

    auto coeff = _mm256_max_ps(zero, _mm256_mul_ps(diff, div_as_rcp_of_ss));
    // max((psd - sigmas[h]) / (psd + 1e-15f), 0.0f);

    auto result = _mm256_mul_ps(dftc_loop, coeff);
    _mm256_storeu_ps(dftc + h, result);
  }
}

