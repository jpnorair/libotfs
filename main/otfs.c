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



int otfs_init(void** handle) {
    return vl_multifs_init(handle);
}

int otfs_deinit(void* handle) {
    return vl_multifs_deinit(handle);
}


int otfs_load_defaults(void* handle, otfs_t* fs, size_t maxalloc) {
    vlFSHEADER header;
    
    if (fs == NULL) {
        return -1;
    }
    
    //Section between these comments could be refactored
    vworm_fsheader_defload(&header);
    
    fs->alloc = vworm_fsalloc((const vlFSHEADER*)&header);
    if (fs->alloc >= maxalloc) {
        return -2;
    }
    //End of refactorable section
    
    fs->base = malloc(fs->alloc);
    if (fs->base == NULL) {
        return -3;
    }
    
    return vworm_fsdata_defload(fs->base, (const vlFSHEADER*)&header);
}



int otfs_new(void* handle, const otfs_t* fs) {
#if (OT_FEATURE_MULTIFS == ENABLED)
    id_tmpl user_id;
    int rc;

    user_id.length  = 8;
    user_id.value   = fs->uid.u8;
    
    rc = vl_multifs_add(handle, (void*)fs->base, &user_id);
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




int otfs_del(void* handle, const otfs_t* fs, bool unload) {
    id_tmpl user_id;
    int rc;
    
    if (fs == NULL) {
        return -1;
    }
    
    user_id.length  = 8;
    user_id.value   = fs->uid.u8;
    rc              = vl_multifs_del(handle, &user_id);
    if (rc == 0) {
        if ((unload == true) && (fs->base != NULL)) {
            free(fs->base);
        }
    }
    
    return rc;
}



int otfs_setfs(void* handle, const uint8_t* eui64_bytes) {
    id_tmpl user_id;
    void* getfs;
    
    user_id.length  = 8;
    user_id.value   = eui64_bytes;
    
    return vl_multifs_switch(handle, &getfs, &user_id);
}





