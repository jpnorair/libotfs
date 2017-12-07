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
#if (OT_FEATURE(MULTIFS))
#   include "Judy.h"
#endif

int otfs_defaults(otfs_t* fs, size_t maxalloc) {
    
    

}


int otfs_new(const otfs_t* fs) {
    const vlFSHEADER* fsheader;
    id_tmpl user_id;
    
    // The filesystem header is at the front of the fs section
    fsheader = fs->fs_base;

    vworm_init(fsheader);
    
#   if (OTFS_FEATURE_MULTIFS)
    vl_multifs_init(NULL);

    user_id.length  = 8;
    user_id.value   = fs->fs_id.u8;
    if (vl_multifs_add(fsheader, &user_id) != 0) {
        return -1;
    }
#   endif

    ///@note might be wise to put below features into otfs_setfs() (or a
    ///      subroutine of such that takes a direct pointer).
    
    // Initialize veelite for this context
    ///@todo veelite will need context input
    vl_init(NULL);
    
    // Initialize auth_init for this FS context
    auth_init();
    
    return 0;   
}


