#define PY_SSIZE_T_CLEAN
#include <python3.11/Python.h>
#include <stdio.h>
#include <crypt.h>

// most of this code is copy and pasted from
// https://github.com/python/cpython/blob/3.11/Modules/clinic/_cryptmodule.c.h
// with the exception of what methods are named as to
// not cause a module naming conflict since the "crypt"
// module hasn't been removed (yet, python3.13)
PyDoc_STRVAR(cryptit__doc__,
"cryptit($module, word, salt, /)\n"
"--\n"
"\n"
"Hash a *word* with the given *salt* and return the hashed password.\n"
"\n"
"*word* will usually be a user\'s password.  *salt* (either a random 2 or 16\n"
"character string, possibly prefixed with $digit$ to indicate the method)\n"
"will be used to perturb the encryption algorithm and produce distinct\n"
"results for a given *word*.");


static PyObject *
cryptit_impl(PyObject *self, const char *word, const char *salt)
{
    char *hash;
    struct crypt_data data;
    Py_BEGIN_ALLOW_THREADS
    // depending on the prefix of the salt (sha256/512)
    // and the rounds (if specified), this could take
    // considerable amounts of time so drop the GIL
    memset(&data, 0, sizeof(data));
    hash = crypt_r(word, salt, &data);
    Py_END_ALLOW_THREADS
    if (hash == NULL)
        return PyErr_SetFromErrno(PyExc_OSError);
    return Py_BuildValue("s", hash);
}


static PyObject *
method_cryptit(PyObject *self, PyObject *const *args, Py_ssize_t nargs)
{
    PyObject *return_value = NULL;
    const char *word;
    const char *salt;

    if (!_PyArg_CheckPositional("method_cryptit", nargs, 2, 2))
        goto exit;
    if (!PyUnicode_Check(args[0])) {
        _PyArg_BadArgument("method_cryptit", "argument 1", "str", args[0]);
        goto exit;
    }
    Py_ssize_t word_length;
    word = PyUnicode_AsUTF8AndSize(args[0], &word_length);
    if (word == NULL)
        goto exit;
    if (strlen(word) != (size_t)word_length) {
        PyErr_SetString(PyExc_ValueError, "embedded null character");
        goto exit;
    }
    if (!PyUnicode_Check(args[1])) {
        _PyArg_BadArgument("method_cryptit", "argument 2", "str", args[1]);
        goto exit;
    }
    Py_ssize_t salt_length;
    salt = PyUnicode_AsUTF8AndSize(args[1], &salt_length);
    if (salt == NULL)
        goto exit;
    if (strlen(salt) != (size_t)salt_length) {
        PyErr_SetString(PyExc_ValueError, "embedded null character");
        goto exit;
    }
	return_value = cryptit_impl(self, word, salt);

exit:
    return return_value;
}


static PyMethodDef cryptitmethods[] = {
    {"cryptit", _PyCFunction_CAST(method_cryptit), METH_FASTCALL, cryptit__doc__},
    {NULL, NULL, 0, NULL}
};


static PyModuleDef_Slot _cryptit_slots[] = {
    {0, NULL}
};


static struct PyModuleDef cryptitmodule = {
    PyModuleDef_HEAD_INIT,
    "_cryptit",
    NULL,
    0,
    cryptitmethods,
    _cryptit_slots,
    NULL,
    NULL,
    NULL
};


PyMODINIT_FUNC PyInit_cryptit(void)
{
    return PyModuleDef_Init(&cryptitmodule);
}
