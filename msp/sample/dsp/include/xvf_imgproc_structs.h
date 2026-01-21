#ifndef _IMGPROC_FRAME_STRUCTS_H_
#define _IMGPROC_FRAME_STRUCTS_H_

#include <stdint.h>
#include <stdbool.h>

/* IVP library data types */

typedef float       float32_t;
typedef double      double64_t;

typedef int32_t XI_ERR_TYPE;
typedef uint8_t xi_bool;

typedef int16_t XI_Q0_15;
typedef int16_t XI_Q5_10;
typedef int16_t XI_Q6_9;
typedef int16_t XI_Q7_8;
typedef int16_t XI_Q8_7;
typedef int16_t XI_Q12_3;
typedef int16_t XI_Q13_2;

typedef int32_t XI_Q0_31;
typedef int32_t XI_Q1_30;
typedef int32_t XI_Q11_20;
typedef int32_t XI_Q12_19;
typedef int32_t XI_Q13_18;
typedef int32_t XI_Q15_16;
typedef int32_t XI_Q16_15;
typedef int32_t XI_Q22_9;
typedef int32_t XI_Q28_3;

typedef XI_Q0_15 XI_Q15;
typedef XI_Q0_31 XI_Q31;
typedef uint16_t XI_Q0_16;
typedef uint16_t XI_Q11_5;
typedef uint16_t XI_Q6_10;


enum xcvKernelSize {
    KERNEL_TYPE_3x3 = 0,
    KERNEL_TYPE_5x5,
    KERNEL_TYPE_7x7,
    KERNEL_TYPE_MAX
};

enum xcvNormType {
    NORM_L1 = 1,
    NORM_L2
};

enum xcvKernelType {
    TYPE_U8 = 0,
    TYPE_S16,
    TYPE_U16
};

enum xcvInterpolationType {
    XCV_INTERPOLATION_TYPE_NEAREST_NEIGHBOR = 0,
    XCV_INTERPOLATION_TYPE_BILINEAR,
    XCV_INTERPOLATION_TYPE_AREA,
    XCV_INTERPOLATION_TYPE_BICUBIC,
    XCV_INTERPOLATION_TYPE_LANCOZ
};

enum xcvConectivityType {
    XCV_4_CONNECTIVITY = 0,
    XCV_8_CONNECTIVITY
};

enum xcvTemplateType {
    XCV_TMPL_HAMMING = 0,
    XCV_TMPL_L1,
    XCV_TMPL_L2,
    XCV_TMPL_CCORR,
    XCV_TMPL_L2_NORM,
    XCV_TMPL_CCORR_NORM
};

typedef struct frameInfo {
    void     *pFrameData;
    int32_t  frameWidth;
    int32_t  frameHeight;
    int32_t  framePitch;
    uint32_t pixelRes;
} tFrameInfo;

typedef struct kernelParams {
    uint32_t borderType;
    uint32_t kernelType;
} tKernelParams;

typedef struct kernelParams2 {
    uint32_t borderType;
    uint32_t interpolationType;
} tKernelParams2;


typedef struct {
    int16_t x;
    int16_t y;
} xi_point;

typedef struct {
    int32_t x;
    int32_t y;
} xi_point32;

typedef struct {
    XI_Q16_15 x;
    XI_Q16_15 y;
} xi_point_fpt;

typedef struct {
    float32_t x;
    float32_t y;
} xi_point_f;

typedef struct {
    uint16_t width;
    uint16_t height;
} xi_size;

typedef struct {
    float32_t a11;
    float32_t a12;
    float32_t a21;
    float32_t a22;
    float32_t xt;
    float32_t yt;
} xi_affine;

typedef struct {
    XI_Q13_18 a11;
    XI_Q13_18 a12;
    XI_Q13_18 a21;
    XI_Q13_18 a22;
    XI_Q13_18 xt;
    XI_Q13_18 yt;
} xi_affine_fpt;

typedef struct {
    float32_t a11;
    float32_t a12;
    float32_t a13;
    float32_t a21;
    float32_t a22;
    float32_t a23;
    float32_t a31;
    float32_t a32;
    float32_t a33;
} xi_perspective;

typedef struct {
    XI_Q13_18 a11;
    XI_Q13_18 a12;
    XI_Q13_18 a13;
    XI_Q13_18 a21;
    XI_Q13_18 a22;
    XI_Q13_18 a23;
    XI_Q13_18 a31;
    XI_Q13_18 a32;
    XI_Q13_18 a33;
} xi_perspective_fpt;

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} xi_rect;

typedef struct {
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    XI_Q5_10 angle;
} xi_rotated_rect;

typedef struct {
    float32_t x;
    float32_t y;
    float32_t width;
    float32_t height;
    float32_t angle;
} xi_rotated_rect_f;

typedef struct {
    int32_t M00;
    int64_t M10;
    int64_t M01;
    int64_t M11;
    int64_t M20;
    int64_t M02;
} xi_moments;

#if defined __GNUC__
    #define XI_INLINE_OR_DIE static inline __attribute__ ((always_inline))
    #define XI_NOINLINE __attribute__ ((noinline))
    #define XI_ALIGNED(alignment) __attribute__((aligned(alignment)))
#else
    #define XI_INLINE_OR_DIE static
    #define XI_NOINLINE
    #define XI_ALIGNED(alignment)
#endif

#ifndef XI_KEYPOINT_ALIGNMENT
    #define XI_KEYPOINT_ALIGNMENT 1 // can be changed to support 32B alignment for xi_keypoint
#endif

typedef struct XI_ALIGNED(XI_KEYPOINT_ALIGNMENT)
{
    int32_t x;
    int32_t y;
    float32_t   strength;
    float32_t   scale;
    float32_t   orientation;
    int32_t tracking_status;
    float32_t   error;
}
xi_keypoint;

typedef struct {
    XI_Q13_18 rho;
    XI_Q13_18 theta;
} xi_line_polar_fpt;

typedef struct {
    uint32_t  size;   // number of pyramid levels
    float32_t     scale;
    uint32_t levels; // array of pyramid levels
} xi_pyramid, *xi_pPyramid;

/**
 * Argument structure used for function xvfFilterSobel_U8S16().
 *
 * @param height      image height for src and dst
 * @param width       image width for src and dst
 * @param srcPitch    image pitch for src
 * @param dstPitch    image pitch for dst @p height
 * @param borderType  indicates type of padding to be used for pixels outside frame.
 * @param kernelType  kernelType = 3x3 or 5x5 (3x3 will be supported right now)
 * @param tileWidth   width of tile for IVP processing
 * @param tileHeight  height of tile for IVP processing
 * @param src         source image pointer
 * @param dstX        output array with computed x derivative
 * @param dstY        output array with computed y derivative
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       kernelType;
    xi_bool       normalize;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dstX;
    uint32_t dstY;
} xvfFilterSobel_U8S16_API_T;

/**
 * Argument structure used for function xvfFilterGaussian_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param kernelType     kernelType = 3x3 or 5x5 (3x3 will be supported right now)
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       kernelType;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfFilterGaussian_U8_API_T;

/**
 * Argument structure used for function xvfCanny_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param kernelType     kerlenType = U8 or S16
 * @param kernelSize     kernelSize = 3x3 or 5x5 (3x3 will be supported right now)
 * @param norm           norm = L1 or L2
 * @param low_thresh     low threshold for deciding canny edge
 * @param high_thresh    high threshold for deciding canny edge
 * @param trace_edges    boolean flag to trace or not trace edges
 * @param src            source image pointer
 * @param dst            edge detection destination image pointer
 * @param dst_trace      edge tracing destination img pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       kernelSize;
    int32_t       kernelType;
    int32_t       norm;
    uint32_t      low_thresh;
    uint32_t      high_thresh;
    int32_t       overlap;
    bool          trace_edges;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t dst_trace;
} xvfCanny_API_T;

/**
 * Argument structure used for function xvfMedianBlur_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param mask_width     width of mask
 * @param mask_height    height of mask *
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param mask           pointer to mask data
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       mask_width;
    int32_t       mask_height;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t mask;
} xvfMedianBlur_U8_API_T;

/**
 * Argument structure used for function xvfDilate_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param mask_width     width of mask
 * @param mask_height    height of mask
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param mask           pointer to mask data
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       mask_width;
    int32_t       mask_height;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t mask;
} xvfDilate_U8_API_T;

/**
 * Argument structure used for function xvfErode_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param mask_width     width of mask
 * @param mask_height    height of mask *
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param mask           pointer to mask data
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       mask_width;
    int32_t       mask_height;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t mask;
} xvfErode_U8_API_T;

/**
 * Argument structure used for function xvfBoxFilter_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param ksize          box filter dimension ksize x ksize
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfBoxFilter_U8_API_T;

/**
 * Argument structure used for function xvfGuidedFilter_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param gdWidth        guiding image width
 * @param gdHeight       guiding image height
 * @parag gdPitch        guiding image pitch
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param eps            eps value as input
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param guide          guiding image pointer
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      gdHeight;
    uint32_t      gdWidth;
    uint32_t      gdPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    double64_t    eps;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t guide;
    uint32_t src;
    uint32_t dst;
} xvfGuidedFilter_U8_API_T;

/**
 * Argument structure used for function xvfRGBToYUV420_U8().
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitchY      pitch for destination image Y plane
 * @param dstPitchUV     pitch for destination image U and V planes
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer to RGB
 * @param dstY           destination image Y plane pointer
 * @param dstU           destination image U plane pointer
 * @param dstV           destination image V plane pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitchY;
    uint32_t      dstPitchUV;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t srcRGB;
    uint32_t dstY;
    uint32_t dstU;
    uint32_t dstV;
} xvfRGBToYUV420_U8_API_T;

/**
 * Argument structure used for function xvfYUV420ToRGB_U8().
 * @param height      image height for src and dst
 * @param width       image width for src and dst
 * @param srcPitchY   pitch for source image Y plane
 * @param srcPitchUV  pitch for source image U and V plane
 * @param dstPitch    pitch for destination
 * @param tileWidth   width of tile for IVP processing
 * @param tileHeight  height of tile for IVP processing
 * @param srcY        source image Y plane pointer
 * @param srcU        source image U plane pointer
 * @param srcV        source image V plane pointer
 * @param dstRGB      destination image pointer to RGB
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitchY;
    uint32_t      srcPitchUV;
    uint32_t      dstPitch;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t srcY;
    uint32_t srcU;
    uint32_t srcV;
    uint32_t dstRGB;
} xvfYUV420ToRGB_U8_API_T;

/**
 * Argument structure used for function xvfRGBToGray_U8().
 * @param height          image height for src and dst
 * @param width           image width for src and dst
 * @param srcPitchR       image pitch for src R
 * @param srcPitchG       image pitch for src G
 * @param srcPitchB       image pitch for src B
 * @param dstPitchY       pitch for destination image Y plane
 * @param tileWidth       width of tile for IVP processing
 * @param tileHeight      height of tile for IVP processing
 * @param srcR            source image pointer to R component
 * @param srcG            source image pointer to G component
 * @param srcB            source image pointer to B component
 * @param dstY            destination image Y plane pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitchR;
    uint32_t      srcPitchG;
    uint32_t      srcPitchB;
    uint32_t      dstPitchY;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t srcR;
    uint32_t srcG;
    uint32_t srcB;
    uint32_t dstY;
} xvfRGBToGray_U8_API_T;

/**
 * Argument structure used for function xvfTranspose_U8().
 * @param srcHeight  image height for src
 * @param srcWidth   image width for src
 * @param srcPitch   image pitch for src
 * @param dstPitch   image pitch for dst
 * @param src        source image pointer
 * @param dst        destination image pointer
 */
typedef struct {
    uint32_t      srcHeight;
    uint32_t      srcWidth;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfTranspose_U8_API_T;

/**
 * Argument structure used for function test_xvfIntegralImage_U8U32().
 * @param srcWidth       image width for @p src image
 * @param srcHeight      image height for @p src image
 * @param srcStride      image pitch for @p src image
 * @param dstPitch       image pitch for @p dst image
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t     srcWidth;
    uint32_t     srcHeight;
    uint32_t     srcStride;
    uint32_t     dstStride;
    uint32_t     tileWidth;
    uint32_t     tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfIntegralImage_U8U32_API_t;

/**
 * Argument structure used for function xvfAffineTransform_U8().
 * @param srcHeight      image height for @p src image
 * @param srcWidth       image width for @p src image
 * @param srcPitch       image pitch for @p src image
 * @param dstHeight      image height for @p dst image
 * @param dstWidth       image width for @p dst image
 * @param dstPitch       image pitch for @p dst image
 * @param borderType     indicates type of padding to be used for pixels outside frame.
 * @param interpolationType  interpolation type used by kernel
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param affineMatrix   inverse affine matrix coefficients in float array
 */
typedef struct {
    uint32_t      srcHeight;
    uint32_t      srcWidth;
    uint32_t      srcPitch;
    uint32_t      dstHeight;
    uint32_t      dstWidth;
    uint32_t      dstPitch;
    int32_t       borderType;
    enum xcvInterpolationType interpolationType;
    uint32_t     tileWidth;
    uint32_t     tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t affineMatrix;
} xvfAffineTransform_U8_API_T;

/**
 * Argument structure used for function xvfPerspectiveTransform_U8().
 * @param srcHeight      image height for @p src image
 * @param srcWidth       image width for @p src image
 * @param srcPitch       image pitch for @p src image *
 * @param dstHeight      image height for @p dst image
 * @param dstWidth       image width for @p dst image
 * @param dstPitch       image pitch for @p dst image
 * @param borderType     indicates type of padding to be used for pixels outside frame.
 * @param interpolationType  interpolation type used by kernel
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param perspectiveMatrix   inverse perspective matrix coefficients in float array
 */
typedef struct {
    uint32_t      srcHeight;
    uint32_t      srcWidth;
    uint32_t      srcPitch;
    uint32_t      dstHeight;
    uint32_t      dstWidth;
    uint32_t      dstPitch;
    uint32_t      borderType;
    enum xcvInterpolationType interpolationType;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t perspectiveMatrix;
} xvfPerspectiveTransform_U8_API_T;

/**
 * Argument structure used for function xvfYUV420To444_U8().
 * @param srcHeight   image height for src and dst
 * @param srcWidth    image width for src and dst
 * @param srcPitchUV  pitch for source image UV plane
 * @param dstPitchUV  pitch for destination image U and V planes
 * @param srcUV       source interleaved image UV pointer
 * @param dstU        destination image U pointer
 * @param dstV        destination image V pointer
 */
typedef struct {
    uint32_t       srcHeight;
    uint32_t       srcWidth;
    uint32_t       srcPitchUV;
    uint32_t       dstPitchUV;
    int32_t        tileWidth;
    int32_t        tileHeight;
    uint8_t  *srcUV;
    uint8_t  *dstU;
    uint8_t  *dstV;
} xvfYUV420To444_U8_API_T;

/**
 * Argument structure used for function xvfConnectedComponent_U8U16().
 * @params srcWidth        image height for src
 * @params srcHeight       image width for src
 * @params srcStride       pitch for source image
 * @params dstStride       pitch for destination image
 * @params ConectivityType connectivity type
 * @params src             source image pointer
 * @params dst             destination image pointer
 */
typedef struct {
    uint32_t      srcWidth;
    uint32_t      srcHeight;
    uint32_t      srcStride;
    uint32_t      dstStride;
    enum xcvConectivityType ConectivityType;
    uint32_t src;
    uint32_t dst;
} xvfConnectedComponent_U8U16_API_T;

/**
 * Argument structure used for function xvfxcvScale_U8().
 * @params srcWidth      Source Image width
 * @params srcHeight     Source Image height
 * @params srcStride     Stride of source image. If set to 0, srcStride=srcWidth as default
 * @params dstWidth      Destination Image width
 * @params dstHeight     Destination Image height
 * @params dstStride     Stride of Destination image.  If set to 0, dstStride=dstWidth as default
 * @params interpolation The interpolation method used for scaling
 * @params src           Input image
 * @params dst           Output image
 */
typedef struct {
    uint32_t      srcWidth;
    uint32_t      srcHeight;
    uint32_t      srcStride;
    uint32_t      dstWidth;
    uint32_t      dstHeight;
    uint32_t      dstStride;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    enum xcvInterpolationType interpolation;
    uint32_t src;
    uint32_t dst;
} xvfScale_U8_API_T;

/**
 * Argument structure used for function xvfBilaterFilter_U8/S16().
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param kernelSize     kernelSize = 5 for 5x5 and 9 for 9x9
 * @param bytesPerPel    1 for U8 variants and 2 for S16 variants.
 * @param sigma          sigma value for LUT generation
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       kernelSize;
    uint32_t      bytesPerPel;
    float         sigma;
    float         spaceSigma;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfBilateralFilter_U8_API_T;


/**
 * Argument structure used for function xvfPyrDown_U8().
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @params dst           Output image
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfPyrDown_U8_API_T;

/**
 * Argument structure used for function xvfLKTrackPoint_U8().
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param numPryLevels   number of pyramid layers
 * @param winWidth       window width
 * @param winHeight      window height
 * @param prev            source image pointer
 * @param next            source next image pointer
 * @param  pointsToTrack  Points to track pointer
 * @param  trackedPoint   Tracked points pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    float         minEigThreshold;
    float         criteria_epsilon;
    int32_t       maxIters;
    int32_t       numPoints;
    int32_t       borderType;
    int32_t       numPryLevels;
    int32_t       winWidth;
    int32_t       winHeight;
    uint32_t prev;
    uint32_t next;
    uint32_t pointsToTrack;
    uint32_t trackedPoint;
} xvfLKTrackPoint_U8_API_T;

/**
 * Argument structure used for function xvfCornerHarris().
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param kernelSize     kernel size - 3, 5 etc
 * @param blockSize      covariance block size
 * @param bytesPerPelSrc    number of bytes per pixel for src 1 for U8, 2 for S16
 * @param bytesPerPelDst    number of bytes per pixel for dst 2 for S16, 4 for S32
 * @param sensivity      sensivity parameter
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    uint32_t      borderType;
    uint32_t      kernelSize;
    uint32_t      blockSize;
    uint32_t      bytesPerPelSrc;
    uint32_t      bytesPerPelDst;
    float         sensivity;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfCornerHarris_API_T;

#define MAX_PYRAMID 10
/**
 * Argument structure used for function xvfLKPyrDown_U8().
 * @param height         image height for level 0
 * @param width          image width for level 0
 * @param srcPitch       image pitch for level 0
 * @param  minEigThreshold  minimum threshold for termination
 * @param  criteria_epsilon epsilon value
 * @param  maxIters      maximum iterations for termination
 * @param  numPoints     number of points to be tracked
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param numPryLevels   number of pyramid layers
 * @param winWidth       window width
 * @param winHeight      window height
 * @param PyrTileWidth   Tile width
 * @param PyrTileWidth   Tile height
 * @param prev           array of pointers of previous image pyramid
 * @param next           array of pointers of next image pyramid
 * @param pointsToTrack  points to track
 * @param trackedPoint   tracked points
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    float         minEigThreshold;
    float         criteria_epsilon;
    int32_t       maxIters;
    int32_t       numPoints;
    int32_t       borderType;
    int32_t       numPryLevels;
    int32_t       winWidth;
    int32_t       winHeight;
    int32_t       PyrTileWidth;
    int32_t       PyrTileHeight;
    uint32_t prev[MAX_PYRAMID];
    uint32_t next[MAX_PYRAMID];
    uint32_t pointsToTrack;
    uint32_t trackedPoint;
} xvfLKPyrDown_U8_API_T;

/**
 * Argument structure used for function xvfPyrDown_S16().
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfPyrDown_S16_API_T;

/**
 * Argument structure used for function xvfLKTrackPoint_S16().
 * @param height         image height for level 0
 * @param width          image width for level 0
 * @param srcPitch       image pitch for level 0
 * @param  minEigThreshold  minimum threshold for termination
 * @param  criteria_epsilon epsilon value
 * @param  maxIters      maximum iterations for termination
 * @param  numPoints     number of points to be tracked
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param numPryLevels   number of pyramid layers
 * @param winWidth       window width
 * @param winHeight      window height
 * @param PyrTileWidth   Tile width
 * @param PyrTileWidth   Tile height
 * @param prev           array of pointers of previous image pyramid
 * @param next           array of pointers of next image pyramid
 * @param pointsToTrack  points to track
 * @param trackedPoint   tracked points
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    float         minEigThreshold;
    float         criteria_epsilon;
    int32_t       maxIters;
    int32_t       numPoints;
    int32_t       borderType;
    int32_t       numPryLevels;
    int32_t       winWidth;
    int32_t       winHeight;
    uint32_t prev;
    uint32_t next;
    uint32_t pointsToTrack;
    uint32_t trackedPoint;
} xvfLKTrackPoint_S16_API_T;

/**
 * Argument structure used for function xvfLKPyrDown_S16().
 * @param height         image height for level 0
 * @param width          image width for level 0
 * @param srcPitch       image pitch for level 0
 * @param  minEigThreshold  minimum threshold for termination
 * @param  criteria_epsilon epsilon value
 * @param  maxIters      maximum iterations for termination
 * @param  numPoints     number of points to be tracked
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param numPryLevels   number of pyramid layers
 * @param winWidth       window width
 * @param winHeight      window height
 * @param PyrTileWidth   Tile width
 * @param PyrTileWidth   Tile height
 * @param prev           array of pointers of previous image pyramid
 * @param next           array of pointers of next image pyramid
 * @param pointsToTrack  points to track
 * @param trackedPoint   tracked points
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    float         minEigThreshold;
    float         criteria_epsilon;
    int32_t       maxIters;
    int32_t       numPoints;
    int32_t       borderType;
    int32_t       numPryLevels;
    int32_t       winWidth;
    int32_t       winHeight;
    int32_t       PyrTileWidth;
    int32_t       PyrTileHeight;
    uint32_t prev[MAX_PYRAMID];
    uint32_t next[MAX_PYRAMID];
    uint32_t pointsToTrack;
    uint32_t trackedPoint;
} xvfLKPyrDown_S16_API_T;

/**
 * Argument structure used for function xvfCensus_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfCensus_U8_API_T;

/**
 * Argument structure used for function xvfFlipHorizontal().
 * @param srcHeight      image height for @p src image
 * @param srcWidth       image width for @p src image
 * @param srcPitch       image pitch for @p src image
 * @param dstHeight      image height for @p dst image
 * @param dstWidth       image width for @p dst image
 * @param dstPitch       image pitch for @p dst image
 * @param bytesPerPel    bytes per pixel, For variant U8 its 1 and for variant S16 its value is 2
 * @param borderType     indicates type of padding to be used for pixels outside frame.
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      srcHeight;
    uint32_t      srcWidth;
    uint32_t      srcPitch;
    uint32_t      dstHeight;
    uint32_t      dstWidth;
    uint32_t      dstPitch;
    uint32_t      bytesPerPel;
    int32_t       borderType;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfFlipHorizontal_API_T;

/**
 * Argument structure used for function xvfFlipVertical().
 * @param srcHeight      image height for @p src image
 * @param srcWidth       image width for @p src image
 * @param srcPitch       image pitch for @p src image
 * @param dstHeight      image height for @p dst image
 * @param dstWidth       image width for @p dst image
 * @param dstPitch       image pitch for @p dst image
 * @param bytesPerPel    bytes per pixel, For variant U8 its 1 and for variant S16 its value is 2
 * @param borderType     indicates type of padding to be used for pixels outside frame.
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      srcHeight;
    uint32_t      srcWidth;
    uint32_t      srcPitch;
    uint32_t      dstHeight;
    uint32_t      dstWidth;
    uint32_t      dstPitch;
    uint32_t      bytesPerPel;
    int32_t       borderType;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfFlipVertical_API_T;

/**
 * Argument structure used for function xvfFilterFAST_U8 / S16.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param threshold      threshold value
 * @param tileWidth      tile width
 * @param tileHeight     tile height
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       threshold;
    int32_t       bytesPerPel;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfFilterFAST_API_T;

/**
 * Argument structure used for function xvfNonMaximaSuppression.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param kernelType     TYPE_U8/TYPE_U16/TYPE_S16
 * @param dim            dimension 2D/3D
 * @param type           0 - Symmetric, 1 - Asymmentric
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source current image pointer
 * @param src_p          source prev image pointer; unused when dim = 2
 * @param src_n          source next image pointer; unused when dim = 2
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    uint32_t      kernelType;
    uint32_t      dim;
    uint32_t      type;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t src_p;
    uint32_t src_n;
    uint32_t dst;
} xvfNonMaximaSuppression_API_T;

/**
 * Argument structure used for function xvfNcc().
 * @param srcHeight       image height for src
 * @param srcWidth        image width for src
 * @param srcPitch        image pitch for src
 * @param tmplWidth       template width
 * @param tmplHeight      template height
 * @param tmplPitch       template pitch
 * @param dstWidth        width of destination image
 * @param dstHeight       height of destination image
 * @param dstPitch        pitch for destination image Y plane
 * @param tileWidth       width of tile for IVP processing
 * @param tileHeight      height of tile for IVP processing
 * @param src             source image pointer
 * @param tmpl            pointer to template image
 * @param dst             destination image plane pointer
 */
typedef struct {
    uint32_t      srcWidth;
    uint32_t      srcHeight;
    uint32_t      srcPitch;
    uint32_t      tmplWidth;
    uint32_t      tmplHeight;
    uint32_t      tmplPitch;
    uint32_t      dstWidth;
    uint32_t      dstHeight;
    uint32_t      dstPitch;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t tmpl;
    uint32_t dst;
} xvfNcc_API_T;

/**
 * Argument structure used for function xvfTemplateMatching().
 * @param srcHeight       image height for src
 * @param srcWidth        image width for src
 * @param srcPitch        image pitch for src
 * @param tmplWidth       template width
 * @param tmplHeight      template height
 * @param tmplPitch       template pitch
 * @param dstWidth        width of destination image
 * @param dstHeight       height of destination image
 * @param dstPitch        pitch for destination image Y plane
 * @param tileWidth       width of tile for IVP processing
 * @param tileHeight      height of tile for IVP processing
 * @param tmplMatchType   variant of template matching algorithm
 * @param src             source image pointer
 * @param tmpl            pointer to template image
 * @param dst             destination image plane pointer
 */
typedef struct {
    uint32_t      srcWidth;
    uint32_t      srcHeight;
    uint32_t      srcPitch;
    uint32_t      tmplWidth;
    uint32_t      tmplHeight;
    uint32_t      tmplPitch;
    uint32_t      dstWidth;
    uint32_t      dstHeight;
    uint32_t      dstPitch;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t      tmplMatchType;
    uint32_t src;
    uint32_t tmpl;
    uint32_t dst;
} xvfTemplateMatch_API_T;

/**
 * Argument structure used for function xvfExtractPoints_U8.
 * @param height         image height for src
 * @param width          image width for src
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param threshold      threshold value
 * @param bytesPerPelSrc src bytes per pixel
 * @param bytesPerPelDst dst bytes per pixel
 * @param tileWidth       width of tile for IVP processing
 * @param tileHeight      height of tile for IVP processing
 * @param src            source image pointer
 * @param dst_pts        destination image pointer for points
 * @param dst_wts        destination image pointer for wts
 * @param count          extracted points count
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       threshold;
    uint32_t      bytesPerPelSrc;
    uint32_t      bytesPerPelDst;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst_pts;
    uint32_t dst_wts;
    uint32_t count;
} xvfExtractPoints_U8_API_T;


typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       threshold;
    uint32_t      bytesPerPelSrc;
    uint32_t      bytesPerPelDst;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst_pts;
    uint32_t dst_wts;
    uint32_t count;
} xvfExtractPoints_S16_API_T;

typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       threshold;
    uint32_t      bytesPerPelSrc;
    uint32_t      bytesPerPelDst;
    int32_t       tileWidth;
    int32_t       tileHeight;
    int32_t *src;
    uint32_t dst_pts;
    int32_t       *dst_wts;
    uint32_t count;
} xvfExtractPoints_S32_API_T;
/**
 * Argument structure used for function xvfCornerHarrisNonMaxima().
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param min_dist       minimum distance
 * @param count          output count after NonMaxima
 * @param src_pts        source points pointer
 * @param src_wts        source weights pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       min_dist;
    int32_t       count;
    uint32_t src_pts;
    uint32_t src_wts;
} xvfCornerHarrisNonMaxima_API_T;

/**
 * Argument structure used for function xvfSort_S16.
 * @param order_flag     order ascending/descending flag
 * @param width          width of src array
 * @param srcPitch       Pitch of src array
 * @param src_keys        keys src pointer
 * @param src_values      values src pointer
 */
typedef struct {
    xi_bool      order_flag;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t src_keys;
    uint32_t src_values;

} xvfSort_S16_API_T;
/**
 * Argument structure used for function xvfExtractKeyPoints_U8.
 * @param height         image height for src
 * @param width          image width for src
 * @param srcPitch       image pitch for src
 * @param dstPitch       pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst_pts        destination keypoints pointer
 * @param count          count of extracted keypoints
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst_pts;
    uint32_t count;
} xvfExtractKeyPoints_U8_API_T;

/**
 * Argument structure used for function xvfEqualizeHist.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination pointer to store equalizeHist output
 * @param dst_acc        destination pointer to store accumulate output (histogram of the frame)
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t dst_acc;
} xvfEqualizeHist_API_T;

/**
 * Argument structure used for function xvfPyrU8_U8().
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfPyrUp_U8_API_T;

/**
 * Argument structure used for function xvfPyrUP_S16().
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfPyrUp_S16_API_T;

/**
 * Argument structure used for function xvfSepfilter2d_R_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param ksize          sepfilter2d dimension ksize x ksize
 * @param bias           bias value
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param lshift         left shift
 * @param rshift         right shift
 * @param divisor        divisor
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param kernelX        pointer to kernel in X direction
 * @param kernelY        pointer to kernel in Y direction
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    int32_t      bias;
    int32_t       tileWidth;
    int32_t       tileHeight;
    int32_t       lshift;
    int32_t       rshift;
    int32_t       divisor;
    uint32_t src;
    uint32_t dst;
    uint32_t kernelX;
    uint32_t kernelY;
} xvfSepFilter2d_R_U8_API_T;

/**
 * Argument structure used for function xvfSepfilter2d_R_S16.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param ksize          sepfilter2d dimension ksize x ksize
 * @param bias           bias value
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param rshift         right shift
 * @param divisor        divisor
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param kernelX        pointer to kernel in X direction
 * @param kernelY        pointer to kernel in Y direction
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    int32_t       bias;
    int32_t       tileWidth;
    int32_t       tileHeight;
    int32_t       rshift;
    int32_t       divisor;
    uint32_t src;
    uint32_t dst;
    uint32_t kernelX;
    uint32_t kernelY;
} xvfSepFilter2d_R_S16_API_T;

/**
 * Argument structure used for function xvfSepfilter2d_D_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param ksize          sepfilter2d dimension ksize x ksize
 * @param bias           bias value
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param lshift         left shift
 * @param divisor        divisor
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param kernelX        pointer to kernel in X direction
 * @param kernelY        pointer to kernel in Y direction
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    int32_t bias;
    int32_t       tileWidth;
    int32_t       tileHeight;
    int32_t       lshift;
    int32_t       divisor;
    uint32_t src;
    uint32_t dst;
    uint32_t kernelX;
    uint32_t kernelY;
} xvfSepFilter2d_D_U8_API_T;

/**
 * Argument structure used for function xvfSepfilter2d_D_S16.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param ksize          sepfilter2d dimension ksize x ksize
 * @param bias           bias value
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param divisor        divisor
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param kernelX        pointer to kernel in X direction
 * @param kernelY        pointer to kernel in Y direction
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    int32_t       bias;
    int32_t       tileWidth;
    int32_t       tileHeight;
    int32_t       divisor;
    uint32_t src;
    uint32_t dst;
    uint32_t kernelX;
    uint32_t kernelY;
} xvfSepFilter2d_D_S16_API_T;

/**
 * Argument structure used for function xvffilter2d_R_U8.

 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param ksize          sepfilter2d dimension ksize x ksize
 * @param lshift         left shift
 * @param rshift         right shift
 * @param bias           bias value
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param kernel        pointer to kernel
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    int32_t       lshift;
    int32_t       rshift;
    int32_t       bias;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t kernel;
} xvfFilter2d_R_U8_API_T;

/**
 * Argument structure used for function xvffilter2d_R_S16.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param ksize          sepfilter2d dimension ksize x ksize
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param rshift         right shift
 * @param bias           bias value
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param kernel        pointer to kernel
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    int32_t       tileWidth;
    int32_t       tileHeight;
    int32_t         rshift;
    int32_t         bias;
    uint32_t src;
    uint32_t dst;
    uint32_t kernel;
} xvfFilter2d_R_S16_API_T;

/**
 * Argument structure used for function xvffilter2d_D_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param ksize          sepfilter2d dimension ksize x ksize
 * @param lshift         left shift
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param divisor        divisor
 * @param bias           bias value
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param kernel        pointer to kernel
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    int32_t       lshift;
    int32_t       tileWidth;
    int32_t       tileHeight;
    int32_t       divisor;
    int32_t       bias;
    uint32_t src;
    uint32_t dst;
    uint32_t kernel;
} xvfFilter2d_D_U8_API_T;

/**
 * Argument structure used for function xvffilter2d_D_S16.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING *
 * @param ksize          sepfilter2d dimension ksize x ksize
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param divisor        divisor
 * @param bias           bias value
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param kernel        pointer to kernel
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    int32_t       tileWidth;
    int32_t       tileHeight;
    int32_t       divisor;
    int32_t       bias;
    uint32_t src;
    uint32_t dst;
    uint32_t kernel;
} xvfFilter2d_D_S16_API_T;

/**
 * Argument structure used for function xvfLaplacianPyrDown_U8.

 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param dstPitch_next  image pitch for dst next
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param dst_next       destination next image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    uint32_t      dstPitch_next;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t dst_next;
} xvfLaplacianPyrDown_U8_API_T;

/**
 * Argument structure used for function xvfLaplacianReconstruct.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param laplacian      laplacian  image pointer
 * @param dst            destination image pointer
 *
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t laplacian;
    uint32_t dst;
} xvfLaplacianReconstruct_S16_API_T;

typedef struct {
    uint32_t      LapHeight;
    uint32_t      LapWidth;
    uint32_t      LapPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t laplacian;
    uint32_t dst;
} xvfLaplacianReconstruct2_S16_API_T;

/**
 * Argument structure used for function xvfLBP_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param kernelType     KernelType
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       kernelType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfLBP_U8_API_T;

/**
 * Argument structure used for function xvfMCT.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t  dst;
} xvfMCT_API_T;

typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t  dst;
} xvfMCT_S16U16_API_T;

/**
 * Argument structure used for function xvfBoxFilter_S16.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param ksize          box filter dimension ksize x ksize
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfBoxFilter_S16_API_T;

/**
 * Argument structure used for function xvfConvolve_RS_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param kwidth         width of kernel
 * @param kheight        height of kernel
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param shift          value of shift
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param kernel         Convolution kernel pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       kwidth;
    int32_t       kheight;
    int32_t       tileWidth;
    int32_t       tileHeight;
    int32_t       shift;
    uint32_t src;
    uint32_t dst;
    uint32_t kernel;
} xvfConvolve_RS_U8_API_T;

/**
 * Argument structure used for function     xvfUpsample2XNearest_I8_API_T.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t dst;
}   xvfUpsample2XNearest_I8_API_T;

/**
 * Argument structure used for function xvfLaplacian3x3_U8S16_API_T.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param normalize      normalize
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    xi_bool       normalize;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfLaplacian3x3_U8S16_API_T;

/**
 * Argument structure used for function xvfRemapNearest_I8_API_T
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param mapPitch       pitch of map
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param map            map pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dst_width;
    uint32_t      dst_height;
    uint32_t      dstPitch;
    uint32_t      mapPitch;
    int32_t       borderType;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t map;
} xvfRemapNearest_I8_API_T;

/**
 * Argument structure used for function xvfCornerShiTomasi().
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param kernelSize     kernel size - 3, 5 etc
 * @param blockSize      covariance block size
 * @param bytesPerPelSrc    number of bytes per pixel for src 1 for U8, 2 for S16
 * @param bytesPerPelDst    number of bytes per pixel for dst 2 for S16, 4 for S32
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    uint32_t      borderType;
    uint32_t      kernelSize;
    uint32_t      blockSize;
    uint32_t      bytesPerPelSrc;
    uint32_t      bytesPerPelDst;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
} xvfCornerShiTomasi_API_T;

/**
 * Argument structure used for function xvfOpticalFlowBM_U8().
 * @param blockSize       size of matched blocks
 * @param shiftSize       gap between two blocks of the image
 * @param maxRange        maximum possible shift of each block in x and y directions
 * @param height          image height for src
 * @param width           image width for src
 * @param srcPitch        image pitch for src
 * @param dstPitch        pitch for dst
 * @param dstHeight       height for dst
 * @param borderType      borderType = ZERO_PADDING / EDGE_PADDING
 * @param prev            source prev image pointer
 * @param next            source next image pointer
 * @param velx            dst velocity in x direction pointer
 * @param vely            dst velocity in y direction pointer
 */
typedef struct {
    xi_size     blockSize;
    xi_size     shiftSize;
    xi_size     maxRange;
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    uint32_t      dstHeight;
    int32_t       borderType;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t prev;
    uint32_t next;
    uint32_t velx;
    uint32_t vely;
} xvfOpticalFlowBM_U8_API_T;

/**
 * Argument structure used for function xvfHoughLines().
 * @param lut_height      lut height
 * @param lut_width       lut width
 * @param lut_pitch       lut pitch
 * @param img_size       image size
 * @param map_size       map size
 * @param lut_tileWidth       width of tile for IVP processing
 * @param lut_tileWidth      height of tile for IVP processing
 * @param dstLut          destination lut pointer
 */
typedef struct {
    uint32_t      lut_height;
    uint32_t      lut_width;
    uint32_t      lut_pitch;
    xi_size       img_size;
    xi_size       map_size;
    uint32_t      lut_tileWidth;
    uint32_t      lut_tileHeight;
    uint32_t dstLUT;
} xvfHoughLines_API_T;

/**
 * Argument structure used for function xvfGatherLocationsEq.
 * @param maxLocations   max locations
 * @param value          value
 * @param height         image height for src
 * @param width          image width for src
 * @param srcPitch       image pitch for src
 * @param dstWidth       width for dst
 * @param dstHeight      height for dst
 * @param dstPitch       pitch of dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param count          count
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param locations      locations pointer
 *
 */
typedef struct {
    uint32_t      maxLocations;
    int32_t       value;
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstWidth;
    uint32_t      dstHeight;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       count;
    int32_t       tileWidth;
    int32_t       tileHeight;
    uint32_t src;
    uint32_t locations;
} xvfGatherLocationsEq_API_T;

/**
 * Argument structure used for function xvfHoughLinesTransform.
 * @param numLocations         number of elements in array locations
 * @param mapSize              Width and height of responseMap
 * @param tilewidth            width of tile
 * @param imgsize              size of img
 * @param src                  src pointer
 * @param locations            pointer to array of points
 * @param responseMap          pointer to response map
 * @param houghLutPtr          pointer to HoughLUT generated by function xiPrepareHoughLines()
 */
typedef struct {
    uint32_t   numLocations;
    xi_size    mapSize;
    uint32_t   tileWidth;
    xi_size    imgSize;
    void *src;
    xi_point   *locations;
    uint16_t   *responseMap;
    uint32_t   *houghLutPtr;
} xvfHoughLinesTransform_API_T;

/**
 * Argument structure used for function xvfHoughLinesExtract.
 * @param threshold            min number of points required to select a line
 * @param cnt                  total number of lines extracted
 * @param maxLines             maximum number of lines
 * @param mapSize              dimensions of response map
 * @param imgSize              dimensions of input image
 * @param tileWidth            width of tile for IVP processing
 * @param tileHeight           height of tile for IVP processing
 * @param responseMap          pointer to response map
 * @param lines                pointer to buffer storing line params (rho, theta)
 * @param lut                  pointer to lut
 *
 */
typedef struct {
    uint32_t   threshold;
    int32_t    cnt;
    int32_t    maxLines;
    xi_size    mapSize;
    xi_size    imgSize;
    uint32_t   tileWidth;
    uint32_t   tileHeight;
    uint16_t   *responseMap;
    uint32_t lines;
    uint32_t lut;
} xvfHoughLinesExtract_API_T;

/**
 * Argument structure used for function xvfCicleFilter_U8.
 * @param height         image height for src and dst
 * @param width          image width for src and dst
 * @param srcPitch       image pitch for src
 * @param dstPitch       image pitch for dst
 * @param borderType     borderType = ZERO_PADDING / EDGE_PADDING
 * @param ksize          box filter dimension ksize x ksize
 * @param tileWidth      width of tile for IVP processing
 * @param tileHeight     height of tile for IVP processing
 * @param src            source image pointer
 * @param dst            destination image pointer
 * @param radius         radius pointer
 */
typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t src;
    uint32_t dst;
    uint32_t radius;
} xvfCircleFilter_U8_API_T;

typedef struct {
    uint32_t      height;
    uint32_t      width;
    uint32_t      srcPitch;
    uint32_t      dstPitch;
    int32_t       borderType;
    int32_t       ksize;
    uint32_t      tileWidth;
    uint32_t      tileHeight;
    uint32_t  src;
    uint32_t dst;
    uint32_t radius;
    uint32_t  weight;
} xvfCircleFilter_U16_U8_API_T;
#endif
