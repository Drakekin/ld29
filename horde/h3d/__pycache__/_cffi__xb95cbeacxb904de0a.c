
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
  return 0;
}

static int _cffi_setup_custom(PyObject *lib)
{
  return _cffi_e_enum_Light(lib);
}

static PyMethodDef _cffi_methods[] = {
  {"_cffi_setup", _cffi_setup, METH_VARARGS},
  {NULL, NULL}    /* Sentinel */
};

PyMODINIT_FUNC
init_cffi__xb95cbeacxb904de0a(void)
{
  PyObject *lib;
  lib = Py_InitModule("_cffi__xb95cbeacxb904de0a", _cffi_methods);
  if (lib == NULL || 0 < 0)
    return;
  _cffi_init();
  return;
}
