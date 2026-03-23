
                                       dfttest v1.9.8
                                       Original code by tritical
                                       16-bit modification by Firesledge
                                       Avs+ port by DJATOM
                                       High bit depth support and other works by pinterf

Info:

   2D/3D frequency domain denoiser.

   Requires libfftw3f-3.dll to be in the search path.
   http://www.fftw.org/install/windows.html


---------------------------------------------------------------------------------------------------

Syntax:

   dfttest(bool Y, bool U, bool V, int ftype, float sigma, float sigma2, float pmin,
           float pmax, int sbsize, int smode, int sosize, int tbsize, int tmode,
           int tosize, int swin, int twin, float sbeta, float tbeta, bool zmean,
           string sfile, string sfile2, string pminfile, string pmaxfile, float f0beta,
           string nfile, int threads, int opt, string nstring, string sstring,
           string ssx, string ssy, string sst, int dither, bool lsb, bool lsb_in,
           bool quiet)


---------------------------------------------------------------------------------------------------

Parameters:

   Y,U,V -

       If true, the corresponding plane is processed.  Otherwise, it is copied through
       to the output image as is.

       default:  true,true,true


   ftype -

       Controls the filter type.  Possible settings are:

           0 -  generalized wiener filter

                 mult = max((psd-sigma)/psd,0)^f0beta

           1 -  hard threshold

                 mult = psd < sigma ? 0.0 : 1.0;

           2 -  multiplier

                 mult = sigma

           3 -  multiplier switched based on psd value

                 mult = (psd >= pmin && psd <= pmax) ? sigma : sigma2

           4 -  multiplier modified based on psd value and range

                 mult = sigma*sqrt((psd*pmax)/((psd+pmin)*(psd+pmax)))

       The real and imaginary parts of each complex dft coefficient are multiplied
       by the corresponding 'mult' value.

          ** psd = magnitude squared = real*real + imag*imag

       default:  0


   sigma,sigma2 -

       Value of sigma and sigma2 (used as described in ftype parameter description).
       If using the sfile or sstring parameter then the sigma parameter is ignored.
       If using the sfile2 parameter then the sigma2 parameter is ignored.

           NOTE:  Starting in v1.5, these values are normalized based on the
                  non-coherent power gain of the window when ftype<2.  That
                  is to say that for ftype<2, where sigma/sigma2 correspond 
                  to power, they are now independent of the window size and
                  windowing function used, and that they directly correspond
                  to power. For convenience, the normalization factor is output
                  using OutputDebugString() when the filter loads. To convert
                  between old and new sigma values, simply multiply the pre v1.5
                  sigma value by the scaling factor. This scaling is also
                  applied to values loaded from sfile/sfile2 files.

       default:  16.0,16.0


   pmin,pmax -

       Used as described in the ftype parameter description.  If using the pminfile
       parameter then the pmin parameter is ignored.  If using the pmaxfile parameter
       then the pmax parameter is ignored.

           NOTE:  Starting in v1.5, these values are normalized based on the
                  non-coherent power gain of the window. They are now independent
                  of the window size and windowing function used, and directly
                  correspond to power. For convenience, the normalization factor
                  is output using OutputDebugString() when the filter loads. To
                  convert between old and new pmin/pmax values, simply multiply
                  the pre v1.5 values by the scaling factor. This scaling is
                  also applied to values loaded from pmin/pmax files.

       default:  0.0,500.0


   sbsize -

       Sets the length of the sides of the spatial window.  Must be 1 or greater.
       Must be odd if using smode = 0.

       default:  12


   smode -

       Sets the mode for spatial operation.  There are two possible settings:

          0 -  process every pixel independently... center the spatial window
               on the current pixel, filter, move to the next pixel, repeat.
               Spatial overlapping 'sosize' not used.

          1 -  process the spatial dimension in blocks of sbsize.  Spatial
               overlapping 'sosize' used.

       default:  1


   sosize -

       Sets the spatial overlap amount.  Must be in the range 0 to sbsize-1 (inclusive).
       If sosize is greater than sbsize>>1, then sbsize%(sbsize-sosize) must equal 0.
       In other words, overlap greater than 50% requires that sbsize-sosize be a divisor
       of sbsize.

       default:  9


   tbsize -

       Sets the length of the temporal dimension (i.e. number of frames).  Must be at
       least 1.  Must be odd if using tmode = 0.

       default:  5


   tmode -

       Sets the mode for temporal operation.  There are two possible settings:

          0 -  process every frame independently... center the temporal window
               on the current frame, filter, move to the next frame, repeat.
               Temporal overlapping 'tosize' not used.

          1 -  process the temporal dimension in blocks of tbsize.  Temporal
               overlapping 'tosize' used.  

       default:  0


   tosize -

       Sets the temporal overlap amount.  Must be in the range 0 to tbsize-1 (inclusive).
       If tosize is greater than tbsize>>1, then tbsize%(tbsize-tosize) must equal 0.
       In other words, overlap greater than 50% requires that tbsize-tosize be a divisor
       of tbsize.

       default:  0


   swin,twin -

       Sets the type of analysis/synthesis window to be used for spatial (swin) and
       temporal (twin) processing.  Possible settings:

		 0:  hanning
		 1:  hamming
		 2:  blackman
		 3:  4 term blackman-harris
		 4:  kaiser-bessel
		 5:  7 term blackman-harris
		 6:  flat top
		 7:  rectangular
		 8:  Bartlett
		 9:  Bartlett-Hann
		10:  Nuttall
		11:  Blackman-Nuttall

       default:  0,7


   sbeta,tbeta -

       Sets the beta value for kaiser-bessel window type.  sbeta goes with swin,
       tbeta goes with twin.  Not used unless the corresponding window value
       is set to 4.

       default:  2.5,2.5


   zmean -

       Controls whether the window mean is subtracted out (zero'd) prior to
       filtering in the frequency domain.

       default:  true


   sfile -

       Specifies an input file listing sigma values for each dft coefficient.
       There can be multiple lines with multiple coefficients per line.
       Separate coefficients on the same line using ',' or ' '.  Placing a 
       '#' at the beginning of a line will cause that line to be ignored. The
       coefficients are read from the file in left to right, top to bottom order.

       The dft transform results in tbsize*sbsize*((sbsize>>1)+1) coefficients.
       You must give these many sigma values in the sfile.  Assuming 2D (tbsize=1),
       and sbsize=8 (i.e. 8x8 window).  The resulting dft transform has 40
       coefficients organized as follows:

              0  1  2  3  4
              5  6  7  8  9
             10 11 12 13 14
             15 16 17 18 19
             20 21 22 23 24
             25 26 27 28 29
             30 31 32 33 34
             35 36 37 38 39

       The number given here specifies which sigma value from the sfile
       corresponds to that dft coefficient.

       The DC coefficient is in the upper left.  The top row corresponds to
       purely horizontal frequencies, and the frequencies increase from left to
       right. In this example '4' corresponds to the highest horizontal frequency.

       The left-most column corresponds to purely vertical frequencies, but the
       highest frequency is at the (sbsize>>1) row (assuming the numbering starts
       at 0)... in this case '20' corresponds to the highest vertical frequency.
       The frequencies then decrease from (sbsize>>1) to sbsize-1.  Basically,
       the first (sbsize>>1) rows correspond to the positive frequencies and
       the last (sbsize>>1)-1 rows correspond to the negative frequencies.

       In the 8x8 case, the single highest frequency is located at '24'.

       In the case that tbsize > 1, the first set of ((sbsize>>1)+1)*sbsize
       cofficients correspond to the lowest frequencies temporally (with the
       relations described for the 2D case holding within that set) and the
       frequencies increase temporally from set to set up to the tbsize>>1 set.
       The frequencies then decrease from there to the tbsize-1 set (again
       the positive vs negative frequencies as mentioned previously). If tbsize=3,
       you get 120 coefficients:

              0   1   2   3   4
              5   6   7   8   9
             10  11  12  13  14
             15  16  17  18  19
             20  21  22  23  24
             25  26  27  28  29
             30  31  32  33  34
             35  36  37  38  39

             40  41  42  43  44
             45  46  47  48  49
             50  51  52  53  54
             55  56  57  58  59
             60  61  62  63  64
             65  66  67  68  69
             70  71  72  73  74
             75  76  77  78  79

             80  81  82  83  84
             85  86  87  88  89
             90  91  92  93  94
             95  96  97  98  99
            100 101 102 103 104
            105 106 107 108 109
            110 111 112 113 114
            115 116 117 118 119

       The DC coefficient is still at '0'.  The highest purely temporal frequency is
       at '40'.  The highest overall frequency is at '64'.

       default:  ""


   sfile2,pminfile,pmaxfile -

       Can be used to give different values of sigma2, pmin, and pmax for each
       dft coefficient respectively.  Entry and format is exactly the same as
       described in the sfile parameter description.  If sfile2 is not given then
       the value of sigma2 is used for every dft coefficient.  If pminfile is
       not given then the value of pmin is used for every dft coefficient.  If
       pmaxfile is not given then the value of pmax is used for every dft coefficient.

       default:  "","",""


   f0beta -

       Power term in ftype=0. The ftype=0 formula is:

              max((psd-sigma)/psd,0)^f0beta

       For f0beta=1, this equation corresponds to the wiener filter with
       spectral subtraction as the estimate of the signal power. For f0beta=0.5,
       the equation corresponds to spectral subtraction. The 1.0 and 0.5 cases
       are separated from the general routine in the code to allow for fast
       operation. Other values will result in the general routine being used,
       which has to perform a pow() computation, and is therefore much slower.

       default:  1.0


   nfile -

       When ftype<2, a nfile can be used to specify block locations in the video
       from which dfttest will estimate the noise power spectrum (sigma) to
       be used for filtering.

       When the noise to be removed is not white (i.e. doesn't have a flat power
       spectrum), specifying only a single sigma value is not adequate. Prior
       to v1.5, using dfftest in such cases meant you would have to figure out the
       noise spectrum on your own, and then use an sfile to input the sigma values.
       Now dfttest can perform the task of estimating the noise spectrum.

       The nfile should list locations in the video that consist of noise on
       a flat background, one entry per line. The line syntax is:

                 frame_number,plane,ypos,xpos   e.g.  0,0,20,20

           plane:  (0=Y,1=U,2=V)
           ypos/xpos:  the upper left position of the block
                       (0,0 is the upper left of the frame)

           dfttest positions a window (of the type defined by sbsize/tbsize/swin/twin)
           at the specified location, and estimates the power using fft magnitude^2.
           When tbsize>1, frame_number specifies the first frame of the temporal block.
           Make sure that the window size is large enough to capture the full noise
           pattern.

           If you list multiple blocks (multiple lines in the nfile), then the
           estimates obtained at each block are averaged to form the final estimate.
           Having more block locations to use lowers the variance of the estimate.
           The more block locations you specify the closer the true noise spectrum
           will be estimated, resulting in better denoising. When listing multiple
           block locations, it is best/preferred if the locations do not overlap.

           Typically, subtracting out the noise power spectrum is not adequate becase
           it is only the average. In any one block the noise spectrum has the potential
           to exceed the average in a frequency bin. Therefore, one typically over
           subtracts based on some multiple of the noise spectrum (usually in the range
           of 3-8). The default used in dfttest is 5 if ftype=0 and 7 if ftype=1. If you
           want to use another value, then on some line in the nfile put the following:

                  a=over_subtraction_factor   e.g.  a=3.5

           To comment out a line in a nfile (have it be ignored), place a '#' at the
           beginning of the line.

           An example:

              avisource("noisy_source.avi")
              dfttest(f0beta=0.5,U=false,V=false,nfile="nfile.txt")

                Here, dfftest is being used with default settings to filter only
                the Y plane, expect for f0beta=0.5 resulting in spectral subtraction
                instead of wiener filtering.  nfile is listing locations of only
                noise, and has the following lines:

                   0,0,20,40
                   5,0,100,380
                   14,0,400,100
                   a=5.2

                The first line corresponds to frame 0, y-plane, at x,y location (40,20).
                The estimate from that block will be averaged with the other two
                estimates, and the over subtraction factor is set equal to 5.2.

       When using a nfile, the estimated noise spectrum is output to 
       "noise_spectrum-date_string.txt", located in the current directory. It lists the
       power of each dft coefficient (layout is the same as explained in the sfile
       description). The average noise power is also calculated. As of v1.7, this file
       is compatible (can be used) with the sfile parameter.

       default:  ""


   threads -

       Sets the number of threads used for processing.  If set to 0, then threads
       is set equal to the number of detected processors.

       default:  0


   opt -

       Sets which cpu optimizations are used.  Possible settings:

          0 - auto detect
          1 - c routines
          2 - sse/sse2 routines
          3 - sse/sse2/avx routines
          4 - sse/sse2/avx/avx2 routines

       default:  0


   nstring -

       Same functionality as 'nfile', but allows entering window locations directly in
       the script instead of creating a separate file. The list of frame/plane/ypos/xpos
       quadruples is stored as a string with each quadruple separated by a space.
       Example:

          If you use an nfile that looks like:

              a=4.0
              35,0,45,68
              28,0,23,87

          You can use the following nstring and get the same result:

             nstring="a:4.0 35,0,45,68 28,0,23,87"

       The one restriction is that the oversubtraction factor (a:x.x) must be the first
       entry in the string (as opposed to nfiles where the a=x.x can be placed anywhere).
       If it is not supplied, then the same default oversubtraction factor is used as
       is used for the nfile option.

       default:  ""


   sstring/ssx/ssy/sst -

       Used to specify functions of sigma based on frequency. If you want sigma to vary
       based on frequency, then use 'sstring' instead of the 'sigma' parameter. sstring
       allows you to enter values of sigma for different normalized [0.0,1.0] frequency
       locations. Values for locations between the ones you explicitly specify are computed
       via linear interpolation. The frequency range, which is dependent on sbsize/tbsize,
       is normalized to [0.0,1.0] with 0.0 being the lowest frequency and 1.0 being the
       highest frequency. You MUST specify sigma values for those end point locations
       (0.0 and 1.0)! You can specify as many other locations as you wish, and they don't
       have to be in any particular order. Each frequency/sigma pair is given as "f.f:s.s".
       The list of frequency/sigma pairs is saved as a string, with each pair separated by
       a space.

       For example, if you want a linear ramp of sigma from 1.0 for the lowest frequency
       to 10.0 for the highest frequency use:

            sstring = "0.0:1.0 1.0:10.0"

             "0.0:1.0"  =>  this means sigma=1.0 at frequency 0.0

             "1.0:10.0"  => this means sigma=10.0 at frequency 1.0

             Sigma values for frequencies between 0.0 and 1.0 will be computed via
             linear interpolation.

       Or if you want a band-stop filter that passes low and high frequencies (filters
       middle frequencies) use something like:

            sstring = "0.0:0.0 0.15:10.0 0.85:10.0 1.0:0.0"

       To help visualize the process, the resulting filter spectrum is output to
       "filter_spectrum-date_string.txt" using the same format as the "noise_spectrum.txt"
       file that is output by the nfile/nstring options. The format of this file is compatible
       with 'sfile' input.

       There are two methods for computing sigma values for a given frequency bin based on
       sstring. The first computes the normalized frequency location of each dimension 
       (horizontal,vertical,temporal), interpolates sigma for each of those dimensions, 
       and then multiples the individual sigmas to obtain the final sigma value. So that
       everything scales correctly, all sigma values entered in sstring are first raised to
       the 1/#_dimensions power before perform performing linear interpolation and multiplying.
       The second method (based on fft3dfilter's system) works by computing a single location
       from the seperate dimension locations (x,y,z) as:

           new = sqrt((x*x+y*y+z*z)/3.0)

       sigma is then interpolated to this location. By default the first system is used.
       To use the second system simply put a '$' sign at the beginning of sstring as shown
       below:

            sstring = "$ 0.0:1.0 1.0:10.0"


        ---------------- ssx/ssy/sst explanation -------------------------------

       sstring breaks the 1D (sbsize=1), 2D (for tbsize=1), or 3D (for sbsize>1 and tbsize>1) 
       frequency spectrum into chunks by normalizing each dimension to [0.0,1.0]... i.e. the
       frequency range [0.0,0.25] is a cube covering the first 1/4 of each dimension. This works
       fine if you want to treat all dimensions the same in terms of how sigma should vary.
       However, if you wanted to ramp sigma based only on temporal frequency or horizontal
       frequency, this is too limited. This is where ssx/ssy/sst come in!
       
       ssx/ssy/sst allow you to specify sigma as a function of horizontal (ssx), vertical (ssy),
       and temporal (sst) frequency only. The syntax is exactly the same as that of sstring. To
       get the final sigma value for a frequency location, the three separate values (one for
       each dimension) are computed and then multiplied together. As with sstring the sigma values
       are first raised to the 1/#_dimensions power before performing linear interpolation and
       multiplying. If you don't specify all three strings, then a flat function equal to the
       'sigma' parameter is used for the missing dimensions. For dimensions of size one (the
       spatial dimenions if sbsize=1 or the temporal dimension for tbsize=1) the corresponding
       string is ignored.

       For example:

            ssx="0.0:1.0 1.0:10.0",ssy="0.0:1.0 1.0:10.0",sst="0.0:1.0 1.0:10.0"

       will give the same result as

            sstring="0.0:1.0 1.0:10.0"

       Or if you want to ramp sigma based on temporal frequency:

            sigma=10.0,sst="0.0:1.0 1.0:10.0"

            This will use 10.0 for the horizontal/vertical dimensions, and ramp
            sigma from 1.0 to 10.0 in the temporal dimension.

       If 'sstring' is specified, it takes precedence over ssx/ssy/sst. Again, the
       "filter_spectrum-date_string.txt" output file is helpful in visualizing the result.

       default:  ""


   dither -

       Controls whether dithering is performed when converting from float to unsigned char
       for output. Internally dfttest works on floating point values. For output the
       result must be quantized back to unsigned char values. Prior to v1.8 this was always
       done by simply rounding. Possible settings:

           0 -     no dithering (same as v1.7 and prior)
           1 -     Floyd-Steinberg dithering
           2-100 - Floyd-Steinberg dithering with increasing amounts of uniform random
                   noise added prior to the dithering process

       Obviously dither=0 is the fastest, and dither=1 is slightly faster than dither>=2
       due to not having to generate a random number for every pixel. However, this part
       doesn't take much time compared to the actual filtering operation. dither=1 should
       combat any banding introduced by dfttest's quantization, but probably wont help
       banding in the source. dither>=2 can combat banding in the source.

       default:  0


   lsb -

       When set to true, dfttest outputs 16-bit pixel components by separating the most
       significant bytes (MSB) and the least significant bytes (LSB). The top part of the
       frame contains the MSB of all pixels and the bottom part their LSB. Therefore the
       output frame height is doubled. Use this if you want to perform the dithering
       later, with a separate tool.

       default:  false


   lsb_in -

       When set to true, the input is supposed to have 16-bit pixel components, of the
       same format as the output given with lsb = true. The sigma scale remains relative
       to the MSB, meaning that a given value will have the same visual results with
       16-bit and 8-bit clips.

       default:  false


   quiet -

       Prevents dfttest to write a filter spectrum file when sigma is specified with
       sstring/ssx/ssy/sst.

       default:  true


---------------------------------------------------------------------------------------------------

Changes:
2026.03.23 v1.9.8
  - Thread safety: use Avisynth+ V12 global lock (AcquireGlobalLock/ReleaseGlobalLock) around
    FFTW plan creation and destruction. Falls back to legacy std::mutex on older Avisynth+ versions.
  - Update avisynth headers to V12 interface

2021.10.28 v1.9.7
  - pass Avisynth+ frame properties

2020.03.24 v1.9.6
  - Enhanced: avx2 routines for mod-8 sbsize cases
  - parameter opt allow 4 for avx2
  - fix: a very broken filter_3_sse (ftype = 3)
  - fix: filter_6_sse: number limited with 1e-15 instead of 0 (used in an ftype = 0 sub-case)
  - filter_6_sse: replacing (rsqrt+rcp) with direct sqrt, more accurate and not slower

2020.03.23 v1.9.5
  - Avisynth+ high bit depth support 10-32 bits (base version, needs more optimization)
    new formats: planar RGB and Y (greyscale)
  - Fix: minor image noise at stacked16 input (lsb_in = true)
    proc0_16_SSE2 contained a bug: every 2nd pixel contains traces of the lsb part of pixels 4 positions to the right
    Regression since 1.9.4
  - source: no more external asm, YUY2 converters replaced with simd code

2020.03.23 v1.9.4.4 - pinterf 
  - fix: make fft3w plans thread safe
  - MSVC project to VS2019, add v142, v141_xp and ClangCl config.
  - refresh avisynth headers
  - source to C++17 strict conformance
  - AVX option is always available

2018.10.14 v1.9.4.3 - DJATOM
  - Fixed one nasty bug, causing crash on non-AVX CPUs.

2017.09.04 - DJATOM
  - Adaptive MT mode: MT_MULTI_INSTANCE for threads=1 and MT_SERIALIZED for > 1 internal
  - Compilation: silence #693 for Intel Compiler

2017.08.14 - DJATOM
  - x64 ready: ported almost all inline asm to intrinsics (dropped some SSE functions, we're in 2017 now).
  - AddMean and RemoveMean got their AVX codepaths.
  - PlanarFrame updated from JPSDR's NNEDI3 repo (x64 friendly, HBD ready).
  - proc0_16 got SSE2 codepath (I can see noticeable speed-up against old version).
  - opt parameter change: 2 - SSE/SSE2, 3 - SSE/SSE2/AVX codepath is used.

2013-08-04  v1.9.4
  + Compatible the new Avisynth 2.6 colorspaces, excepted Y8.

2012-04-20  v1.9.3
  - Does no longer issue a tbsize-related error with null-length clips.

2012-03-23  v1.9.2
  - The quiet parameter is not true by default.

2012-03-11  v1.9.1
  - Fixed a stupid regression (from v1.8 mod16a) on the dither parameter.

2011-11-28  v1.9
  + Added the quiet parameter to deactivate the filter spectrum output.

2011-05-12  v1.8 mod16b
  + Added the lsb_in parameter to input 16 bit data.

2010-06-26  v1.8 mod16a
  + Added the lsb parameter to output 16 bit data.

2010-06-22  v1.8

  + added dither parameter and functionality
  + attach date string to filter_spectrum.txt and noise_spectrum.txt output
  + changed sstring handling and added option to function like fft3dfilter

2010-06-21  v1.7

  + added nstring/sstring/ssx/ssy/sst parameters and functionality
  + allow space as delimiter in input files
  - fixed missing emms in sse routine for f0beta != (1.0 or 0.5) and ftype=0

2009-06-04  v1.6

  - fixed window normalization causing tmode=0 to always result in a rectangular
    temporal window, and smode=0 to always result in a rectangular spatial
    window.
  - changed default for twin to 7

2009-04-11  v1.5

  + added f0beta in ftype=0
  + added nfile parameter (noise power estimation)
  + normalization of sigma/sigma2/pmin/pmax based on non-coherent power gain

2009-04-06  v1.4

  - fix threading issue that could result in corrupted output

2009-01-27  v1.3

  + more assembly optimizations
  + tmode=1 caching (don't need to recalculate all involved temporal blocks on every frame)
  - replicate temporal dimension at beginning/end, don't mirror

2009-01-24  v1.2

  + added filter types 3/4 and corresponding parameters (sigma2,pmin,pmax,
    sfile2,pminfile,pmaxfile)
  + more asm optimizations
  - fixed problem with global function pointers
  - changed name of 'cfile' parameter to 'sfile'
  - the value given for sigma is no longer squared on initialization
  - sigma now defaults to 2.0
  - tbsize now defaults to 5

2007-11-22  v1.1

  + more sse optimizations
  - fixed a bug causing the bottom part of the frame to be incorrectly
    processed with some sbsize/sosize combinations

2007-11-21  v1.0

  - initial release


contact:  forum.doom9.org - tritical  or  kes25c@mizzou.edu
