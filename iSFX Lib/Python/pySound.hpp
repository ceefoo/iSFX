#include <Python.h>
//#include "structmember.h"

// iSFX C++ Header
#include "iSFX.hpp"

// C Python Headers
#include "pySystem.hpp"

// Standard Headers
#include <iostream>

struct Sound {
    PyObject_HEAD
    iSFX::Sound* _sound;
};

static void
Sound_dealloc(Sound* self)
{
  delete self->_sound;
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
Sound_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Sound *self;

    self = (Sound *)type->tp_alloc(type, 0);
    if (self != NULL) {
      self->_sound = NULL;
      //self->update = boost::signals2::signal<void(unsigned int)>();
    }

    return (PyObject *)self;
}

static int
Sound_init(Sound *self, PyObject *args, PyObject *kwds)
{
  const char *sound_path;
  System* system;
  FMOD_RESULT res;
  if (!PyArg_ParseTuple(args, "Os", &system, &sound_path))
      return NULL;
  self->_sound = new iSFX::Sound(system->_system, sound_path);
}


static PyMemberDef Sound_members[] = {
    {NULL}  /* Sentinel */
};

static PyObject * Sound_Play(Sound* self) { self->_sound->Play(); Py_RETURN_NONE; }
static PyObject * Sound_Pause(Sound* self) { self->_sound->Pause(); Py_RETURN_NONE; }
static PyObject * Sound_Stop(Sound* self) { self->_sound->Stop(); Py_RETURN_NONE; }
static PyObject * Sound_GetName(Sound* self) { return PyUnicode_FromString(self->_sound->GetName().c_str()); }
static PyObject * Sound_MemoryUsed(Sound* self) { return PyLong_FromLong(self->_sound->MemoryUsed()); }
static PyObject * Sound_TogglePaused(Sound* self) { self->_sound->TogglePaused(); Py_RETURN_NONE; }
static PyObject * Sound_GetPCMData(Sound* self) { self->_sound->GetPCMData(); Py_RETURN_NONE; }
static PyObject * Sound_Fade(Sound* self, PyObject *args, PyObject *keywds) {
  double dv;
  unsigned int dt;
  const char* method = NULL;
  const char* then = NULL;
  static char *kwlist[] = {"dv", "dt", "method", "then", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, keywds, "dI|ss", kwlist, 
                                       &dv, &dt, &method, &then))
    return NULL;
    
  unsigned int m = 0;
  unsigned int t = 0;
  if (method != NULL) {
    std::string s (method);
  }
  if (then != NULL) {
    std::string s (then);
    if (s == "stop") {
      t = 1;
    } else if (s == "pause") {
      t = 2;
    }
  }
  self->_sound->Fade(dv, dt, m, t);
  Py_RETURN_NONE;
}

static PyMethodDef Sound_methods[] = {
    {"play", (PyCFunction)Sound_Play, METH_NOARGS, "Plays a sound" }, 
    {"stop", (PyCFunction)Sound_Stop, METH_NOARGS, "Stops a sound" },
    {"pause", (PyCFunction)Sound_Pause, METH_NOARGS, "Pauses a sound" },
    {"getName", (PyCFunction)Sound_GetName, METH_NOARGS, "Gets the sound's name" },
    {"memoryUsed", (PyCFunction)Sound_MemoryUsed, METH_NOARGS, "Returns the memory used by the sound" },
    {"togglePaused", (PyCFunction)Sound_TogglePaused, METH_NOARGS, "Toggles pause/play" },
    {"fade", (PyCFunction)Sound_Fade, METH_VARARGS | METH_KEYWORDS, "Fades the sound" },
    {"getPCMData", (PyCFunction)Sound_GetPCMData, METH_NOARGS, "Get PCM Data" },
    {NULL}  /* Sentinel */
};

static PyTypeObject SoundType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iSFX.Sound",             /* tp_name */
    sizeof(Sound),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Sound_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Sound objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Sound_methods,             /* tp_methods */
    Sound_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Sound_init,      /* tp_init */
    0,                         /* tp_alloc */
    Sound_new,                 /* tp_new */
};