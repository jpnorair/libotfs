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
  * @file       /test/auth.c
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    R101
  * @date       20 April 2018
  * @brief      Unit Tests for Auth Module (otlib/auth.h)
  *
  * The Unit Test for Auth Module also includes a functional test of the 
  * cryptography library (OTEAX)
  *
  ******************************************************************************
  */


#include <otfs.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <oteax.h>

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



char* sub_yesno(int exp) {
    static char* yes = "Yes";
    static char* no = "No";
    return (exp) ? yes : no;
}


static void sub_printctx(void* ctx, keytype_t keytype) {
    size_t ctx_words = 0;

    if (keytype == KEYTYPE_AES128) {
        ctx_words = (sizeof(eax_ctx)+3)/4;
    }
    
    for (int i=0; i<ctx_words; i++) {
        fprintf(stdout, "ctx[%02d] = %08X\n", i, ((uint32_t*)ctx)[i]);
    }
    fprintf(stdout, "\n");
}


static void print_hex(uint8_t* data, int length) {
    int i;
    for (i=0; i<length;) {
        printf("%02X ", data[i]);
        i++;
        if ((i % 16) == 0) {
            putchar('\n');
        }
    }
    if ((i % 16) != 0) {
        putchar('\n');
    }
}


int main(void) {
    uint32_t    fs_base[1024];
    vlFSHEADER* fs_head;
    int rc;

    printf("Auth features test\n");
    printf("===============================================================================\n");
    printf("Name of app in use with libotfs: %s\n", LIBOTFS_APP_NAME);
    
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
    
	auth_init();
	fprintf(stderr, KYEL "%s %d\n" KNRM, __FUNCTION__, __LINE__);
    
    ///1. test auth_check against root and user, for a few files.
    {   keytype_t   keytype;
        void*       keyroot;
        void*       keyuser;      
    
        keytype = auth_get_key(&keyroot, 0);
        if (keytype != KEYTYPE_AES128) {
            fprintf(stderr, KRED "Error, key-index %d not found\n" KNRM, 0);
        }
        else {
            sub_printctx(keyroot, keytype);
        }
        
        keytype = auth_get_key(&keyuser, 1);
        if (keytype != KEYTYPE_AES128) {
            fprintf(stderr, KRED "Error, key-index %d not found\n" KNRM, 1);
        }
        else {
            sub_printctx(keyuser, keytype);
        }
    }
    
    ///2. test local root mode encryption and decryption
    fprintf(stdout, KYEL "\nTest local-root crypto. (%s %d)\n" KNRM, __FUNCTION__, __LINE__);
    {   ot_int rc;
        uint8_t data[48] = {   0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                              10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                              20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                              30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
                              40, 41, 42,  0,  0,  0,  0,  0
                        };
        uint8_t nonce[7] = { 0, 1, 2, 3, 4, 5, 6 };
        
        rc = auth_encrypt(nonce, data, 43, 0);
        
        fprintf(stdout, "%sauth_encrypt(...) returned %d\n" KNRM, (rc==4)?KGRN:KRED, rc);
        print_hex(data, 47);
        
        rc = auth_decrypt(nonce, data, 43, 0);
        fprintf(stdout, "%sauth_decrypt(...) returned %d\n" KNRM, (rc==4)?KGRN:KRED, rc);
        print_hex(data, 43);
    }
    
    ///3. Add three new keys and test search
    fprintf(stdout, KYEL "\nAdd a new key to key table (%s %d)\n" KNRM, __FUNCTION__, __LINE__);
    {   ot_int rc;
        ot_uint key_index, key_index2, key_index3;
        id_tmpl user_id;
        uint8_t idval[8];
        id_tmpl user_id2;
        uint8_t idval2[8];
        id_tmpl user_id3;
        uint8_t idval3[8];
        
        uint8_t keydata[16] = { 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4 };
        uint8_t keydata2[16] = { 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8 };
        uint8_t keydata3[16] = { 1, 2, 2, 1, 9, 9, 9, 9, 3, 4, 4, 3, 0, 0, 0, 0 };
        
        user_id.length  = 2;
        user_id.value   = idval;
        idval[0]        = 0xBE;
        idval[1]        = 0xEF;
        
        user_id2.length = 2;
        user_id2.value  = idval2;
        idval2[0]       = 0xB0;
        idval2[1]       = 0x0B;
        
        user_id3.length = 2;
        user_id3.value  = idval3;
        idval3[0]       = 0xDE;
        idval3[1]       = 0xAD;
    
        rc = auth_create_key(&key_index, KEYTYPE_AES128, 3600, (void*)keydata, (const id_tmpl*)&user_id);
        fprintf(stdout, "%sauth_create_key([01010101020202020303030304040404], [BEEF]) returned %d, with key_index=%d\n" KNRM,
                            (rc>=0)?KGRN:KRED, rc, key_index);
                            
        rc = auth_create_key(&key_index2, KEYTYPE_AES128, 3600, (void*)keydata2, (const id_tmpl*)&user_id2);
        fprintf(stdout, "%sauth_create_key([...], [BOOB]) returned %d, with key_index=%d\n" KNRM,
                            (rc>=0)?KGRN:KRED, rc, key_index2);
                            
        rc = auth_create_key(&key_index3, KEYTYPE_AES128, 3600, (void*)keydata3, (const id_tmpl*)&user_id3);
        fprintf(stdout, "%sauth_create_key([...], [DEAD]) returned %d, with key_index=%d\n" KNRM,
                            (rc>=0)?KGRN:KRED, rc, key_index3);
        
        rc = auth_search_user((const id_tmpl*)&user_id2, b00111000);
        fprintf(stdout, "%sauth_search_user([B00B], AUTHMOD_user) returned %d\n" KNRM, 
                            (rc==key_index2)?KGRN:KRED, rc);
        
        rc = auth_search_user((const id_tmpl*)&user_id2, b00111000);
        fprintf(stdout, "%sauth_search_user([BEEF], AUTHMOD_user) returned %d\n" KNRM, 
                            (rc==key_index)?KGRN:KRED, rc);
        
        rc = auth_search_user((const id_tmpl*)&user_id3, b00111000);
        fprintf(stdout, "%sauth_search_user([DEAD], AUTHMOD_user) returned %d\n" KNRM, 
                            (rc==key_index3)?KGRN:KRED, rc);
    }
    

    ///6. test encryption and decryption using new key from (3)

    
    
    auth_deinit();
    
    return 0;
}

