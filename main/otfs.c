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
  * @file       /otfs.c
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R100
  * @date       31 Oct 2014
  * @brief      Top Level Functions for libotfs
  *
  * Initialization and other such functions.
  *
  ******************************************************************************
  */

#include "otfs.h"

#include <otstd.h>
#include <otlib.h>

// for malloc
#include <stdlib.h>




static void sub_loadfs(otfs_t* dstfs, void* loadbase, const id_tmpl* user_id) {
    if (dstfs != NULL) {
        dstfs->base    = loadbase;
        dstfs->alloc   = vl_get_fsalloc((vlFSHEADER*)loadbase);
        memcpy(&dstfs->uid.u8[0], user_id->value, user_id->length);
    }
}





int otfs_init(void** handle) {
#if (OT_FEATURE_MULTIFS == ENABLED)
    return vl_multifs_init(handle);
#else
	return 0;
#endif
}

int otfs_deinit(void* handle, void (*free_fn)(void*)) {
#if (OT_FEATURE_MULTIFS == ENABLED)
    if (free_fn != NULL) {
        otfs_t      fs;
        uint64_t    uid = 0;
        int         rc;
        
        rc = otfs_iterator_start(handle, &fs, (uint8_t*)&uid);
        while (rc == 0) {
            if (free_fn != NULL) {
                free_fn(fs.base);
            }
            rc = otfs_iterator_next(handle, &fs, (uint8_t*)&uid);
        }
    }

    return vl_multifs_deinit(handle);
#else

    return 0;
#endif
}


int otfs_load_defaults(void* handle, otfs_t* fs, size_t maxalloc) {
    vlFSHEADER header;
    
    if (fs == NULL) {
        return -1;
    }
    
    //Section between these comments could be refactored
    vworm_fsheader_defload(&header);
    
#	if (OT_FEATURE_MULTIFS == ENABLED)
    fs->alloc = vworm_fsalloc((const vlFSHEADER*)&header);
    if (fs->alloc >= maxalloc) {
        return -2;
    }
    //End of refactorable section
    
    fs->base = malloc(fs->alloc);
    if (fs->base == NULL) {
        return -3;
    }
#	endif
    
    return vworm_fsdata_defload(fs->base, (const vlFSHEADER*)&header);
}



int otfs_new(void* handle, const otfs_t* fs) {
#if (OT_FEATURE_MULTIFS == ENABLED)
    id_tmpl user_id;
    int rc;

    user_id.length  = 8;
    user_id.value   = (uint8_t*)&fs->uid.u8[0];

    rc = vl_multifs_add(handle, (void*)fs->base, (const id_tmpl*)&user_id);
    if (rc != 0) {
        return -rc;
    }

    vworm_init(fs->base, NULL);

#else 
    vworm_init(NULL, NULL);
#endif

    ///@note might be wise to put below features into otfs_setfs() (or a
    ///      subroutine of such that takes a direct pointer).

    // Initialize veelite for this context
    ///@todo veelite will need context input
    vl_init(NULL);

    // Initialize auth_init for this FS context
    auth_init();

    return 0;   
}




int otfs_del(void* handle, const otfs_t* fs, void (*free_fn)(void*)) {
#if (OT_FEATURE_MULTIFS == ENABLED)
    id_tmpl user_id;
    int rc;
    
    if (fs == NULL) {
        return -1;
    }
    
    user_id.length  = 8;
    user_id.value   = (uint8_t*)fs->uid.u8;
    rc              = vl_multifs_del(handle, (const id_tmpl*)&user_id);
    if (rc == 0) {
        if ((free_fn != NULL) && (fs->base != NULL)) {
            free_fn(fs->base);
        }
    }
    
    return rc;
#else
	return 0;
#endif
}



int otfs_setfs(void* handle, otfs_t* fs, const uint8_t* eui64_bytes) {
#if (OT_FEATURE_MULTIFS == ENABLED)
    int rc;
    id_tmpl user_id;
    void* fsbase;
    user_id.length  = 8;
    user_id.value   = (uint8_t*)eui64_bytes;
    
    rc = vl_multifs_switch(handle, (void**)&fsbase, (const id_tmpl*)&user_id);
    if (rc == 0) {
        sub_loadfs(fs, fsbase, &user_id);
    }
    
    return rc;
#else
	return 0;
#endif
}



int otfs_activeuid(void* handle, uint8_t* eui64_bytes) {
#if (OT_FEATURE_MULTIFS == ENABLED)
    id_tmpl user_id;
    user_id.length  = 8;
    user_id.value   = (uint8_t*)eui64_bytes;
    return vl_multifs_activeid(handle, (id_tmpl*)&user_id);

#else
    vlFILE* fp;
    fp = ISF_open_su(1);
    vl_load(fp, 8, eui64_bytes);
    vl_close(fp);
    
    return 0;
#endif
}




int otfs_iterator_start(void* handle, otfs_t* fs, uint8_t* eui64_bytes) {
#if (OT_FEATURE_MULTIFS == ENABLED)
    ot_u8 rc;
    id_tmpl user_id;
    void* fsbase;
    
    user_id.length  = 0;
    user_id.value   = eui64_bytes;

    rc = vl_multifs_start(handle, &fsbase, &user_id);
    if ((rc == 0) && (user_id.length == 8)) {
        sub_loadfs(fs, fsbase, &user_id);
        return 0;
    }
    
    return 1;
    
#else
	return 0;
#endif
}

int otfs_iterator_next(void* handle, otfs_t* fs, uint8_t* eui64_bytes) {
#if (OT_FEATURE_MULTIFS == ENABLED)
    ot_u8 rc;
    id_tmpl user_id;
    void* fsbase;

    user_id.length  = 0;
    user_id.value   = eui64_bytes;

    rc = vl_multifs_next(handle, &fsbase, &user_id);
    if ((rc == 0) && (user_id.length == 8)) {
        sub_loadfs(fs, fsbase, &user_id);
        return 0;
    }
    
    return 1;
    
#else
	return 0;
#endif
}


