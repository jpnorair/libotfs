/* Copyright 2017 JP Norair
  *
  * Licensed under the OpenTag License, Version 1.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  * http://www.indigresso.com/wiki/doku.php?id=opentag:license_1_0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  */
/**
  * @file       /otfs.h
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R100
  * @date       31 Oct 2014
  * @brief      Top Level Header for libotfs
  *
  * Only necessary for compiling as libotfs, and when linking to the library.
  *
  * Libotfs is not thread-safe.
  *
  ******************************************************************************
  */

#ifndef __OTFS_H
#define __OTFS_H

/// Std C Headers.  No special rules here
#include <stdint.h>
#include <stddef.h>

/// OpenTag subsystem Headers.  In order for installation of libotfs to work properly,
/// these must use < > brackets and have only single space between #include and <.
/// The installation uses sed to modify these line on the installed copy of this header.

// All headers, for testing.
#include <app/config.h>
#include <otplatform.h>
#include <otstd.h>
#include <otlib.h>
// Main headers we want to expose
#include <otlib/alp.h>
#include <otlib/auth.h>
#include <otsys/veelite.h>




typedef struct OT_PACKED {
    uint8_t oui24[3];
    uint8_t ext40[5];
} otfs_eui64_t;


typedef union {
    uint64_t        u64;
    uint32_t        u32[2];
    uint16_t        u16[4];
    
#   if !defined(__C2000__)
    uint8_t         u8[8];
    otfs_eui64_t    eui64;
#   endif
} otfs_id_union;


typedef struct {
    otfs_id_union   uid;
    void*           base;
    size_t          alloc;
} otfs_t;



int otfs_init(void** handle);

int otfs_deinit(void* handle, void (*free_fn)(void*));


/** @brief Load Application Defaults into an empty FS
  * @param fs   (otfs_t*) pointer to already allocated otfs_t variable to empty fs
  * @retval     (int) returns negative values on error, else size of filesystem in bytes (octets)
  */
int otfs_load_defaults(void* handle, otfs_t* fs, size_t maxalloc);


/** @brief Create a new OTFS instance.
  * @param fs   (const otfs_t*) pointer to already allocated and non-empty otfs_t varable
  * @retval     (int) return zero on success, or non-zero on error
  *
  * The fs variable supplied as the parameter must be a non-empty fs.
  * Using otfs_defaults() before otfs_new() is one way to populate an
  * fs with default values.
  */
int otfs_new(void* handle, const otfs_t* fs);


/** @brief Delete an OTFS instance.
  * @param fs       (const otfs_t*) pointer to already allocated and non-empty otfs_t varable
  * @param free_fn  (void (*)(void*)) Function to free FS subelements, or NULL
  * @retval         (int) return zero on success, or non-zero on error
  */
int otfs_del(void* handle, const otfs_t* fs, void (*free_fn)(void*));


/** @brief Select an FS to do operations on.
  * @param handle (void*) otfs handle
  * @param fs   (otfs_t*) Result Variable for FS, or NULL if you don't want result
  * @retval     (int) return zero on success, or non-zero on error
  *
  */
int otfs_setfs(void* handle, otfs_t* fs, const uint8_t* eui64_bytes);

int otfs_activeuid(void* handle, uint8_t* eui64_bytes);


int otfs_iterator_start(void* handle, otfs_t* fs, uint8_t* eui64_bytes);

int otfs_iterator_next(void* handle, otfs_t* fs, uint8_t* eui64_bytes);


#endif
