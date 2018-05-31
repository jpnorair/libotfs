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
  * @file       /test/time.c
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

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

// Default parameters
#define DEF_INSTANCES_FS    12
#define DEF_FS_ALLOC        2048






int main(void) {
    otfs_t* fs;
    int rc;
    
    printf("Time features test\n");
    printf("===============================================================================\n");
    printf("Name of app in use with libotfs: %s\n", LIBOTFS_APP_NAME);
    printf("Time Enabled?                    %s\n", sub_yesno(OT_FEATURE_TIME == ENABLED));
    printf("size of vl_header_t:             %zu\n", sizeof(vl_header_t));
    
    srand(time(NULL));
    
    // Allocate fs bundle
    fs = malloc(sizeof(otfs_t) * DEF_INSTANCES_FS);
    if (fs == NULL) {
        fprintf(stderr, "%sError: malloc returned NULL (LINE %d)\n", KRED, __LINE__-2);
        return -1;
    }
    
    otfs_init(NULL);
    
    // Add the UIDs to the Filesystems.  This should be done first.
    // They are random numbers that are backchecked against the previous ones.
    for (int i=0; i<DEF_INSTANCES_FS; i++) {
        arc4random_buf(&fs[i].uid.u64, 8);
        for (int j=0; j<i; j++) {
            if (fs[i].uid.u64 == fs[j].uid.u64) {
                i--;
                break;
            }
        }
    }
    
    // Add filesystems to OTFS
    for (int i=0; i<DEF_INSTANCES_FS; i++) {
        rc = otfs_load_defaults(NULL, &fs[i], DEF_FS_ALLOC);
        if (rc < 0) {
            fprintf(stderr, "%sError: otfs_defaults() returned %d (LINE %d)\n", KRED, rc, __LINE__-2);
        }
        else {
            rc = otfs_new(NULL, &fs[i]);
            if (rc != 0) {
                fprintf(stderr, "%sError: otfs_new() returned %d (LINE %d)\n", KRED, rc, __LINE__-2);
            }
            else {
                printf("Device FS [%016llX] Added, %d bytes (%d/%d)\n", fs[i].uid.u64, fs[i].alloc, i, DEF_INSTANCES_FS);
            }
        }
    }
    
    /// Test 1: select different filesystem, do a write
    ///         The low level features are tested elsewhere, so this test doesn't
    ///         go into extreme depths of fuzzing the low level Veelite calls.
    for (int i=0; i<DEF_INSTANCES_FS; i++) {
        rc = otfs_setfs(NULL, &fs[i].uid.u8[0]);
        if (rc != 0) {
            fprintf(stderr, "%sError: otfs_setfs() returned %d (LINE %d)\n", KRED, rc, __LINE__-2);
        }
        else {
            uint8_t testwrite[32];
            printf("Device FS [%016llX] Activated\n", fs[i].uid.u64);

            arc4random_buf(testwrite, 32);
            rc = sub_store(0x11, testwrite, 32);
            if (rc == 0) {
                printf("Wrote 32 bytes to File %d\n", 0x11);
                sub_hexdump(testwrite, 32, 16);
            }
            
            printf("\n");
        }

    }
    
    
    // End: free memory cell by cell
    for (int i=0; i<DEF_INSTANCES_FS; i++) {
        rc = otfs_del(NULL, &fs[i], true);
        if (rc != 0) {
            fprintf(stderr, "%sError: otfs_del() returned %d (LINE %d)\n", KRED, rc, __LINE__-2);
        }
        else {
            printf("FS instance %d deleted\n", i);
        }
    }
    
    
    // Final Deallocation
    otfs_deinit(NULL);
    printf("\nJudy array totally deallocated\n");
    
    return 0;
}

