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
  *
  */
/**
  * @file       /platform/c2000/otlib_eax.c
  * @author     JP Norair
  * @version    R100
  * @date       28 Oct 2017
  * @brief      EAX Cryptographic Driver for POSIX & STD C
  * @defgroup   EAX Driver
  * @ingroup    EAX Driver
  *
  * 
  ******************************************************************************
  */

#include <otstd.h>
#if (OT_FEATURE(DLL_SECURITY) || OT_FEATURE(NL_SECURITY) || OT_FEATURE(VL_SECURITY))

// This is the OTlib crypto header that defines the driver functions
#include <otlib/crypto.h>

// This is the OTEAX library include, which does the heavy lifting
#include <oteax.h>

OT_INLINE ot_int EAXdrv_init(void* key, void* context) {
    return (ot_int)eax_init_and_key(key, context);
}

OT_INLINE ot_int EAXdrv_clear(void* context) {
    return (ot_int)eax_end(context);
}

OT_INLINE ot_int EAXdrv_encrypt(void* nonce, void* data, ot_uint datalen, void* context) {
    return (ot_int)eax_encrypt_message(nonce, data, datalen, context);
}

OT_INLINE ot_int EAXdrv_decrypt(void* nonce, void* data, ot_uint datalen, void* context) {
    return (ot_int)eax_decrypt_message(nonce, data, datalen, context);
}


#endif
