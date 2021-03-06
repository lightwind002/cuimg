#ifndef CUIMG_TEXTURE_H_
# define CUIMG_TEXTURE_H_

# include <cuimg/gpu/cuda.h>
# include <cuimg/gpu/device_image2d.h>
# include <cuimg/error.h>

#ifndef NVCC
template<typename U, unsigned D, enum cudaTextureReadMode READMODE>
struct texture
{
  operator const textureReference*() { return 0; }
};

#endif

namespace cuimg
{

#ifndef NO_CUDA
  template <typename V>
  cudaTextureObject_t create_texture(const device_image2d<V>& in)
  {
    cudaTextureObject_t         texInput;
    cudaResourceDesc            texRes;
    memset(&texRes,0,sizeof(cudaResourceDesc));

    texRes.resType            = cudaResourceTypePitch2D;
    texRes.res.pitch2D.devPtr = in.data();
    texRes.res.pitch2D.desc   = cudaCreateChannelDesc<typename V::cuda_bt>();
    texRes.res.pitch2D.pitchInBytes = in.pitch();
    texRes.res.pitch2D.width = in.ncols();
    texRes.res.pitch2D.height = in.nrows();

    cudaTextureDesc             texDescr;
    memset(&texDescr,0,sizeof(cudaTextureDesc));

    texDescr.normalizedCoords = 0;
    texDescr.filterMode       = cudaFilterModePoint;
    texDescr.addressMode[0] = cudaAddressModeClamp;
    texDescr.addressMode[1] = cudaAddressModeClamp;
    texDescr.addressMode[2] = cudaAddressModeClamp;

    texDescr.readMode = cudaReadModeElementType;
    cudaCreateTextureObject(&texInput, &texRes, &texDescr, NULL);
    return texInput;
  }
#endif

  inline void cudaUnbindTexture(int t)
  {
    return;
  }

  template<typename U, typename T,
           enum cudaTextureReadMode READMODE>
  void bindTexture2d(const host_image2d<U>& img, ::texture<T, 2, READMODE>& texref)
  {
    assert(0);
  }


  template<typename U>
  void bindTexture2d(const host_image2d<U>& img, int texref)
  {
    assert(0);
  }

#ifdef NVCC
  template<typename U, typename T,
           enum cudaTextureReadMode READMODE>
  void bindTexture2d(const device_image2d<U>& img, ::texture<T, 2, READMODE>& texref)
  {
    cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<T>();
    cudaBindTexture2D(0, texref, (void*)img.data(), channelDesc,
                      img.ncols(), img.nrows(), img.pitch());
    check_cuda_error();
  }

  template<typename T, enum cudaTextureReadMode READMODE>
  __host__ __device__ inline T tex2D(::texture<T, 2, READMODE>& texref, const point2d<int>& p)
  {
    return tex2D(texref, p.col(), p.row());
  }


  template<typename T, typename U, enum cudaTextureReadMode READMODE>
  __host__ __device__ inline T tex2D(const flag<GPU>&, ::texture<T, 2, READMODE>& texref,
                              const Image2d<U>& img, const point2d<int>& p)
  {
    return tex2D(texref, p.col(), p.row());
  }

  template<typename T, typename U, enum cudaTextureReadMode READMODE>
  __host__ __device__ inline T tex2D(const flag<GPU>&, ::texture<T, 2, READMODE>& texref,
                            const Image2d<U>& img, int c, int r)
  {
    return tex2D(texref, c, r);
  }

#endif

  // CPU fallbacks.

  template<typename T, typename U, enum cudaTextureReadMode READMODE>
  __host__ __device__ inline T tex2D(const flag<CPU>&, ::texture<T, 2, READMODE>& texref,
                                     const Image2d<U>& img, const point2d<int>& p)
  {
    return T(exact(img)(p));
  }

  template<typename T, typename U, enum cudaTextureReadMode READMODE>
  __host__ __device__ inline T tex2D(const flag<CPU>&, ::texture<T, 2, READMODE>& texref,
                                     const Image2d<U>& img, int c, int r)
  {
    return T(exact(img)(r, c));
  }

  template<typename U>
    __host__ __device__ inline typename U::value_type tex2D(const flag<CPU>&, int texref,
                                                            const Image2d<U>& img, const point2d<int>& p)
  {
    return exact(img)(p);
  }

  template<typename U>
  __host__ __device__ inline typename U::value_type tex2D(const flag<CPU>&, int texref,
                                                          const Image2d<U>& img, int c, int r)
  {
    return exact(img)(r, c);
  }

#ifndef NVCC

  __host__ __device__ inline int tex2D(int texref, int c, int r)
  {
    return 0;
  }

  template<typename T, enum cudaTextureReadMode READMODE>
  __host__ __device__ inline T tex2D(::texture<T, 2, READMODE>& texref, int c, int r)
  {
    return T();
  }

#endif

#define EXPAND(X) X


#define UNIT_STATIC_2(X, Y) X##_##Y
#define UNIT_STATIC_1(X, Y) UNIT_STATIC_2(X, Y)
#define UNIT_STATIC(X) UNIT_STATIC_1(UNIT_ID, X)

#define REGISTER_TEXTURE2D_TYPE_(PREFIX, VTYPE)                         \
  ::texture<VTYPE, 2, cudaReadModeElementType> PREFIX##_##VTYPE;        \
template <>                                                             \
struct PREFIX<VTYPE>                                                    \
{                                                                       \
  static __host__ __device__ inline ::texture<VTYPE, 2, cudaReadModeElementType>& tex() { return PREFIX##_##VTYPE; } \
};

#define REGISTER_TEXTURE2D_PROXY_2(P)                                   \
REGISTER_TEXTURE2D_TYPE_(P, float1)                                     \
REGISTER_TEXTURE2D_TYPE_(P, float2)                                     \
REGISTER_TEXTURE2D_TYPE_(P, float4)                                     \
REGISTER_TEXTURE2D_TYPE_(P, int1)                                       \
REGISTER_TEXTURE2D_TYPE_(P, int2)                                       \
REGISTER_TEXTURE2D_TYPE_(P, int4)                                       \
REGISTER_TEXTURE2D_TYPE_(P, uint1)                                      \
REGISTER_TEXTURE2D_TYPE_(P, uint2)                                      \
REGISTER_TEXTURE2D_TYPE_(P, uint4)                                      \
REGISTER_TEXTURE2D_TYPE_(P, char1)                                      \
REGISTER_TEXTURE2D_TYPE_(P, char2)                                      \
REGISTER_TEXTURE2D_TYPE_(P, char4)                                      \
REGISTER_TEXTURE2D_TYPE_(P, uchar1)                                     \
REGISTER_TEXTURE2D_TYPE_(P, uchar2)                                     \
REGISTER_TEXTURE2D_TYPE_(P, uchar3)                                     \
REGISTER_TEXTURE2D_TYPE_(P, uchar4)                                     \
REGISTER_TEXTURE2D_TYPE_(P, short1)                                     \
REGISTER_TEXTURE2D_TYPE_(P, short2)                                     \
REGISTER_TEXTURE2D_TYPE_(P, short4)                                     \
REGISTER_TEXTURE2D_TYPE_(P, ushort1)                                    \
REGISTER_TEXTURE2D_TYPE_(P, ushort2)                                    \
REGISTER_TEXTURE2D_TYPE_(P, ushort4)                                    \
REGISTER_TEXTURE2D_TYPE_(P, long1)                                      \
REGISTER_TEXTURE2D_TYPE_(P, long2)                                      \
REGISTER_TEXTURE2D_TYPE_(P, long4)                                      \
REGISTER_TEXTURE2D_TYPE_(P, ulong1)                                     \
REGISTER_TEXTURE2D_TYPE_(P, ulong2)                                     \
REGISTER_TEXTURE2D_TYPE_(P, ulong4)                                     \
  template<typename T, unsigned N>                                      \
  struct P<improved_builtin<T, N> >                                     \
{                                                                       \
  typedef typename improved_builtin<T, N>::cuda_bt cuda_bt;             \
  static __device__ inline ::texture<cuda_bt, 2, cudaReadModeElementType>& tex() { return P<cuda_bt>::tex(); } \
};

#define REGISTER_TEXTURE2D_PROXY_1(P) REGISTER_TEXTURE2D_PROXY_2(P)
#define REGISTER_TEXTURE2D_PROXY(P) REGISTER_TEXTURE2D_PROXY_1(UNIT_STATIC(P))

}


#endif
