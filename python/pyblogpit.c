#include <Python.h>
#include <blogpit.h>

static PyObject *BlogpitError = NULL;


//
// Blogpit type
//
//
typedef struct {
	PyObject_HEAD
	struct blogpit *B;
} blogpit_Object;


/*
 * Blogpit.__init__(path, branch)
 */
static int
__blogpit_init(blogpit_Object *self, PyObject *args, PyObject *kwds)
{
	self->B = NULL;

	char *p, *b;
	if ( !PyArg_ParseTuple(args, "ss", &p, &b) ) {
		return -1;
	}
	
	struct blogpit *pit = blogpit_open(p, b);

	if ( pit == NULL ) {
		PyErr_SetString( BlogpitError, "Unable to open repository");
		return -1;
	}
	self->B = pit;

	return 0;
}

static PyObject*
__blogpit_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	blogpit_Object *self;

	self = (blogpit_Object*)type->tp_alloc(type, 0);
	if ( self != NULL ) {
		self->B = NULL;
	}

	return (PyObject *)self;
}

static PyObject*
__blogpit_repr(blogpit_Object *self)
{
	return PyString_FromFormat("Blogpit");
}

static void
__blogpit_dealloc(blogpit_Object *self)
{
	if ( self->B != NULL ) {
		blogpit_close(self->B);
		self->B = NULL;
	}
}

static PyObject*
__blogpit_getarticle(blogpit_Object *self, PyObject *args)
{
	char *path;
	if ( !PyArg_ParseTuple(args, "s", &path) ) {
		return NULL;
	}

	size_t len;
	void *data = blogpit_getarticle(self->B, path, &len);

	PyObject *str;
	if ( data ) {
		str = PyUnicode_FromStringAndSize(data, len);
		free(data);
	} else {
		str = PyUnicode_FromString("");
	}
	return str;
}

static PyObject*
__blogpit_sections(blogpit_Object *self, PyObject *args)
{
	char *path;
	if ( !PyArg_ParseTuple(args, "s", &path) ) {
		return NULL;
	}

	char **sections = blogpit_sections(self->B, path);
	int i;
	ssize_t count=0;
	for ( i=0; sections[i]; i++ ) {
		count++;
	}

	PyObject *list = PyList_New(count);
	for ( i=0; sections[i]; i++ ) {
		PyList_SetItem(list, i, PyUnicode_FromString(sections[i]));
		free(sections[i]);
	}
	free(sections);

	return list;
}

static PyObject*
__blogpit_articles(blogpit_Object *self, PyObject *args)
{
	char *path;
	if ( !PyArg_ParseTuple(args, "s", &path) ) {
		return NULL;
	}

	char **articles = blogpit_articles(self->B, path);
	int i;
	ssize_t count=0;
	for ( i=0; articles[i]; i++ ) {
		count++;
	}

	PyObject *list = PyList_New(count);
	for ( i=0; articles[i]; i++ ) {
		PyList_SetItem(list, i, PyUnicode_FromString(articles[i]));
		free(articles[i]);
	}
	free(articles);
	
	return list;
}

static PyObject*
__blogpit_version(blogpit_Object *self)
{
	char *version = blogpit_version(self->B);
	return PyUnicode_FromString(version);
}

static PyObject*
__blogpit_setarticle(blogpit_Object *self, PyObject *args)
{
	char *path;
	char *content = NULL;
	int content_len;
	char *msg = NULL;
	if ( !PyArg_ParseTuple(args, "s" "es#" "|es", 
				&path, 
				"utf-8", &content, &content_len, 
				"utf-8", &msg) ) {
		return NULL;
	}

	// Cast (size_t) is needed
	if ( blogpit_setarticle(self->B, path, content, (size_t)content_len, msg) != 0 ) {
		PyMem_Free(content);
		Py_RETURN_FALSE;
	}

	PyMem_Free(content);


	Py_RETURN_TRUE;
}

static PyMethodDef blogpit_methods[] = {
	{"getarticle", (PyCFunction) __blogpit_getarticle, METH_VARARGS, "Get the article at the given path."},
	{"setarticle", (PyCFunction) __blogpit_setarticle, METH_VARARGS, "Set article content."},
	{"sections", (PyCFunction) __blogpit_sections, METH_VARARGS, "Get the sections names at a given path."},
	{"articles", (PyCFunction) __blogpit_articles, METH_VARARGS, "Get the article names at a given path."},
	{"version", (PyCFunction) __blogpit_version, METH_NOARGS, "Get the id of the current version."},
	{NULL, NULL, 0, NULL} // Sentinel
};

static PyTypeObject blogpit_Type = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"pyblogpit.Blogpit",       /*tp_name*/
	sizeof(blogpit_Object),    /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	__blogpit_dealloc,           /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	__blogpit_repr,              /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,        /*tp_flags*/
	"Blogpit objects",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	blogpit_methods,             /* tp_methods */
	0,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)__blogpit_init,      /* tp_init */
	0,                         /* tp_alloc */
	__blogpit_new,                 /* tp_new */
};

//
// blogpit module
//

PyMethodDef moduleMethods[] = {
	{NULL, NULL},
};

void initpyblogpit()
{
	PyObject *m;

	if ( PyType_Ready(&blogpit_Type) < 0 ) {
		return;
	}

	m = Py_InitModule("pyblogpit", moduleMethods);
	if ( m == NULL ) {
		return;
	}

	// Exception type
	BlogpitError = PyErr_NewException("pyblogpit.error", NULL, NULL);
	Py_INCREF(BlogpitError);
	PyModule_AddObject(m, "error", BlogpitError);

	// Blogpit type
	Py_INCREF(&blogpit_Type);
	PyModule_AddObject(m, "Blogpit", (PyObject *)&blogpit_Type);
}

