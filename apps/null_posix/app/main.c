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

#include <otfs.h>

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

    for (i=0; i<255; i++) {
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




int main(void) {
    
    vworm_init();
    
    test_veelite_maxopen();
    
    test_veelite_openclose();
    
    return 0;
}
