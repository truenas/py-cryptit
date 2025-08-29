#define PY_SSIZE_T_CLEAN
#include <Python.h>
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
method_cryptit(PyObject *self, PyObject *args)
{
    const char *word;
    const char *salt;

    if (!PyArg_ParseTuple(args, "ss:cryptit", &word, &salt))
        return NULL;

    return cryptit_impl(self, word, salt);
}


static PyMethodDef cryptitmethods[] = {
    {"cryptit", method_cryptit, METH_VARARGS, cryptit__doc__},
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
