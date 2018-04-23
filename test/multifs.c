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
  */
/**
  * @file       /test/multifs.c
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R101
  * @date       20 April 2018
  * @brief      Functional Tests for MultiFS CRUD
  *
  ******************************************************************************
  */


#include <otfs.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Default parameters
#define DEF_INSTANCES_FS    12
#define DEF_FS_ALLOC        2048

typedef struct {
    otfs_t      info;
    uint32_t    data[DEF_FS_ALLOC/4];
} fs_bundle_t;



void sub_hexdump(void* base, size_t bytes, size_t cols) {
    uint8_t* a = (uint8_t*)base;
    
    for (int i=0; i<bytes; ) {
        if ((i%cols) == 0) {
            printf("%05d: ", i);
        }
    
        printf("%02X ", a[i]);
        
        i++;
        if ((i%cols) == 0) {
            printf("\n");
        }
    }
    printf("\n");

}




int main(void) {
    fs_bundle_t* fs;
    int rc;
    
    printf("MultiFS CRUD test\n");
    printf("===============================================================================\n");
    printf("Name of app in use with libotfs: %s\n", LIBOTFS_APP_NAME);
    printf("size of vl_header_t: %zu\n", sizeof(vl_header_t));
    
    srand(time(NULL));
    
    // Allocate fs bundle
    fs = malloc(sizeof(fs_bundle_t) * DEF_INSTANCES_FS);
    if (fs == NULL) {
        fprintf(stderr, "Error: malloc returned NULL (LINE %d)\n", __LINE__-2);
        return -1;
    }
    
    // Add filesystems to OTFS
    for (int i=0; i<DEF_INSTANCES_FS; i++) {
        rc = otfs_defaults(&fs[i].info, DEF_FS_ALLOC);
        if (rc != 0) {
            fprintf(stderr, "Error: otfs_defaults() returned %d (LINE %d)\n", rc, __LINE__-2);
        }
        
        rc = otfs_new(&fs[i].info);
        if (rc != 0) {
            fprintf(stderr, "Error: otfs_new() returned %d (LINE %d)\n", rc, __LINE__-2);
        }
    }
    
    

    
    
    // End: clear memory
    for (int i=0; i<DEF_INSTANCES_FS; i++) {
        rc = otfs_del(&fs[i].info, true);
        if (rc != 0) {
            fprintf(stderr, "Error: otfs_del() returned %d (LINE %d)\n", rc, __LINE__-2);
        }
    }
    
    
    fs_head = (vlFSHEADER*)overhead_files;
    printf("Input FS Header:\n");
    printf("->ftab_alloc    = %d\n", fs_head->ftab_alloc);
#   if (OT_FEATURE(VLACTIONS) == ENABLED)
    printf("->res_act0      = %d\n", fs_head->res_act0);
    printf("->res_act2      = %d\n", fs_head->res_act2);
#   endif
    printf("->gfb.alloc     = %d\n", fs_head->gfb.alloc);
    printf("->gfb.used      = %d\n", fs_head->gfb.used);
    printf("->gfb.files     = %d\n", fs_head->gfb.files);
    printf("->iss.alloc     = %d\n", fs_head->iss.alloc);
    printf("->iss.used      = %d\n", fs_head->iss.used);
    printf("->iss.files     = %d\n", fs_head->iss.files);
    printf("->isf.alloc     = %d\n", fs_head->isf.alloc);
    printf("->isf.used      = %d\n", fs_head->isf.used);
    printf("->isf.files     = %d\n", fs_head->isf.files);
#   if (OT_FEATURE(VLMODTIME) == ENABLED)
    printf("->res_time0     = %d\n", fs_head->res_time0);
    printf("->res_time4     = %d\n", fs_head->res_time4);
#   endif
    
    rc = vworm_init((void*)fs_base, fs_head);
    printf("vworm_init() returned %d\n", rc);
    

    //sub_hexdump(fs_base, sizeof(fs_base), sizeof(vl_header_t));
    //return 0;
    
    printf("STARTING File open limit test\nShould open 3 times, then not open\n");
    test_veelite_maxopen();
    printf("ENDING File open limit test\n\n");
    
    printf("STARTING Open/Close test\n");
    test_veelite_openclose();
    printf("ENDING Open/Close test\n\n");
    
    printf("STARTING Load/Store test\n");
    test_veelite_loadstore();
    printf("ENDING Load/Store test\n\n");
    
    printf("STARTING Memptr test\n");
    test_veelite_memptr();
    printf("ENDING Memptr test\n\n");
    
    return 0;
}

