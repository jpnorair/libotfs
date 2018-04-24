/* Copyright 2017, JP Norair
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
  * @file       /otsys/veelite_multifs.c
  * @author     JP Norair
  * @version    R100
  * @date       30 Nov 2017
  * @brief      Veelite MultiFS features
  * @ingroup    Veelite
  *
  * Veelite MultiFS is used to clone multiple filesystems on a single device.
  * The MultiFS feature is intended for Gateways that are proxying endpoints.
  *
  ******************************************************************************
  */

#include <otstd.h>

#if (OT_FEATURE(VEELITE) && OT_FEATURE(MULTIFS))

#include <platform/config.h>
#include <otlib/utils.h>
#include <otlib/auth.h>
#include <otsys/veelite.h>


// Veelite init function
#if (CC_SUPPORT == SIM_GCC)
#   include <otplatform.h>
#endif


// For implementation using Judy datatype.
// Intended for larger-scale POSIX system with CPU cache
#include <Judy.h>

static void* fstab = NULL;     // Judy-based FS Table




// For implementation using regular hash table.
//#include <uthash.h>




ot_u8 vl_multifs_init(void** handle) {
    /// Judy seems to be self initializing
    return 0;
}

ot_u8 vl_multifs_deinit(void** handle) {
    unsigned int bytes;
    JHSFA(bytes, fstab);
    return 0;
}


ot_u8 vl_multifs_add(vlFSHEADER* newfs, id_tmpl* fsid) {
    unsigned int* new_value;
    
    {   uint64_t test;
        memcpy(&test, fsid->value, 8);
        printf("--> UID = %016llX\n", test);
        printf("--> Value = %016llX\n", (unsigned int)newfs);
    }
    
    JHSI( new_value, fstab, fsid->value, fsid->length ); 
    
    /// Error on case when out of memory.
    /// 0x05 Veelite error is: "Cannot create file: Supplied length (in header) 
    /// is beyond file limits."  The variant for MultiFS is 0x15.
    if (new_value == PJERR) {
        return 0x15;
    }
    
    /// Error on case when FSID already exists.
    /// 0x02 Veelite error is: "Cannot create file: File ID already exists."
    /// The variant for MultiFS is 0x12.
    if (*new_value == 0) {
        return 0x12;
    }
    
    /// Finally attach the newfs after errors are handled.  It is important to
    /// have the new_value data type be an integer type that is as big as the 
    /// pointer type on the platform.
    *new_value = (unsigned int)newfs;
    
    return 0;
}


ot_u8 vl_multifs_del(id_tmpl* fsid) {
    MCU_TYPE_INT rc;
    
    JHSD(rc, fstab, fsid->value, fsid->length);
    
    /// Error on case when FSID cannot be found.
    /// 0x01 Veelite error is: "Cannot access file: File ID does not exist"
    return (rc == 0) ? 0 : 0x11;
}


ot_u8 vl_multifs_switch(vlFSHEADER* getfs, id_tmpl* fsid) {
    MCU_TYPE_UINT* get_value;
    
    {   uint64_t test;
        memcpy(&test, fsid->value, 8);
        printf("--> fsid->length = %d, fsid->value = %016llX\n", fsid->length, test);
    }
    
    
    /// First, retrieve the FS context based on fsid.
    JHSG(get_value, fstab, fsid->value, fsid->length);
    
    if (get_value == NULL) {
        return 0x11;
    }
    if (getfs != NULL) {
        *getfs = *((vlFSHEADER*)*get_value);
    }
    
    /// Now, switch the context internally so that Veelite interface works with
    /// the new FS. 
    
    return 0;
}





#endif

