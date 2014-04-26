
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

#define _cffi_from_c_SIGNED(x, type)                                     \
    (sizeof(type) <= sizeof(long) ? PyInt_FromLong(x) :                  \
                                    PyLong_FromLongLong(x))
#define _cffi_from_c_UNSIGNED(x, type)                                   \
    (sizeof(type) < sizeof(long) ? PyInt_FromLong(x) :                   \
     sizeof(type) == sizeof(long) ? PyLong_FromUnsignedLong(x) :         \
                                    PyLong_FromUnsignedLongLong(x))

#define _cffi_to_c_SIGNED(o, type)                                       \
    (sizeof(type) == 1 ? _cffi_to_c_i8(o) :                              \
     sizeof(type) == 2 ? _cffi_to_c_i16(o) :                             \
     sizeof(type) == 4 ? _cffi_to_c_i32(o) :                             \
     sizeof(type) == 8 ? _cffi_to_c_i64(o) :                             \
     (Py_FatalError("unsupported size for type " #type), 0))
#define _cffi_to_c_UNSIGNED(o, type)                                     \
    (sizeof(type) == 1 ? _cffi_to_c_u8(o) :                              \
     sizeof(type) == 2 ? _cffi_to_c_u16(o) :                             \
     sizeof(type) == 4 ? _cffi_to_c_u32(o) :                             \
     sizeof(type) == 8 ? _cffi_to_c_u64(o) :                             \
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
        PyErr_SetNone(PyExc_ImportError);
        return;
    }
    memcpy(_cffi_exports, PyCapsule_GetPointer(c_api_object, "cffi"),
           _CFFI_NUM_EXPORTS * sizeof(void *));
}

#define _cffi_type(num) ((CTypeDescrObject *)PyList_GET_ITEM(_cffi_types, num))

/**********/


enum NodeParams {
        NameStr = 1,
        AttachmentStr
    };

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
  return 0;
}

static PyObject *
_cffi_f_h3dAddCameraNode(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dAddCameraNode", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dAddCameraNode(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dAddEmitterNode(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  int x2;
  int x3;
  int x4;
  int x5;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;

  if (!PyArg_ParseTuple(args, "OOOOOO:h3dAddEmitterNode", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_SIGNED(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dAddEmitterNode(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dAddGroupNode(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dAddGroupNode", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dAddGroupNode(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dAddJointNode(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dAddJointNode", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dAddJointNode(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dAddLightNode(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  int x2;
  char const * x3;
  char const * x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:h3dAddLightNode", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(0), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(0), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dAddLightNode(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dAddMeshNode(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  int x2;
  int x3;
  int x4;
  int x5;
  int x6;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:h3dAddMeshNode", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_SIGNED(arg5, int);
  if (x5 == (int)-1 && PyErr_Occurred())
    return NULL;

  x6 = _cffi_to_c_SIGNED(arg6, int);
  if (x6 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dAddMeshNode(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dAddModelNode(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dAddModelNode", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dAddModelNode(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dAddNodes(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dAddNodes", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dAddNodes(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dAddResource(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dAddResource", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dAddResource(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dCastRay(PyObject *self, PyObject *args)
{
  int x0;
  float x1;
  float x2;
  float x3;
  float x4;
  float x5;
  float x6;
  int x7;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;
  PyObject *arg7;

  if (!PyArg_ParseTuple(args, "OOOOOOOO:h3dCastRay", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_float(arg1);
  if (x1 == (float)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_float(arg2);
  if (x2 == (float)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_float(arg3);
  if (x3 == (float)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_float(arg4);
  if (x4 == (float)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_float(arg5);
  if (x5 == (float)-1 && PyErr_Occurred())
    return NULL;

  x6 = _cffi_to_c_float(arg6);
  if (x6 == (float)-1 && PyErr_Occurred())
    return NULL;

  x7 = _cffi_to_c_SIGNED(arg7, int);
  if (x7 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dCastRay(x0, x1, x2, x3, x4, x5, x6, x7); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dCheckExtension(PyObject *self, PyObject *arg0)
{
  char const * x0;
  Py_ssize_t datasize;
  _Bool result;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dCheckExtension(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dCheckNodeTransFlag(PyObject *self, PyObject *args)
{
  int x0;
  _Bool x1;
  _Bool result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dCheckNodeTransFlag", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c__Bool(arg1);
  if (x1 == (_Bool)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dCheckNodeTransFlag(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dCheckNodeVisibility(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  _Bool x2;
  _Bool x3;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:h3dCheckNodeVisibility", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c__Bool(arg2);
  if (x2 == (_Bool)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c__Bool(arg3);
  if (x3 == (_Bool)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dCheckNodeVisibility(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dClear(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dClear(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dClearOverlays(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dClearOverlays(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dCloneResource(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dCloneResource", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dCloneResource(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dCreateTexture(PyObject *self, PyObject *args)
{
  char const * x0;
  int x1;
  int x2;
  int x3;
  int x4;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:h3dCreateTexture", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dCreateTexture(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dFinalizeFrame(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dFinalizeFrame(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dFindNodes(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  int x2;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dFindNodes", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dFindNodes(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dFindResElem(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  char const * x3;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:h3dFindResElem", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(0), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dFindResElem(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dFindResource(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  Py_ssize_t datasize;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dFindResource", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dFindResource(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetCameraProjMat(PyObject *self, PyObject *args)
{
  int x0;
  float * x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dGetCameraProjMat", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(2), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dGetCameraProjMat(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dGetCastRayResult(PyObject *self, PyObject *args)
{
  int x0;
  int * x1;
  float * x2;
  float * x3;
  Py_ssize_t datasize;
  _Bool result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:h3dGetCastRayResult", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(3), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(2), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(2), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetCastRayResult(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dGetError(PyObject *self, PyObject *no_arg)
{
  _Bool result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetError(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dGetMessage(PyObject *self, PyObject *args)
{
  int * x0;
  float * x1;
  Py_ssize_t datasize;
  char const * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dGetMessage", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(3), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(2), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetMessage(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_h3dGetModelAnimParams(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  float * x2;
  float * x3;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:h3dGetModelAnimParams", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(2), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(2), arg3) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dGetModelAnimParams(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dGetNextResource(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dGetNextResource", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetNextResource(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetNodeAABB(PyObject *self, PyObject *args)
{
  int x0;
  float * x1;
  float * x2;
  float * x3;
  float * x4;
  float * x5;
  float * x6;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;

  if (!PyArg_ParseTuple(args, "OOOOOOO:h3dGetNodeAABB", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(2), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(2), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(2), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(2), arg4) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg5, (char **)&x5);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x5 = alloca(datasize);
    memset((void *)x5, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x5, _cffi_type(2), arg5) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(2), arg6) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dGetNodeAABB(x0, x1, x2, x3, x4, x5, x6); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dGetNodeChild(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dGetNodeChild", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetNodeChild(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetNodeFindResult(PyObject *self, PyObject *arg0)
{
  int x0;
  int result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetNodeFindResult(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetNodeFlags(PyObject *self, PyObject *arg0)
{
  int x0;
  int result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetNodeFlags(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetNodeParamF(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  float result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dGetNodeParamF", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetNodeParamF(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_float(result);
}

static PyObject *
_cffi_f_h3dGetNodeParamI(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dGetNodeParamI", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetNodeParamI(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetNodeParamStr(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  char const * result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dGetNodeParamStr", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetNodeParamStr(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_h3dGetNodeParent(PyObject *self, PyObject *arg0)
{
  int x0;
  int result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetNodeParent(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetNodeTransMats(PyObject *self, PyObject *args)
{
  int x0;
  float const * * x1;
  float const * * x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dGetNodeTransMats", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(4), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(4), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(4), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(4), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dGetNodeTransMats(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dGetNodeTransform(PyObject *self, PyObject *args)
{
  int x0;
  float * x1;
  float * x2;
  float * x3;
  float * x4;
  float * x5;
  float * x6;
  float * x7;
  float * x8;
  float * x9;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;
  PyObject *arg7;
  PyObject *arg8;
  PyObject *arg9;

  if (!PyArg_ParseTuple(args, "OOOOOOOOOO:h3dGetNodeTransform", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(2), arg1) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(2), arg2) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(2), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(2), arg4) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg5, (char **)&x5);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x5 = alloca(datasize);
    memset((void *)x5, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x5, _cffi_type(2), arg5) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(2), arg6) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg7, (char **)&x7);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x7 = alloca(datasize);
    memset((void *)x7, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x7, _cffi_type(2), arg7) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg8, (char **)&x8);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x8 = alloca(datasize);
    memset((void *)x8, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x8, _cffi_type(2), arg8) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg9, (char **)&x9);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x9 = alloca(datasize);
    memset((void *)x9, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x9, _cffi_type(2), arg9) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dGetNodeTransform(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dGetNodeType(PyObject *self, PyObject *arg0)
{
  int x0;
  int result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetNodeType(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetOption(PyObject *self, PyObject *arg0)
{
  int x0;
  float result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetOption(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_float(result);
}

static PyObject *
_cffi_f_h3dGetRenderTargetData(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  int x2;
  int * x3;
  int * x4;
  int * x5;
  void * x6;
  int x7;
  Py_ssize_t datasize;
  _Bool result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;
  PyObject *arg7;

  if (!PyArg_ParseTuple(args, "OOOOOOOO:h3dGetRenderTargetData", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg3, (char **)&x3);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x3 = alloca(datasize);
    memset((void *)x3, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x3, _cffi_type(3), arg3) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(3), arg4) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(3), arg5, (char **)&x5);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x5 = alloca(datasize);
    memset((void *)x5, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x5, _cffi_type(3), arg5) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(5), arg6, (char **)&x6);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x6 = alloca(datasize);
    memset((void *)x6, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x6, _cffi_type(5), arg6) < 0)
      return NULL;
  }

  x7 = _cffi_to_c_SIGNED(arg7, int);
  if (x7 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetRenderTargetData(x0, x1, x2, x3, x4, x5, x6, x7); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dGetResElemCount(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dGetResElemCount", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetResElemCount(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetResName(PyObject *self, PyObject *arg0)
{
  int x0;
  char const * result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetResName(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_h3dGetResParamF(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  int x3;
  int x4;
  float result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:h3dGetResParamF", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetResParamF(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_float(result);
}

static PyObject *
_cffi_f_h3dGetResParamI(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  int x3;
  int result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:h3dGetResParamI", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetResParamI(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetResParamStr(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  int x3;
  char const * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:h3dGetResParamStr", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetResParamStr(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_h3dGetResType(PyObject *self, PyObject *arg0)
{
  int x0;
  int result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetResType(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dGetStat(PyObject *self, PyObject *args)
{
  int x0;
  _Bool x1;
  float result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dGetStat", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c__Bool(arg1);
  if (x1 == (_Bool)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetStat(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_float(result);
}

static PyObject *
_cffi_f_h3dGetVersionString(PyObject *self, PyObject *no_arg)
{
  char const * result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dGetVersionString(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(0));
}

static PyObject *
_cffi_f_h3dHasEmitterFinished(PyObject *self, PyObject *arg0)
{
  int x0;
  _Bool result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dHasEmitterFinished(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dInit(PyObject *self, PyObject *no_arg)
{
  _Bool result;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dInit(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dIsResLoaded(PyObject *self, PyObject *arg0)
{
  int x0;
  _Bool result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dIsResLoaded(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dLoadResource(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  int x2;
  Py_ssize_t datasize;
  _Bool result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dLoadResource", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dLoadResource(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dMapResStream(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  int x3;
  _Bool x4;
  _Bool x5;
  void * result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;

  if (!PyArg_ParseTuple(args, "OOOOOO:h3dMapResStream", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c__Bool(arg4);
  if (x4 == (_Bool)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c__Bool(arg5);
  if (x5 == (_Bool)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dMapResStream(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_pointer((char *)result, _cffi_type(5));
}

static PyObject *
_cffi_f_h3dQueryUnloadedResource(PyObject *self, PyObject *arg0)
{
  int x0;
  int result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dQueryUnloadedResource(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dRelease(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dRelease(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dReleaseUnusedResources(PyObject *self, PyObject *no_arg)
{

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dReleaseUnusedResources(); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dRemoveNode(PyObject *self, PyObject *arg0)
{
  int x0;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dRemoveNode(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dRemoveResource(PyObject *self, PyObject *arg0)
{
  int x0;
  int result;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dRemoveResource(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_SIGNED(result, int);
}

static PyObject *
_cffi_f_h3dRender(PyObject *self, PyObject *arg0)
{
  int x0;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dRender(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dResizePipelineBuffers(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dResizePipelineBuffers", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dResizePipelineBuffers(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetMaterialUniform(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  float x2;
  float x3;
  float x4;
  float x5;
  Py_ssize_t datasize;
  _Bool result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;

  if (!PyArg_ParseTuple(args, "OOOOOO:h3dSetMaterialUniform", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_float(arg2);
  if (x2 == (float)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_float(arg3);
  if (x3 == (float)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_float(arg4);
  if (x4 == (float)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_float(arg5);
  if (x5 == (float)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dSetMaterialUniform(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dSetModelAnimParams(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  float x2;
  float x3;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:h3dSetModelAnimParams", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_float(arg2);
  if (x2 == (float)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_float(arg3);
  if (x3 == (float)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetModelAnimParams(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetModelMorpher(PyObject *self, PyObject *args)
{
  int x0;
  char const * x1;
  float x2;
  Py_ssize_t datasize;
  _Bool result;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dSetModelMorpher", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_float(arg2);
  if (x2 == (float)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dSetModelMorpher(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dSetNodeFlags(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  _Bool x2;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dSetNodeFlags", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c__Bool(arg2);
  if (x2 == (_Bool)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetNodeFlags(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetNodeParamF(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  float x3;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;

  if (!PyArg_ParseTuple(args, "OOOO:h3dSetNodeParamF", &arg0, &arg1, &arg2, &arg3))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_float(arg3);
  if (x3 == (float)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetNodeParamF(x0, x1, x2, x3); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetNodeParamI(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dSetNodeParamI", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetNodeParamI(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetNodeParamStr(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  char const * x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dSetNodeParamStr", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg2, (char **)&x2);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x2 = alloca(datasize);
    memset((void *)x2, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x2, _cffi_type(0), arg2) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetNodeParamStr(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetNodeParent(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  _Bool result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dSetNodeParent", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dSetNodeParent(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dSetNodeTransMat(PyObject *self, PyObject *args)
{
  int x0;
  float const * x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dSetNodeTransMat", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(6), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(6), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetNodeTransMat(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetNodeTransform(PyObject *self, PyObject *args)
{
  int x0;
  float x1;
  float x2;
  float x3;
  float x4;
  float x5;
  float x6;
  float x7;
  float x8;
  float x9;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;
  PyObject *arg7;
  PyObject *arg8;
  PyObject *arg9;

  if (!PyArg_ParseTuple(args, "OOOOOOOOOO:h3dSetNodeTransform", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_float(arg1);
  if (x1 == (float)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_float(arg2);
  if (x2 == (float)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_float(arg3);
  if (x3 == (float)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_float(arg4);
  if (x4 == (float)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_float(arg5);
  if (x5 == (float)-1 && PyErr_Occurred())
    return NULL;

  x6 = _cffi_to_c_float(arg6);
  if (x6 == (float)-1 && PyErr_Occurred())
    return NULL;

  x7 = _cffi_to_c_float(arg7);
  if (x7 == (float)-1 && PyErr_Occurred())
    return NULL;

  x8 = _cffi_to_c_float(arg8);
  if (x8 == (float)-1 && PyErr_Occurred())
    return NULL;

  x9 = _cffi_to_c_float(arg9);
  if (x9 == (float)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetNodeTransform(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetNodeUniforms(PyObject *self, PyObject *args)
{
  int x0;
  float * x1;
  int x2;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;

  if (!PyArg_ParseTuple(args, "OOO:h3dSetNodeUniforms", &arg0, &arg1, &arg2))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(2), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(2), arg1) < 0)
      return NULL;
  }

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetNodeUniforms(x0, x1, x2); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetOption(PyObject *self, PyObject *args)
{
  int x0;
  float x1;
  _Bool result;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dSetOption", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_float(arg1);
  if (x1 == (float)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { result = h3dSetOption(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  return _cffi_from_c_UNSIGNED(result, _Bool);
}

static PyObject *
_cffi_f_h3dSetResParamF(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  int x3;
  int x4;
  float x5;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;

  if (!PyArg_ParseTuple(args, "OOOOOO:h3dSetResParamF", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_float(arg5);
  if (x5 == (float)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetResParamF(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetResParamI(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  int x3;
  int x4;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:h3dSetResParamI", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_SIGNED(arg4, int);
  if (x4 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetResParamI(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetResParamStr(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  int x3;
  char const * x4;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:h3dSetResParamStr", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(0), arg4) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetResParamStr(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetShaderPreambles(PyObject *self, PyObject *args)
{
  char const * x0;
  char const * x1;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dSetShaderPreambles", &arg0, &arg1))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(0), arg0) < 0)
      return NULL;
  }

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg1, (char **)&x1);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x1 = alloca(datasize);
    memset((void *)x1, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x1, _cffi_type(0), arg1) < 0)
      return NULL;
  }

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetShaderPreambles(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetupCameraView(PyObject *self, PyObject *args)
{
  int x0;
  float x1;
  float x2;
  float x3;
  float x4;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;

  if (!PyArg_ParseTuple(args, "OOOOO:h3dSetupCameraView", &arg0, &arg1, &arg2, &arg3, &arg4))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_float(arg1);
  if (x1 == (float)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_float(arg2);
  if (x2 == (float)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_float(arg3);
  if (x3 == (float)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_float(arg4);
  if (x4 == (float)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetupCameraView(x0, x1, x2, x3, x4); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dSetupModelAnimStage(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  int x2;
  int x3;
  char const * x4;
  _Bool x5;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;

  if (!PyArg_ParseTuple(args, "OOOOOO:h3dSetupModelAnimStage", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_SIGNED(arg2, int);
  if (x2 == (int)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_SIGNED(arg3, int);
  if (x3 == (int)-1 && PyErr_Occurred())
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(0), arg4, (char **)&x4);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x4 = alloca(datasize);
    memset((void *)x4, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x4, _cffi_type(0), arg4) < 0)
      return NULL;
  }

  x5 = _cffi_to_c__Bool(arg5);
  if (x5 == (_Bool)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dSetupModelAnimStage(x0, x1, x2, x3, x4, x5); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dShowOverlays(PyObject *self, PyObject *args)
{
  float const * x0;
  int x1;
  float x2;
  float x3;
  float x4;
  float x5;
  int x6;
  int x7;
  Py_ssize_t datasize;
  PyObject *arg0;
  PyObject *arg1;
  PyObject *arg2;
  PyObject *arg3;
  PyObject *arg4;
  PyObject *arg5;
  PyObject *arg6;
  PyObject *arg7;

  if (!PyArg_ParseTuple(args, "OOOOOOOO:h3dShowOverlays", &arg0, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7))
    return NULL;

  datasize = _cffi_prepare_pointer_call_argument(
      _cffi_type(6), arg0, (char **)&x0);
  if (datasize != 0) {
    if (datasize < 0)
      return NULL;
    x0 = alloca(datasize);
    memset((void *)x0, 0, datasize);
    if (_cffi_convert_array_from_object((char *)x0, _cffi_type(6), arg0) < 0)
      return NULL;
  }

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  x2 = _cffi_to_c_float(arg2);
  if (x2 == (float)-1 && PyErr_Occurred())
    return NULL;

  x3 = _cffi_to_c_float(arg3);
  if (x3 == (float)-1 && PyErr_Occurred())
    return NULL;

  x4 = _cffi_to_c_float(arg4);
  if (x4 == (float)-1 && PyErr_Occurred())
    return NULL;

  x5 = _cffi_to_c_float(arg5);
  if (x5 == (float)-1 && PyErr_Occurred())
    return NULL;

  x6 = _cffi_to_c_SIGNED(arg6, int);
  if (x6 == (int)-1 && PyErr_Occurred())
    return NULL;

  x7 = _cffi_to_c_SIGNED(arg7, int);
  if (x7 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dShowOverlays(x0, x1, x2, x3, x4, x5, x6, x7); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dUnloadResource(PyObject *self, PyObject *arg0)
{
  int x0;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dUnloadResource(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dUnmapResStream(PyObject *self, PyObject *arg0)
{
  int x0;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dUnmapResStream(x0); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dUpdateEmitter(PyObject *self, PyObject *args)
{
  int x0;
  float x1;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dUpdateEmitter", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_float(arg1);
  if (x1 == (float)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dUpdateEmitter(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
_cffi_f_h3dUpdateModel(PyObject *self, PyObject *args)
{
  int x0;
  int x1;
  PyObject *arg0;
  PyObject *arg1;

  if (!PyArg_ParseTuple(args, "OO:h3dUpdateModel", &arg0, &arg1))
    return NULL;

  x0 = _cffi_to_c_SIGNED(arg0, int);
  if (x0 == (int)-1 && PyErr_Occurred())
    return NULL;

  x1 = _cffi_to_c_SIGNED(arg1, int);
  if (x1 == (int)-1 && PyErr_Occurred())
    return NULL;

  Py_BEGIN_ALLOW_THREADS
  _cffi_restore_errno();
  { h3dUpdateModel(x0, x1); }
  _cffi_save_errno();
  Py_END_ALLOW_THREADS

  Py_INCREF(Py_None);
  return Py_None;
}

static int _cffi_setup_custom(PyObject *lib)
{
  return _cffi_e_enum_NodeParams(lib);
}

static PyMethodDef _cffi_methods[] = {
  {"h3dAddCameraNode", _cffi_f_h3dAddCameraNode, METH_VARARGS},
  {"h3dAddEmitterNode", _cffi_f_h3dAddEmitterNode, METH_VARARGS},
  {"h3dAddGroupNode", _cffi_f_h3dAddGroupNode, METH_VARARGS},
  {"h3dAddJointNode", _cffi_f_h3dAddJointNode, METH_VARARGS},
  {"h3dAddLightNode", _cffi_f_h3dAddLightNode, METH_VARARGS},
  {"h3dAddMeshNode", _cffi_f_h3dAddMeshNode, METH_VARARGS},
  {"h3dAddModelNode", _cffi_f_h3dAddModelNode, METH_VARARGS},
  {"h3dAddNodes", _cffi_f_h3dAddNodes, METH_VARARGS},
  {"h3dAddResource", _cffi_f_h3dAddResource, METH_VARARGS},
  {"h3dCastRay", _cffi_f_h3dCastRay, METH_VARARGS},
  {"h3dCheckExtension", _cffi_f_h3dCheckExtension, METH_O},
  {"h3dCheckNodeTransFlag", _cffi_f_h3dCheckNodeTransFlag, METH_VARARGS},
  {"h3dCheckNodeVisibility", _cffi_f_h3dCheckNodeVisibility, METH_VARARGS},
  {"h3dClear", _cffi_f_h3dClear, METH_NOARGS},
  {"h3dClearOverlays", _cffi_f_h3dClearOverlays, METH_NOARGS},
  {"h3dCloneResource", _cffi_f_h3dCloneResource, METH_VARARGS},
  {"h3dCreateTexture", _cffi_f_h3dCreateTexture, METH_VARARGS},
  {"h3dFinalizeFrame", _cffi_f_h3dFinalizeFrame, METH_NOARGS},
  {"h3dFindNodes", _cffi_f_h3dFindNodes, METH_VARARGS},
  {"h3dFindResElem", _cffi_f_h3dFindResElem, METH_VARARGS},
  {"h3dFindResource", _cffi_f_h3dFindResource, METH_VARARGS},
  {"h3dGetCameraProjMat", _cffi_f_h3dGetCameraProjMat, METH_VARARGS},
  {"h3dGetCastRayResult", _cffi_f_h3dGetCastRayResult, METH_VARARGS},
  {"h3dGetError", _cffi_f_h3dGetError, METH_NOARGS},
  {"h3dGetMessage", _cffi_f_h3dGetMessage, METH_VARARGS},
  {"h3dGetModelAnimParams", _cffi_f_h3dGetModelAnimParams, METH_VARARGS},
  {"h3dGetNextResource", _cffi_f_h3dGetNextResource, METH_VARARGS},
  {"h3dGetNodeAABB", _cffi_f_h3dGetNodeAABB, METH_VARARGS},
  {"h3dGetNodeChild", _cffi_f_h3dGetNodeChild, METH_VARARGS},
  {"h3dGetNodeFindResult", _cffi_f_h3dGetNodeFindResult, METH_O},
  {"h3dGetNodeFlags", _cffi_f_h3dGetNodeFlags, METH_O},
  {"h3dGetNodeParamF", _cffi_f_h3dGetNodeParamF, METH_VARARGS},
  {"h3dGetNodeParamI", _cffi_f_h3dGetNodeParamI, METH_VARARGS},
  {"h3dGetNodeParamStr", _cffi_f_h3dGetNodeParamStr, METH_VARARGS},
  {"h3dGetNodeParent", _cffi_f_h3dGetNodeParent, METH_O},
  {"h3dGetNodeTransMats", _cffi_f_h3dGetNodeTransMats, METH_VARARGS},
  {"h3dGetNodeTransform", _cffi_f_h3dGetNodeTransform, METH_VARARGS},
  {"h3dGetNodeType", _cffi_f_h3dGetNodeType, METH_O},
  {"h3dGetOption", _cffi_f_h3dGetOption, METH_O},
  {"h3dGetRenderTargetData", _cffi_f_h3dGetRenderTargetData, METH_VARARGS},
  {"h3dGetResElemCount", _cffi_f_h3dGetResElemCount, METH_VARARGS},
  {"h3dGetResName", _cffi_f_h3dGetResName, METH_O},
  {"h3dGetResParamF", _cffi_f_h3dGetResParamF, METH_VARARGS},
  {"h3dGetResParamI", _cffi_f_h3dGetResParamI, METH_VARARGS},
  {"h3dGetResParamStr", _cffi_f_h3dGetResParamStr, METH_VARARGS},
  {"h3dGetResType", _cffi_f_h3dGetResType, METH_O},
  {"h3dGetStat", _cffi_f_h3dGetStat, METH_VARARGS},
  {"h3dGetVersionString", _cffi_f_h3dGetVersionString, METH_NOARGS},
  {"h3dHasEmitterFinished", _cffi_f_h3dHasEmitterFinished, METH_O},
  {"h3dInit", _cffi_f_h3dInit, METH_NOARGS},
  {"h3dIsResLoaded", _cffi_f_h3dIsResLoaded, METH_O},
  {"h3dLoadResource", _cffi_f_h3dLoadResource, METH_VARARGS},
  {"h3dMapResStream", _cffi_f_h3dMapResStream, METH_VARARGS},
  {"h3dQueryUnloadedResource", _cffi_f_h3dQueryUnloadedResource, METH_O},
  {"h3dRelease", _cffi_f_h3dRelease, METH_NOARGS},
  {"h3dReleaseUnusedResources", _cffi_f_h3dReleaseUnusedResources, METH_NOARGS},
  {"h3dRemoveNode", _cffi_f_h3dRemoveNode, METH_O},
  {"h3dRemoveResource", _cffi_f_h3dRemoveResource, METH_O},
  {"h3dRender", _cffi_f_h3dRender, METH_O},
  {"h3dResizePipelineBuffers", _cffi_f_h3dResizePipelineBuffers, METH_VARARGS},
  {"h3dSetMaterialUniform", _cffi_f_h3dSetMaterialUniform, METH_VARARGS},
  {"h3dSetModelAnimParams", _cffi_f_h3dSetModelAnimParams, METH_VARARGS},
  {"h3dSetModelMorpher", _cffi_f_h3dSetModelMorpher, METH_VARARGS},
  {"h3dSetNodeFlags", _cffi_f_h3dSetNodeFlags, METH_VARARGS},
  {"h3dSetNodeParamF", _cffi_f_h3dSetNodeParamF, METH_VARARGS},
  {"h3dSetNodeParamI", _cffi_f_h3dSetNodeParamI, METH_VARARGS},
  {"h3dSetNodeParamStr", _cffi_f_h3dSetNodeParamStr, METH_VARARGS},
  {"h3dSetNodeParent", _cffi_f_h3dSetNodeParent, METH_VARARGS},
  {"h3dSetNodeTransMat", _cffi_f_h3dSetNodeTransMat, METH_VARARGS},
  {"h3dSetNodeTransform", _cffi_f_h3dSetNodeTransform, METH_VARARGS},
  {"h3dSetNodeUniforms", _cffi_f_h3dSetNodeUniforms, METH_VARARGS},
  {"h3dSetOption", _cffi_f_h3dSetOption, METH_VARARGS},
  {"h3dSetResParamF", _cffi_f_h3dSetResParamF, METH_VARARGS},
  {"h3dSetResParamI", _cffi_f_h3dSetResParamI, METH_VARARGS},
  {"h3dSetResParamStr", _cffi_f_h3dSetResParamStr, METH_VARARGS},
  {"h3dSetShaderPreambles", _cffi_f_h3dSetShaderPreambles, METH_VARARGS},
  {"h3dSetupCameraView", _cffi_f_h3dSetupCameraView, METH_VARARGS},
  {"h3dSetupModelAnimStage", _cffi_f_h3dSetupModelAnimStage, METH_VARARGS},
  {"h3dShowOverlays", _cffi_f_h3dShowOverlays, METH_VARARGS},
  {"h3dUnloadResource", _cffi_f_h3dUnloadResource, METH_O},
  {"h3dUnmapResStream", _cffi_f_h3dUnmapResStream, METH_O},
  {"h3dUpdateEmitter", _cffi_f_h3dUpdateEmitter, METH_VARARGS},
  {"h3dUpdateModel", _cffi_f_h3dUpdateModel, METH_VARARGS},
  {"_cffi_setup", _cffi_setup, METH_VARARGS},
  {NULL, NULL}    /* Sentinel */
};

PyMODINIT_FUNC
init_cffi__x6269a330xf326dda8(void)
{
  PyObject *lib;
  lib = Py_InitModule("_cffi__x6269a330xf326dda8", _cffi_methods);
  if (lib == NULL || 0 < 0)
    return;
  _cffi_init();
  return;
}
