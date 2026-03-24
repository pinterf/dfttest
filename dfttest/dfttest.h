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

#define _WIN32_WINNT 0x0500  // Windows Server 2000 or above

#include <windows.h>
#include <malloc.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <thread>
#include <condition_variable>
#include <time.h>
#include "avisynth.h"
#include "PlanarFrame.h"
#include "MersenneTwister.h"
#include <emmintrin.h>
#include "fmath.h"

typedef float fftwf_complex[2];
typedef struct fftwf_plan_s* fftwf_plan;
typedef fftwf_plan(*fftwf_plan_dft_r2c_3d_proc) (int nx, int ny, int nz, float* r, fftwf_complex* c, int flags);
typedef fftwf_plan(*fftwf_plan_dft_c2r_3d_proc) (int nx, int ny, int nz, fftwf_complex* c, float* r, int flags);
typedef fftwf_plan(*fftwf_plan_dft_r2c_2d_proc) (int nx, int ny, float* r, fftwf_complex* c, int flags);
typedef fftwf_plan(*fftwf_plan_dft_c2r_2d_proc) (int nx, int ny, fftwf_complex* c, float* r, int flags);
typedef void (*fftwf_destroy_plan_proc) (fftwf_plan);
typedef void (*fftwf_execute_dft_r2c_proc) (fftwf_plan, float*, fftwf_complex*);
typedef void (*fftwf_execute_dft_c2r_proc) (fftwf_plan, fftwf_complex*, float*);

#define FFTW_MEASURE (0U)
#define FFTW_DESTROY_INPUT (1U << 0)
#define FFTW_PRESERVE_INPUT (1U << 4)
#define FFTW_PATIENT (1U << 5)
#define FFTW_ESTIMATE (1U << 6)

#define EXTRA(a,b) (((a)%(b))?((b)-((a)%(b))):0)

struct PS_INFO;
void threadPool(PS_INFO* ps);
void func_0(PS_INFO* ps);
void func_1(PS_INFO* ps);


void removeMean_C(float* dftc, const float* dftgc, const int ccnt, float* dftc2);
void removeMean_SSE(float* dftc, const float* dftgc, const int ccnt, float* dftc2);

void addMean_C(float* dftc, const int ccnt, const float* dftc2);
void addMean_SSE(float* dftc, const int ccnt, const float* dftc2);

void filter_0_C(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_1_C(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_2_C(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_3_C(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_4_C(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_5_C(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_6_C(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_0_SSE(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_1_SSE(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_2_SSE(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_3_SSE4(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_4_SSE(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_5_SSE2(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);
void filter_6_SSE(float* dftc, const float* sigmas, const int ccnt,
  const float* pmin, const float* pmax, const float* sigmas2);

void proc0_uint8_to_float_C(const unsigned char* s0, const float* s1, float* d,
  const int p0, const int p1, const int offset_lsb);
void proc0_uint8_to_float_SSE2_4pixels(const unsigned char* s0, const float* s1, float* d,
  const int p0, const int p1, const int offset_lsb);
void proc0_uint8_to_float_SSE2_8pixels(const unsigned char* s0, const float* s1, float* d,
  const int p0, const int p1, const int offset_lsb);

void proc0_stacked16_to_float_C(const unsigned char* s0, const float* s1, float* d,
  const int p0, const int p1, const int offset_lsb);
void proc0_stacked16_to_float_SSE2_4pixels(const unsigned char* s0, const float* s1, float* d,
  const int p0, const int p1, const int offset_lsb);

void proc1_C(const float* s0, const float* s1, float* d,
  const int p0, const int p1);
void proc1_SSE_4(const float* s0, const float* s1, float* d,
  const int p0, const int p1);
void proc1_SSE_8(const float* s0, const float* s1, float* d,
  const int p0, const int p1);

void intcast_float_to_uint8_t_C(const float* p, unsigned char* dst, const int src_height,
  const int src_width, const int dst_pitch, const int width);
void intcast_float_to_stacked16_C(const float* p, unsigned char* dst, unsigned char* dst_lsb, const int src_height,
  const int src_width, const int dst_pitch, const int width);
void dither_C(const float* p, unsigned char* dst, const int src_height,
  const int src_width, const int dst_pitch, const int width, const int mode);
void intcast_float_to_uint8_t_SSE2_8pixels(const float* p, unsigned char* dst, const int src_height,
  const int src_width, const int dst_pitch, const int width);

double getWinValue(double n, double size, int win, double beta);
void createWindow(float* hw, const int tmode, const int tbsize,
  const int tosize, const int twin, const double tbeta, const int smode,
  const int sbsize, const int sosize, const int swin, const double sbeta);

class nlFrame
{
public:
  int fnum;
  PlanarFrame* pf, * ppf;
  nlFrame();
  nlFrame(PlanarFrame* tp, VideoInfo& vi);
  ~nlFrame();
  void setFNum(int i);
};

class nlCache
{
public:
  nlFrame** frames;
  int start_pos, size;
  nlCache();
  nlCache(int _size, PlanarFrame* tp, VideoInfo& vi);
  ~nlCache();
  void resetCacheStart(int first, int last);
  int getCachePos(int n);
};

using proc0_t = void(const unsigned char*, const float*, float*, const int, const int, const int);

struct PS_INFO {
  int pixelsize;
  bool zmean;
  int showx, showy, showp;
  int ccnt, stopz, barea, pos;
  int type, b, sbsize, sosize;
  int nthreads, tidx, * cty;
  float* hw, * sigmas, * dftr, ** ebuff;
  float* pmins, * pmaxs, * sigmas2, f0beta;
  fftwf_complex* dftc, * dftc2, * dftgc;
  fftwf_plan ft, fti;
  fftwf_execute_dft_r2c_proc fftwf_execute_dft_r2c;
  fftwf_execute_dft_c2r_proc fftwf_execute_dft_c2r;
  PlanarFrame* pf;
  nlCache* fc;
  const unsigned char** pfplut;
  int sheight[3], eheight[3], ofs_lsb[3];
  void (*removeMean)(float*, const float*, const int, float*);
  void (*addMean)(float*, const int, const float*);
  void (*filterCoeffs)(float*, const float*, const int, const float*,
    const float*, const float*);
  proc0_t* proc0;
  void (*proc1)(const float*, const float*, float*, const int,
    const int);
  std::mutex* csect;
  std::mutex mtx;
  std::condition_variable cv;
  int job_state; // 0=idle, 1=working, -1=shutdown
};

class dfttest : public GenericVideoFilter
{
private:
  bool has_at_least_v8;
  bool has_at_least_v12; // global lock support
  size_t avisynth_if_ver;
  size_t avisynth_bugfix_ver;
  IScriptEnvironment* env_saved; // save for destructor (global lock)
  int bits_per_pixel;
  int pixelsize;
  bool Y, U, V, zmean, lsb_in_flag, lsb_out_flag, quiet_flag;
  int proc_height; // Real processing height, not depending on the 8/16 bit stacked16 stuff.
  int ftype;
  float sigma, sigma2;
  float pmin, pmax;
  int sbsize, smode, sosize, swin;
  int tbsize, tmode, tosize, twin;
  int opt, threads, dither;
  int noxl, noyl, noxc, noyc;
  int barea, bvolume;
  int ccnt, ebframe;
  int lsb_in_hmul;
  double sbeta, tbeta;
  float** ebuff, * hw;
  float norm, * sigmas;
  float* pmins, * pmaxs, * sigmas2;
  fftwf_plan ft, fti, ftg;
  fftwf_complex* dftgc;
  PlanarFrame* dstPF;
  PlanarFrame* dstPF_lsb;
  PlanarFrame* dstPF_all;
  nlCache* fc;
  nlFrame* nlf;
  HINSTANCE hLib;
  PS_INFO** pssInfo;
  std::thread* thds;
  std::mutex csect;
  const char* sfile, * sfile2, * pminfile, * pmaxfile;
  VideoInfo vi_src;
  VideoInfo vi_byte;
  fftwf_destroy_plan_proc fftwf_destroy_plan;
  fftwf_plan_dft_r2c_3d_proc fftwf_plan_dft_r2c_3d;
  fftwf_plan_dft_c2r_3d_proc fftwf_plan_dft_c2r_3d;
  fftwf_plan_dft_r2c_2d_proc fftwf_plan_dft_r2c_2d;
  fftwf_plan_dft_c2r_2d_proc fftwf_plan_dft_c2r_2d;
  fftwf_execute_dft_r2c_proc fftwf_execute_dft_r2c;
  fftwf_execute_dft_c2r_proc fftwf_execute_dft_c2r;
  int mapn(int n);

  template<typename pixel_t>
  void copyPad(PlanarFrame* src, PlanarFrame* dst, IScriptEnvironment* env);

  PVideoFrame GetFrame_S(int n, IScriptEnvironment* env);
  PVideoFrame GetFrame_T(int n, IScriptEnvironment* env);
  PVideoFrame build_output_frame(PVideoFrame &src_for_fp, IScriptEnvironment* env);
  void bypass_plane(PlanarFrame& frame, int plane);
  void conv_result_plane_to_int(int width, int height, int b, int ebuff_index, IScriptEnvironment* env);
  void merge_msb_lsb();
  void processTemporalBlock(int pos);
  void loadFile(float* dest, const char* src, const float wscale,
    IScriptEnvironment* env);
  void getNoiseSpectrum(const char* fname, const char* nstring,
    float* dest, const float wscale, IScriptEnvironment* env);
  void sigmaFromString(float* dest, const char* sstring, const char* ssx,
    const char* ssy, const char* sst, const float wscale, IScriptEnvironment* env);
  void outputSigmaFile(const char* fname, const float* s, const float scale,
    const bool zmean2);

public:
  PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env) override;
  dfttest(PClip _child, bool _Y, bool _U, bool _V, int _ftype,
    float _sigma, float _sigma2, float _pmin, float _pmax, int _sbsize,
    int _smode, int _sosize, int _tbsize, int _tmode, int _tosize,
    int _swin, int _twin, double _sbeta, double _tbeta, bool _zmean,
    const char* _sfile, const char* _sfile2, const char* _pminfile,
    const char* _pmaxfile, float _f0beta, const char* _nfile, int _threads,
    int _opt, const char* _nstring, const char* _sstring, const char* _ssx,
    const char* _ssy, const char* _sst, const int _dither,
    bool _lsb_in_flag, bool _lsb_out_flag, bool _quiet_flag,
    IScriptEnvironment* env);
  ~dfttest();
  int __stdcall SetCacheHints(int cachehints, int frame_range) override
  {
    switch (cachehints)
    {
    case CACHE_GET_MTMODE:
      return (threads == 1 ? MT_MULTI_INSTANCE : MT_SERIALIZED);
    default:
      return 0;
    }
  }
};
