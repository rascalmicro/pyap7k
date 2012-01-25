/*
 * gpiomodule.c - Python bindings for Linux GPIO access through sysfs
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

/*
 * feature: lock
 * normally we don't hold a lock on the value/direction files
 * to prevent other applications accessing them
 * - this might become a feature in later releases
 */

#include <Python.h>
#include <pythread.h>
#include "structmember.h"
#include "fcntl.h"
#include "string.h"
#include <poll.h>

PyDoc_STRVAR(GPIO_module_doc,
	"This module defines an object type that allows GPIO transactions\n"
	"on hosts running the Linux kernel.  The host kernel must have GPIO\n"
	"support and GPIO sysfs support.\n"
	"All of these can be either built-in to the kernel, or loaded from\n"
	"modules.");

#define MAXPATH 48

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define GPIO_BASE_PATH "/sys/class/gpio/gpio"

#define GPIO_PATH(x,buf) \
				snprintf((buf), MAXPATH, "%s%d", GPIO_BASE_PATH, (x))
#define GPIO_DIREC(x,buf) \
				snprintf((buf), MAXPATH, "%s%d/direction", GPIO_BASE_PATH, (x))
#define GPIO_VALUE(x,buf) \
				snprintf((buf), MAXPATH, "%s%d/value", GPIO_BASE_PATH, (x))

// GPIO_EDGE: kernel needs to be patched with gpiolib-allow-poll-on-value - patch
// this patch seems to be in mainline kernel from 2.6.32 on
#define GPIO_EDGE(x,buf) \
				snprintf((buf), MAXPATH, "%s%d/edge", GPIO_BASE_PATH, (x))

typedef struct {
	PyObject_HEAD

	int gpio;	/* number of gpio */
	PyObject * direction;
	PyObject * trigger;
	PyObject * callback;

	char e_path[MAXPATH];
	char d_path[MAXPATH];
	char v_path[MAXPATH];

	int fd_val;	/* value fd */
	int fd_dir;	/* direction fd */
	int fd_edge;	/* interrupt edge fd  */
	
	PyInterpreterState* interp;
	PyThread_type_lock lock;
} GPIO;

static void dumpinfo(GPIO* self) {
	printf("self: %x\n",self);
	printf("callback: %x\n",self->callback);
	printf("lock: %x\n",self->lock);
	printf("InterpreterState:%x\n", self->interp);
}

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

static void t_bootstrap(void* rawself)
{
	GPIO* self = (GPIO*) rawself;
	PyThreadState *tstate;
	PyObject* args = NULL;
	char buf;
	struct pollfd pfd;

	pfd.fd = self->fd_val;
//	pfd.events = POLLERR;
//	pfd.events = POLLIN;

	tstate = PyThreadState_New(self->interp);
	PyEval_AcquireThread(tstate);
	dumpinfo(self);
	int retval;

	PyObject *res = NULL;

//	read( self->fd_val, &buf, 1 );

	while (1) {
/*		if (self->callback == NULL ) {
			printf("poll thread<pre-poll>: callback is NULL; exiting loop\n");
			break;
		}
*/
		Py_BEGIN_ALLOW_THREADS		
		retval = poll(&pfd, 1, 20);	// timeout to check if cb still set
		Py_END_ALLOW_THREADS

		if (self->callback == NULL) {
			printf("poll thread<past-poll>: callback is NULL; exiting loop\n");
			break;
		}

		if (retval == 0) continue;	// poll timed out
		printf("%x\n", self->callback);
		if (retval < 0) {
 			PyErr_SetFromErrno(PyExc_IOError);
			break;
		}

		printf("retval: %i, pfd.revents: %x\n", retval, pfd.revents);

		lseek(self->fd_val, 0, SEEK_SET);
		if ( read( self->fd_val, &buf, 1 ) != 1 ) {
			PyErr_SetFromErrno( PyExc_IOError );
			break;
		}

		res = NULL;

		args = Py_BuildValue("(i)", buf - 48);

	PyGILState_STATE s = PyGILState_Ensure();
		res = PyObject_CallObject(self->callback, args);
	PyGILState_Release(s);

		Py_DECREF(args);
		if (res == NULL) {
			Py_XDECREF(res);
			break; /* Pass error back */
		}

		/* Here maybe use res
		  maybe exit loop at some defined return value (i.e. res == -1)
		 */
		Py_DECREF(res);
     	}
	if (PyErr_Occurred()) {
		if (PyErr_ExceptionMatches(PyExc_SystemExit))
			PyErr_Clear();
		else {
			PySys_WriteStderr("Unhandled exception in alarm:\n");
			PyErr_PrintEx(0);
		}
	}
//?	PyThread_free_lock(self->lock);
	PyThread_release_lock(self->lock);
	printf("terminating poll thread\n");
	Py_DECREF(self);
	PyThreadState_Clear(tstate);
	PyThreadState_DeleteCurrent();
	PyThread_exit_thread();
}

static PyObject *
getDirection(GPIO * self) {

	PyObject * res;
	char buf[5];		// in, out, low or high

	getl(self->fd_dir, &buf[0]);

	res = PyBytes_FromString(&buf[0]);

	Py_INCREF(res);
	return res;
}

static PyObject *
setDirection(GPIO * self, PyObject * cdir) {
	int len;
	int res;

	len = strlen(PyBytes_AsString(cdir));
	lseek(self->fd_dir, 0, SEEK_SET);
	res = write ( self->fd_dir, PyBytes_AsString( cdir ), len );
	if ( res != len ) {
		PyErr_SetFromErrno( PyExc_IOError );
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *
getTrigger(GPIO * self) {

	PyObject * res;
	char buf[10];

	getl(self->fd_edge, &buf[0]);

	res = PyBytes_FromString(&buf[0]);

	Py_INCREF(res);
	return res;
}

static PyObject *
setTrigger(GPIO * self, PyObject * ctrig) {
	int len;
	int res;

	len = strlen(PyBytes_AsString(ctrig));
	lseek(self->fd_edge, 0, SEEK_SET);
	res = write ( self->fd_edge, PyBytes_AsString( ctrig ), len );
	if ( res != len ) {
		PyErr_SetFromErrno( PyExc_IOError );
		return NULL;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

int exportGpio( int gpio ) {
	char c_gpio[4];		// 4 chars should be enough for GPIO number
				// plus trailing '\0'
	int fd;

	if ((fd = open(GPIO_EXPORT, O_WRONLY ) ) < 0 ) {
		PyErr_SetFromErrno( PyExc_IOError );
		return -1;
	}

	sprintf(c_gpio, "%i", gpio);
	write( fd, &c_gpio, strlen(c_gpio) );
	close( fd );
	return 0;
}

static PyObject *
GPIO_get_value(GPIO * self) {
	int dirfd;
	char cval[2];
	PyObject * i;

	getl(self->fd_val, &cval[0]);
	i = PyInt_FromString(&cval[0], NULL, 10);
	
	// printf("\nget_value() returning %i (%i)\n", cval[0] - 48, PyInt_AsLong(i));
	return i;

}

static int
GPIO_set_value(GPIO * self, PyObject * cval) {
	char val;

	if (PyInt_AsLong(cval) == 0) {
		val = '0';
	} else {
		val = '1';
	}

	lseek(self->fd_val, 0, SEEK_SET);
	if ( write ( self->fd_val, &val, 1 ) != 1 ) {
		PyErr_SetFromErrno( PyExc_IOError );
		return -1;
	}
		
	return 0;
}

static PyObject *
GPIO_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	GPIO *self;

	if ((self = (GPIO *)type->tp_alloc(type, 0)) == NULL)
		return NULL;

	self->callback = NULL;
	self->direction = NULL;
	self->trigger = NULL;
	self->gpio = -1;
	self->fd_val = -1;
	self->fd_dir = -1;
	self->fd_edge = -1;

	return (PyObject *)self;
}

static void
GPIO_dealloc(GPIO *self)
{
	printf("%x:GPIO_dealloc()\n", self);
	PyObject * tmp;
	if (self->callback != NULL) {
		tmp = self->callback;
		self->callback = NULL;
		Py_DECREF( tmp );		
		Py_BEGIN_ALLOW_THREADS
		printf("waiting for poll to finish...");
		PyThread_acquire_lock(self->lock, 1);  /* wait for thread to finish */
		printf("finished!\n");
		Py_END_ALLOW_THREADS
	}
	if (self->lock != NULL)
		PyThread_release_lock(self->lock);

	Py_XDECREF(self->direction);
	Py_XDECREF(self->trigger);
	close(self->fd_val);
	close(self->fd_dir);
	close(self->fd_edge);
	PyObject_Del((PyObject*) self);
//	self->ob_type->tp_free((PyObject *)self);
}

static int
GPIO_init(GPIO *self, PyObject *args, PyObject *kwds)
{
	int fd = -1;
	int gpio = -1;
	
	PyObject * direction = NULL;
	PyObject * trigger = NULL;
	PyObject * tmp = NULL;

	static char *kwlist[] = { "gpio", "direction", "trigger", NULL };

	if ( !PyArg_ParseTupleAndKeywords(args, kwds, "i|OO:__init__",
			kwlist, &gpio, &direction, &trigger ) )
		return -1;

	if (gpio < 0)
		return -1;

	self->gpio = gpio;

	GPIO_VALUE( gpio, self->v_path );
	if ( ( fd = open( self->v_path, O_RDWR, 0 ) ) == -1 ) {
		// try to get gpio exported:
		if ( exportGpio( gpio ) == 0 ) {
			// check if export was (really) successful
			
			if ( ( fd = open( self->v_path, O_RDWR ) ) == -1) {
				// export failed
				PyErr_SetFromErrno( PyExc_IOError );
				return -1;
			}
		} else {
			PyErr_SetString( PyExc_StandardError,
				"Export failed." );
			return -1;
		}
	}

	GPIO_EDGE(gpio, self->e_path);
	GPIO_DIREC(gpio, self->d_path);

	self->fd_val = fd;
	if ( ( self->fd_dir = open( self->d_path, O_RDWR ) ) == -1 ) {
		return -1;
	}

	if ( ( self->fd_edge = open( self->e_path, O_RDWR ) ) == -1 ) {
		return -1;
	}

	if (direction) {
		setDirection( self, direction );
		tmp = self->direction;
		Py_INCREF(direction);
		self->direction = direction;
		Py_XDECREF(tmp);
	} else {
		// no direction requested, use current
		Py_XDECREF(self->direction);
		self->direction = getDirection( self );
		Py_INCREF(self->direction);
	}

	if (trigger) {
		setTrigger( self, trigger );
		tmp = self->trigger;
		Py_INCREF(trigger);
		self->trigger = trigger;
		Py_XDECREF(tmp);
	} else {
		// no trigger requested, use current
		Py_XDECREF(self->trigger);
		self->trigger = getTrigger( self );
		Py_INCREF(self->trigger);
	}

	return 0;
}

PyDoc_STRVAR( GPIO_type_doc,
	"GPIO(gpio) -> GPIO\n\n"
	"Return a new GPIO object that is connected to the\n"
	"specified GPIO via sysfs interface.\n");

static PyObject *
GPIO_get_direction( GPIO *self, void *closure )
{
	PyObject * result = ( PyObject * ) self->direction ;
	Py_INCREF( result );
	return result;
}

static int
GPIO_set_direction(GPIO *self, PyObject *val, void *closure)
{
	char * direction;
	char cdir;

	if (val == NULL) {
		PyErr_SetString(PyExc_TypeError,
			"Cannot delete attribute");
		return -1;
	}

	if ( setDirection(self, val) == NULL )
		return -1;

	if ( PyObject_Compare(val, getDirection( self ) ) != 0 ) {
		PyErr_SetString(PyExc_IOError, "setting direction failed");
		return -1;
	}

	self->direction = val;

	return 0;
}


static PyObject *
GPIO_get_trigger( GPIO *self, void *closure )
{
	PyObject * result = ( PyObject * ) self->trigger;
	Py_INCREF( result );
	return result;
}

static int
GPIO_set_trigger(GPIO *self, PyObject *val, void *closure)
{
	char * trigger;
	char cdir;

	if (val == NULL) {
		PyErr_SetString(PyExc_TypeError,
			"Cannot delete attribute");
		return -1;
	}

	if ( setTrigger(self, val) == NULL )
		return -1;

	if ( PyObject_Compare(val, getTrigger( self ) ) != 0 ) {
		PyErr_SetString(PyExc_IOError, "setting trigger failed");
		return -1;
	}

	self->trigger = val;

	return 0;
}

static PyObject *
GPIO_get_callback(GPIO *self, void *closure)
{
	PyObject * result = (PyObject*) self->callback;
	if (result == NULL)
		result = Py_None;
	Py_INCREF(result);
	return result;
}

static int
GPIO_set_callback(GPIO *self, PyObject *val, void *closure)
{
	long ident;
	PyObject * tmp;

	if ( val == Py_None ) {
		printf("deleting attribute \"callback\"; this should stop the polling thread\n");
		tmp = self->callback;
		self->callback = NULL;
		Py_XDECREF( tmp );
		return 0;
	}
	if (self->callback != NULL) {
		tmp = self->callback;
		self->callback = NULL;
		Py_DECREF( tmp );
		Py_BEGIN_ALLOW_THREADS
		PyThread_acquire_lock(self->lock, 1);  /* wait for thread to finish */
		Py_END_ALLOW_THREADS	
	}
	if ( !PyCallable_Check( val ) != 0 ) {
		PyErr_SetString(PyExc_TypeError, "set_callback: parameter must be a callable python object (function, method, class) or None");
		return -1;
	}
	self->callback = val;
	Py_INCREF( self->callback );
	
	if (self->lock == NULL) {
		PyEval_InitThreads(); /* Start the interpreter's thread-awareness */
		self->interp = PyThreadState_Get()->interp;
		self->lock = PyThread_allocate_lock();
	}

	if (self->lock == NULL) 
		return PyErr_NoMemory();
	Py_INCREF(self);  /* t_bootstrap consumes a ref */
	ident = PyThread_start_new_thread(t_bootstrap, (void*) self);
	if (ident == -1) {
		Py_DECREF(self);
		PyErr_SetString(PyExc_RuntimeError, "can't start new thread");
		return -1;
	}
	return 0;
}

//+	{ "none",    0 },
//+	{ "falling", BIT(FLAG_TRIG_FALL) },
//+	{ "rising",  BIT(FLAG_TRIG_RISE) },
//+	{ "both",    BIT(FLAG_TRIG_FALL) | BIT(FLAG_TRIG_RISE) },

static PyGetSetDef GPIO_getset[] = {
	{"value", (getter)GPIO_get_value, (setter)GPIO_set_value,
			"set value if configured as output"},
	{"direction", (getter)GPIO_get_direction, (setter)GPIO_set_direction,
			"Defines GPIO direction (in, out, low or high)"},
	{"trigger", (getter)GPIO_get_trigger, (setter)GPIO_set_trigger,
			"Defines interrupt level (none, falling, rising or both)"},
	{"callback", (getter)GPIO_get_callback, (setter)GPIO_set_callback,
			"set callable python object to be called on interrupt"},
	{NULL},
};

static PyTypeObject GPIO_type = {
	PyObject_HEAD_INIT(NULL)
	0,				/* ob_size */
	"GPIO",				/* tp_name */
	sizeof(GPIO),			/* tp_basicsize */
	0,				/* tp_itemsize */
	(destructor)GPIO_dealloc,	/* tp_dealloc */
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
	GPIO_type_doc,			/* tp_doc */
	0,				/* tp_traverse */
	0,				/* tp_clear */
	0,				/* tp_richcompare */
	0,				/* tp_weaklistoffset */
	0,				/* tp_iter */
	0,				/* tp_iternext */
	0,			/* tp_methods */
	0,				/* tp_members */
	GPIO_getset,			/* tp_getset */
	0,				/* tp_base */
	0,				/* tp_dict */
	0,				/* tp_descr_get */
	0,				/* tp_descr_set */
	0,				/* tp_dictoffset */
	(initproc)GPIO_init,		/* tp_init */
	0,				/* tp_alloc */
	GPIO_new,			/* tp_new */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initgpio(void)
{
	PyObject* m;

	if (PyType_Ready(&GPIO_type) < 0)
		return;

	m = Py_InitModule3("gpio", 0, GPIO_module_doc);

	Py_INCREF(&GPIO_type);
	PyModule_AddObject(m, "GPIO", (PyObject *)&GPIO_type);
}

