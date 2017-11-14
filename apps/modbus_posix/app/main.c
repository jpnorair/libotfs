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
  * @file       /apps/null_posix/app/main.c
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R101
  * @date       31 Oct 2017
  * @brief      Unit Tests for Null_Posix Library
  *
  ******************************************************************************
  */

#if 0

#include <otfs.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int test_veelite_openclose(void);
int test_veelite_maxopen(void);



#if (OT_FEATURE(ALPEXT) == ENABLED)
ot_bool alp_ext_proc(alp_tmpl* alp, id_tmpl* user_id) {
    return 0;
}
#endif

int test_veelite_openclose(void) {
    uint8_t id;

    vl_init();

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
    
    vl_init();

    for (i=0; i<16; i++) {
        fp = ISF_open_su(0);
        if (fp == NULL) {
            printf("File failed to open on try %d\n", i);
        }
        else {
            printf("File opened on try %d\n", i);
        }
    }
    
    vl_init();
    
    return 0;
}



void sub_randload(uint8_t* dst, int bytes) {
    // This is a new function in stdlib
    arc4random_buf((void*)dst, bytes);
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
    vl_init();
    
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
            memcpy(ptr, temp, alloc);
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
    vl_init();
    
    for (id=0; id<ISF_NUM_STOCK_FILES; id++) {
        sub_memptr(id);
    }
    for (id=255; id>(255-ISF_NUM_EXT_FILES-ISF_NUM_USER_FILES); id--) {
        sub_memptr(id);
    }
    return 0;
}



int main(void) {
    
    srand(time(NULL));
    vworm_init();
    
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

#endif

