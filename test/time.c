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
  * @brief      Functional Tests for Time Module (otsys/time.h)
  *
  ******************************************************************************
  */


#include <otfs.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <math.h>
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



void sub_uptime_loop(int iters, int period_us) {
	uint32_t last_check;
	uint32_t this_check;
	int ti_target = round((double)period_us / 976.5625);
	
	
	last_check = time_uptime();
	printf("Starting Uptime: %u\n", last_check);
	printf("--------------------------------------------------------\n");
	
	for (; iters>0; iters--) {
		usleep(period_us);
		this_check = time_uptime();
		printf("Uptime: %u ti   -- period/target: %d/%d\n", time_uptime(), (this_check-last_check), ti_target);
		last_check = this_check;
	}
	
	printf("--------------------------------------------------------\n\n");
}


int main(void) {
    otfs_t* fs;
    int rc;
    ot_time timeval;
    ot_u32 utcval;
    
    printf("Time features test\n");
    printf("===============================================================================\n");
    printf("Name of app in use with libotfs: %s\n", LIBOTFS_APP_NAME);
    printf("Time Enabled?                    %s\n\n", sub_yesno(OT_FEATURE_TIME == ENABLED));
    
    srand(time(NULL));
    
    // Initialize Time Module with present time in seconds from epoch
    time_init_utc(time(NULL));
    
    // Print uptime several times, in one second intervals
    sub_uptime_loop(8, 1000000);
   	
   	// Print raw values from ot_time
   	printf("Time Values\n");
	printf("--------------------------------------------------------\n");
   	time_get(&timeval);
   	printf("time.upper  = %08X\n", timeval.upper);
   	printf("time.clocks = %08X\n", timeval.clocks);
   	utcval = time_get_utc();
   	printf("time-utc    = %08X (%u)\n", utcval, utcval);
   	printf("--------------------------------------------------------\n\n");
   	
   	// Now set the time precisely, make sure uptime is monotonic, print-out again the values
   	printf("Re-set the time precisely, make sure uptime consistent\n");
	printf("--------------------------------------------------------\n");
   	printf("Uptime (0): %u ti\n", time_uptime());
   	time_get(&timeval);
   	printf("time (0):   %08X %08X\n", timeval.upper, timeval.clocks);
	time_load_now(&timeval);
	time_set(timeval);
	printf("time (1):   %08X %08X\n", timeval.upper, timeval.clocks);
   	printf("Uptime (1): %u ti\n", time_uptime());
   	printf("--------------------------------------------------------\n\n");
   	
    return 0;
}

