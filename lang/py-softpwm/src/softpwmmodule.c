/*
 * softpwmmodule.c - Python bindings for Linux softpwm access through softpwm-dev
 * Copyright (C) 2009 Volker Thoms <unconnected@gmx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <Python.h>
#include "structmember.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "gpio-softpwm.h"
#include <linux/types.h>
#include <sys/ioctl.h>

PyDoc_STRVAR(softpwm_module_doc,
	"This module defines an object type that allows to set duty cycles\n"
	"for pwm pins on the softpwm linux kernel module\n");

typedef struct {
	PyObject_HEAD

	int fd;		/* open file descriptor: /dev/softpwm */
} softpwm;

static PyObject *
softpwm_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	softpwm *self;

	if ((self = (softpwm *)type->tp_alloc(type, 0)) == NULL)
		return NULL;

	self->fd = -1;

	return (PyObject *)self;
}

PyDoc_STRVAR(softpwm_close_doc,
	"close()\n\n"
	"Disconnects the object from the interface.\n");

static PyObject *
softpwm_close(softpwm *self)
{
	if ((self->fd != -1) && (close(self->fd) == -1)) {
		PyErr_SetFromErrno(PyExc_IOError);
		return NULL;
	}

	self->fd = -1;

	Py_INCREF(Py_None);
	return Py_None;
}

static void
softpwm_dealloc(softpwm *self)
{
	PyObject *ref = softpwm_close(self);
	Py_XDECREF(ref);

	self->ob_type->tp_free((PyObject *)self);
}

#define MAXPATH 16

PyDoc_STRVAR(softpwm_open_doc,
	"open(bus)\n\n"
	"Connects the object to the specified softpwm.\n");

static PyObject *
softpwm_open(softpwm *self, PyObject *args, PyObject *kwds)
{
	static char *kwlist[] = {NULL};

	if ((self->fd = open("/dev/softpwm", 0)) == -1) {
		PyErr_SetFromErrno(PyExc_IOError);
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static int
softpwm_init(softpwm *self, PyObject *args, PyObject *kwds)
{
	static char *kwlist[] = { NULL};

	softpwm_open(self, args, kwds);
	if (PyErr_Occurred())
		return -1;

	return 0;
}

PyDoc_STRVAR(softpwm_enable_doc,
	"enable(bool)\n\n"
	"enable or disable PWM");

static PyObject *
softpwm_enable(softpwm *self, PyObject *args)
{
	PyObject * boolval;
	__u8 i = 0;

	if (!PyArg_ParseTuple(args, "O:enable", &boolval))
		return NULL;
	
	if (!PyBool_Check( boolval ) ) {
		PyErr_SetString(PyExc_StandardError, "Argument must be of type PyBool.\n");
		return NULL;
	}

	if (boolval == Py_True) {
		i = SOFTPWM_ON;
	} else {
		i = SOFTPWM_OFF;
	}

	if ( ioctl(self->fd, SOFTPWM_IOC_ENABLE, &i) < 0) {
		PyErr_SetFromErrno(PyExc_IOError);
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PyDoc_STRVAR(softpwm_set_doc,
	"set(pin, width)\n\n"
	"set pulsewidth (width) for pin (pin).\n"
	"pin must be integer 0..15\n"
	"width must be integer 0..1999");

static PyObject *
softpwm_set(softpwm *self, PyObject *args)
{
	int i, pw;
	struct spwm_ioctl spwms;

	if (!PyArg_ParseTuple(args, "ii:set", &i, &pw))
		return NULL;

	spwms.pin_offset = i;
	spwms.pulse_width = pw;

	if ( ioctl(self->fd, 0, &spwms) < 0) {
		PyErr_SetFromErrno(PyExc_IOError);
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}


PyDoc_STRVAR(softpwm_type_doc,
	"softpwm() -> softpwm\n\n"
	"Return a new softpwm object that is connected to the\n"
	"/dev/softpwm device interface.\n");

static PyMethodDef softpwm_methods[] = {
	{"set", (PyCFunction)softpwm_set, METH_VARARGS,
		softpwm_set_doc},
	{"enable", (PyCFunction)softpwm_enable, METH_VARARGS,
		softpwm_enable_doc},
	{NULL},
};

static PyTypeObject softpwm_type = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size */
	"softpwm",			/* tp_name */
	sizeof(softpwm),			/* tp_basicsize */
	0,				/* tp_itemsize */
	(destructor)softpwm_dealloc,	/* tp_dealloc */
	0,				/* tp_print */
	0,				/* tp_getattr */
	0,				/* tp_setattr */
	0,				/* tp_compare */
	0,				/* tp_repr */
	0,				/* tp_as_number */
	0,				/* tp_as_sequence */
	0,				/* tp_as_mapping */
	0,				/* tp_hash */
	0,				/* tp_call */
	0,				/* tp_str */
	0,				/* tp_getattro */
	0,				/* tp_setattro */
	0,				/* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,		/* tp_flags */
	softpwm_type_doc,			/* tp_doc */
	0,				/* tp_traverse */
	0,				/* tp_clear */
	0,				/* tp_richcompare */
	0,				/* tp_weaklistoffset */
	0,				/* tp_iter */
	0,				/* tp_iternext */
	softpwm_methods,			/* tp_methods */
	0,				/* tp_members */
	0,				/* tp_getset */
	0,				/* tp_base */
	0,				/* tp_dict */
	0,				/* tp_descr_get */
	0,				/* tp_descr_set */
	0,				/* tp_dictoffset */
	(initproc)softpwm_init,		/* tp_init */
	0,				/* tp_alloc */
	softpwm_new,			/* tp_new */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initsoftpwm(void)
{
	PyObject* m;

	if (PyType_Ready(&softpwm_type) < 0)
		return;

	m = Py_InitModule3("softpwm", softpwm_methods, softpwm_module_doc);

	Py_INCREF(&softpwm_type);
	PyModule_AddObject(m, "softpwm", (PyObject *)&softpwm_type);
}

