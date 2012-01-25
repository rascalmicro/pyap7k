/*
 * gpiomodule.c - Python bindings to control the hardware PWM uniot on AP700x
 * through sysfs
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
#include <pythread.h>
#include "structmember.h"
#include "fcntl.h"
#include "string.h"
#include <poll.h>

PyDoc_STRVAR(PWM_module_doc,
	"This module defines an object type that allows to control the PWM units\n"
	"on hosts running the Linux kernel. It utilizes the Generic PWM API from\n"
	"Bill Gatliff.");

#define MAXPATH 48

#define PWM_BASE_PATH "/sys/class/pwm/atmel_pwmc.0:"

#define PWM_DUTY_NS(x,buf) \
				snprintf((buf), MAXPATH, "%s%d/duty_ns", PWM_BASE_PATH, (x))
#define PWM_PERIOD_NS(x,buf) \
				snprintf((buf), MAXPATH, "%s%d/period_ns", PWM_BASE_PATH, (x))
#define PWM_REQUEST(x,buf) \
				snprintf((buf), MAXPATH, "%s%d/request", PWM_BASE_PATH, (x))
#define PWM_POLARITY(x,buf) \
				snprintf((buf), MAXPATH, "%s%d/polarity", PWM_BASE_PATH, (x))
#define PWM_RUN(x,buf) \
				snprintf((buf), MAXPATH, "%s%d/run", PWM_BASE_PATH, (x))

typedef struct {
	PyObject_HEAD

	int channel;	/* number of PWM channel [0..3] */

	int fd_duty;
	int fd_period;
	int fd_run;
	int fd_polarity;
	
} PWM;

static int getl( int fd, char * res ) {
	int i = 0;
	char c;

	while(1) {
		lseek( fd, i, SEEK_SET);
		read( fd, &c, 1 );
		if (c == '\n') {
			res[i] = '\0';
			break;
		}
		res[i] = c;
		i++;
	}
	return i;
}

static PyObject *
do_start(PWM * self, char value) {
	int fd;
	char run_path[MAXPATH];

	write( self->fd_run, &value, 1 );
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
PWM_start( PWM * self ) {
	return do_start(self, '1');
}

static PyObject *
PWM_stop( PWM * self ) {
	return do_start(self, '0');
}

static PyObject *
PWM_request( PWM * self ) {
	int fd;
	char value[10];	// does NULL work?
	char request_path[MAXPATH];

	PWM_REQUEST(self->channel, &request_path[0]);
	if ((fd = open(&request_path[0], O_RDONLY ) ) < 0 ) {
		PyErr_SetFromErrno( PyExc_IOError );
		return NULL;
	}

	read( fd, &value, 10 );	// `cat request´ returns 'sysfs'
	close( fd );
	Py_INCREF(Py_None);
	return Py_None;

}

static PyObject *
PWM_release( PWM * self ) {
	int fd;
	char value = '0';	// sysfs store function doesn't care about
				// _what_ is written
				// '0' is an arbitrary value which does the job
	char request_path[MAXPATH];

	PWM_REQUEST(self->channel, &request_path[0]);
	if ((fd = open(&request_path[0], O_WRONLY ) ) < 0 ) {
		PyErr_SetFromErrno( PyExc_IOError );
		return NULL;
	}

	write( fd, &value, 1 );
	close( fd );
	Py_INCREF(Py_None);
	return Py_None;

}

static PyObject *
PWM_get_period(PWM * self) {
	char cprd[10];
	PyObject * i;

	getl(self->fd_period, &cprd[0]);
	i = PyInt_FromString(&cprd[0], NULL, 10);
	
	//printf("\nget_period() returning %i.\n", PyInt_AsLong(i));
	Py_INCREF(i);
	return i;

}

static int
PWM_set_period(PWM * self, PyObject * prd) {
	char cprd[10];

	snprintf(&cprd[0], 10,  "%i", PyInt_AsLong(prd));
	//printf("setting to: %s, size is %i\n", &cprd[0], strlen(&cprd[0]));
	lseek(self->fd_period, 0, SEEK_SET);
	if ( write ( self->fd_period, &cprd[0], strlen(&cprd[0]) ) < 0 ) {
		PyErr_SetFromErrno( PyExc_IOError );
		return -1;
	}
	return 0;
}

static PyObject *
PWM_get_duty(PWM * self) {
	char cdty[10];
	PyObject * i;

	getl(self->fd_duty, &cdty[0]);
	i = PyInt_FromString(&cdty[0], NULL, 10);
	
	//printf("get_duty() returning %i.\n", PyInt_AsLong(i));
	Py_INCREF(i);
	return i;

}

static int
PWM_set_duty(PWM * self, PyObject * dty) {
	char cdty[10];

	snprintf(&cdty[0], 10,  "%i", PyInt_AsLong(dty));

	lseek(self->fd_duty, 0, SEEK_SET);
	if ( write ( self->fd_duty, &cdty[0], strlen(&cdty[0]) ) < 0 ) {
		PyErr_SetFromErrno( PyExc_IOError );
		return -1;
	}
	return 0;
}

static PyObject *
PWM_get_polarity(PWM * self) {
	char cpol[2];
	PyObject * i;

	getl(self->fd_polarity, &cpol[0]);
	i = PyInt_FromString(&cpol[0], NULL, 10);
	
	//printf("get_polarity() returning %i.\n", PyInt_AsLong(i));
	Py_INCREF(i);
	return i;

}

static int
PWM_set_polarity(PWM * self, PyObject * pol) {
	char cpol;

	snprintf(&cpol, 1,  "%i", PyInt_AsLong(pol));

	//printf("setting polarity to %c", cpol);
	
	lseek(self->fd_polarity, 0, SEEK_SET);
	if ( write ( self->fd_polarity, &cpol, 1 ) != 1 ) {
		PyErr_SetFromErrno( PyExc_IOError );
		return -1;
	}
	return 0;
}


static PyObject *
PWM_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PWM *self;

	if ((self = (PWM *)type->tp_alloc(type, 0)) == NULL)
		return NULL;

	self->channel = -1;
	self->fd_run = -1;
	self->fd_polarity = -1;
	self->fd_duty = -1;
	self->fd_period = -1;

	return (PyObject *)self;
}

static void
PWM_dealloc(PWM *self)
{
	//printf("PWM_dealloc()\n");

	PWM_release(self);
	close(self->fd_period);
	close(self->fd_duty);
	close(self->fd_polarity);
	close(self->fd_run);
	self->ob_type->tp_free((PyObject *)self);
}

static int
PWM_init(PWM *self, PyObject *args, PyObject *kwds)
{
	int channel = -1;
	int polarity = -1;
	int period = -1;
	int duty = -1;

	char duty_path[MAXPATH];
	char period_path[MAXPATH];
	char polarity_path[MAXPATH];
	char run_path[MAXPATH];

	static char *kwlist[] = { "channel", "polarity", "period", "duty", NULL };

	if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|iii:__init__",
			kwlist, &channel, &polarity, &period, &duty ) )
		return -1;

	if (channel < 0)
		return -1;

	self->channel = channel;

	if (PWM_request(self) == NULL) {
		return -1;
	}

	PWM_RUN(channel, run_path);
	if (( self->fd_run = open(run_path, O_WRONLY ) ) < 0 ) {
		PyErr_SetFromErrnoWithFilename( PyExc_IOError, run_path );
		return -1;
	}


	PWM_PERIOD_NS( channel, period_path );
	if ( ( self->fd_period = open( period_path, O_RDWR, 0 ) ) == -1 ) {
		PyErr_SetFromErrnoWithFilename( PyExc_IOError, period_path );
		return -1;
	}

	PWM_DUTY_NS(channel, duty_path);
	if ( ( self->fd_duty = open( duty_path, O_RDWR ) ) == -1 ) {
		PyErr_SetFromErrnoWithFilename( PyExc_IOError, duty_path );
		return -1;
	}

	PWM_POLARITY(channel, polarity_path);
	if ( ( self->fd_polarity = open( polarity_path, O_RDWR ) ) == -1 ) {
		PyErr_SetFromErrnoWithFilename( PyExc_IOError, polarity_path );
		return -1;
	}

	/* TODO: implement setting attributes on initialization */

	return 0;
}

PyDoc_STRVAR( PWM_type_doc,
	"PWM(channel[,period, duty, polarity]) -> PWM\n\n"
	"Return a new PWM object that is connected to the\n"
	"specified PWM channel via sysfs interface.\n");

static PyGetSetDef PWM_getset[] = {
	{"duty", (getter)PWM_get_duty, (setter)PWM_set_duty,
			"get/set duty cycle in nanoseconds"},
	{"period", (getter)PWM_get_period, (setter)PWM_set_period,
			"get/set period in nanoseconds"},
	{"polarity", (getter)PWM_get_polarity, (setter)PWM_set_polarity,
			"get/set polarity (1: active high; 0: active low)"},
	{NULL},
};

static PyMethodDef PWM_methods[] = {
	{"start", (PyCFunction)PWM_start, METH_NOARGS, "start PWM output\n"},
	{"stop", (PyCFunction)PWM_stop, METH_NOARGS, "stop PWM output\n"},
	{NULL},
};

static PyTypeObject PWM_type = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size */
	"PWM",				/* tp_name */
	sizeof(PWM),			/* tp_basicsize */
	0,				/* tp_itemsize */
	(destructor)PWM_dealloc,	/* tp_dealloc */
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
	PWM_type_doc,			/* tp_doc */
	0,				/* tp_traverse */
	0,				/* tp_clear */
	0,				/* tp_richcompare */
	0,				/* tp_weaklistoffset */
	0,				/* tp_iter */
	0,				/* tp_iternext */
	PWM_methods,			/* tp_methods */
	0,				/* tp_members */
	PWM_getset,			/* tp_getset */
	0,				/* tp_base */
	0,				/* tp_dict */
	0,				/* tp_descr_get */
	0,				/* tp_descr_set */
	0,				/* tp_dictoffset */
	(initproc)PWM_init,		/* tp_init */
	0,				/* tp_alloc */
	PWM_new,			/* tp_new */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initpwm(void)
{
	PyObject* m;

	if (PyType_Ready(&PWM_type) < 0)
		return;

	m = Py_InitModule3("pwm", 0, PWM_module_doc);

	Py_INCREF(&PWM_type);
	PyModule_AddObject(m, "PWM", (PyObject *)&PWM_type);
}

