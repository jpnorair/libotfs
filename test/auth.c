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






int main(void) {
    uint32_t    fs_base[1024];
    vlFSHEADER* fs_head;
    int rc;

    printf("Auth features test\n");
    printf("===============================================================================\n");
    printf("Name of app in use with libotfs: %s\n", LIBOTFS_APP_NAME);
    
	rc = vworm_init((void*)fs_base, fs_head);
    printf("vworm_init() returned %d\n", rc);
    
	auth_init();
    
    //1. test auth_check against root and user, for a few files.
    
    //2. test auth_search for root and user
    
    //3. add a key
    
    //4. test local user mode encryption and decryption
    
    //5. test local root mode encryption and decryption
    
    //6. test encryption and decryption using new key from (3)
    
    
    auth_deinit();
    
    return 0;
}

