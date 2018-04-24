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

// MULTIFS feature stores multiple filesystems keyed on 64 bit IDs.
// It uses the "Judy" library, which is used as a sort of growable Hash Table.
//#if (OT_FEATURE(MULTIFS))
//#   include "Judy.h"
//#endif


int otfs_init(void** handle) {
    return vl_multifs_init(handle);
}

int otfs_deinit(void** handle) {
    return vl_multifs_deinit(handle);
}


int otfs_load_defaults(otfs_t* fs, size_t maxalloc) {
    vlFSHEADER header;
    
    if (fs == NULL) {
        return -1;
    }
    
    vworm_fsheader_defload(&header);
    fs->alloc = vworm_fsalloc((const vlFSHEADER*)&header);
    if (fs->alloc >= maxalloc) {
        return -2;
    }
    
    fs->base = malloc(fs->alloc);
    if (fs->base == NULL) {
        return -3;
    }
    
    return vworm_fsdata_defload(fs->base, (const vlFSHEADER*)&header);
}



int otfs_new(const otfs_t* fs) {
#if (OT_FEATURE_MULTIFS == ENABLED)
    id_tmpl user_id;
    const vlFSHEADER* fsheader;
    int rc;

    // The filesystem header is at the front of the fs section
    fsheader = fs->base;
    
    vworm_init(fs->base, fsheader);

    user_id.length  = 8;
    user_id.value   = fs->uid.u8;
    
    rc = vl_multifs_add(fsheader, &user_id);
    if (rc != 0) {
        return -rc;
    }

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




int otfs_del(const otfs_t* fs, bool unload) {
    id_tmpl user_id;
    int rc;
    
    if (fs == NULL) {
        return -1;
    }
    
    user_id.length  = 8;
    user_id.value   = fs->uid.u8;
    rc              = vl_multifs_del(&user_id);
    if (rc == 0) {
        if ((unload == true) && (fs->base != NULL)) {
            free(fs->base);
        }
    }
    
    return rc;
}



int otfs_setfs(const uint8_t* eui64_bytes) {
    id_tmpl user_id;
    vlFSHEADER getfs;
    
    user_id.length  = 8;
    user_id.value   = eui64_bytes;
    
    return vl_multifs_switch(&getfs, &user_id);
}





