/**
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @date Tue  1 Oct 13:52:57 2013
 *
 * @brief Implements some constructions exported to all modules
 */

#define BLITZ_ARRAY_MODULE
#include <blitz.array/cppapi.h>

/**
 * Correct for integer types
 */
static int fix_integer_type_num_inner(bool sign, int nbits) {
  switch (nbits) {
    case 8:
      return sign ? NPY_INT8 : NPY_UINT8;
    case 16:
      return sign ? NPY_INT16 : NPY_UINT16;
    case 32:
      return sign ? NPY_INT32 : NPY_UINT32;
    case 64:
      return sign ? NPY_INT64 : NPY_UINT64;
    default:
      break;
  }

  PyErr_Format(PyExc_NotImplementedError, "no support for integer type with %d bits", nbits);
  return -1;
}

/**
 * Correct for type numbers which are not necessarily bound to one of the
 * supported type numbers.
 */
static int fix_integer_type_num(int type_num) {

  switch(type_num) {
    case NPY_BYTE:
      return fix_integer_type_num_inner(true, NPY_BITSOF_CHAR);
    case NPY_SHORT:
      return fix_integer_type_num_inner(true, NPY_BITSOF_SHORT);
    case NPY_INT:
      return fix_integer_type_num_inner(true, NPY_BITSOF_INT);
    case NPY_LONG:
      return fix_integer_type_num_inner(true, NPY_BITSOF_LONG);
    case NPY_LONGLONG:
      return fix_integer_type_num_inner(true, NPY_BITSOF_LONGLONG);
    case NPY_UBYTE:
      return fix_integer_type_num_inner(false, NPY_BITSOF_CHAR);
    case NPY_USHORT:
      return fix_integer_type_num_inner(false, NPY_BITSOF_SHORT);
    case NPY_UINT:
      return fix_integer_type_num_inner(false, NPY_BITSOF_INT);
    case NPY_ULONG:
      return fix_integer_type_num_inner(false, NPY_BITSOF_LONG);
    case NPY_ULONGLONG:
      return fix_integer_type_num_inner(false, NPY_BITSOF_LONGLONG);
    default:
      break;
  }

  return type_num;
}

/**
 * A simple implementation of the function below. It returns type names such as
 * "numpy.uint64" instead of just "uint64". The type name is also associated to
 * an object which may vanish - so, not that safe.
 */
/**
const char* PyBlitzArray_TypenumAsString (int type_num) {
  PyArray_Descr* dtype = PyArray_DescrFromType(type_num); ///< new reference
  if (!dtype) return 0;
  const char* retval = dtype->typeobj->tp_name;
  Py_DECREF(dtype);
  return retval;
}
*/

const char* PyBlitzArray_TypenumAsString (int type_num) {

  type_num = fix_integer_type_num(type_num);

  switch (type_num) {

    case NPY_BOOL:
      {
        static char s[] = "bool";
        return s;
      }
    case NPY_UINT8:
      {
        static char s[] = "uint8";
        return s;
      }
    case NPY_UINT16:
      {
        static char s[] = "uint16";
        return s;
      }
    case NPY_UINT32:
      {
        static char s[] = "uint32";
        return s;
      }
    case NPY_UINT64:
      {
        static char s[] = "uint64";
        return s;
      }
    case NPY_INT8:
      {
        static char s[] = "int8";
        return s;
      }
    case NPY_INT16:
      {
        static char s[] = "int16";
        return s;
      }
    case NPY_INT32:
      {
        static char s[] = "int32";
        return s;
      }
    case NPY_INT64:
      {
        static char s[] = "int64";
        return s;
      }
    case NPY_FLOAT32:
      {
        static char s[] = "float32";
        return s;
      }
    case NPY_FLOAT64:
      {
        static char s[] = "float64";
        return s;
      }
#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
      {
        static char s[] = "float128";
        return s;
      }
#endif
    case NPY_COMPLEX64:
      {
        static char s[] = "complex64";
        return s;
      }
    case NPY_COMPLEX128:
      {
        static char s[] = "complex128";
        return s;
      }
#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      {
        static char s[] = "complex256";
        return s;
      }
#endif
    default:
      PyErr_Format(PyExc_NotImplementedError, "no support for converting type number %d to string", type_num);
      return 0;
  }

}

template<typename T, int N>
PyObject* allocate_innest(int type_num, Py_ssize_t ndim, Py_ssize_t* shape) {

  try {

    blitz::TinyVector<int,N> tv_shape;
    for (int i=0; i<N; ++i) tv_shape(i) = shape[i];
    PyBlitzArrayObject* retval = (PyBlitzArrayObject*)PyBlitzArray_New(&PyBlitzArray_Type, 0, 0);
    retval->bzarr = static_cast<void*>(new blitz::Array<T,N>(tv_shape));
    retval->type_num = type_num;
    retval->ndim = ndim;
    for (Py_ssize_t i=0; i<N; ++i) retval->shape[i] = shape[i];
    return reinterpret_cast<PyObject*>(retval);

  }

  catch (std::exception& e) {
    PyErr_Format(PyExc_RuntimeError, "caught exception while instantiating blitz.array(@%" PY_FORMAT_SIZE_T "d,'%s'): %s", ndim, PyBlitzArray_TypenumAsString(type_num), e.what());
  }

  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "caught unknown exception while instantiating blitz.array(@%" PY_FORMAT_SIZE_T "d,'%s')", ndim, PyBlitzArray_TypenumAsString(type_num));
  }

  /** some test code
  std::cout << "allocating array" << std::endl;
  std::shared_ptr<blitz::Array<T,N>> retval(new blitz::Array<T,N>(tv_shape),
      &delete_array<T,N>);
  **/

  return 0;

}

template<typename T>
PyObject* allocate_inner(int type_num, Py_ssize_t ndim, Py_ssize_t* shape) {
  switch (ndim) {

    case 1:
      return allocate_innest<T,1>(type_num, ndim, shape);

    case 2:
      return allocate_innest<T,2>(type_num, ndim, shape);

    case 3:
      return allocate_innest<T,3>(type_num, ndim, shape);

    case 4:
      return allocate_innest<T,4>(type_num, ndim, shape);

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot allocate blitz.array(@%" PY_FORMAT_SIZE_T "d,'%s'): this number of dimensions is outside the range of supported dimensions [1,%d]", ndim, PyBlitzArray_TypenumAsString(type_num), BLITZ_ARRAY_MAXDIMS);
      return 0;
  }

}

PyObject* PyBlitzArray_SimpleNew (int type_num, Py_ssize_t ndim, Py_ssize_t* shape) {

  type_num = fix_integer_type_num(type_num);

  switch (type_num) {

    case NPY_BOOL:
      return allocate_inner<bool>(type_num, ndim, shape);

    case NPY_INT8:
      return allocate_inner<int8_t>(type_num, ndim, shape);

    case NPY_INT16:
      return allocate_inner<int16_t>(type_num, ndim, shape);

    case NPY_INT32:
      return allocate_inner<int32_t>(type_num, ndim, shape);

    case NPY_INT64:
      return allocate_inner<int64_t>(type_num, ndim, shape);

    case NPY_UINT8:
      return allocate_inner<uint8_t>(type_num, ndim, shape);

    case NPY_UINT16:
      return allocate_inner<uint16_t>(type_num, ndim, shape);

    case NPY_UINT32:
      return allocate_inner<uint32_t>(type_num, ndim, shape);

    case NPY_UINT64:
      return allocate_inner<uint64_t>(type_num, ndim, shape);

    case NPY_FLOAT32:
      return allocate_inner<float>(type_num, ndim, shape);

    case NPY_FLOAT64:
      return allocate_inner<double>(type_num, ndim, shape);

#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
      return allocate_inner<long double>(type_num, ndim, shape);

#endif

    case NPY_COMPLEX64:
      return allocate_inner<std::complex<float>>(type_num, ndim, shape);

    case NPY_COMPLEX128:
      return allocate_inner<std::complex<double>>(type_num, ndim, shape);

#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      return allocate_inner<std::complex<long double>>(type_num, ndim, shape);

#endif

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot create blitz.array(@%" PY_FORMAT_SIZE_T "d,T) with T having an unsupported numpy type number of %d", ndim, type_num);
      return 0;

  }

}

PyObject* PyBlitzArray_New(PyTypeObject* type, PyObject*, PyObject*) {

  /* Allocates the python object itself */
  PyBlitzArrayObject* self = (PyBlitzArrayObject*)type->tp_alloc(type, 0);

  self->bzarr = 0;
  self->type_num = -1;
  self->ndim = 0;
  self->base = Py_None;
  Py_INCREF(Py_None);

  return reinterpret_cast<PyObject*>(self);
}

template<typename T> void deallocate_inner(PyBlitzArrayObject* o) {

  switch (o->ndim) {

    case 1:
      delete reinterpret_cast<blitz::Array<T,1>*>(o->bzarr);
      break;

    case 2:
      delete reinterpret_cast<blitz::Array<T,2>*>(o->bzarr);
      break;

    case 3:
      delete reinterpret_cast<blitz::Array<T,3>*>(o->bzarr);
      break;

    case 4:
      delete reinterpret_cast<blitz::Array<T,4>*>(o->bzarr);
      break;

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot deallocate blitz.array(@%" PY_FORMAT_SIZE_T "d,%s>, this number of dimensions is outside the range of supported dimensions [1,%d]", o->ndim, PyBlitzArray_TypenumAsString(o->type_num), BLITZ_ARRAY_MAXDIMS);
      return;
  }

  Py_XDECREF(o->base);
  o->ob_type->tp_free((PyObject*)o);
}

void PyBlitzArray_Delete (PyBlitzArrayObject* o) {

  if (!o->bzarr) {
    //shortcut
    Py_XDECREF(o->base);
    o->ob_type->tp_free((PyObject*)o);
    return;
  }

  switch (o->type_num) {

    case NPY_BOOL:
      return deallocate_inner<bool>(o);

    case NPY_INT8:
      return deallocate_inner<int8_t>(o);

    case NPY_INT16:
      return deallocate_inner<int16_t>(o);

    case NPY_INT32:
      return deallocate_inner<int32_t>(o);

    case NPY_INT64:
      return deallocate_inner<int64_t>(o);

    case NPY_UINT8:
      return deallocate_inner<uint8_t>(o);

    case NPY_UINT16:
      return deallocate_inner<uint16_t>(o);

    case NPY_UINT32:
      return deallocate_inner<uint32_t>(o);

    case NPY_UINT64:
      return deallocate_inner<uint64_t>(o);

    case NPY_FLOAT32:
      return deallocate_inner<float>(o);

    case NPY_FLOAT64:
      return deallocate_inner<double>(o);

#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
      return deallocate_inner<long double>(o);

#endif

    case NPY_COMPLEX64:
      return deallocate_inner<std::complex<float>>(o);

    case NPY_COMPLEX128:
      return deallocate_inner<std::complex<double>>(o);

#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      return deallocate_inner<std::complex<long double>>(o);

#endif

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot deallocate blitz.array(@%" PY_FORMAT_SIZE_T "d,T) with T having an unsupported numpy type number of %d", o->ndim, o->type_num);
      return;

  }

}

template <typename T>
PyObject* getitem_inner(PyBlitzArrayObject* o, Py_ssize_t* pos) {

  Py_ssize_t tmp[BLITZ_ARRAY_MAXDIMS];

  /* Fix negative indexes and check ranges */
  for (Py_ssize_t i=0; i<o->ndim; ++i) {
    tmp[i] = pos[i];
    if (tmp[i] < 0) tmp[i] += o->shape[i];
    if (tmp[i] < 0 || tmp[i] >= o->shape[i]) {
      PyErr_Format(PyExc_IndexError, "blitz.array(@%" PY_FORMAT_SIZE_T "d,'%s') position %" PY_FORMAT_SIZE_T "d is out of range: %" PY_FORMAT_SIZE_T "d not in [0,%" PY_FORMAT_SIZE_T "d[", o->ndim, PyBlitzArray_TypenumAsString(o->type_num), i, pos[i], o->shape[i]);
      return 0;
    }
  }

  /* If you get to this point, then you known the indexing is fine */

  switch (o->ndim) {

    case 1:
      {
        T& val = (*reinterpret_cast<blitz::Array<T,1>*>(o->bzarr))((int)tmp[0]);
        return PyArray_Scalar(&val, PyArray_DescrFromType(o->type_num), 0);
      }

    case 2:
      {
        T& val = (*reinterpret_cast<blitz::Array<T,2>*>(o->bzarr))((int)tmp[0], (int)tmp[1]);
        return PyArray_Scalar(&val, PyArray_DescrFromType(o->type_num), 0);
      }

    case 3:
      {
        T& val = (*reinterpret_cast<blitz::Array<T,2>*>(o->bzarr))((int)tmp[0], (int)tmp[1], (int)tmp[2]);
        return PyArray_Scalar(&val, PyArray_DescrFromType(o->type_num), 0);
      }

    case 4:
      {
        T& val = (*reinterpret_cast<blitz::Array<T,2>*>(o->bzarr))((int)tmp[0], (int)tmp[1], (int)tmp[2], (int)tmp[3]);
        return PyArray_Scalar(&val, PyArray_DescrFromType(o->type_num), 0);
      }

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot index blitz.array(@%" PY_FORMAT_SIZE_T "d,'%s'): this number of dimensions is outside the range of supported dimensions [1,%d]", o->ndim, PyBlitzArray_TypenumAsString(o->type_num), BLITZ_ARRAY_MAXDIMS);
      return 0;
  }
}

PyObject* PyBlitzArray_GetItem(PyBlitzArrayObject* o, Py_ssize_t* pos) {

  switch (o->type_num) {

    case NPY_BOOL:
      return getitem_inner<bool>(o, pos);

    case NPY_INT8:
      return getitem_inner<int8_t>(o, pos);

    case NPY_INT16:
      return getitem_inner<int16_t>(o, pos);

    case NPY_INT32:
      return getitem_inner<int32_t>(o, pos);

    case NPY_INT64:
      return getitem_inner<int64_t>(o, pos);

    case NPY_UINT8:
      return getitem_inner<uint8_t>(o, pos);

    case NPY_UINT16:
      return getitem_inner<uint16_t>(o, pos);

    case NPY_UINT32:
      return getitem_inner<uint32_t>(o, pos);

    case NPY_UINT64:
      return getitem_inner<uint64_t>(o, pos);

    case NPY_FLOAT32:
      return getitem_inner<float>(o, pos);

    case NPY_FLOAT64:
      return getitem_inner<double>(o, pos);

#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
      return getitem_inner<long double>(o, pos);

#endif

    case NPY_COMPLEX64:
      return getitem_inner<std::complex<float>>(o, pos);

    case NPY_COMPLEX128:
      return getitem_inner<std::complex<double>>(o, pos);

#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      return getitem_inner<std::complex<long double>>(o, pos);

#endif

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot index blitz.array(@%" PY_FORMAT_SIZE_T "d,T) with T being a data type with an unsupported numpy type number = %d", o->ndim, o->type_num);
      return 0;

  }

}

/**
 * Sets a given item from the blitz::Array<>
 */
template <typename T>
int setitem_inner(PyBlitzArrayObject* o, Py_ssize_t* pos, PyObject* value) {

  Py_ssize_t tmp[BLITZ_ARRAY_MAXDIMS];

  /* Fix negative indexes and check ranges */
  for (Py_ssize_t i=0; i<o->ndim; ++i) {
    tmp[i] = pos[i];
    if (tmp[i] < 0) tmp[i] += o->shape[i];
    if (tmp[i] < 0 || tmp[i] >= o->shape[i]) {
      PyErr_Format(PyExc_IndexError, "array index (tmpition %" PY_FORMAT_SIZE_T "d) is out of range: %" PY_FORMAT_SIZE_T "d not in [0,%" PY_FORMAT_SIZE_T "d[", i, pos[i], o->shape[i]);
      return -1;
    }
  }

  /* If you get to this point, then you known the indexing is fine */

  switch (o->ndim) {

    case 1:
      {
        T c_value = PyBlitzArray_AsCScalar<T>(value);
        if (PyErr_Occurred()) return -1;
        (*reinterpret_cast<blitz::Array<T,1>*>(o->bzarr))((int)tmp[0]) = c_value;
        return 0;
      }

    case 2:
      {
        T c_value = PyBlitzArray_AsCScalar<T>(value);
        if (PyErr_Occurred()) return -1;
        (*reinterpret_cast<blitz::Array<T,2>*>(o->bzarr))((int)tmp[0], (int)tmp[1]) = c_value;
        return 0;
      }

    case 3:
      {
        T c_value = PyBlitzArray_AsCScalar<T>(value);
        if (PyErr_Occurred()) return -1;
        (*reinterpret_cast<blitz::Array<T,3>*>(o->bzarr))((int)tmp[0], (int)tmp[1], (int)tmp[2]) = c_value;
        return 0;
      }

    case 4:
      {
        T c_value = PyBlitzArray_AsCScalar<T>(value);
        if (PyErr_Occurred()) return -1;
        (*reinterpret_cast<blitz::Array<T,4>*>(o->bzarr))((int)tmp[0], (int)tmp[1], (int)tmp[2], (int)tmp[3]) = c_value;
        return 0;
      }

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot set item on blitz.array(@%" PY_FORMAT_SIZE_T "d,'%s'): this number of dimensions is outside the range of supported dimensions [1,%d]", o->ndim, PyBlitzArray_TypenumAsString(o->type_num), BLITZ_ARRAY_MAXDIMS);
      return -1;
  }

}

int PyBlitzArray_SetItem(PyBlitzArrayObject* o, Py_ssize_t* pos, PyObject* value) {

  switch (o->type_num) {

    case NPY_BOOL:
      return setitem_inner<bool>(o, pos, value);

    case NPY_INT8:
      return setitem_inner<int8_t>(o, pos, value);

    case NPY_INT16:
      return setitem_inner<int16_t>(o, pos, value);

    case NPY_INT32:
      return setitem_inner<int32_t>(o, pos, value);

    case NPY_INT64:
      return setitem_inner<int64_t>(o, pos, value);

    case NPY_UINT8:
      return setitem_inner<uint8_t>(o, pos, value);

    case NPY_UINT16:
      return setitem_inner<uint16_t>(o, pos, value);

    case NPY_UINT32:
      return setitem_inner<uint32_t>(o, pos, value);

    case NPY_UINT64:
      return setitem_inner<uint64_t>(o, pos, value);

    case NPY_FLOAT32:
      return setitem_inner<float>(o, pos, value);

    case NPY_FLOAT64:
      return setitem_inner<double>(o, pos, value);

#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
      return setitem_inner<long double>(o, pos, value);

#endif

    case NPY_COMPLEX64:
      return setitem_inner<std::complex<float>>(o, pos, value);

    case NPY_COMPLEX128:
      return setitem_inner<std::complex<double>>(o, pos, value);

#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      return setitem_inner<std::complex<long double>>(o, pos, value);

#endif

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot set item on blitz.array(@%" PY_FORMAT_SIZE_T "d,T) with T being a data type with an unsupported numpy type number = %d", o->ndim, o->type_num);
      return -1;

  }

}

template <typename T>
PyObject* ndarray_copy_inner(PyBlitzArrayObject* o) {

  switch (o->ndim) {

    case 1:
      {
        return PyBlitzArray_AsNumpyArrayCopy(*reinterpret_cast<blitz::Array<T,1>*>(o->bzarr));
      }

    case 2:
      {
        return PyBlitzArray_AsNumpyArrayCopy(*reinterpret_cast<blitz::Array<T,2>*>(o->bzarr));
      }

    case 3:
      {
        return PyBlitzArray_AsNumpyArrayCopy(*reinterpret_cast<blitz::Array<T,3>*>(o->bzarr));
      }

    case 4:
      {
        return PyBlitzArray_AsNumpyArrayCopy(*reinterpret_cast<blitz::Array<T,4>*>(o->bzarr));
      }

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot create a numpy ndarray copy of blitz.array(@%" PY_FORMAT_SIZE_T "d,'%s'): this number of dimensions is outside the range of supported dimensions [1,%d]", o->ndim, PyBlitzArray_TypenumAsString(o->type_num), BLITZ_ARRAY_MAXDIMS);
      return 0;
  }

}

PyObject* PyBlitzArray_AsNumpyArrayCopy(PyBlitzArrayObject* o) {

  switch (o->type_num) {

    case NPY_BOOL:
      return ndarray_copy_inner<bool>(o);

    case NPY_INT8:
      return ndarray_copy_inner<int8_t>(o);

    case NPY_INT16:
      return ndarray_copy_inner<int16_t>(o);

    case NPY_INT32:
      return ndarray_copy_inner<int32_t>(o);

    case NPY_INT64:
      return ndarray_copy_inner<int64_t>(o);

    case NPY_UINT8:
      return ndarray_copy_inner<uint8_t>(o);

    case NPY_UINT16:
      return ndarray_copy_inner<uint16_t>(o);

    case NPY_UINT32:
      return ndarray_copy_inner<uint32_t>(o);

    case NPY_UINT64:
      return ndarray_copy_inner<uint64_t>(o);

    case NPY_FLOAT32:
      return ndarray_copy_inner<float>(o);

    case NPY_FLOAT64:
      return ndarray_copy_inner<double>(o);

#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
      return ndarray_copy_inner<long double>(o);

#endif

    case NPY_COMPLEX64:
      return ndarray_copy_inner<std::complex<float>>(o);

    case NPY_COMPLEX128:
      return ndarray_copy_inner<std::complex<double>>(o);

#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      return ndarray_copy_inner<std::complex<long double>>(o);

#endif

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot create a numpy ndarray copy of blitz.array(@%" PY_FORMAT_SIZE_T "d,T) with T being a data type with an unsupported numpy type number = %d", o->ndim, o->type_num);
      return 0;

  }

}

template <typename T>
PyObject* ndarray_shallow_inner(PyBlitzArrayObject* o) {

  PyObject* retval = 0;

  switch (o->ndim) {

    case 1:
      retval = PyBlitzArray_AsShallowNumpyArray(*reinterpret_cast<blitz::Array<T,1>*>(o->bzarr));
      break;

    case 2:
      retval = PyBlitzArray_AsShallowNumpyArray(*reinterpret_cast<blitz::Array<T,2>*>(o->bzarr));
      break;

    case 3:
      retval = PyBlitzArray_AsShallowNumpyArray(*reinterpret_cast<blitz::Array<T,3>*>(o->bzarr));
      break;

    case 4:
      retval = PyBlitzArray_AsShallowNumpyArray(*reinterpret_cast<blitz::Array<T,4>*>(o->bzarr));
      break;

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot create a numpy ndarray shallow copy of blitz.array(@%" PY_FORMAT_SIZE_T "d,'%s'): this number of dimensions is outside the range of supported dimensions [1,%d]", o->ndim, PyBlitzArray_TypenumAsString(o->type_num), BLITZ_ARRAY_MAXDIMS);
      return 0;

  }

  if (!retval) return 0;

  // link this object with the returned numpy ndarray

#if NPY_FEATURE_VERSION < NUMPY17_API /* NumPy C-API version >= 1.7 */
  PyArray_BASE(reinterpret_cast<PyArrayObject*>(retval)) = reinterpret_cast<PyObject*>(o);
#else
  if (PyArray_SetBaseObject(reinterpret_cast<PyArrayObject*>(retval), reinterpret_cast<PyObject*>(o)) != 0) {
    Py_DECREF(retval);
    return 0;
  }
#endif
  Py_INCREF(reinterpret_cast<PyObject*>(o));

  return retval;
}

PyObject* PyBlitzArray_AsShallowNumpyArray(PyBlitzArrayObject* o) {

  switch (o->type_num) {

    case NPY_BOOL:
      return ndarray_shallow_inner<bool>(o);

    case NPY_INT8:
      return ndarray_shallow_inner<int8_t>(o);

    case NPY_INT16:

      return ndarray_shallow_inner<int16_t>(o);
    case NPY_INT32:

      return ndarray_shallow_inner<int32_t>(o);
    case NPY_INT64:

      return ndarray_shallow_inner<int64_t>(o);
    case NPY_UINT8:

      return ndarray_shallow_inner<uint8_t>(o);

    case NPY_UINT16:
      return ndarray_shallow_inner<uint16_t>(o);

    case NPY_UINT32:
      return ndarray_shallow_inner<uint32_t>(o);

    case NPY_UINT64:
      return ndarray_shallow_inner<uint64_t>(o);

    case NPY_FLOAT32:
      return ndarray_shallow_inner<float>(o);

    case NPY_FLOAT64:
      return ndarray_shallow_inner<double>(o);

#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
      return ndarray_shallow_inner<long double>(o);
#endif

    case NPY_COMPLEX64:
      return ndarray_shallow_inner<std::complex<float>>(o);

    case NPY_COMPLEX128:
      return ndarray_shallow_inner<std::complex<double>>(o);

#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      return ndarray_shallow_inner<std::complex<long double>>(o);
#endif

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot create a numpy ndarray copy of blitz.array(@%" PY_FORMAT_SIZE_T "d,T) with T being a data type with an unsupported numpy type number = %d", o->ndim, o->type_num);
      return 0;

  }

}

PyObject* PyBlitzArray_AsAnyNumpyArray(PyBlitzArrayObject* o) {

  PyObject* retval = PyBlitzArray_AsShallowNumpyArray(o);
  if (retval) return retval;
  return PyBlitzArray_AsNumpyArrayCopy(o);

}

Py_ssize_t PyBlitzArray_NDIM (PyBlitzArrayObject* o) {
  return o->ndim;
}

int PyBlitzArray_TYPE (PyBlitzArrayObject* o) {
  return o->type_num;
}

Py_ssize_t* PyBlitzArray_SHAPE (PyBlitzArrayObject* o) {
  return o->shape;
}

PyObject* PyBlitzArray_PYSHAPE (PyBlitzArrayObject* o) {
  PyObject* retval = PyTuple_New(o->ndim);
  if (!retval) return retval;
  for (Py_ssize_t i = 0; i != o->ndim; ++i) {
#if PY_VERSION_HEX >= 0x03000000
    PyTuple_SET_ITEM(retval, i, PyLong_FromSsize_t(o->shape[i]));
#else
    PyTuple_SET_ITEM(retval, i, PyInt_FromSsize_t(o->shape[i]));
#endif
  }
  return retval;
}

PyArray_Descr* PyBlitzArray_DTYPE (PyBlitzArrayObject* o) {
  return PyArray_DescrFromType(o->type_num);
}

int PyBlitzArray_IndexConverter(PyObject* o, PyBlitzArrayObject** shape) {

  if (!o) {
    PyErr_SetString(PyExc_TypeError, "index/shape must not be NULL");
    return 0;
  }

  if (PyNumber_Check(o)) {
    (*shape)->ndim = 1;
    (*shape)->shape[0] = PyNumber_AsSsize_t(o, PyExc_OverflowError);
    if (PyErr_Occurred()) return 0;
    if ((*shape)->shape[0] < 0) {
      PyErr_Format(PyExc_ValueError, "index/shape values should be >=0; %" PY_FORMAT_SIZE_T "d is invalid", (*shape)->shape[0]);
      return 0;
    }
    return 1;
  }

  /* The other option is to have it as a sequence */
  if (!PySequence_Check(o)) {
    PyErr_SetString(PyExc_TypeError, "shape/index must be a sequence of integers");
    return 0;
  }

  (*shape)->ndim = PySequence_Size(o);

  if ((*shape)->ndim == 0 || (*shape)->ndim > BLITZ_ARRAY_MAXDIMS) {
    PyErr_Format(PyExc_TypeError, "shape/index must be a sequence with at least 1 and at most %d element(s) (you passed a sequence with %" PY_FORMAT_SIZE_T "d elements)", BLITZ_ARRAY_MAXDIMS, (*shape)->ndim);
    return 0;
  }

  for (Py_ssize_t i=0; i<(*shape)->ndim; ++i) {
    PyObject* item = PySequence_GetItem(o, i);
    if (!item) return 0;
    if (!PyNumber_Check(item)) {
      PyErr_Format(PyExc_ValueError, "element %" PY_FORMAT_SIZE_T "d from shape/index sequence should be an number (coercible to integer)", i);
      Py_DECREF(item);
      return 0;
    }
    (*shape)->shape[i] = PyNumber_AsSsize_t(item, PyExc_OverflowError);
    if (PyErr_Occurred()) {
      PyErr_Print();
      PyErr_Format(PyExc_TypeError, "error extracting a size from element %" PY_FORMAT_SIZE_T "d of input shape/index sequence", i);
      Py_DECREF(item);
      return 0;
    }
    if ((*shape)->shape[0] < 0) {
      PyErr_Format(PyExc_ValueError, "shape/index values should be >=0; %" PY_FORMAT_SIZE_T "d is an invalid value at position %" PY_FORMAT_SIZE_T "d of input sequence", (*shape)->shape[0], i);
      Py_DECREF(item);
      return 0;
    }
  }

  return 1;
}

int PyBlitzArray_TypenumConverter(PyObject* o, int** type_num) {

  /* Make sure the dtype is good */
  if (!o) {
    PyErr_SetString(PyExc_TypeError, "dtype must not be NULL");
    return 0;
  }

  PyArray_Descr* dtype = 0;
  if (!PyArray_DescrConverter2(o, &dtype)) return 0; ///< (*dtype) is a new ref
  (**type_num) = dtype->type_num;
  Py_DECREF(dtype);

  switch (fix_integer_type_num(**type_num)) {
    case NPY_BOOL:
    case NPY_UINT8:
    case NPY_UINT16:
    case NPY_UINT32:
    case NPY_UINT64:
    case NPY_INT8:
    case NPY_INT16:
    case NPY_INT32:
    case NPY_INT64:
    case NPY_FLOAT32:
    case NPY_FLOAT64:
#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
#endif
    case NPY_COMPLEX64:
    case NPY_COMPLEX128:
#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      break;
#endif
    default:
    {
      PyErr_Format(PyExc_NotImplementedError, "no support for using type number %d in blitz.array", (**type_num));
      return 0;
    }
  }

  /* At this point, you know everything went well */
  return 1;
}

template <typename T> int behaved_inner(PyBlitzArrayObject* o) {

  switch (o->ndim) {

    case 1:
      return PyBlitzArray_IsBehaved(*reinterpret_cast<blitz::Array<T,1>*>(o->bzarr));

    case 2:
      return PyBlitzArray_IsBehaved(*reinterpret_cast<blitz::Array<T,2>*>(o->bzarr));

    case 3:
      return PyBlitzArray_IsBehaved(*reinterpret_cast<blitz::Array<T,3>*>(o->bzarr));

    case 4:
      return PyBlitzArray_IsBehaved(*reinterpret_cast<blitz::Array<T,4>*>(o->bzarr));

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot check behaviour of blitz.array(@%" PY_FORMAT_SIZE_T "d,'%s'): this number of dimensions is outside the range of supported dimensions [1,%d]", o->ndim, PyBlitzArray_TypenumAsString(o->type_num), BLITZ_ARRAY_MAXDIMS);
      return 0;

  }

}

int PyBlitzArray_IsBehaved (PyBlitzArrayObject* o) {

  switch (o->type_num) {

    case NPY_BOOL:
      return behaved_inner<bool>(o);

    case NPY_INT8:
      return behaved_inner<int8_t>(o);

    case NPY_INT16:

      return behaved_inner<int16_t>(o);
    case NPY_INT32:

      return behaved_inner<int32_t>(o);
    case NPY_INT64:

      return behaved_inner<int64_t>(o);
    case NPY_UINT8:

      return behaved_inner<uint8_t>(o);

    case NPY_UINT16:
      return behaved_inner<uint16_t>(o);

    case NPY_UINT32:
      return behaved_inner<uint32_t>(o);

    case NPY_UINT64:
      return behaved_inner<uint64_t>(o);

    case NPY_FLOAT32:
      return behaved_inner<float>(o);

    case NPY_FLOAT64:
      return behaved_inner<double>(o);

#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
      return behaved_inner<long double>(o);
#endif

    case NPY_COMPLEX64:
      return behaved_inner<std::complex<float>>(o);

    case NPY_COMPLEX128:
      return behaved_inner<std::complex<double>>(o);

#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      return behaved_inner<std::complex<long double>>(o);
#endif

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot check behaviour of blitz.array(@%" PY_FORMAT_SIZE_T "d,T) with T being a data type with an unsupported numpy type number = %d", o->ndim, o->type_num);
      return 0;

  }

}

int PyBlitzArray_NumpyArrayIsBehaved (PyObject* o) {

  if (!PyArray_Check(o)) return 0;

  PyArrayObject* ao = reinterpret_cast<PyArrayObject*>(o);

  // checks if the array is C-style, aligned and in contiguous memory
  if (!PyArray_ISCARRAY_RO(ao)) return 0;

  // checks if the number of dimensions is supported
  if (PyArray_NDIM(ao) < 1 || PyArray_NDIM(ao) > BLITZ_ARRAY_MAXDIMS) return 0;

  // checks if the type number if supported
  switch(fix_integer_type_num(PyArray_DTYPE(ao)->type_num)) {
    case NPY_BOOL:
    case NPY_UINT8:
    case NPY_UINT16:
    case NPY_UINT32:
    case NPY_UINT64:
    case NPY_INT8:
    case NPY_INT16:
    case NPY_INT32:
    case NPY_INT64:
    case NPY_FLOAT32:
    case NPY_FLOAT64:
#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
#endif
    case NPY_COMPLEX64:
    case NPY_COMPLEX128:
#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      break;
#endif
    default:
      return 0;
  }

  // if you get to this point, you can only return yes
  return 1;
}

template <typename T, int N>
PyObject* shallow_numpy_innest(PyArrayObject* o) {

  try {

    blitz::Array<T,N> bz(PyBlitzArray_ShallowFromNumpyArray<T,N>(reinterpret_cast<PyObject*>(o), false));
    if (PyErr_Occurred()) return 0;

    // instantiate type to be returned
    PyBlitzArrayObject* retval = (PyBlitzArrayObject*)PyBlitzArray_New(&PyBlitzArray_Type, 0, 0);
    retval->bzarr = static_cast<void*>(new blitz::Array<T,N>(bz));
    retval->type_num = PyArray_DESCR(o)->type_num;
    retval->ndim = PyArray_NDIM(o);
    for (Py_ssize_t i=0; i<N; ++i) retval->shape[i] = PyArray_SHAPE(o)[i];

    // switch base
    PyObject* tmp = retval->base;
    retval->base = reinterpret_cast<PyObject*>(o);
    Py_INCREF(retval->base);
    Py_DECREF(tmp);

    return reinterpret_cast<PyObject*>(retval);

  }

  catch (std::exception& e) {
    PyErr_Format(PyExc_RuntimeError, "caught exception while instantiating blitz.array(@%d,'%s'): %s", N, PyBlitzArray_TypenumAsString(PyArray_DESCR(o)->type_num), e.what());
  }

  catch (...) {
    PyErr_Format(PyExc_RuntimeError, "caught unknown exception while instantiating blitz.array(@%d,'%s')", N, PyBlitzArray_TypenumAsString(PyArray_DESCR(o)->type_num));
  }

  /** some test code
  std::cout << "allocating array" << std::endl;
  std::shared_ptr<blitz::Array<T,N>> retval(new blitz::Array<T,N>(tv_shape),
      &delete_array<T,N>);
  **/

  return 0;

}

template <typename T>
PyObject* shallow_numpy_inner(PyArrayObject* o) {

  switch (PyArray_NDIM(o)) {

    case 1:
      return shallow_numpy_innest<T,1>(o);

    case 2:
      return shallow_numpy_innest<T,2>(o);

    case 3:
      return shallow_numpy_innest<T,3>(o);

    case 4:
      return shallow_numpy_innest<T,4>(o);

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot shallow convert numpy.ndarray to blitz.array(@%d,'%s'): this number of dimensions is outside the range of supported dimensions [1,%d]", PyArray_NDIM(o), PyBlitzArray_TypenumAsString(PyArray_DESCR(o)->type_num), BLITZ_ARRAY_MAXDIMS);
      return 0;

  }

}

PyObject* PyBlitzArray_ShallowFromNumpyArray(PyObject* o) {

  if (!PyArray_Check(o)) {
    PyErr_SetString(PyExc_TypeError, "shallow conversion to blitz.array requires a numpy.ndarray");
    return 0;
  }

  PyArrayObject* ao = reinterpret_cast<PyArrayObject*>(o);

  switch (fix_integer_type_num(PyArray_DESCR(ao)->type_num)) {

    case NPY_BOOL:
      return shallow_numpy_inner<bool>(ao);

    case NPY_INT8:
      return shallow_numpy_inner<int8_t>(ao);

    case NPY_INT16:

      return shallow_numpy_inner<int16_t>(ao);
    case NPY_INT32:

      return shallow_numpy_inner<int32_t>(ao);
    case NPY_INT64:

      return shallow_numpy_inner<int64_t>(ao);
    case NPY_UINT8:

      return shallow_numpy_inner<uint8_t>(ao);

    case NPY_UINT16:
      return shallow_numpy_inner<uint16_t>(ao);

    case NPY_UINT32:
      return shallow_numpy_inner<uint32_t>(ao);

    case NPY_UINT64:
      return shallow_numpy_inner<uint64_t>(ao);

    case NPY_FLOAT32:
      return shallow_numpy_inner<float>(ao);

    case NPY_FLOAT64:
      return shallow_numpy_inner<double>(ao);

#ifdef NPY_FLOAT128
    case NPY_FLOAT128:
      return shallow_numpy_inner<long double>(ao);
#endif

    case NPY_COMPLEX64:
      return shallow_numpy_inner<std::complex<float>>(ao);

    case NPY_COMPLEX128:
      return shallow_numpy_inner<std::complex<double>>(ao);

#ifdef NPY_COMPLEX256
    case NPY_COMPLEX256:
      return shallow_numpy_inner<std::complex<long double>>(ao);
#endif

    default:
      PyErr_Format(PyExc_NotImplementedError, "cannot shallow convert numpy.ndarray to blitz.array(@%d,T) with T being a data type with an unsupported numpy type number = %d", PyArray_NDIM(ao), PyArray_DESCR(ao)->type_num);
      return 0;

  }

}