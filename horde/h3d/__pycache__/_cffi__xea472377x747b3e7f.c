
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
  return 0;
}

static int _cffi_setup_custom(PyObject *lib)
{
  return _cffi_e_enum_ResTypes(lib);
}

static PyMethodDef _cffi_methods[] = {
  {"_cffi_setup", _cffi_setup, METH_VARARGS},
  {NULL, NULL}    /* Sentinel */
};

PyMODINIT_FUNC
init_cffi__xea472377x747b3e7f(void)
{
  PyObject *lib;
  lib = Py_InitModule("_cffi__xea472377x747b3e7f", _cffi_methods);
  if (lib == NULL || 0 < 0)
    return;
  _cffi_init();
  return;
}
