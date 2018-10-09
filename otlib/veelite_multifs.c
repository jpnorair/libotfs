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


// For implementation using Judy datatype via libjudy (part of hbuilder pkg)
// Intended for larger-scale POSIX system
#include <judy.h>

static void* fstab = NULL;     // Judy-based FS Table




ot_u8 vl_multifs_init(void** new_handle) {
    void* obj;
    obj = judy_open(2, 0);
    
    if (new_handle != NULL) {
        *new_handle = obj;
    }
    else {
        fstab = obj;
    }
    
    return 0;
}


ot_u8 vl_multifs_deinit(void* handle) {
    void* obj;
    obj = (handle != NULL) ? handle : fstab;
    judy_close(obj);
    return 0;
}


ot_u8 vl_multifs_add(void* handle, void* newfsbase, const id_tmpl* fsid) {
    void* obj;
    MCU_TYPE_UINT* new_value;
    
    //{   uint64_t test;
    //    ot_memcpy(&test, fsid->value, 8);
    //    printf("--> UID = %016llX\n", test);
    //    printf("--> Value = %016llX\n", (unsigned int)newfsbase);
    //}
    
    obj         = (handle != NULL) ? handle : fstab;
    new_value   = judy_cell(obj, fsid->value, fsid->length);
    
    /// Error on case when out of memory.
    /// 0x05 Veelite error is: "Cannot create file: Supplied length (in header) 
    /// is beyond file limits."  The variant for MultiFS is 0x15.
    if (new_value == NULL) {
        return 0x15;
    }
    
    /// Error on case when FSID already exists.
    /// 0x02 Veelite error is: "Cannot create file: File ID already exists."
    /// The variant for MultiFS is 0x12.
    if (*new_value != 0) {
        return 0x12;
    }
    
    /// Finally attach the newfs after errors are handled.  It is important to
    /// have the new_value data type be an integer type that is as big as the 
    /// pointer type on the platform.
    //printf("--> Judy Value = %016llX\n", (MCU_TYPE_UINT)newfsbase);
    *new_value = (MCU_TYPE_UINT)newfsbase;
    
    return 0;
}


ot_u8 vl_multifs_del(void* handle, const id_tmpl* fsid) {
    void* obj;
    MCU_TYPE_UINT* val;
    ot_u8 rc;
    
    obj = (handle != NULL) ? handle : fstab;
    val = judy_slot(obj, fsid->value, fsid->length);
    
    if (val != NULL) {
        judy_del(obj);
        rc = 0;
    }
    else {
        /// Error on case when FSID cannot be found.
        /// 0x01 Veelite error is: "Cannot access file: File ID does not exist"
        rc = 0x11;
    }

    return rc;
}


ot_u8 vl_multifs_switch(void* handle, void** getfsbase, const id_tmpl* fsid) {
    void* obj;
    MCU_TYPE_UINT* val;
    ot_u8 rc;
    
    obj = (handle != NULL) ? handle : fstab;
    val = judy_slot(obj, fsid->value, fsid->length);
    
    //{   uint64_t test;
    //    ot_memcpy(&test, fsid->value, 8);
    //    printf("--> fsid->length = %d, fsid->value = %016llX\n", fsid->length, test);
    //    printf("--> Value = %016llX\n", (MCU_TYPE_UINT)val);
    //}
    
    if (val == NULL) {
        rc = 0x11;
    }
    if (getfsbase != NULL) {
        *getfsbase = (void*)*val;
        //printf("--> Judy Value = %016llX\n", (MCU_TYPE_UINT)*getfsbase);
        
        /// Now, switch the context internally so that Veelite interface works with
        /// the new FS.
        vworm_init(*getfsbase, NULL);
        vl_init(NULL);
        rc = 0;
    }
    
    return rc;
}




// void* vl_multifs_spawn(void* handle) {
//     void* obj;
//     
//     obj = (handle != NULL) ? handle : fstab;
//     return (void*)judy_clone((Judy*)obj);
// }
// 
// void vl_multifs_term(void* spawn) {
//     judy_close((Judy*)spawn);
// }

ot_u8 vl_multifs_start(void* handle, void** getfsbase) {
    ot_u8 null_id[1] = {0};
    void* obj;
    MCU_TYPE_UINT* val;
    ot_u8 rc;
    
    obj = (handle != NULL) ? handle : fstab;
    val = judy_strt( (Judy*)obj, (const unsigned char*)null_id, 1);
    
    if (val == NULL) {
        rc = 0x11;
    }
    if (getfsbase != NULL) {
        *getfsbase = (void*)*val;
        vworm_init(*getfsbase, NULL);
        vl_init(NULL);
        rc = 0;
    }
    
    return rc;
}

ot_u8 vl_multifs_next(void* handle, void** getfsbase) {
    void* obj;
    MCU_TYPE_UINT* val;
    ot_u8 rc;
    
    obj = (handle != NULL) ? handle : fstab;
    val = judy_nxt((Judy*)obj);
    
    if (val == NULL) {
        rc = 0x11;
    }
    if (getfsbase != NULL) {
        *getfsbase = (void*)*val;
        vworm_init(*getfsbase, NULL);
        vl_init(NULL);
        rc = 0;
    }
    
    return rc;
}




#endif

