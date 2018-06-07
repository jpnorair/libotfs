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
  * @brief      Functional Tests for Queue Module (otlib/queue.h)
  *
  ******************************************************************************
  */


#include <otfs.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    int rc;
    ot_queue testq;
    ot_u32 qbuf[64];
    ot_u8 stringbuf[32];
    
    printf("Queue features test\n");
    printf("===============================================================================\n");
    printf("Name of app in use with libotfs: %s\n", LIBOTFS_APP_NAME);
    
    
    memset(qbuf, 0, sizeof(qbuf));
    q_init(&testq, (void*)qbuf, 256);
    
    printf("Freshly initialized Queue Print\n");
	printf("--------------------------------------------------------\n");
    q_print(&testq);
    printf("--------------------------------------------------------\n\n");
   	
   	printf("Add a mixed group of values to Queue\n");
	printf("--------------------------------------------------------\n");
	q_writebyte(&testq, 0x01);				printf("write byte: %02X\n", 0x01);
	q_writeshort(&testq, 0x0203);			printf("write short: %04X\n", 0x0203);
	q_writelong(&testq, 0x04050607);		printf("write long: %08X\n", 0x04050607);
	q_writestring(&testq, (ot_u8*)"abcdefghi", 9);	printf("write string: %s\n", "abcdefghi");
    q_print(&testq);
    printf("--------------------------------------------------------\n\n");
   	
   	printf("Pull values off the queue\n");
	printf("--------------------------------------------------------\n");
	printf("read byte: %02X\n", q_readbyte(&testq));
	printf("read short: %04X\n", q_readshort(&testq));
	printf("read long: %08X\n", q_readlong(&testq));
	printf("read string: %.*s\n", q_readstring(&testq, stringbuf, 9), stringbuf);
    q_print(&testq);
    printf("--------------------------------------------------------\n\n");
   	
    return 0;
}

