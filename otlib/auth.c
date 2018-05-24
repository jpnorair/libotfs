/* Copyright 2013 JP Norair
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
  * @file       /otlib/auth.c
  * @author     JP Norair
  * @version    R102
  * @date       21 Aug 2014
  * @brief      Authentication & Crypto Functionality
  * @ingroup    Authentication
  *
  ******************************************************************************
  */

#include <otstd.h>

#include <otlib/auth.h>
#include <otlib/crypto.h>
#include <otlib/memcpy.h>
#include <otlib/rand.h>
#include <otsys/veelite.h>

#define _SEC_NL     OT_FEATURE(NLSECURITY)
#define _SEC_DLL    OT_FEATURE(DLL_SECURITY)
#define _SEC_ALL    (_SEC_NL && _SEC_DLL)
#define _SEC_ANY    (_SEC_NL || _SEC_DLL)



/// User aliases for sandboxed processes only
const id_tmpl*   auth_root;
const id_tmpl*   auth_user;
const id_tmpl*   auth_guest;


typedef struct OT_PACKED {
    uint64_t    id;
    ot_u16      flags;
    ot_u32      EOL;
} authinfo_t;

typedef struct OT_PACKED {
    ot_u16  flags;
    ot_u32  EOL;
    ot_u32  key[4];
} keyfile_t;


///@note the context data element should mirror the one from OTEAX.
typedef struct {   
    uint32_t    ks[44];
    uint32_t    inf;
} eax_ctx_t;

typedef struct {
    eax_ctx_t   ctx;
} authctx_t;

#if (_SEC_DLL)
    static uint32_t dlls_nonce;
    static ot_uint  dlls_size   = 0;

#   if (AUTH_NUM_ELEMENTS >= 0)
    // Static allocation:
    // First two elements are root and admin for the active device.
    static authctx_t    dlls_ctx[2 + AUTH_NUM_ELEMENTS];
    static authinfo_t   dlls_info[2 + AUTH_NUM_ELEMENTS];
    
#   elif (AUTH_NUM_ELEMENTS < 0)
    // Dynamic Allocation:
    // Is allocated at time of initialization.
    // Must be at least 2 elements.
    // Items will only be cleared during deinit/delete.
    static authctx_t* dlls_ctx = NULL;
    static authinfo_t* dlls_info = NULL;
    
#   endif
#endif




// internal stuff

/** @brief Sorts key table based in order of which key is closest to expiration.
  * @param none
  * @retval none
  */
void crypto_sort();

/** @brief Wipes out expired keys
  * @param none
  * @retval none
  */
void crypto_cull();

/** @brief Reorganizes (cleans) the Key Heap & table
  * @param none
  * @retval none
  */
void crypto_clean();





/** Subroutines <BR>
  * ========================================================================<BR>
  */
#if (_SEC_DLL || _SEC_NL)
void sub_expand_key(void* rawkey, eax_ctx_t* ctx) {
/// This routine will expand the key (128 bits) into a much larger key sequence.
/// The key sequence is what is actually used to do cryptographic operations.
    EAXdrv_init(rawkey, (void*)ctx);
}



///@todo Bring this into OT Utils?
ot_bool sub_idcmp(id_tmpl* user_id, authid_t* auth_id) {
    ot_bool id_check;

    if (user_id->length != auth_id->length) {
        return False;
    }
    if (user_id->length == 2) {
        return (((ot_u16*)user_id->value)[0] == ((ot_u16*)auth_id->value)[0]);
    }

    id_check    = (((ot_u32*)user_id->value)[0] == ((ot_u32*)auth_id->value)[0]);
    id_check   &= (((ot_u32*)user_id->value)[1] == ((ot_u32*)auth_id->value)[1]);
    return id_check;
}


ot_bool sub_authcmp(id_tmpl* user_id, id_tmpl* comp_id, ot_u8 mod_flags) {
    if ((user_id == NULL) || (user_id == comp_id))
        return True;

    return (ot_bool)auth_search_user(user_id, mod_flags);
}
#endif








/** High Level Cryptographic Interface Functions <BR>
  * ========================================================================<BR>
  * init, encrypt, decrypt
  */

#ifndef EXTF_auth_init
void auth_init(void) {
#if (_SEC_DLL)
#   define _KFILE_BYTES     (2 + 4 + 16)
    ot_int      i;
    vlFILE*     fp;
    keyfile_t   kfile;

    ///@todo put in code to:
    /// 1. initialize dynamic memory if it's enabled, and if pointer is NULL
    /// 2. 
#   if (AUTH_NUM_ELEMENTS < 0)
    /// Initialize dynamic memory if it's enabled.
    /// - if one of the tables is NULL, make sure to wipe both as clean start point.
    /// - Initialize the table in 64 unit chunks.
    if ((dlls_info == NULL) || (dlls_ctx == NULL)) {
        auth_deinit();
        
        ///@todo do the actual initialization.  Will be done through libjudy.
    }
    
#   endif

    /// If this function gets called when dlls_size < 2, it's a call with empty
    /// tables.
    if (dlls_size < 2) {
        dlls_size   = 2;
        dlls_nonce  = rand_prn32();
    }

    /// The first two keys are local keys that will change whenever the device
    /// reference changes.  Load them into the buffers.
    for (i=0; i<2; i++) {
        fp = ISF_open_su(i+ISF_ID(root_authentication_key));
        if (fp != NULL) {
            vl_load(fp, _KFILE_BYTES, &kfile);
            dlls_info[i].id     = i;
            dlls_info[i].flags  = kfile.flags;
            dlls_info[i].EOL    = kfile.EOL;
            sub_expand_key((void*)kfile.key, &dlls_ctx[i]);
            vl_close(fp);
        }
    }
    
    /// Keys after the first two persist through calls of auth_init().

#   undef _KFILE_BYTES
#endif

#if (_SEC_NLS)
    ///@todo
#endif
}
#endif



#ifndef EXTF_auth_deinit
void auth_deinit(void) {
/// clear all memory used for key storage, and free it if necessary.

#   if (AUTH_NUM_ELEMENTS >= 0)
    // Clear memory elements.  They are statically allocated in this case,
    // so no freeing is required.
    memset(dlls_info, 0, sizeof(dlls_info));
    memset(dlls_ctx, 0, sizeof(dlls_ctx));
    
#   else
    // Clear memory elements and free them.
    if (dlls_info != NULL) {
        memset(dlls_info, 0, sizeof(authinfo_t) * dlls_size);
        free(dlls_info);
    }
    if (dlls_ctx != NULL) {
        memset(dlls_ctx, 0, sizeof(authctx_t) * dlls_size);
        free(dlls_ctx);
    }
    dlls_size = 0;
    
#   endif
}
#endif



#ifndef EXTF_auth_putnonce
void auth_putnonce(void* dst, ot_uint limit) {
    ot_int      pad_bytes;
    ot_int      write_bytes;
    uint32_t    output_nonce;
    
    /// If limit is > 4 (size of nonce in bytes), we advance dst accordingly (Thus it is
    /// padded with its existing contents).
    /// If limit is <= 4, then the write_bytes get shortened.
    write_bytes = 4;
    pad_bytes   = limit - 4;
    if (pad_bytes > 0) {
        (ot_u8*)dst += pad_bytes;
    }
    else {
        write_bytes += pad_bytes;
    }
    
    /// Increment the internal nonce integer each time a nonce is put.
    /// It's also possible to change to network endian here, but it
    /// doesn't technically matter as long as the nonce data is 
    /// conveyed congruently.
    output_nonce = dlls_nonce++;
    
    memcpy_bytes(dst, &output_nonce, write_bytes);
}
#endif



#ifndef EXTF_auth_getnonce
ot_u32 auth_getnonce(void) {
    /// Increment the internal nonce integer each time a nonce is got.
    dlls_nonce++;
    return dlls_nonce;
}
#endif



#ifndef EXTF_auth_encrypt
ot_int auth_encrypt(void* nonce, void* data, ot_uint datalen, ot_uint key_index) {
/// Nonce input is 7 bytes.
/// on Devices without byte access (C2000), nonce will be 8 bytes with last byte 0.

#if (_SEC_ANY)
    /// DLL Encryption stage.
    /// Use AES context from auth_init() to do the encryption.
    ot_int retval;
    
    /// Error if key index is not available
    if (key_index >= dlls_size) {
        return -1;
    }
    
#   ifdef __C2000__
    ot_u32  iv[2];
    iv[0]   = ((ot_u32*)nonce)[0];
    iv[1]   = ((ot_u32*)nonce)[1];
    iv[1]  &= 0x00FFFFFF;
    retval  = EAXdrv_encrypt(nonce, data, datalen, &dlls_ctx[key_index].ctx);
#   else
    retval  = EAXdrv_encrypt(nonce, data, datalen, &dlls_ctx[key_index].ctx);
#   endif

    return (retval != 0) ? -2 : 4;
    
#else
    return -1;
#endif
}
#endif




#ifndef EXTF_auth_decrypt
ot_int auth_decrypt(void* nonce, void* data, ot_uint datalen, ot_uint key_index) {
/// "options" not presently used.
#if (_SEC_ANY)
    return __eaxcrypt(nonce, data, datalen, key_index, options, &EAXdrv_decrypt);
    
#else
    return -1;
#endif
}
#endif



#ifndef EXTF_auth_get_deckey
void* auth_get_deckey(ot_uint index) {
#if (_SEC_DLL)
    return (void*)auth_key[index].cache;
#else
    return NULL;
#endif
}
#endif


#ifndef EXTF_auth_get_enckey
void* auth_get_enckey(ot_uint index) {
#if (_SEC_TWINKEYS)
    ot_u32* enckey;
    enckey  = auth_key[index].cache;
    enckey += (auth_key[index].info.options) ? auth_key[index].info.length : 0;
    return (void*)enckey;
#else
    return auth_get_deckey(index);
#endif
}
#endif













/** User Authentication Routines <BR>
  * ========================================================================<BR>
  * Specifically, the Auth-Sec ALP should have hooks into these functions.
  */
  
ot_u8 auth_search_user(id_tmpl* user_id, ot_u8 req_mod) {
///@todo this function must be written.  It must search the authentication table
///      in order to find if the user is qualified to operate at the requested
///      mod level.  Return 0 on success, non-zero otherwise.
    return 0;
}



const id_tmpl* auth_get_user(ot_u16 user_index) {
/// @todo this must be tied into a table to return a handle.  
/// Right now it just returns root if index is zero, user if index is 1, and 
/// guest if index is anything else
    switch (user_index) {
        case 0: return auth_root;
        case 1: return auth_user;
       default: return auth_guest;
    }
}


ot_bool auth_isroot(id_tmpl* user_id) {
/// NULL is how root is implemented in internal calls
#if (_SEC_NLS)
    return sub_authcmp(user_id, auth_root, AUTH_FLAG_ISROOT);
#elif (_SEC_DLL)
    return (ot_bool)((user_id == NULL) || (user_id == auth_root));
#else
    return (ot_bool)(user_id == NULL);
#endif
}


ot_bool auth_isuser(id_tmpl* user_id) {
/// NULL is how root is implemented in internal calls
#if (_SEC_NLS)
    return sub_authcmp(user_id, auth_user, AUTH_FLAG_ISUSER);
#elif (_SEC_DLL)
    return (ot_bool)((user_id == NULL) || (user_id == auth_user));
#else
    return (ot_bool)(user_id == NULL);
#endif
}



ot_u8 auth_check(ot_u8 data_mod, ot_u8 req_mod, id_tmpl* user_id) {
#if (_SEC_ANY)
/// Find the ID in the table, then mask the user's mod with the file's mod
/// and the mod from the request (i.e. read, write).

    return (ot_u8)auth_search_user(user_id, (data_mod & req_mod));

#else
/// If the code gets here then there was not a user match, or the device is not
/// implementing user authentication.  Try guest access.
    return (0x07 & data_mod & req_mod);
#endif
}










/** Functions Typically Used with ALP <BR>
  * ========================================================================<BR>
  * Specifically, the Auth-Sec ALP should have hooks into these functions.
  * @todo implement!
  */

ot_u8 auth_find_keyindex(auth_handle* handle, id_tmpl* user_id) {
    return 255;
}

ot_u8 auth_read_key(auth_handle* handle, ot_uint key_index) {
    return 255;
}

ot_u8 auth_update_key(auth_handle* handle, ot_uint key_index) {
    return 255;
}

ot_u8 auth_create_key(ot_uint* key_index, auth_handle* handle) {
    return 255;
}

ot_u8 auth_delete_key(ot_uint key_index) {
    return 255;
}


