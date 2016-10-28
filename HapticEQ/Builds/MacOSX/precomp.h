//
//  precomp.h
//  HapticEQ
//
//  Created by aaron karp on 8/29/16.
//
//

#ifndef precomp_h
#define precomp_h


#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>

#ifdef HAVE_TEGRA_OPTIMIZATION
#include "opencv2/imgproc/imgproc_tegra.hpp"
#else
#define GET_OPTIMIZED(func) (func)
#endif

/* helper tables */
extern const uchar icvSaturate8u_cv[];
#define CV_FAST_CAST_8U(t)  (assert(-256 <= (t) && (t) <= 512), icvSaturate8u_cv[(t)+256])
#define CV_CALC_MIN_8U(a,b) (a) -= CV_FAST_CAST_8U((a) - (b))
#define CV_CALC_MAX_8U(a,b) (a) += CV_FAST_CAST_8U((b) - (a))

// -256.f ... 511.f
extern const float icv8x32fTab_cv[];
#define CV_8TO32F(x)  icv8x32fTab_cv[(x)+256]

// (-128.f)^2 ... (255.f)^2
extern const float icv8x32fSqrTab[];
#define CV_8TO32F_SQR(x)  icv8x32fSqrTab[(x)+128]

namespace cv
{
    
    static inline Point normalizeAnchor( Point anchor, Size ksize )
    {
        if( anchor.x == -1 )
            anchor.x = ksize.width/2;
        if( anchor.y == -1 )
            anchor.y = ksize.height/2;
        CV_Assert( anchor.inside(Rect(0, 0, ksize.width, ksize.height)) );
        return anchor;
    }
    
    void preprocess2DKernel( const Mat& kernel, vector<Point>& coords, vector<uchar>& coeffs );
    void crossCorr( const Mat& src, const Mat& templ, Mat& dst,
                   Size corrsize, int ctype,
                   Point anchor=Point(0,0), double delta=0,
                   int borderType=BORDER_REFLECT_101 );
    
}

typedef struct CvPyramid
{
    uchar **ptr;
    CvSize *sz;
    double *rate;
    int *step;
    uchar *state;
    int level;
}
CvPyramid;

#define  CV_COPY( dst, src, len, idx ) \
for( (idx) = 0; (idx) < (len); (idx)++) (dst)[idx] = (src)[idx]

#define  CV_SET( dst, val, len, idx )  \
for( (idx) = 0; (idx) < (len); (idx)++) (dst)[idx] = (val)

/* performs convolution of 2d floating-point array with 3x1, 1x3 or separable 3x3 mask */
void icvSepConvSmall3_32f( float* src, int src_step, float* dst, int dst_step,
                          CvSize src_size, const float* kx, const float* ky, float* buffer );

#undef   CV_CALC_MIN
#define  CV_CALC_MIN(a, b) if((a) > (b)) (a) = (b)

#undef   CV_CALC_MAX
#define  CV_CALC_MAX(a, b) if((a) < (b)) (a) = (b)

CvStatus CV_STDCALL
icvCopyReplicateBorder_8u( const uchar* src, int srcstep, CvSize srcroi,
                          uchar* dst, int dststep, CvSize dstroi,
                          int left, int right, int cn, const uchar* value = 0 );

CvStatus CV_STDCALL icvGetRectSubPix_8u_C1R
( const uchar* src, int src_step, CvSize src_size,
 uchar* dst, int dst_step, CvSize win_size, CvPoint2D32f center );
CvStatus CV_STDCALL icvGetRectSubPix_8u32f_C1R
( const uchar* src, int src_step, CvSize src_size,
 float* dst, int dst_step, CvSize win_size, CvPoint2D32f center );
CvStatus CV_STDCALL icvGetRectSubPix_32f_C1R
( const float* src, int src_step, CvSize src_size,
 float* dst, int dst_step, CvSize win_size, CvPoint2D32f center );

CvStatus CV_STDCALL icvGetQuadrangleSubPix_8u_C1R
( const uchar* src, int src_step, CvSize src_size,
 uchar* dst, int dst_step, CvSize win_size, const float *matrix );
CvStatus CV_STDCALL icvGetQuadrangleSubPix_8u32f_C1R
( const uchar* src, int src_step, CvSize src_size,
 float* dst, int dst_step, CvSize win_size, const float *matrix );
CvStatus CV_STDCALL icvGetQuadrangleSubPix_32f_C1R
( const float* src, int src_step, CvSize src_size,
 float* dst, int dst_step, CvSize win_size, const float *matrix );

#include "_geom.h"


#endif /* precomp_h */
