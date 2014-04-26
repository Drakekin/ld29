
#include <Python.h>
#include <stddef.h>

#ifdef MS_WIN32
#include <malloc.h>   /* for alloca() */
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
typedef unsigned char _Bool;
#endif

#if PY_MAJOR_VERSION < 3
# undef PyCapsule_CheckExact
# undef PyCapsule_GetPointer
# define PyCapsule_CheckExact(capsule) (PyCObject_Check(capsule))
# define PyCapsule_GetPointer(capsule, name) \
    (PyCObject_AsVoidPtr(capsule))
#endif

#if PY_MAJOR_VERSION >= 3
# define PyInt_FromLong PyLong_FromLong
#endif

#define _cffi_from_c_double PyFloat_FromDouble
#define _cffi_from_c_float PyFloat_FromDouble
#define _cffi_from_c_long PyInt_FromLong
#define _cffi_from_c_ulong PyLong_FromUnsignedLong
#define _cffi_from_c_longlong PyLong_FromLongLong
#define _cffi_from_c_ulonglong PyLong_FromUnsignedLongLong

#define _cffi_to_c_double PyFloat_AsDouble
#define _cffi_to_c_float PyFloat_AsDouble

#define _cffi_from_c_int(x, type)                                        \
    (((type)-1) > 0 ?   /* unsigned */                                   \
        (sizeof(type) < sizeof(long) ? PyInt_FromLong(x) :               \
         sizeof(type) == sizeof(long) ? PyLong_FromUnsignedLong(x) :     \
                                        PyLong_FromUnsignedLongLong(x))  \
      : (sizeof(type) <= sizeof(long) ? PyInt_FromLong(x) :              \
                                        PyLong_FromLongLong(x)))

#define _cffi_to_c_int(o, type)                                          \
    (sizeof(type) == 1 ? (((type)-1) > 0 ? _cffi_to_c_u8(o)              \
                                         : _cffi_to_c_i8(o)) :           \
     sizeof(type) == 2 ? (((type)-1) > 0 ? _cffi_to_c_u16(o)             \
                                         : _cffi_to_c_i16(o)) :          \
     sizeof(type) == 4 ? (((type)-1) > 0 ? _cffi_to_c_u32(o)             \
                                         : _cffi_to_c_i32(o)) :          \
     sizeof(type) == 8 ? (((type)-1) > 0 ? _cffi_to_c_u64(o)             \
                                         : _cffi_to_c_i64(o)) :          \
     (Py_FatalError("unsupported size for type " #type), 0))

#define _cffi_to_c_i8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[1])
#define _cffi_to_c_u8                                                    \
                 ((int(*)(PyObject *))_cffi_exports[2])
#define _cffi_to_c_i16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[3])
#define _cffi_to_c_u16                                                   \
                 ((int(*)(PyObject *))_cffi_exports[4])
#define _cffi_to_c_i32                                                   \
                 ((int(*)(PyObject *))_cffi_exports[5])
#define _cffi_to_c_u32                                                   \
                 ((unsigned int(*)(PyObject *))_cffi_exports[6])
#define _cffi_to_c_i64                                                   \
                 ((long long(*)(PyObject *))_cffi_exports[7])
#define _cffi_to_c_u64                                                   \
                 ((unsigned long long(*)(PyObject *))_cffi_exports[8])
#define _cffi_to_c_char                                                  \
                 ((int(*)(PyObject *))_cffi_exports[9])
#define _cffi_from_c_pointer                                             \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[10])
#define _cffi_to_c_pointer                                               \
    ((char *(*)(PyObject *, CTypeDescrObject *))_cffi_exports[11])
#define _cffi_get_struct_layout                                          \
    ((PyObject *(*)(Py_ssize_t[]))_cffi_exports[12])
#define _cffi_restore_errno                                              \
    ((void(*)(void))_cffi_exports[13])
#define _cffi_save_errno                                                 \
    ((void(*)(void))_cffi_exports[14])
#define _cffi_from_c_char                                                \
    ((PyObject *(*)(char))_cffi_exports[15])
#define _cffi_from_c_deref                                               \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[16])
#define _cffi_to_c                                                       \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[17])
#define _cffi_from_c_struct                                              \
    ((PyObject *(*)(char *, CTypeDescrObject *))_cffi_exports[18])
#define _cffi_to_c_wchar_t                                               \
    ((wchar_t(*)(PyObject *))_cffi_exports[19])
#define _cffi_from_c_wchar_t                                             \
    ((PyObject *(*)(wchar_t))_cffi_exports[20])
#define _cffi_to_c_long_double                                           \
    ((long double(*)(PyObject *))_cffi_exports[21])
#define _cffi_to_c__Bool                                                 \
    ((_Bool(*)(PyObject *))_cffi_exports[22])
#define _cffi_prepare_pointer_call_argument                              \
    ((Py_ssize_t(*)(CTypeDescrObject *, PyObject *, char **))_cffi_exports[23])
#define _cffi_convert_array_from_object                                  \
    ((int(*)(char *, CTypeDescrObject *, PyObject *))_cffi_exports[24])
#define _CFFI_NUM_EXPORTS 25

typedef struct _ctypedescr CTypeDescrObject;

static void *_cffi_exports[_CFFI_NUM_EXPORTS];
static PyObject *_cffi_types, *_cffi_VerificationError;

static int _cffi_setup_custom(PyObject *lib);   /* forward */

static PyObject *_cffi_setup(PyObject *self, PyObject *args)
{
    PyObject *library;
    int was_alive = (_cffi_types != NULL);
    if (!PyArg_ParseTuple(args, "OOO", &_cffi_types, &_cffi_VerificationError,
                                       &library))
        return NULL;
    Py_INCREF(_cffi_types);
    Py_INCREF(_cffi_VerificationError);
    if (_cffi_setup_custom(library) < 0)
        return NULL;
    return PyBool_FromLong(was_alive);
}

static void _cffi_init(void)
{
    PyObject *module = PyImport_ImportModule("_cffi_backend");
    PyObject *c_api_object;

    if (module == NULL)
        return;

    c_api_object = PyObject_GetAttrString(module, "_C_API");
    if (c_api_object == NULL)
        return;
    if (!PyCapsule_CheckExact(c_api_object)) {
        Py_DECREF(c_api_object);
        PyErr_SetNone(PyExc_ImportError);
        return;
    }
    memcpy(_cffi_exports, PyCapsule_GetPointer(c_api_object, "cffi"),
           _CFFI_NUM_EXPORTS * sizeof(void *));
    Py_DECREF(c_api_object);
}

#define _cffi_type(num) ((CTypeDescrObject *)PyList_GET_ITEM(_cffi_types, num))

/**********/



enum Options {
    MaxLogLevel = 1,
    MaxNumMessages,
    TrilinearFiltering,
    MaxAnisotropy,
    TexCompression,
    SRGBLinearization,
    LoadTextures,
    FastAnimation,
    ShadowMapSize,
    SampleCount,
    WireframeMode,
    DebugViewMode,
    DumpFailedShaders,
    GatherTimeStats
};

enum Stats {
    TriCount = 100,
    BatchCount,
    LightPassCount,
    FrameTime,
    AnimationTime,
    GeoUpdateTime,
    ParticleSimTime,
    FwdLightsGPUTime,
    DefLightsGPUTime,
    ShadowsGPUTime,
    ParticleGPUTime,
    TextureVMem,
    GeometryVMem
};

enum ResTypes {
    Undefined = 0,
    SceneGraph,
    Geometry,
    Animation,
    Material,
    Code,
    Shader,
    Texture,
    ParticleEffect,
    Pipeline
};

enum ResFlags {
    NoQuery = 1,
    NoTexCompression = 2,
    NoTexMipmaps = 4,
    TexCubemap = 8,
    TexDynamic = 16,
    TexRenderable = 32,
    TexSRGB = 64
};

enum Formats {
    Unknown = 0,
    TEX_BGRA8,
    TEX_DXT1,
    TEX_DXT3,
    TEX_DXT5,
    TEX_RGBA16F,
    TEX_RGBA32F
};

enum GeoRes {
    GeometryElem = 200,
    GeoIndexCountI,
    GeoVertexCountI,
    GeoIndices16I,
    GeoIndexStream,
    GeoVertPosStream,
    GeoVertTanStream,
    GeoVertStaticStream
};

enum AnimRes {
    EntityElem = 300,
    EntFrameCountI
};

enum MatRes {
    MaterialElem = 400,
    MatSamplerElem,
    MatUniformElem,
    MatMatClassStr,
    MatMatLinkI,
    MatMatShaderI,
    MatSampNameStr,
    MatSampTexResI,
    MatUnifNameStr,
    MatUnifValueF4
};

enum ShaderRes {
    ContextElem = 600,
    SamplerElem,
    UniformElem,
    ContNameStr,
    SampNameStr,
    SampDefTexResI,
    UnifNameStr,
    UnifSizeI,
    UnifDefValueF4
};

enum TexRes {
    TextureElem = 700,
    ImageElem,
    TexFormatI,
    TexSliceCountI,
    ImgWidthI,
    ImgHeightI,
    ImgPixelStream
};

enum PartEffRes {
    ParticleElem = 800,
    ChanMoveVelElem,
    ChanRotVelElem,
    ChanSizeElem,
    ChanColRElem,
    ChanColGElem,
    ChanColBElem,
    ChanColAElem,
    PartLifeMinF,
    PartLifeMaxF,
    ChanStartMinF,
    ChanStartMaxF,
    ChanEndRateF,
    ChanDragElem
};

enum PipeRes {
    StageElem = 900,
    StageNameStr,
    StageActivationI
};

enum NodeTypes {
    Undefined = 0,
    Group,
    Model,
    Mesh,
    Joint,
    Light,
    Camera,
    Emitter
};

enum NodeFlags {
    NoDraw = 1,
    NoCastShadow = 2,
    NoRayQuery = 4,
    Inactive = 7
};

enum NodeParams {
    NameStr = 1,
    AttachmentStr
};

enum Model {
    GeoResI = 200,
    SWSkinningI,
    LodDist1F,
    LodDist2F,
    LodDist3F,
    LodDist4F,
    AnimCountI
};

enum Mesh {
    MatResI = 300,
    BatchStartI,
    BatchCountI,
    VertRStartI,
    VertREndI,
    LodLevelI
};

enum Joint {
    JointIndexI = 400
};

enum Light {
    MatResI = 500,
    RadiusF,
    FovF,
    ColorF3,
    ColorMultiplierF,
    ShadowMapCountI,
    ShadowSplitLambdaF,
    ShadowMapBiasF,
    LightingContextStr,
    ShadowContextStr
};

enum Camera {
    PipeResI = 600,
    OutTexResI,
    OutBufIndexI,
    LeftPlaneF,
    RightPlaneF,
    BottomPlaneF,
    TopPlaneF,
    NearPlaneF,
    FarPlaneF,
    ViewportXI,
    ViewportYI,
    ViewportWidthI,
    ViewportHeightI,
    OrthoI,
    OccCullingI
};

enum Emitter {
    MatResI = 700,
    PartEffResI,
    MaxCountI,
    RespawnCountI,
    DelayF,
    EmissionRateF,
    SpreadAngleF,
    ForceF3
};

enum ModelUpdateFlags {
    Animation = 1,
    Geometry = 2
};


static int _cffi_e_enum_AnimRes(PyObject *lib)
{
  if ((EntityElem) < 0 || (unsigned long)(EntityElem) != 300UL) {
    char buf[64];
    if ((EntityElem) < 0)
        snprintf(buf, 63, "%ld", (long)(EntityElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(EntityElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "AnimRes", "EntityElem", buf, "300");
    return -1;
  }
  if ((EntFrameCountI) < 0 || (unsigned long)(EntFrameCountI) != 301UL) {
    char buf[64];
    if ((EntFrameCountI) < 0)
        snprintf(buf, 63, "%ld", (long)(EntFrameCountI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(EntFrameCountI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "AnimRes", "EntFrameCountI", buf, "301");
    return -1;
  }
  return 0;
}

static int _cffi_e_enum_Camera(PyObject *lib)
{
  if ((PipeResI) < 0 || (unsigned long)(PipeResI) != 600UL) {
    char buf[64];
    if ((PipeResI) < 0)
        snprintf(buf, 63, "%ld", (long)(PipeResI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(PipeResI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "PipeResI", buf, "600");
    return -1;
  }
  if ((OutTexResI) < 0 || (unsigned long)(OutTexResI) != 601UL) {
    char buf[64];
    if ((OutTexResI) < 0)
        snprintf(buf, 63, "%ld", (long)(OutTexResI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(OutTexResI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "OutTexResI", buf, "601");
    return -1;
  }
  if ((OutBufIndexI) < 0 || (unsigned long)(OutBufIndexI) != 602UL) {
    char buf[64];
    if ((OutBufIndexI) < 0)
        snprintf(buf, 63, "%ld", (long)(OutBufIndexI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(OutBufIndexI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "OutBufIndexI", buf, "602");
    return -1;
  }
  if ((LeftPlaneF) < 0 || (unsigned long)(LeftPlaneF) != 603UL) {
    char buf[64];
    if ((LeftPlaneF) < 0)
        snprintf(buf, 63, "%ld", (long)(LeftPlaneF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(LeftPlaneF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "LeftPlaneF", buf, "603");
    return -1;
  }
  if ((RightPlaneF) < 0 || (unsigned long)(RightPlaneF) != 604UL) {
    char buf[64];
    if ((RightPlaneF) < 0)
        snprintf(buf, 63, "%ld", (long)(RightPlaneF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(RightPlaneF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "RightPlaneF", buf, "604");
    return -1;
  }
  if ((BottomPlaneF) < 0 || (unsigned long)(BottomPlaneF) != 605UL) {
    char buf[64];
    if ((BottomPlaneF) < 0)
        snprintf(buf, 63, "%ld", (long)(BottomPlaneF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(BottomPlaneF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "BottomPlaneF", buf, "605");
    return -1;
  }
  if ((TopPlaneF) < 0 || (unsigned long)(TopPlaneF) != 606UL) {
    char buf[64];
    if ((TopPlaneF) < 0)
        snprintf(buf, 63, "%ld", (long)(TopPlaneF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TopPlaneF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "TopPlaneF", buf, "606");
    return -1;
  }
  if ((NearPlaneF) < 0 || (unsigned long)(NearPlaneF) != 607UL) {
    char buf[64];
    if ((NearPlaneF) < 0)
        snprintf(buf, 63, "%ld", (long)(NearPlaneF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(NearPlaneF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "NearPlaneF", buf, "607");
    return -1;
  }
  if ((FarPlaneF) < 0 || (unsigned long)(FarPlaneF) != 608UL) {
    char buf[64];
    if ((FarPlaneF) < 0)
        snprintf(buf, 63, "%ld", (long)(FarPlaneF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(FarPlaneF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "FarPlaneF", buf, "608");
    return -1;
  }
  if ((ViewportXI) < 0 || (unsigned long)(ViewportXI) != 609UL) {
    char buf[64];
    if ((ViewportXI) < 0)
        snprintf(buf, 63, "%ld", (long)(ViewportXI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ViewportXI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "ViewportXI", buf, "609");
    return -1;
  }
  if ((ViewportYI) < 0 || (unsigned long)(ViewportYI) != 610UL) {
    char buf[64];
    if ((ViewportYI) < 0)
        snprintf(buf, 63, "%ld", (long)(ViewportYI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ViewportYI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "ViewportYI", buf, "610");
    return -1;
  }
  if ((ViewportWidthI) < 0 || (unsigned long)(ViewportWidthI) != 611UL) {
    char buf[64];
    if ((ViewportWidthI) < 0)
        snprintf(buf, 63, "%ld", (long)(ViewportWidthI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ViewportWidthI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "ViewportWidthI", buf, "611");
    return -1;
  }
  if ((ViewportHeightI) < 0 || (unsigned long)(ViewportHeightI) != 612UL) {
    char buf[64];
    if ((ViewportHeightI) < 0)
        snprintf(buf, 63, "%ld", (long)(ViewportHeightI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ViewportHeightI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "ViewportHeightI", buf, "612");
    return -1;
  }
  if ((OrthoI) < 0 || (unsigned long)(OrthoI) != 613UL) {
    char buf[64];
    if ((OrthoI) < 0)
        snprintf(buf, 63, "%ld", (long)(OrthoI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(OrthoI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "OrthoI", buf, "613");
    return -1;
  }
  if ((OccCullingI) < 0 || (unsigned long)(OccCullingI) != 614UL) {
    char buf[64];
    if ((OccCullingI) < 0)
        snprintf(buf, 63, "%ld", (long)(OccCullingI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(OccCullingI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Camera", "OccCullingI", buf, "614");
    return -1;
  }
  return _cffi_e_enum_AnimRes(lib);
}

static int _cffi_e_enum_Emitter(PyObject *lib)
{
  if ((MatResI) < 0 || (unsigned long)(MatResI) != 700UL) {
    char buf[64];
    if ((MatResI) < 0)
        snprintf(buf, 63, "%ld", (long)(MatResI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatResI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Emitter", "MatResI", buf, "700");
    return -1;
  }
  if ((PartEffResI) < 0 || (unsigned long)(PartEffResI) != 701UL) {
    char buf[64];
    if ((PartEffResI) < 0)
        snprintf(buf, 63, "%ld", (long)(PartEffResI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(PartEffResI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Emitter", "PartEffResI", buf, "701");
    return -1;
  }
  if ((MaxCountI) < 0 || (unsigned long)(MaxCountI) != 702UL) {
    char buf[64];
    if ((MaxCountI) < 0)
        snprintf(buf, 63, "%ld", (long)(MaxCountI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MaxCountI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Emitter", "MaxCountI", buf, "702");
    return -1;
  }
  if ((RespawnCountI) < 0 || (unsigned long)(RespawnCountI) != 703UL) {
    char buf[64];
    if ((RespawnCountI) < 0)
        snprintf(buf, 63, "%ld", (long)(RespawnCountI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(RespawnCountI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Emitter", "RespawnCountI", buf, "703");
    return -1;
  }
  if ((DelayF) < 0 || (unsigned long)(DelayF) != 704UL) {
    char buf[64];
    if ((DelayF) < 0)
        snprintf(buf, 63, "%ld", (long)(DelayF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(DelayF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Emitter", "DelayF", buf, "704");
    return -1;
  }
  if ((EmissionRateF) < 0 || (unsigned long)(EmissionRateF) != 705UL) {
    char buf[64];
    if ((EmissionRateF) < 0)
        snprintf(buf, 63, "%ld", (long)(EmissionRateF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(EmissionRateF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Emitter", "EmissionRateF", buf, "705");
    return -1;
  }
  if ((SpreadAngleF) < 0 || (unsigned long)(SpreadAngleF) != 706UL) {
    char buf[64];
    if ((SpreadAngleF) < 0)
        snprintf(buf, 63, "%ld", (long)(SpreadAngleF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(SpreadAngleF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Emitter", "SpreadAngleF", buf, "706");
    return -1;
  }
  if ((ForceF3) < 0 || (unsigned long)(ForceF3) != 707UL) {
    char buf[64];
    if ((ForceF3) < 0)
        snprintf(buf, 63, "%ld", (long)(ForceF3));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ForceF3));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Emitter", "ForceF3", buf, "707");
    return -1;
  }
  return _cffi_e_enum_Camera(lib);
}

static int _cffi_e_enum_Formats(PyObject *lib)
{
  if ((Unknown) < 0 || (unsigned long)(Unknown) != 0UL) {
    char buf[64];
    if ((Unknown) < 0)
        snprintf(buf, 63, "%ld", (long)(Unknown));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Unknown));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Formats", "Unknown", buf, "0");
    return -1;
  }
  if ((TEX_BGRA8) < 0 || (unsigned long)(TEX_BGRA8) != 1UL) {
    char buf[64];
    if ((TEX_BGRA8) < 0)
        snprintf(buf, 63, "%ld", (long)(TEX_BGRA8));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TEX_BGRA8));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Formats", "TEX_BGRA8", buf, "1");
    return -1;
  }
  if ((TEX_DXT1) < 0 || (unsigned long)(TEX_DXT1) != 2UL) {
    char buf[64];
    if ((TEX_DXT1) < 0)
        snprintf(buf, 63, "%ld", (long)(TEX_DXT1));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TEX_DXT1));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Formats", "TEX_DXT1", buf, "2");
    return -1;
  }
  if ((TEX_DXT3) < 0 || (unsigned long)(TEX_DXT3) != 3UL) {
    char buf[64];
    if ((TEX_DXT3) < 0)
        snprintf(buf, 63, "%ld", (long)(TEX_DXT3));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TEX_DXT3));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Formats", "TEX_DXT3", buf, "3");
    return -1;
  }
  if ((TEX_DXT5) < 0 || (unsigned long)(TEX_DXT5) != 4UL) {
    char buf[64];
    if ((TEX_DXT5) < 0)
        snprintf(buf, 63, "%ld", (long)(TEX_DXT5));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TEX_DXT5));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Formats", "TEX_DXT5", buf, "4");
    return -1;
  }
  if ((TEX_RGBA16F) < 0 || (unsigned long)(TEX_RGBA16F) != 5UL) {
    char buf[64];
    if ((TEX_RGBA16F) < 0)
        snprintf(buf, 63, "%ld", (long)(TEX_RGBA16F));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TEX_RGBA16F));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Formats", "TEX_RGBA16F", buf, "5");
    return -1;
  }
  if ((TEX_RGBA32F) < 0 || (unsigned long)(TEX_RGBA32F) != 6UL) {
    char buf[64];
    if ((TEX_RGBA32F) < 0)
        snprintf(buf, 63, "%ld", (long)(TEX_RGBA32F));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TEX_RGBA32F));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Formats", "TEX_RGBA32F", buf, "6");
    return -1;
  }
  return _cffi_e_enum_Emitter(lib);
}

static int _cffi_e_enum_GeoRes(PyObject *lib)
{
  if ((GeometryElem) < 0 || (unsigned long)(GeometryElem) != 200UL) {
    char buf[64];
    if ((GeometryElem) < 0)
        snprintf(buf, 63, "%ld", (long)(GeometryElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeometryElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "GeoRes", "GeometryElem", buf, "200");
    return -1;
  }
  if ((GeoIndexCountI) < 0 || (unsigned long)(GeoIndexCountI) != 201UL) {
    char buf[64];
    if ((GeoIndexCountI) < 0)
        snprintf(buf, 63, "%ld", (long)(GeoIndexCountI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeoIndexCountI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "GeoRes", "GeoIndexCountI", buf, "201");
    return -1;
  }
  if ((GeoVertexCountI) < 0 || (unsigned long)(GeoVertexCountI) != 202UL) {
    char buf[64];
    if ((GeoVertexCountI) < 0)
        snprintf(buf, 63, "%ld", (long)(GeoVertexCountI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeoVertexCountI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "GeoRes", "GeoVertexCountI", buf, "202");
    return -1;
  }
  if ((GeoIndices16I) < 0 || (unsigned long)(GeoIndices16I) != 203UL) {
    char buf[64];
    if ((GeoIndices16I) < 0)
        snprintf(buf, 63, "%ld", (long)(GeoIndices16I));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeoIndices16I));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "GeoRes", "GeoIndices16I", buf, "203");
    return -1;
  }
  if ((GeoIndexStream) < 0 || (unsigned long)(GeoIndexStream) != 204UL) {
    char buf[64];
    if ((GeoIndexStream) < 0)
        snprintf(buf, 63, "%ld", (long)(GeoIndexStream));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeoIndexStream));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "GeoRes", "GeoIndexStream", buf, "204");
    return -1;
  }
  if ((GeoVertPosStream) < 0 || (unsigned long)(GeoVertPosStream) != 205UL) {
    char buf[64];
    if ((GeoVertPosStream) < 0)
        snprintf(buf, 63, "%ld", (long)(GeoVertPosStream));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeoVertPosStream));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "GeoRes", "GeoVertPosStream", buf, "205");
    return -1;
  }
  if ((GeoVertTanStream) < 0 || (unsigned long)(GeoVertTanStream) != 206UL) {
    char buf[64];
    if ((GeoVertTanStream) < 0)
        snprintf(buf, 63, "%ld", (long)(GeoVertTanStream));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeoVertTanStream));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "GeoRes", "GeoVertTanStream", buf, "206");
    return -1;
  }
  if ((GeoVertStaticStream) < 0 || (unsigned long)(GeoVertStaticStream) != 207UL) {
    char buf[64];
    if ((GeoVertStaticStream) < 0)
        snprintf(buf, 63, "%ld", (long)(GeoVertStaticStream));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeoVertStaticStream));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "GeoRes", "GeoVertStaticStream", buf, "207");
    return -1;
  }
  return _cffi_e_enum_Formats(lib);
}

static int _cffi_e_enum_Joint(PyObject *lib)
{
  if ((JointIndexI) < 0 || (unsigned long)(JointIndexI) != 400UL) {
    char buf[64];
    if ((JointIndexI) < 0)
        snprintf(buf, 63, "%ld", (long)(JointIndexI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(JointIndexI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Joint", "JointIndexI", buf, "400");
    return -1;
  }
  return _cffi_e_enum_GeoRes(lib);
}

static int _cffi_e_enum_Light(PyObject *lib)
{
  if ((MatResI) < 0 || (unsigned long)(MatResI) != 500UL) {
    char buf[64];
    if ((MatResI) < 0)
        snprintf(buf, 63, "%ld", (long)(MatResI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatResI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Light", "MatResI", buf, "500");
    return -1;
  }
  if ((RadiusF) < 0 || (unsigned long)(RadiusF) != 501UL) {
    char buf[64];
    if ((RadiusF) < 0)
        snprintf(buf, 63, "%ld", (long)(RadiusF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(RadiusF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Light", "RadiusF", buf, "501");
    return -1;
  }
  if ((FovF) < 0 || (unsigned long)(FovF) != 502UL) {
    char buf[64];
    if ((FovF) < 0)
        snprintf(buf, 63, "%ld", (long)(FovF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(FovF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Light", "FovF", buf, "502");
    return -1;
  }
  if ((ColorF3) < 0 || (unsigned long)(ColorF3) != 503UL) {
    char buf[64];
    if ((ColorF3) < 0)
        snprintf(buf, 63, "%ld", (long)(ColorF3));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ColorF3));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Light", "ColorF3", buf, "503");
    return -1;
  }
  if ((ColorMultiplierF) < 0 || (unsigned long)(ColorMultiplierF) != 504UL) {
    char buf[64];
    if ((ColorMultiplierF) < 0)
        snprintf(buf, 63, "%ld", (long)(ColorMultiplierF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ColorMultiplierF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Light", "ColorMultiplierF", buf, "504");
    return -1;
  }
  if ((ShadowMapCountI) < 0 || (unsigned long)(ShadowMapCountI) != 505UL) {
    char buf[64];
    if ((ShadowMapCountI) < 0)
        snprintf(buf, 63, "%ld", (long)(ShadowMapCountI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ShadowMapCountI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Light", "ShadowMapCountI", buf, "505");
    return -1;
  }
  if ((ShadowSplitLambdaF) < 0 || (unsigned long)(ShadowSplitLambdaF) != 506UL) {
    char buf[64];
    if ((ShadowSplitLambdaF) < 0)
        snprintf(buf, 63, "%ld", (long)(ShadowSplitLambdaF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ShadowSplitLambdaF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Light", "ShadowSplitLambdaF", buf, "506");
    return -1;
  }
  if ((ShadowMapBiasF) < 0 || (unsigned long)(ShadowMapBiasF) != 507UL) {
    char buf[64];
    if ((ShadowMapBiasF) < 0)
        snprintf(buf, 63, "%ld", (long)(ShadowMapBiasF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ShadowMapBiasF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Light", "ShadowMapBiasF", buf, "507");
    return -1;
  }
  if ((LightingContextStr) < 0 || (unsigned long)(LightingContextStr) != 508UL) {
    char buf[64];
    if ((LightingContextStr) < 0)
        snprintf(buf, 63, "%ld", (long)(LightingContextStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(LightingContextStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Light", "LightingContextStr", buf, "508");
    return -1;
  }
  if ((ShadowContextStr) < 0 || (unsigned long)(ShadowContextStr) != 509UL) {
    char buf[64];
    if ((ShadowContextStr) < 0)
        snprintf(buf, 63, "%ld", (long)(ShadowContextStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ShadowContextStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Light", "ShadowContextStr", buf, "509");
    return -1;
  }
  return _cffi_e_enum_Joint(lib);
}

static int _cffi_e_enum_MatRes(PyObject *lib)
{
  if ((MaterialElem) < 0 || (unsigned long)(MaterialElem) != 400UL) {
    char buf[64];
    if ((MaterialElem) < 0)
        snprintf(buf, 63, "%ld", (long)(MaterialElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MaterialElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "MatRes", "MaterialElem", buf, "400");
    return -1;
  }
  if ((MatSamplerElem) < 0 || (unsigned long)(MatSamplerElem) != 401UL) {
    char buf[64];
    if ((MatSamplerElem) < 0)
        snprintf(buf, 63, "%ld", (long)(MatSamplerElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatSamplerElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "MatRes", "MatSamplerElem", buf, "401");
    return -1;
  }
  if ((MatUniformElem) < 0 || (unsigned long)(MatUniformElem) != 402UL) {
    char buf[64];
    if ((MatUniformElem) < 0)
        snprintf(buf, 63, "%ld", (long)(MatUniformElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatUniformElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "MatRes", "MatUniformElem", buf, "402");
    return -1;
  }
  if ((MatMatClassStr) < 0 || (unsigned long)(MatMatClassStr) != 403UL) {
    char buf[64];
    if ((MatMatClassStr) < 0)
        snprintf(buf, 63, "%ld", (long)(MatMatClassStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatMatClassStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "MatRes", "MatMatClassStr", buf, "403");
    return -1;
  }
  if ((MatMatLinkI) < 0 || (unsigned long)(MatMatLinkI) != 404UL) {
    char buf[64];
    if ((MatMatLinkI) < 0)
        snprintf(buf, 63, "%ld", (long)(MatMatLinkI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatMatLinkI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "MatRes", "MatMatLinkI", buf, "404");
    return -1;
  }
  if ((MatMatShaderI) < 0 || (unsigned long)(MatMatShaderI) != 405UL) {
    char buf[64];
    if ((MatMatShaderI) < 0)
        snprintf(buf, 63, "%ld", (long)(MatMatShaderI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatMatShaderI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "MatRes", "MatMatShaderI", buf, "405");
    return -1;
  }
  if ((MatSampNameStr) < 0 || (unsigned long)(MatSampNameStr) != 406UL) {
    char buf[64];
    if ((MatSampNameStr) < 0)
        snprintf(buf, 63, "%ld", (long)(MatSampNameStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatSampNameStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "MatRes", "MatSampNameStr", buf, "406");
    return -1;
  }
  if ((MatSampTexResI) < 0 || (unsigned long)(MatSampTexResI) != 407UL) {
    char buf[64];
    if ((MatSampTexResI) < 0)
        snprintf(buf, 63, "%ld", (long)(MatSampTexResI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatSampTexResI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "MatRes", "MatSampTexResI", buf, "407");
    return -1;
  }
  if ((MatUnifNameStr) < 0 || (unsigned long)(MatUnifNameStr) != 408UL) {
    char buf[64];
    if ((MatUnifNameStr) < 0)
        snprintf(buf, 63, "%ld", (long)(MatUnifNameStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatUnifNameStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "MatRes", "MatUnifNameStr", buf, "408");
    return -1;
  }
  if ((MatUnifValueF4) < 0 || (unsigned long)(MatUnifValueF4) != 409UL) {
    char buf[64];
    if ((MatUnifValueF4) < 0)
        snprintf(buf, 63, "%ld", (long)(MatUnifValueF4));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatUnifValueF4));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "MatRes", "MatUnifValueF4", buf, "409");
    return -1;
  }
  return _cffi_e_enum_Light(lib);
}

static int _cffi_e_enum_Mesh(PyObject *lib)
{
  if ((MatResI) < 0 || (unsigned long)(MatResI) != 300UL) {
    char buf[64];
    if ((MatResI) < 0)
        snprintf(buf, 63, "%ld", (long)(MatResI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MatResI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Mesh", "MatResI", buf, "300");
    return -1;
  }
  if ((BatchStartI) < 0 || (unsigned long)(BatchStartI) != 301UL) {
    char buf[64];
    if ((BatchStartI) < 0)
        snprintf(buf, 63, "%ld", (long)(BatchStartI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(BatchStartI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Mesh", "BatchStartI", buf, "301");
    return -1;
  }
  if ((BatchCountI) < 0 || (unsigned long)(BatchCountI) != 302UL) {
    char buf[64];
    if ((BatchCountI) < 0)
        snprintf(buf, 63, "%ld", (long)(BatchCountI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(BatchCountI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Mesh", "BatchCountI", buf, "302");
    return -1;
  }
  if ((VertRStartI) < 0 || (unsigned long)(VertRStartI) != 303UL) {
    char buf[64];
    if ((VertRStartI) < 0)
        snprintf(buf, 63, "%ld", (long)(VertRStartI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(VertRStartI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Mesh", "VertRStartI", buf, "303");
    return -1;
  }
  if ((VertREndI) < 0 || (unsigned long)(VertREndI) != 304UL) {
    char buf[64];
    if ((VertREndI) < 0)
        snprintf(buf, 63, "%ld", (long)(VertREndI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(VertREndI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Mesh", "VertREndI", buf, "304");
    return -1;
  }
  if ((LodLevelI) < 0 || (unsigned long)(LodLevelI) != 305UL) {
    char buf[64];
    if ((LodLevelI) < 0)
        snprintf(buf, 63, "%ld", (long)(LodLevelI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(LodLevelI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Mesh", "LodLevelI", buf, "305");
    return -1;
  }
  return _cffi_e_enum_MatRes(lib);
}

static int _cffi_e_enum_Model(PyObject *lib)
{
  if ((GeoResI) < 0 || (unsigned long)(GeoResI) != 200UL) {
    char buf[64];
    if ((GeoResI) < 0)
        snprintf(buf, 63, "%ld", (long)(GeoResI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeoResI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Model", "GeoResI", buf, "200");
    return -1;
  }
  if ((SWSkinningI) < 0 || (unsigned long)(SWSkinningI) != 201UL) {
    char buf[64];
    if ((SWSkinningI) < 0)
        snprintf(buf, 63, "%ld", (long)(SWSkinningI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(SWSkinningI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Model", "SWSkinningI", buf, "201");
    return -1;
  }
  if ((LodDist1F) < 0 || (unsigned long)(LodDist1F) != 202UL) {
    char buf[64];
    if ((LodDist1F) < 0)
        snprintf(buf, 63, "%ld", (long)(LodDist1F));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(LodDist1F));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Model", "LodDist1F", buf, "202");
    return -1;
  }
  if ((LodDist2F) < 0 || (unsigned long)(LodDist2F) != 203UL) {
    char buf[64];
    if ((LodDist2F) < 0)
        snprintf(buf, 63, "%ld", (long)(LodDist2F));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(LodDist2F));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Model", "LodDist2F", buf, "203");
    return -1;
  }
  if ((LodDist3F) < 0 || (unsigned long)(LodDist3F) != 204UL) {
    char buf[64];
    if ((LodDist3F) < 0)
        snprintf(buf, 63, "%ld", (long)(LodDist3F));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(LodDist3F));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Model", "LodDist3F", buf, "204");
    return -1;
  }
  if ((LodDist4F) < 0 || (unsigned long)(LodDist4F) != 205UL) {
    char buf[64];
    if ((LodDist4F) < 0)
        snprintf(buf, 63, "%ld", (long)(LodDist4F));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(LodDist4F));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Model", "LodDist4F", buf, "205");
    return -1;
  }
  if ((AnimCountI) < 0 || (unsigned long)(AnimCountI) != 206UL) {
    char buf[64];
    if ((AnimCountI) < 0)
        snprintf(buf, 63, "%ld", (long)(AnimCountI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(AnimCountI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Model", "AnimCountI", buf, "206");
    return -1;
  }
  return _cffi_e_enum_Mesh(lib);
}

static int _cffi_e_enum_ModelUpdateFlags(PyObject *lib)
{
  if ((Animation) < 0 || (unsigned long)(Animation) != 1UL) {
    char buf[64];
    if ((Animation) < 0)
        snprintf(buf, 63, "%ld", (long)(Animation));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Animation));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ModelUpdateFlags", "Animation", buf, "1");
    return -1;
  }
  if ((Geometry) < 0 || (unsigned long)(Geometry) != 2UL) {
    char buf[64];
    if ((Geometry) < 0)
        snprintf(buf, 63, "%ld", (long)(Geometry));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Geometry));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ModelUpdateFlags", "Geometry", buf, "2");
    return -1;
  }
  return _cffi_e_enum_Model(lib);
}

static int _cffi_e_enum_NodeFlags(PyObject *lib)
{
  if ((NoDraw) < 0 || (unsigned long)(NoDraw) != 1UL) {
    char buf[64];
    if ((NoDraw) < 0)
        snprintf(buf, 63, "%ld", (long)(NoDraw));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(NoDraw));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeFlags", "NoDraw", buf, "1");
    return -1;
  }
  if ((NoCastShadow) < 0 || (unsigned long)(NoCastShadow) != 2UL) {
    char buf[64];
    if ((NoCastShadow) < 0)
        snprintf(buf, 63, "%ld", (long)(NoCastShadow));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(NoCastShadow));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeFlags", "NoCastShadow", buf, "2");
    return -1;
  }
  if ((NoRayQuery) < 0 || (unsigned long)(NoRayQuery) != 4UL) {
    char buf[64];
    if ((NoRayQuery) < 0)
        snprintf(buf, 63, "%ld", (long)(NoRayQuery));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(NoRayQuery));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeFlags", "NoRayQuery", buf, "4");
    return -1;
  }
  if ((Inactive) < 0 || (unsigned long)(Inactive) != 7UL) {
    char buf[64];
    if ((Inactive) < 0)
        snprintf(buf, 63, "%ld", (long)(Inactive));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Inactive));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeFlags", "Inactive", buf, "7");
    return -1;
  }
  return _cffi_e_enum_ModelUpdateFlags(lib);
}

static int _cffi_e_enum_NodeParams(PyObject *lib)
{
  if ((NameStr) < 0 || (unsigned long)(NameStr) != 1UL) {
    char buf[64];
    if ((NameStr) < 0)
        snprintf(buf, 63, "%ld", (long)(NameStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(NameStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeParams", "NameStr", buf, "1");
    return -1;
  }
  if ((AttachmentStr) < 0 || (unsigned long)(AttachmentStr) != 2UL) {
    char buf[64];
    if ((AttachmentStr) < 0)
        snprintf(buf, 63, "%ld", (long)(AttachmentStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(AttachmentStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeParams", "AttachmentStr", buf, "2");
    return -1;
  }
  return _cffi_e_enum_NodeFlags(lib);
}

static int _cffi_e_enum_NodeTypes(PyObject *lib)
{
  if ((Undefined) < 0 || (unsigned long)(Undefined) != 0UL) {
    char buf[64];
    if ((Undefined) < 0)
        snprintf(buf, 63, "%ld", (long)(Undefined));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Undefined));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeTypes", "Undefined", buf, "0");
    return -1;
  }
  if ((Group) < 0 || (unsigned long)(Group) != 1UL) {
    char buf[64];
    if ((Group) < 0)
        snprintf(buf, 63, "%ld", (long)(Group));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Group));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeTypes", "Group", buf, "1");
    return -1;
  }
  if ((Model) < 0 || (unsigned long)(Model) != 2UL) {
    char buf[64];
    if ((Model) < 0)
        snprintf(buf, 63, "%ld", (long)(Model));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Model));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeTypes", "Model", buf, "2");
    return -1;
  }
  if ((Mesh) < 0 || (unsigned long)(Mesh) != 3UL) {
    char buf[64];
    if ((Mesh) < 0)
        snprintf(buf, 63, "%ld", (long)(Mesh));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Mesh));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeTypes", "Mesh", buf, "3");
    return -1;
  }
  if ((Joint) < 0 || (unsigned long)(Joint) != 4UL) {
    char buf[64];
    if ((Joint) < 0)
        snprintf(buf, 63, "%ld", (long)(Joint));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Joint));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeTypes", "Joint", buf, "4");
    return -1;
  }
  if ((Light) < 0 || (unsigned long)(Light) != 5UL) {
    char buf[64];
    if ((Light) < 0)
        snprintf(buf, 63, "%ld", (long)(Light));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Light));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeTypes", "Light", buf, "5");
    return -1;
  }
  if ((Camera) < 0 || (unsigned long)(Camera) != 6UL) {
    char buf[64];
    if ((Camera) < 0)
        snprintf(buf, 63, "%ld", (long)(Camera));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Camera));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeTypes", "Camera", buf, "6");
    return -1;
  }
  if ((Emitter) < 0 || (unsigned long)(Emitter) != 7UL) {
    char buf[64];
    if ((Emitter) < 0)
        snprintf(buf, 63, "%ld", (long)(Emitter));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Emitter));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "NodeTypes", "Emitter", buf, "7");
    return -1;
  }
  return _cffi_e_enum_NodeParams(lib);
}

static int _cffi_e_enum_Options(PyObject *lib)
{
  if ((MaxLogLevel) < 0 || (unsigned long)(MaxLogLevel) != 1UL) {
    char buf[64];
    if ((MaxLogLevel) < 0)
        snprintf(buf, 63, "%ld", (long)(MaxLogLevel));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MaxLogLevel));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "MaxLogLevel", buf, "1");
    return -1;
  }
  if ((MaxNumMessages) < 0 || (unsigned long)(MaxNumMessages) != 2UL) {
    char buf[64];
    if ((MaxNumMessages) < 0)
        snprintf(buf, 63, "%ld", (long)(MaxNumMessages));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MaxNumMessages));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "MaxNumMessages", buf, "2");
    return -1;
  }
  if ((TrilinearFiltering) < 0 || (unsigned long)(TrilinearFiltering) != 3UL) {
    char buf[64];
    if ((TrilinearFiltering) < 0)
        snprintf(buf, 63, "%ld", (long)(TrilinearFiltering));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TrilinearFiltering));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "TrilinearFiltering", buf, "3");
    return -1;
  }
  if ((MaxAnisotropy) < 0 || (unsigned long)(MaxAnisotropy) != 4UL) {
    char buf[64];
    if ((MaxAnisotropy) < 0)
        snprintf(buf, 63, "%ld", (long)(MaxAnisotropy));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(MaxAnisotropy));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "MaxAnisotropy", buf, "4");
    return -1;
  }
  if ((TexCompression) < 0 || (unsigned long)(TexCompression) != 5UL) {
    char buf[64];
    if ((TexCompression) < 0)
        snprintf(buf, 63, "%ld", (long)(TexCompression));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TexCompression));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "TexCompression", buf, "5");
    return -1;
  }
  if ((SRGBLinearization) < 0 || (unsigned long)(SRGBLinearization) != 6UL) {
    char buf[64];
    if ((SRGBLinearization) < 0)
        snprintf(buf, 63, "%ld", (long)(SRGBLinearization));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(SRGBLinearization));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "SRGBLinearization", buf, "6");
    return -1;
  }
  if ((LoadTextures) < 0 || (unsigned long)(LoadTextures) != 7UL) {
    char buf[64];
    if ((LoadTextures) < 0)
        snprintf(buf, 63, "%ld", (long)(LoadTextures));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(LoadTextures));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "LoadTextures", buf, "7");
    return -1;
  }
  if ((FastAnimation) < 0 || (unsigned long)(FastAnimation) != 8UL) {
    char buf[64];
    if ((FastAnimation) < 0)
        snprintf(buf, 63, "%ld", (long)(FastAnimation));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(FastAnimation));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "FastAnimation", buf, "8");
    return -1;
  }
  if ((ShadowMapSize) < 0 || (unsigned long)(ShadowMapSize) != 9UL) {
    char buf[64];
    if ((ShadowMapSize) < 0)
        snprintf(buf, 63, "%ld", (long)(ShadowMapSize));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ShadowMapSize));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "ShadowMapSize", buf, "9");
    return -1;
  }
  if ((SampleCount) < 0 || (unsigned long)(SampleCount) != 10UL) {
    char buf[64];
    if ((SampleCount) < 0)
        snprintf(buf, 63, "%ld", (long)(SampleCount));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(SampleCount));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "SampleCount", buf, "10");
    return -1;
  }
  if ((WireframeMode) < 0 || (unsigned long)(WireframeMode) != 11UL) {
    char buf[64];
    if ((WireframeMode) < 0)
        snprintf(buf, 63, "%ld", (long)(WireframeMode));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(WireframeMode));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "WireframeMode", buf, "11");
    return -1;
  }
  if ((DebugViewMode) < 0 || (unsigned long)(DebugViewMode) != 12UL) {
    char buf[64];
    if ((DebugViewMode) < 0)
        snprintf(buf, 63, "%ld", (long)(DebugViewMode));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(DebugViewMode));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "DebugViewMode", buf, "12");
    return -1;
  }
  if ((DumpFailedShaders) < 0 || (unsigned long)(DumpFailedShaders) != 13UL) {
    char buf[64];
    if ((DumpFailedShaders) < 0)
        snprintf(buf, 63, "%ld", (long)(DumpFailedShaders));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(DumpFailedShaders));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "DumpFailedShaders", buf, "13");
    return -1;
  }
  if ((GatherTimeStats) < 0 || (unsigned long)(GatherTimeStats) != 14UL) {
    char buf[64];
    if ((GatherTimeStats) < 0)
        snprintf(buf, 63, "%ld", (long)(GatherTimeStats));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GatherTimeStats));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Options", "GatherTimeStats", buf, "14");
    return -1;
  }
  return _cffi_e_enum_NodeTypes(lib);
}

static int _cffi_e_enum_PartEffRes(PyObject *lib)
{
  if ((ParticleElem) < 0 || (unsigned long)(ParticleElem) != 800UL) {
    char buf[64];
    if ((ParticleElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ParticleElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ParticleElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ParticleElem", buf, "800");
    return -1;
  }
  if ((ChanMoveVelElem) < 0 || (unsigned long)(ChanMoveVelElem) != 801UL) {
    char buf[64];
    if ((ChanMoveVelElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanMoveVelElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanMoveVelElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanMoveVelElem", buf, "801");
    return -1;
  }
  if ((ChanRotVelElem) < 0 || (unsigned long)(ChanRotVelElem) != 802UL) {
    char buf[64];
    if ((ChanRotVelElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanRotVelElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanRotVelElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanRotVelElem", buf, "802");
    return -1;
  }
  if ((ChanSizeElem) < 0 || (unsigned long)(ChanSizeElem) != 803UL) {
    char buf[64];
    if ((ChanSizeElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanSizeElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanSizeElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanSizeElem", buf, "803");
    return -1;
  }
  if ((ChanColRElem) < 0 || (unsigned long)(ChanColRElem) != 804UL) {
    char buf[64];
    if ((ChanColRElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanColRElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanColRElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanColRElem", buf, "804");
    return -1;
  }
  if ((ChanColGElem) < 0 || (unsigned long)(ChanColGElem) != 805UL) {
    char buf[64];
    if ((ChanColGElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanColGElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanColGElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanColGElem", buf, "805");
    return -1;
  }
  if ((ChanColBElem) < 0 || (unsigned long)(ChanColBElem) != 806UL) {
    char buf[64];
    if ((ChanColBElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanColBElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanColBElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanColBElem", buf, "806");
    return -1;
  }
  if ((ChanColAElem) < 0 || (unsigned long)(ChanColAElem) != 807UL) {
    char buf[64];
    if ((ChanColAElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanColAElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanColAElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanColAElem", buf, "807");
    return -1;
  }
  if ((PartLifeMinF) < 0 || (unsigned long)(PartLifeMinF) != 808UL) {
    char buf[64];
    if ((PartLifeMinF) < 0)
        snprintf(buf, 63, "%ld", (long)(PartLifeMinF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(PartLifeMinF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "PartLifeMinF", buf, "808");
    return -1;
  }
  if ((PartLifeMaxF) < 0 || (unsigned long)(PartLifeMaxF) != 809UL) {
    char buf[64];
    if ((PartLifeMaxF) < 0)
        snprintf(buf, 63, "%ld", (long)(PartLifeMaxF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(PartLifeMaxF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "PartLifeMaxF", buf, "809");
    return -1;
  }
  if ((ChanStartMinF) < 0 || (unsigned long)(ChanStartMinF) != 810UL) {
    char buf[64];
    if ((ChanStartMinF) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanStartMinF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanStartMinF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanStartMinF", buf, "810");
    return -1;
  }
  if ((ChanStartMaxF) < 0 || (unsigned long)(ChanStartMaxF) != 811UL) {
    char buf[64];
    if ((ChanStartMaxF) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanStartMaxF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanStartMaxF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanStartMaxF", buf, "811");
    return -1;
  }
  if ((ChanEndRateF) < 0 || (unsigned long)(ChanEndRateF) != 812UL) {
    char buf[64];
    if ((ChanEndRateF) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanEndRateF));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanEndRateF));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanEndRateF", buf, "812");
    return -1;
  }
  if ((ChanDragElem) < 0 || (unsigned long)(ChanDragElem) != 813UL) {
    char buf[64];
    if ((ChanDragElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ChanDragElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ChanDragElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PartEffRes", "ChanDragElem", buf, "813");
    return -1;
  }
  return _cffi_e_enum_Options(lib);
}

static int _cffi_e_enum_PipeRes(PyObject *lib)
{
  if ((StageElem) < 0 || (unsigned long)(StageElem) != 900UL) {
    char buf[64];
    if ((StageElem) < 0)
        snprintf(buf, 63, "%ld", (long)(StageElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(StageElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PipeRes", "StageElem", buf, "900");
    return -1;
  }
  if ((StageNameStr) < 0 || (unsigned long)(StageNameStr) != 901UL) {
    char buf[64];
    if ((StageNameStr) < 0)
        snprintf(buf, 63, "%ld", (long)(StageNameStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(StageNameStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PipeRes", "StageNameStr", buf, "901");
    return -1;
  }
  if ((StageActivationI) < 0 || (unsigned long)(StageActivationI) != 902UL) {
    char buf[64];
    if ((StageActivationI) < 0)
        snprintf(buf, 63, "%ld", (long)(StageActivationI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(StageActivationI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "PipeRes", "StageActivationI", buf, "902");
    return -1;
  }
  return _cffi_e_enum_PartEffRes(lib);
}

static int _cffi_e_enum_ResFlags(PyObject *lib)
{
  if ((NoQuery) < 0 || (unsigned long)(NoQuery) != 1UL) {
    char buf[64];
    if ((NoQuery) < 0)
        snprintf(buf, 63, "%ld", (long)(NoQuery));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(NoQuery));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResFlags", "NoQuery", buf, "1");
    return -1;
  }
  if ((NoTexCompression) < 0 || (unsigned long)(NoTexCompression) != 2UL) {
    char buf[64];
    if ((NoTexCompression) < 0)
        snprintf(buf, 63, "%ld", (long)(NoTexCompression));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(NoTexCompression));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResFlags", "NoTexCompression", buf, "2");
    return -1;
  }
  if ((NoTexMipmaps) < 0 || (unsigned long)(NoTexMipmaps) != 4UL) {
    char buf[64];
    if ((NoTexMipmaps) < 0)
        snprintf(buf, 63, "%ld", (long)(NoTexMipmaps));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(NoTexMipmaps));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResFlags", "NoTexMipmaps", buf, "4");
    return -1;
  }
  if ((TexCubemap) < 0 || (unsigned long)(TexCubemap) != 8UL) {
    char buf[64];
    if ((TexCubemap) < 0)
        snprintf(buf, 63, "%ld", (long)(TexCubemap));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TexCubemap));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResFlags", "TexCubemap", buf, "8");
    return -1;
  }
  if ((TexDynamic) < 0 || (unsigned long)(TexDynamic) != 16UL) {
    char buf[64];
    if ((TexDynamic) < 0)
        snprintf(buf, 63, "%ld", (long)(TexDynamic));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TexDynamic));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResFlags", "TexDynamic", buf, "16");
    return -1;
  }
  if ((TexRenderable) < 0 || (unsigned long)(TexRenderable) != 32UL) {
    char buf[64];
    if ((TexRenderable) < 0)
        snprintf(buf, 63, "%ld", (long)(TexRenderable));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TexRenderable));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResFlags", "TexRenderable", buf, "32");
    return -1;
  }
  if ((TexSRGB) < 0 || (unsigned long)(TexSRGB) != 64UL) {
    char buf[64];
    if ((TexSRGB) < 0)
        snprintf(buf, 63, "%ld", (long)(TexSRGB));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TexSRGB));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResFlags", "TexSRGB", buf, "64");
    return -1;
  }
  return _cffi_e_enum_PipeRes(lib);
}

static int _cffi_e_enum_ResTypes(PyObject *lib)
{
  if ((Undefined) < 0 || (unsigned long)(Undefined) != 0UL) {
    char buf[64];
    if ((Undefined) < 0)
        snprintf(buf, 63, "%ld", (long)(Undefined));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Undefined));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResTypes", "Undefined", buf, "0");
    return -1;
  }
  if ((SceneGraph) < 0 || (unsigned long)(SceneGraph) != 1UL) {
    char buf[64];
    if ((SceneGraph) < 0)
        snprintf(buf, 63, "%ld", (long)(SceneGraph));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(SceneGraph));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResTypes", "SceneGraph", buf, "1");
    return -1;
  }
  if ((Geometry) < 0 || (unsigned long)(Geometry) != 2UL) {
    char buf[64];
    if ((Geometry) < 0)
        snprintf(buf, 63, "%ld", (long)(Geometry));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Geometry));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResTypes", "Geometry", buf, "2");
    return -1;
  }
  if ((Animation) < 0 || (unsigned long)(Animation) != 3UL) {
    char buf[64];
    if ((Animation) < 0)
        snprintf(buf, 63, "%ld", (long)(Animation));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Animation));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResTypes", "Animation", buf, "3");
    return -1;
  }
  if ((Material) < 0 || (unsigned long)(Material) != 4UL) {
    char buf[64];
    if ((Material) < 0)
        snprintf(buf, 63, "%ld", (long)(Material));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Material));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResTypes", "Material", buf, "4");
    return -1;
  }
  if ((Code) < 0 || (unsigned long)(Code) != 5UL) {
    char buf[64];
    if ((Code) < 0)
        snprintf(buf, 63, "%ld", (long)(Code));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Code));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResTypes", "Code", buf, "5");
    return -1;
  }
  if ((Shader) < 0 || (unsigned long)(Shader) != 6UL) {
    char buf[64];
    if ((Shader) < 0)
        snprintf(buf, 63, "%ld", (long)(Shader));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Shader));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResTypes", "Shader", buf, "6");
    return -1;
  }
  if ((Texture) < 0 || (unsigned long)(Texture) != 7UL) {
    char buf[64];
    if ((Texture) < 0)
        snprintf(buf, 63, "%ld", (long)(Texture));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Texture));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResTypes", "Texture", buf, "7");
    return -1;
  }
  if ((ParticleEffect) < 0 || (unsigned long)(ParticleEffect) != 8UL) {
    char buf[64];
    if ((ParticleEffect) < 0)
        snprintf(buf, 63, "%ld", (long)(ParticleEffect));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ParticleEffect));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResTypes", "ParticleEffect", buf, "8");
    return -1;
  }
  if ((Pipeline) < 0 || (unsigned long)(Pipeline) != 9UL) {
    char buf[64];
    if ((Pipeline) < 0)
        snprintf(buf, 63, "%ld", (long)(Pipeline));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(Pipeline));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ResTypes", "Pipeline", buf, "9");
    return -1;
  }
  return _cffi_e_enum_ResFlags(lib);
}

static int _cffi_e_enum_ShaderRes(PyObject *lib)
{
  if ((ContextElem) < 0 || (unsigned long)(ContextElem) != 600UL) {
    char buf[64];
    if ((ContextElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ContextElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ContextElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ShaderRes", "ContextElem", buf, "600");
    return -1;
  }
  if ((SamplerElem) < 0 || (unsigned long)(SamplerElem) != 601UL) {
    char buf[64];
    if ((SamplerElem) < 0)
        snprintf(buf, 63, "%ld", (long)(SamplerElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(SamplerElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ShaderRes", "SamplerElem", buf, "601");
    return -1;
  }
  if ((UniformElem) < 0 || (unsigned long)(UniformElem) != 602UL) {
    char buf[64];
    if ((UniformElem) < 0)
        snprintf(buf, 63, "%ld", (long)(UniformElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(UniformElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ShaderRes", "UniformElem", buf, "602");
    return -1;
  }
  if ((ContNameStr) < 0 || (unsigned long)(ContNameStr) != 603UL) {
    char buf[64];
    if ((ContNameStr) < 0)
        snprintf(buf, 63, "%ld", (long)(ContNameStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ContNameStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ShaderRes", "ContNameStr", buf, "603");
    return -1;
  }
  if ((SampNameStr) < 0 || (unsigned long)(SampNameStr) != 604UL) {
    char buf[64];
    if ((SampNameStr) < 0)
        snprintf(buf, 63, "%ld", (long)(SampNameStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(SampNameStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ShaderRes", "SampNameStr", buf, "604");
    return -1;
  }
  if ((SampDefTexResI) < 0 || (unsigned long)(SampDefTexResI) != 605UL) {
    char buf[64];
    if ((SampDefTexResI) < 0)
        snprintf(buf, 63, "%ld", (long)(SampDefTexResI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(SampDefTexResI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ShaderRes", "SampDefTexResI", buf, "605");
    return -1;
  }
  if ((UnifNameStr) < 0 || (unsigned long)(UnifNameStr) != 606UL) {
    char buf[64];
    if ((UnifNameStr) < 0)
        snprintf(buf, 63, "%ld", (long)(UnifNameStr));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(UnifNameStr));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ShaderRes", "UnifNameStr", buf, "606");
    return -1;
  }
  if ((UnifSizeI) < 0 || (unsigned long)(UnifSizeI) != 607UL) {
    char buf[64];
    if ((UnifSizeI) < 0)
        snprintf(buf, 63, "%ld", (long)(UnifSizeI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(UnifSizeI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ShaderRes", "UnifSizeI", buf, "607");
    return -1;
  }
  if ((UnifDefValueF4) < 0 || (unsigned long)(UnifDefValueF4) != 608UL) {
    char buf[64];
    if ((UnifDefValueF4) < 0)
        snprintf(buf, 63, "%ld", (long)(UnifDefValueF4));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(UnifDefValueF4));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "ShaderRes", "UnifDefValueF4", buf, "608");
    return -1;
  }
  return _cffi_e_enum_ResTypes(lib);
}

static int _cffi_e_enum_Stats(PyObject *lib)
{
  if ((TriCount) < 0 || (unsigned long)(TriCount) != 100UL) {
    char buf[64];
    if ((TriCount) < 0)
        snprintf(buf, 63, "%ld", (long)(TriCount));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TriCount));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "TriCount", buf, "100");
    return -1;
  }
  if ((BatchCount) < 0 || (unsigned long)(BatchCount) != 101UL) {
    char buf[64];
    if ((BatchCount) < 0)
        snprintf(buf, 63, "%ld", (long)(BatchCount));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(BatchCount));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "BatchCount", buf, "101");
    return -1;
  }
  if ((LightPassCount) < 0 || (unsigned long)(LightPassCount) != 102UL) {
    char buf[64];
    if ((LightPassCount) < 0)
        snprintf(buf, 63, "%ld", (long)(LightPassCount));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(LightPassCount));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "LightPassCount", buf, "102");
    return -1;
  }
  if ((FrameTime) < 0 || (unsigned long)(FrameTime) != 103UL) {
    char buf[64];
    if ((FrameTime) < 0)
        snprintf(buf, 63, "%ld", (long)(FrameTime));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(FrameTime));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "FrameTime", buf, "103");
    return -1;
  }
  if ((AnimationTime) < 0 || (unsigned long)(AnimationTime) != 104UL) {
    char buf[64];
    if ((AnimationTime) < 0)
        snprintf(buf, 63, "%ld", (long)(AnimationTime));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(AnimationTime));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "AnimationTime", buf, "104");
    return -1;
  }
  if ((GeoUpdateTime) < 0 || (unsigned long)(GeoUpdateTime) != 105UL) {
    char buf[64];
    if ((GeoUpdateTime) < 0)
        snprintf(buf, 63, "%ld", (long)(GeoUpdateTime));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeoUpdateTime));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "GeoUpdateTime", buf, "105");
    return -1;
  }
  if ((ParticleSimTime) < 0 || (unsigned long)(ParticleSimTime) != 106UL) {
    char buf[64];
    if ((ParticleSimTime) < 0)
        snprintf(buf, 63, "%ld", (long)(ParticleSimTime));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ParticleSimTime));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "ParticleSimTime", buf, "106");
    return -1;
  }
  if ((FwdLightsGPUTime) < 0 || (unsigned long)(FwdLightsGPUTime) != 107UL) {
    char buf[64];
    if ((FwdLightsGPUTime) < 0)
        snprintf(buf, 63, "%ld", (long)(FwdLightsGPUTime));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(FwdLightsGPUTime));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "FwdLightsGPUTime", buf, "107");
    return -1;
  }
  if ((DefLightsGPUTime) < 0 || (unsigned long)(DefLightsGPUTime) != 108UL) {
    char buf[64];
    if ((DefLightsGPUTime) < 0)
        snprintf(buf, 63, "%ld", (long)(DefLightsGPUTime));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(DefLightsGPUTime));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "DefLightsGPUTime", buf, "108");
    return -1;
  }
  if ((ShadowsGPUTime) < 0 || (unsigned long)(ShadowsGPUTime) != 109UL) {
    char buf[64];
    if ((ShadowsGPUTime) < 0)
        snprintf(buf, 63, "%ld", (long)(ShadowsGPUTime));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ShadowsGPUTime));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "ShadowsGPUTime", buf, "109");
    return -1;
  }
  if ((ParticleGPUTime) < 0 || (unsigned long)(ParticleGPUTime) != 110UL) {
    char buf[64];
    if ((ParticleGPUTime) < 0)
        snprintf(buf, 63, "%ld", (long)(ParticleGPUTime));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ParticleGPUTime));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "ParticleGPUTime", buf, "110");
    return -1;
  }
  if ((TextureVMem) < 0 || (unsigned long)(TextureVMem) != 111UL) {
    char buf[64];
    if ((TextureVMem) < 0)
        snprintf(buf, 63, "%ld", (long)(TextureVMem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TextureVMem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "TextureVMem", buf, "111");
    return -1;
  }
  if ((GeometryVMem) < 0 || (unsigned long)(GeometryVMem) != 112UL) {
    char buf[64];
    if ((GeometryVMem) < 0)
        snprintf(buf, 63, "%ld", (long)(GeometryVMem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(GeometryVMem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "Stats", "GeometryVMem", buf, "112");
    return -1;
  }
  return _cffi_e_enum_ShaderRes(lib);
}

static int _cffi_e_enum_TexRes(PyObject *lib)
{
  if ((TextureElem) < 0 || (unsigned long)(TextureElem) != 700UL) {
    char buf[64];
    if ((TextureElem) < 0)
        snprintf(buf, 63, "%ld", (long)(TextureElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TextureElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "TexRes", "TextureElem", buf, "700");
    return -1;
  }
  if ((ImageElem) < 0 || (unsigned long)(ImageElem) != 701UL) {
    char buf[64];
    if ((ImageElem) < 0)
        snprintf(buf, 63, "%ld", (long)(ImageElem));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ImageElem));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "TexRes", "ImageElem", buf, "701");
    return -1;
  }
  if ((TexFormatI) < 0 || (unsigned long)(TexFormatI) != 702UL) {
    char buf[64];
    if ((TexFormatI) < 0)
        snprintf(buf, 63, "%ld", (long)(TexFormatI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TexFormatI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "TexRes", "TexFormatI", buf, "702");
    return -1;
  }
  if ((TexSliceCountI) < 0 || (unsigned long)(TexSliceCountI) != 703UL) {
    char buf[64];
    if ((TexSliceCountI) < 0)
        snprintf(buf, 63, "%ld", (long)(TexSliceCountI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(TexSliceCountI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "TexRes", "TexSliceCountI", buf, "703");
    return -1;
  }
  if ((ImgWidthI) < 0 || (unsigned long)(ImgWidthI) != 704UL) {
    char buf[64];
    if ((ImgWidthI) < 0)
        snprintf(buf, 63, "%ld", (long)(ImgWidthI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ImgWidthI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "TexRes", "ImgWidthI", buf, "704");
    return -1;
  }
  if ((ImgHeightI) < 0 || (unsigned long)(ImgHeightI) != 705UL) {
    char buf[64];
    if ((ImgHeightI) < 0)
        snprintf(buf, 63, "%ld", (long)(ImgHeightI));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ImgHeightI));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "TexRes", "ImgHeightI", buf, "705");
    return -1;
  }
  if ((ImgPixelStream) < 0 || (unsigned long)(ImgPixelStream) != 706UL) {
    char buf[64];
    if ((ImgPixelStream) < 0)
        snprintf(buf, 63, "%ld", (long)(ImgPixelStream));
    else
        snprintf(buf, 63, "%lu", (unsigned long)(ImgPixelStream));
    PyErr_Format(_cffi_VerificationError,
                 "enum %s: %s has the real value %s, not %s",
                 "TexRes", "ImgPixelStream", buf, "706");
    return -1;
  }
  return _cffi_e_enum_Stats(lib);
}

static int _cffi_setup_custom(PyObject *lib)
{
  return _cffi_e_enum_TexRes(lib);
}

static PyMethodDef _cffi_methods[] = {
  {"_cffi_setup", _cffi_setup, METH_VARARGS},
  {NULL, NULL}    /* Sentinel */
};

PyMODINIT_FUNC
init_cffi__xb0dd5984x51d5a3d8(void)
{
  PyObject *lib;
  lib = Py_InitModule("_cffi__xb0dd5984x51d5a3d8", _cffi_methods);
  if (lib == NULL || 0 < 0)
    return;
  _cffi_init();
  return;
}
