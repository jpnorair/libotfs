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
  * @file       /test/main.c
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R101
  * @date       31 Oct 2017
  * @brief      Unit Tests for low level veelite functions.
  *
  ******************************************************************************
  */


#include <otfs.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __SHITTY_RANDOM__
#   include <time.h>
#   define RANDOM_BUF(PTRU8, LEN) do { \
            int _len_ = LEN; \
            uint8_t* _ptr_ = PTRU8; \
            srand(time(NULL)); \
            while (_len_-- > 0) { *_ptr_++ = rand() & 255; } \
        } while(0)
#else
#   ifdef __linux__
#       include <bsd/stdlib.h>
#   endif
#   define RANDOM_BUF(PTRU8, LEN) arc4random_buf(PTRU8, LEN)
#endif

int test_veelite_openclose(void);
int test_veelite_maxopen(void);



#if (OT_FEATURE(ALPEXT) == ENABLED)
ot_bool alp_ext_proc(alp_tmpl* alp, const id_tmpl* user_id) {
    return 0;
}
#endif

int test_veelite_openclose(void) {
    uint8_t id;

    vl_init(NULL);

    for (id=0; id<ISF_NUM_STOCK_FILES; id++) {
        vlFILE* fp;
        fp = ISF_open_su(id);
        if (fp == NULL) {
            printf("FAIL: File %d didn't open!!!\n", id);
        }
        else {
            printf("PASS: File %d, alloc=%d, length=%d\n", id, vl_checkalloc(fp), vl_checklength(fp));
        }
        vl_close(fp);
    }
    
    for (id=255; id>(255-ISF_NUM_EXT_FILES-ISF_NUM_USER_FILES); id--) {
        vlFILE* fp;
        fp = ISF_open_su(id);
        if (fp == NULL) {
            printf("FAIL: File %d didn't open!!!\n", id);
        }
        else {
            printf("PASS: File %d, alloc=%d, length=%d\n", id, vl_checkalloc(fp), vl_checklength(fp));
        }
        vl_close(fp);
    }
    
    return 0;
}




int test_veelite_maxopen(void) {
    int i;
    vlFILE* fp;
    
    vl_init(NULL);

    for (i=0; i<16; i++) {
        fp = ISF_open_su(0);
        if (fp == NULL) {
            printf("File failed to open on try %d\n", i);
        }
        else {
            printf("File opened on try %d\n", i);
        }
    }
    
    vl_init(NULL);
    
    return 0;
}



void sub_randload(uint8_t* dst, int bytes) {
    // This is a new function in stdlib
    RANDOM_BUF((void*)dst, bytes);
}

int sub_streamcmp(uint8_t* s1, uint8_t* s2, int size) {
    int test = 0;
    while (--size >= 0) {
        test += (*s1++ != *s2++);
    }
    return test;
}

int sub_loadstore(uint8_t id) {
    uint8_t temp[256];
    uint8_t check[256];
    vlFILE* fp;
    
    fp = ISF_open_su(id);
    if (fp == NULL) {
        printf("FAIL: File %d didn't open!!!\n", id);
    }
    else {
        int length;
        int errors;
        int alloc   = vl_checkalloc(fp);
        
        sub_randload(temp, alloc);
        vl_store(fp, alloc, temp);
        
        length = vl_checklength(fp);
        if (length != alloc) {
            printf("FAIL: File %d, length after store does not match number of stored bytes\n", id);
        }
        else if (alloc == 0) {
            printf("SKIP: File %d has 0 byte alloc.\n", id);
        }
        else {
            vl_load(fp, alloc, check);
            errors = sub_streamcmp(temp, check, alloc);
            if (errors != 0) {
                printf("FAIL: File %d has %d errors detected in store->load comparison\n", id, errors);
            }
            else {
                printf("PASS: File %d passes store->load check\n", id);
            }
        }
    }
    vl_close(fp);
    return 0;
}

int test_veelite_loadstore(void) {
    uint8_t id;
    vl_init(NULL);
    
    for (id=0; id<ISF_NUM_STOCK_FILES; id++) {
        sub_loadstore(id);
    }
    for (id=255; id>(255-ISF_NUM_EXT_FILES-ISF_NUM_USER_FILES); id--) {
        sub_loadstore(id);
    }
    
    return 0;
}




int sub_memptr(uint8_t id) {
    uint8_t temp[256];
    uint8_t check[256];
    uint8_t* ptr;
    int     alloc;
    int     errors;
    vlFILE* fp;
    
    fp = ISF_open_su(id);
    if (fp == NULL) {
        printf("FAIL: File %d didn't open!!!\n", id);
        return 1;
    }
    
    alloc = vl_checkalloc(fp);
    if (alloc == 0) {
        printf("SKIP: File %d has 0 byte alloc.\n", id);
    }
    else {
        ptr = vl_memptr(fp);
        if (ptr != NULL) {
            sub_randload(temp, alloc);
            ot_memcpy(ptr, temp, alloc);
            vl_close(fp);
        
            fp = ISF_open_su(id);
            if (fp != NULL) {
                vl_load(fp, alloc, check);
                errors = sub_streamcmp(check, temp, alloc);
                if (errors == 0) {
                    printf("PASS: File %d passes store->load check\n", id);
                }
                else {
                    printf("FAIL: File %d has %d errors detected in store->load comparison\n", id, errors);
                }
                vl_close(fp);
            }
        }
        else {
            printf("FAIL: File %d returned NULL on vl_memptr()\n", id);
        }
    }
    
    vl_close(fp);
    
    return 0;
}

int test_veelite_memptr(void) {
    uint8_t id;
    vl_init(NULL);
    
    for (id=0; id<ISF_NUM_STOCK_FILES; id++) {
        sub_memptr(id);
    }
    for (id=255; id>(255-ISF_NUM_EXT_FILES-ISF_NUM_USER_FILES); id--) {
        sub_memptr(id);
    }
    return 0;
}




void sub_fileaction(void* handle) {
    vlFILE* fp = (vlFILE*)handle;
    printf("sub_fileaction() called\n");
    printf(" --> fp->header = %d\n", fp->header);
    printf(" --> fp->start  = %d\n", fp->start);
    printf(" --> fp->alloc  = %d\n", fp->alloc);
    printf(" --> fp->idmod  = %04X\n", fp->idmod);
    printf(" --> fp->length = %d\n", fp->length);
    printf(" --> fp->flags  = %04X\n", fp->flags);
}

int test_veelite_actions(void) {
    ot_int rc;
    uint8_t id;
    vlFILE* fp;
    ot_u16 test;
    
    vl_init(NULL);
    
    rc = vl_add_action(VL_ISF_BLOCKID, 0, VL_FLAG_MODDED, &sub_fileaction);
    
    fp = ISF_open_su(0);
    test = vl_read(fp, 0);
    vl_write(fp, 0, test+1);
    vl_close(fp);
    
    return 0;
}





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
    uint32_t    fs_base[1024];
    vlFSHEADER* fs_head;
    int rc;
    
    printf("Name of app in use with libotfs: %s\n", LIBOTFS_APP_NAME);
    printf("size of vl_header_t: %zu\n", sizeof(vl_header_t));
    
    srand(time(NULL));
    
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
#   if (OT_FEATURE(VLACCTIME) == ENABLED)
    printf("->res_time8     = %d\n", fs_head->res_time8);
    printf("->res_time12    = %d\n", fs_head->res_time12);
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
    
    printf("STARTING VL-Actions test\n");
    test_veelite_actions();
    printf("ENDING VL-Actions test\n\n");
    
    
    
    return 0;
}

