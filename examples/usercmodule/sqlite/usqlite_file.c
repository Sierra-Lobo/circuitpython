/*
MIT License

Copyright(c) 2021 Elvin Slavik

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "usqlite.h"

#include "py/objstr.h"
#include "py/objmodule.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/builtin.h"

#include "shared-bindings/os/__init__.h"



#include "stdio.h"
#include <fcntl.h>

#include "py/runtime.h"
#include "py/objstr.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"


#if MICROPY_VFS_FAT
#include "extmod/vfs_fat.h"
#endif

#if MICROPY_VFS_LFS1 || MICROPY_VFS_LFS2
#include "extmod/vfs_lfs.h"
#endif

#if defined(MICROPY_VFS_POSIX) && MICROPY_VFS_POSIX
#include "extmod/vfs_posix.h"
#endif



#if !MICROPY_VFS 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#ifdef _MSC_VER
#include <direct.h> // For mkdir
#endif
#include "py/mpconfig.h"

#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/mphal.h"
#include "py/mpthread.h"
#include "extmod/vfs.h"

#ifdef __ANDROID__
#define USE_STATFS 1
#endif

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 25)
#include <sys/random.h>
#define _HAVE_GETRANDOM
#endif
#endif

#include "py/objstr.h"
typedef struct _mp_obj_listdir_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    DIR *dir;
} mp_obj_listdir_t;
STATIC mp_obj_t listdir_next(mp_obj_t self_in) {
    mp_obj_listdir_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->dir == NULL) {
        goto done;
    }
    MP_THREAD_GIL_EXIT();
    struct dirent *dirent = readdir(self->dir);
    if (dirent == NULL) {
        closedir(self->dir);
        MP_THREAD_GIL_ENTER();
        self->dir = NULL;
    done:
        return MP_OBJ_STOP_ITERATION;
    }
    MP_THREAD_GIL_ENTER();

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
    t->items[0] = mp_obj_new_str(dirent->d_name, strlen(dirent->d_name));

    #ifdef _DIRENT_HAVE_D_TYPE
    #ifdef DTTOIF
    t->items[1] = MP_OBJ_NEW_SMALL_INT(DTTOIF(dirent->d_type));
    #else
    if (dirent->d_type == DT_DIR) {
        t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFDIR);
    } else if (dirent->d_type == DT_REG) {
        t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG);
    } else {
        t->items[1] = MP_OBJ_NEW_SMALL_INT(dirent->d_type);
    }
    #endif
    #else
    // DT_UNKNOWN should have 0 value on any reasonable system
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0);
    #endif

    #ifdef _DIRENT_HAVE_D_INO
    t->items[2] = MP_OBJ_NEW_SMALL_INT(dirent->d_ino);
    #else
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0);
    #endif
    return MP_OBJ_FROM_PTR(t);
}
STATIC mp_obj_t mod_os_ilistdir(size_t n_args, const mp_obj_t *args); 
STATIC mp_obj_t mod_os_ilistdir(size_t n_args, const mp_obj_t *args) {
    const char *path = ".";
    if (n_args > 0) {
        path = mp_obj_str_get_str(args[0]);
    }
    mp_obj_listdir_t *o = m_new_obj(mp_obj_listdir_t);
    o->base.type = &mp_type_polymorph_iter;
    MP_THREAD_GIL_EXIT();
    o->dir = opendir(path);
    MP_THREAD_GIL_ENTER();
    o->iternext = listdir_next;
    return MP_OBJ_FROM_PTR(o);
}

#endif

extern const mp_obj_module_t mp_module_io;

// ------------------------------------------------------------------------------

bool usqlite_file_exists(const char *pathname) {
	/*
    mp_obj_t os = mp_module_get_loaded_or_builtin(MP_QSTR_os);
    mp_obj_t ilistdir = usqlite_method(os, MP_QSTR_listdir);
	*/
	//usqlite_printf("exists called %s\n", pathname);
    char path[MAXPATHNAME + 1];
    strcpy(path, pathname);
    const char *filename = pathname;

    char *lastSep = strrchr(path, '/');
    if (lastSep) {
        *lastSep++ = 0;
        filename = lastSep;
    } else {
        lastSep = strrchr(path, '\\');
        if (lastSep) {
            *lastSep++ = 0;
            filename = lastSep;
        } else {
            path[0] = '.';
            path[1] = 0;
        }
    }
	
    bool exists = false;
	size_t n_args = 1;
	mp_obj_t args[] = {mp_obj_new_str(path, strlen(path))};
#if MICROPY_VFS
	mp_obj_t listdir = mp_vfs_ilistdir(n_args, args); //mp_call_function_1(listdir, mp_obj_new_str(path, strlen(path)));
#else
	mp_obj_t listdir = mod_os_ilistdir(n_args, args);
#endif
	mp_obj_t entry = mp_iternext(listdir);

    while (entry != MP_OBJ_STOP_ITERATION) {
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(entry);

        int type = mp_obj_get_int(t->items[1]);
        if (type == 0x8000) {
            const char *name = mp_obj_str_get_str(t->items[0]);
            if ((exists = (strcmp(filename, name) == 0))) {
                break;
            }
        }

        entry = mp_iternext(listdir);
    }

    return exists;
}

// ------------------------------------------------------------------------------
STATIC mp_vfs_mount_t *lookup_path(mp_obj_t path_in, mp_obj_t *path_out) {
    const char *path = mp_obj_str_get_str(path_in);
    const char *p_out;
    *path_out = mp_const_none;
    mp_vfs_mount_t *vfs = mp_vfs_lookup_path(path, &p_out);
    if (vfs != MP_VFS_NONE && vfs != MP_VFS_ROOT) {
        *path_out = mp_obj_new_str_of_type(mp_obj_get_type(path_in),
            (const byte *)p_out, strlen(p_out));
    }
    return vfs;
}

int usqlite_file_open(MPFILE *file, const char *pathname, int flags) {
	LOGFUNC;
	//usqlite_printf("open called %s\n", pathname);

    mp_obj_t filename = mp_obj_new_str(pathname, strlen(pathname));

    char mode[8];
    memset(mode, 0, sizeof(mode));
    char *pMode = mode;

    if (flags & SQLITE_OPEN_CREATE) {
        if (!usqlite_file_exists(pathname)) {
            *pMode++ = 'w';
			*pMode++ = 'b';
			*pMode++ = '+';
        }
		else {
			*pMode++ = 'r';
			*pMode++ = 'b';
			*pMode++ = '+';
		}
		//	mp_raise_ValueError(MP_ERROR_TEXT("flagss 2"));

        //*pMode++ = '+';
    } else if (flags & SQLITE_OPEN_READWRITE) {
			mp_raise_ValueError(MP_ERROR_TEXT("flagss 3"));
        *pMode++ = 'r';
        *pMode++ = '+';
    } else if (flags & SQLITE_OPEN_READONLY) {
			mp_raise_ValueError(MP_ERROR_TEXT("flagss 4"));
        *pMode++ = 'r';
    } else {
			mp_raise_ValueError(MP_ERROR_TEXT("flagss 5"));
        *pMode++ = 'r';
    }


    mp_obj_t filemode = mp_obj_new_str(mode, strlen(mode));



//	mp_raise_TypeError(MP_ERROR_TEXT("cannot open file1"));
	#if MICROPY_VFS
	mp_vfs_mount_t *vfs = lookup_path(filename, &filename);
   	#endif
	mp_obj_t meth[2 + 2];
	mp_obj_t args[2] = {filename, filemode};
	size_t n_args = 2;
//	mp_raise_ValueError(MP_ERROR_TEXT("called load method..."));
   	#if MICROPY_VFS
	mp_load_method(vfs->obj, MP_QSTR_open, meth);
	memcpy(meth + 2, args, n_args * sizeof(*args));
	file->stream = mp_call_method_n_kw(n_args, 0, meth);
	#else
    file->stream = mp_builtin_open(2, args, NULL);//mp_call_method_n_kw(n_args, 0, meth);
	#endif
	strcpy(file->pathname, pathname);
	file->flags = flags;
	return SQLITE_OK;







/*
    mp_obj_t open = usqlite_method(&mp_module_io, MP_QSTR_open);
    file->stream = mp_call_function_2(open, filename, filemode);
    strcpy(file->pathname, pathname);
    file->flags = flags;
*/

    return SQLITE_OK;
}

/*
mp_obj_t args[2] =
{
    filename,
    filemode
};

file->stream = mp_builtin_open(2, args, NULL);
*/

// ------------------------------------------------------------------------------

int usqlite_file_close(MPFILE *file) {
    LOGFUNC;

	//usqlite_printf("close called\n");
    if (file->stream) {
        usqlite_logprintf(___FUNC___ " %s\n", file->pathname);

        mp_stream_close(file->stream);
        file->stream = NULL;

        if (file->flags & SQLITE_OPEN_DELETEONCLOSE) {
            usqlite_file_delete(file->pathname);
        }
    }

    return SQLITE_OK;
}

// ------------------------------------------------------------------------------

int usqlite_file_read(MPFILE *file, void *pBuf, size_t nBuf) {
    LOGFUNC;

	//usqlite_printf("read called\n");
    int error = 0;
    mp_uint_t size = mp_stream_rw(file->stream, pBuf, nBuf, &error, MP_STREAM_RW_READ);
    if (size != nBuf) {
        usqlite_errprintf("write error: %d", error);
    }
	//usqlite_printf("read no error\n");

    return size;
}

// ------------------------------------------------------------------------------

int usqlite_file_write(MPFILE *file, const void *pBuf, size_t nBuf) {
    LOGFUNC;

	//usqlite_printf("write called %d\n", nBuf);
    int error = 0;

    mp_uint_t size = mp_stream_rw(file->stream, (void *)pBuf, nBuf, &error, MP_STREAM_RW_WRITE);
    if (size != nBuf) {
        usqlite_errprintf("write error: %d", error);
    }
	//usqlite_printf("write no error\n");

    return size;
}

// ------------------------------------------------------------------------------

int usqlite_file_flush(MPFILE *file) {
    LOGFUNC;

	//usqlite_printf("flush called\n");
    const mp_stream_p_t *stream = mp_get_stream(file->stream);

    int error = 0;
    mp_uint_t result = stream->ioctl(file->stream, MP_STREAM_FLUSH, 0, &error);
    if (result == MP_STREAM_ERROR) {
        usqlite_errprintf("flush error: %d", error);

        return error;
    }

    return 0;
}

// ------------------------------------------------------------------------------

int usqlite_file_seek(MPFILE *file, int offset, int origin) {
    LOGFUNC;

	//usqlite_printf("seek called offset %d origin %d\n", offset, origin);
    struct mp_stream_seek_t seek;

    seek.offset = offset;
    seek.whence = origin;

    const mp_stream_p_t *stream = mp_get_stream(file->stream);

    int error;
    mp_uint_t result = stream->ioctl(file->stream, MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek, &error);
    if (result == MP_STREAM_ERROR) {
        usqlite_errprintf("seek error: %d", error);
        return -1;
    }
	//usqlite_printf("seek no error offset %d origin %d\n", offset, origin);

    return seek.offset;
}

// ------------------------------------------------------------------------------

int usqlite_file_tell(MPFILE *file) {
    LOGFUNC;

    return usqlite_file_seek(file, 0, MP_SEEK_CUR);
}

// ------------------------------------------------------------------------------

int usqlite_file_delete(const char *pathname) {
    LOGFUNC;

	//usqlite_printf("delete called %s\n", pathname);
    //usqlite_logprintf("%s: %s\n", __func__, pathname);

    mp_obj_t filename = mp_obj_new_str(pathname, strlen(pathname));
    //mp_obj_t remove = usqlite_method(mp_module_get_loaded_or_builtin(MP_QSTR_uos), MP_QSTR_remove);
    //mp_call_function_1(remove, filename);

    return SQLITE_OK;
}

// ------------------------------------------------------------------------------
/*
static mp_obj_t fileIoctl(MPFILE* file, size_t n_args, const mp_obj_t* args)
{
    mp_buffer_info_t bufinfo;
    uintptr_t val = 0;
    if (n_args > 2) {
        if (mp_get_buffer(args[2], &bufinfo, MP_BUFFER_WRITE)) {
            val = (uintptr_t)bufinfo.buf;
        }
        else {
            val = mp_obj_get_int_truncated(args[2]);
        }
    }

    const mp_stream_p_t* stream_p = mp_get_stream(args[0]);
    int error;
    mp_uint_t res = stream_p->ioctl(args[0], mp_obj_get_int(args[1]), val, &error);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(error);
    }

    return mp_obj_new_int(res);
}
*/

// -----------------------------------------------------------------------------
