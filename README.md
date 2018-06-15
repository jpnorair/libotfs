# libotfs
**libotfs** is a _library_ for implementing an OpenTag Filesystem (OTFS) on a system implementing Standard C (STDC) and POSIX interfaces.  libotfs implements OTFS in a way that is binary-compatible with the normal, embedded OpenTag implementation.  Therefore, a binary image of an OTFS filesystem can work equally with an embedded implementation of OpenTag as well as a POSIX implementation of libotfs.

## Building

### QUICKSTART

Use `make deps` to build the dependencies, then you can build libotfs.

```
make deps
make lib
make install
```

### Making Dependencies

libotfs is part of the H-Builder middleware package, and it has dependencies that are also part of H-Builder.  The dependencies include the list below.  There are no sub-dependencies: both libjudy and OTEAX are completely self-contained.

* **libjudy** : Compact Judy Array Implementation
* **OTEAX** : OpenTag EAX implementation (cryptography)

The dependency code packages should be in the same root directory as the libotfs code package.  See the example below.  **_hbpkg** and **_hbsys** are directories that hold the compiled code packages.

```
+ repository
+-- _hbpkg
+-- _hbsys
+-- libjudy
+-- libotfs
+-- OTEAX
```

Within the libotfs directory, you can build and install the dependencies with `make deps` (see example below of what your shell might look like).  If these dependencies have been already built and installed into the H-Builder packaging system, you can skip this stage.

```
user@host ~/repository/libotfs> make deps
```
When this is done, you should be able to look into the \_hbpkg and \_hbsys directories to see the results of the dependency installations.

### Making libotfs

If the dependencies are built and installed, you can then build and install libotfs.  The process is very simple.

```
user@host ~/repository/libotfs> make lib
user@host ~/repository/libotfs> make install
```

The `make all` command will build the lib together with any unit & functional tests that accompany libotfs.

## Top Level OTFS API

The top level API is exposed in otfs.h.  This is the interface a user application will call in order to:

* manipulate a group of filesystems
* manipulate a single filesystem as an object-like unit.

OTFS manages a group of filesystems.  The group is implemented as a Judy Array (similar to a hash table).  You can add and remove filesystems from the group.  You can checkout a filesystem in order to do low-level accesses within the filesystem.

### otfs_init

**int otfs_init(void\*\* handle);**

Initialize an OTFS filesystem table, which also allocates basic table memory.  It will return a (void*) handle, which may be used in other OTFS calls to specify the GROUP of filesystems.  It is OK to use NULL, which will operate from an internal filesystem group handle.

### otfs_deinit

**int otfs_deinit(void* handle);**

Deinitialize an OTFS filesystem table, which also frees **all** the memory.  You should use the (void*) handle returned from otfs_init(), or pass NULL if you're using the NULL handle.

### otfs_load_defaults

**int otfs_load_defaults(void* handle, otfs_t* fs, size_t maxalloc);**

Load default filesystem data into a filesystem object (otfs_t*) and also allocate memory for the new filesystem.  The Default filesystem data is determined at the compile-time of libotfs itself, and it is stored within libotfs.  maxalloc may be provided to by the user -- if the filesystem is bigger than this, the function will return an error without doing any allocation or loading.

### otfs_new

**int otfs_new(void* handle, const otfs_t* fs);**

Add a new filesystem to the group of filesystems.  The group is specified by (void\*) handle.  The new filesystem is specified by (otfs_t\*) fs.  The user should call otfs_load_defaults() first, to create a new fs, or the user can build an fs himself if he knows how.

### otfs_del

**int otfs_del(void* handle, const otfs_t* fs, bool unload);**

This is the opposite of otfs_new().  If you specify unload==true, this call will also deallocate (free) all memory used by the filesystem element.

### otfs_setfs

**int otfs_setfs(void* handle, const uint8_t* eui64_bytes);**

This is the function to use for "checking out" a filesystem within the filesystem group.  You specify the FS to checkout based on the eui64_bytes parameter, which is a 64 bit identifier (this is part of the otfs_t struct that is used to specify an FS).  The checkout process returns nothing.  Internally, OTFS is configured such that all low-level calls will be done on the filesystem selected by this function (specified by 64 bit ID).  Calling otfs_setfs() on a different ID is the only way to change the context so that low-level calls begin to operate on a different filesystem.

## OTFS Usage Examples

* The tests from libotfs are one source of usage examples for the OTFS code.
* The **otdb** project also uses libotfs.
* The **smut** project also uses libotfs.

## Low-Level APIs

libotfs implements a subset of OpenTag, and it exposes many of the OpenTag features from **otlib** and **otsys**.  

The Low-Level filesystem access features are implemented in the  **Veelite** interface.

Other features from **otlib** and **otsys** exist mainly because the Veelite module requires them, but the user may use them independently if he desires.


