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

///@todo bring this into OT_config.h eventually, when the feature gets supported



/// User aliases for sandboxed processes only
const id_tmpl*   auth_root;
const id_tmpl*   auth_user;
const id_tmpl*   auth_guest;


typedef enum {
    ID_localguest   = -1,
    ID_localroot    = 0,
    ID_localuser    = 1,
    ID_normal       = 2,
} idtype_t;

typedef struct OT_PACKED {
    uint64_t    id;
    ot_u16      flags;
    ot_u32      EOL;
} authinfo_t;

///@note keyfile_t must match the structure of the "root & user authentication
///      key" files stored in the filesystem.
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

#if (_SEC_ANY)
#   undef   AUTH_NUM_ELEMENTS
#   define  AUTH_NUM_ELEMENTS   3

    static uint32_t dlls_nonce;
    static ot_uint  dlls_size   = 0;

#   if (AUTH_NUM_ELEMENTS >= 0)
    // Static allocation:
    // First two elements are root and admin for the active device.
    static authctx_t    dlls_ctx[AUTH_NUM_ELEMENTS];
    static authinfo_t   dlls_info[AUTH_NUM_ELEMENTS];
    
#   elif (AUTH_NUM_ELEMENTS < 0)
    // Dynamic Allocation:
    // Is allocated at time of initialization.
    // Must be at least 2 elements.
    // Items will only be cleared during deinit/delete.
    static authctx_t* dlls_ctx = NULL;
    static authinfo_t* dlls_info = NULL;
    
#   endif

#else
#   undef   AUTH_NUM_ELEMENTS
#   define  AUTH_NUM_ELEMENTS   0

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
#if (_SEC_ANY)
idtype_t sub_make_id64(uint64_t* id64, const id_tmpl* user_id) {    
    if ((user_id == NULL) || (user_id == auth_root)) {
        return ID_localroot;
    }
    if (user_id == auth_user) {
        return ID_localuser;
    }
    if (user_id == auth_guest) {
        return ID_localguest;
    }

    ///@todo make sure this works.  May be endian dependent.
    *id64  = ((uint64_t)1 << (user_id->length * 8)) - 1; 
    *id64 &= *(uint64_t*)user_id->value;
    
    return ID_normal;
}


void sub_expand_key(void* rawkey, eax_ctx_t* ctx) {
/// This routine will expand the key (128 bits) into a much larger key sequence.
/// The key sequence is what is actually used to do cryptographic operations.
    EAXdrv_init(rawkey, (void*)ctx);
}



///@todo Bring this into OT Utils?
ot_bool sub_idcmp(const id_tmpl* user_id, uint64_t id) {
    ot_int length;
    length = (id < 65536) ? 2 : 8;
    return (ot_bool)((length == user_id->length) && (*(uint64_t*)user_id->value == id));
}


ot_bool sub_authcmp(const id_tmpl* user_id, const id_tmpl* comp_id, ot_u8 mod_flags) {
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
#if (_SEC_ANY)
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
            vl_load(fp, _KFILE_BYTES, (ot_u8*)&kfile);
            dlls_info[i].id     = i;
            dlls_info[i].flags  = kfile.flags;
            dlls_info[i].EOL    = kfile.EOL;
            sub_expand_key((void*)kfile.key, &dlls_ctx[i].ctx);
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
#   if (AUTH_NUM_ELEMENTS > 0)
    // Clear memory elements.  They are statically allocated in this case,
    // so no freeing is required.
    memset(dlls_info, 0, sizeof(dlls_info));
    memset(dlls_ctx, 0, sizeof(dlls_ctx));
    
#   elif (AUTH_NUM_ELEMENTS < 0)
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
void auth_putnonce(void* dst, ot_uint total_size) {
#if (_SEC_ANY)
    ot_int      pad_bytes;
    ot_int      write_bytes;
    uint32_t    output_nonce;
    ot_u8*      dst_u8 = (ot_u8*)dst;
    
    /// If total_size is > 4 (size of nonce in bytes), we advance dst accordingly (Thus it is
    /// padded with its existing contents).
    /// If total_size is <= 4, then the write_bytes get shortened.
    write_bytes = 4;
    pad_bytes   = total_size - 4;
    if (pad_bytes > 0) {
        dst_u8 += pad_bytes;
    }
    else {
        write_bytes += pad_bytes;
    }
    
    /// Increment the internal nonce integer each time a nonce is put.
    /// It's also possible to change to network endian here, but it
    /// doesn't technically matter as long as the nonce data is 
    /// conveyed congruently.
    output_nonce = dlls_nonce++;
    
    ot_memcpy(dst_u8, &output_nonce, write_bytes);
#endif
}
#endif



#ifndef EXTF_auth_putnonce_q
void auth_putnonce_q(ot_queue* q, ot_uint total_size) {
#if (_SEC_ANY)
    ot_int      pad_bytes;
    ot_int      write_bytes;
    uint32_t    output_nonce;

    write_bytes = 4;
    pad_bytes   = total_size - 4;
    if (pad_bytes > 0) {
        q->putcursor += pad_bytes;
    }
    else {
        write_bytes += pad_bytes;
    }
    
    output_nonce = dlls_nonce++;
    q_writelong_be(q, output_nonce);
#endif
}
#endif



#ifndef EXTF_auth_getnonce
ot_u32 auth_getnonce(void) {
#if (_SEC_ANY)
    /// Increment the internal nonce integer each time a nonce is got.
    dlls_nonce++;
    return dlls_nonce;
#else
    return 0;
#endif
}
#endif



ot_int sub_do_crypto(void* nonce, void* data, ot_uint datalen, ot_uint key_index,
                        ot_int (*EAXdrv_fn)(void*, void*, ot_uint, void*) ) {
#if (_SEC_ANY)
    /// Nonce input is 7 bytes.
    /// on Devices without byte access (C2000), nonce will be 8 bytes with last byte 0.

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
    retval  = EAXdrv_fn(nonce, data, datalen, (EAXdrv_t*)&dlls_ctx[key_index].ctx);
#   else
    retval  = EAXdrv_fn(nonce, data, datalen, (EAXdrv_t*)&dlls_ctx[key_index].ctx);
#   endif

    return (retval != 0) ? -2 : 4;
    
#else
    return -1;
    
#endif
}



#ifndef EXTF_auth_encrypt
ot_int auth_encrypt(void* nonce, void* data, ot_uint datalen, ot_uint key_index) {
#if (_SEC_ANY)
    return sub_do_crypto(nonce, data, datalen, key_index, &EAXdrv_encrypt);
#else
    return -1;
#endif
}
#endif



#ifndef EXTF_auth_decrypt
ot_int auth_decrypt(void* nonce, void* data, ot_uint datalen, ot_uint key_index) {
/// EAX cryptography is symmetric, so decrypt and encrypt are almost identical.
#if (_SEC_ANY)
    return sub_do_crypto(nonce, data, datalen, key_index, &EAXdrv_decrypt);
#else
    return -1;
#endif
}
#endif




#ifndef EXTF_auth_get_enckey
ot_int auth_get_enckey(void** key, ot_uint index) {
///@todo not sure if this function should be removed
#if (_SEC_ANY)
    if ((key != NULL) && (index < dlls_size)) {
        *((ot_u32**)key) = dlls_ctx[index].ctx.ks;
        return sizeof(dlls_ctx[index].ctx.ks);
    }
#endif

    return -1;
}
#endif




#ifndef EXTF_auth_get_deckey
ot_int auth_get_deckey(void** key, ot_uint index) {
/// EAX cryptography is symmetric, so decrypt and encrypt are the same.
    return auth_get_enckey(key, index);
}
#endif
















/** User Authentication Routines <BR>
  * ========================================================================<BR>
  * Specifically, the Auth-Sec ALP should have hooks into these functions.
  */
  
ot_int auth_search_user(const id_tmpl* user_id, ot_u8 req_mod) {
/// Compare user-id and mod against stored keys.
/// The req_mod input is a bitfield with the structure: --rwxrwx, which is
/// defined by the veelite FS as follows:
/// --rwx--- = file perms for User
/// -----rwx = file perms for Guest
/// --000000 = file only accessible by root
/// 
/// The Authentication system doesn't work with Guests (just User and Root).
/// We need to convert req_mod into a form that works for Auth, which is 
/// below:
/// --rwx--- = key is suitable for Root read/write/exec access.
/// -----rwx = key is suitable for User read/write/exec access.
///
    ot_int i; 
    uint64_t id_u64;
    idtype_t idtype;

    idtype = sub_make_id64(&id_u64, user_id);
    if (status)

#if (_SEC_ANY)
#   if (AUTH_NUM_ELEMENTS > 0)
    ///@todo Current implementation is linear search.  In the future maybe
    ///      implement binary search, although for small tables typical for
    ///      this static allocation, it might be faster with linear search.
    // Mask-out unused bits in 64 bit ID. Auth system always uses 64 bit ID.
    
    
    // handle root case (req_mod == 0)
    // else, mask-out the don't care bits
    req_mod = (req_mod == 0) ? (7 << 3) : (req_mod >> 3) & 7;
    
    // If adjusted req_mod is zero, there is no auth to do, return Guest.
    // If adjusted req_mod is non-zero, then we need to search through keys.
    if (req_mod != 0)
        // Linear Search
        // - Compare id, also compare mod bits against stored flags
        // - If key timeout is enabled (EOL != 0), then make sure key isn't expired
        // - If key is expired, set flags to the INACTIVE and wipe context.
        // - Do NOT actually delete the key,  That is the job of the app.
        for (i=0; i<dlls_size; i++) {
            if (id_u64 == dlls_info[i].id) {
                if ((req_mod & dlls_info[i].flags) == dlls_info[i].flags) {
                    if (dlls_info[i].EOL != 0) {
                        if (dlls_info[i].EOL <= time_get_utc()) {
                            memset((void*)dlls_ctx[i], 0, sizeof(authctx_t));
                            dlls_info[i].flags = AUTH_KEYFLAGS_INACTIVE;
                            continue;
                        }
                    }
                    // Key is found, and valid
                    return i;
                }
            }
        }
    }
    
    return -1;
    
#   elif (AUTH_NUM_ELEMENTS < 0)
    ///@todo Dynamic Allocation: uses libjudy
    return -1;
    
#   else
    return -1;
    
#   endif

#else
    return -1;
    
#endif
}



ot_int auth_get_user(const id_tmpl* user_id, ot_u16 index) {
#   if (_SEC_ANY)
    if ((user_id != NULL) && (index < dlls_size)) {
        ot_int length;
        length = (dlls_info[index].id < 65536) ? 2 : 8;
        ot_memcpy(user_id->value, &dlls_info[index].id, length);
        return length;
    }
#   endif
    return -1;
}


ot_bool auth_isroot(const id_tmpl* user_id) {
/// Here's the trick: 
/// - Using NULL for user_id is ok for root calls from internal firmware.  Check this first.
/// - An external root key may be somewhere else in the list.  Check this last.

#if (_SEC_ANY)
    ot_int length;
    if (user_id == NULL) {
        return true;
    }
    ///@todo change 0x30 into a #define
    return (ot_bool)(auth_search_user(user_id, 0x30) >= 0);
    
#else
    /// When security/auth features are not compiled-in, just check against NULL.
    return (ot_bool)(user_id == NULL);
    
#endif
}


ot_bool auth_isuser(const id_tmpl* user_id) {
/// Here's the trick:
/// - Null is the root key, which is always OK to use for user calls.
/// - An external root/user key may be somewhere else in the list.  Check this last.
#if (_SEC_ANY)
    ot_int length;
    if (user_id == NULL) {
        return true;
    }
    ///@todo change 0x0C into a #define
    return (ot_bool)(auth_search_user(user_id, 0x0C) >= 0);

#else
    return (ot_bool)(user_id == NULL);
    
#endif
}



ot_u8 auth_check(ot_u8 data_mod, ot_u8 req_mod, const id_tmpl* user_id) {
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

ot_u8 auth_find_keyindex(ot_uint* key_index, const id_tmpl* user_id) {
    return 255;
}

ot_u8 auth_read_key(void* handle, ot_uint key_index) {
    return 255;
}

ot_u8 auth_update_key(void* handle, ot_uint key_index) {
    return 255;
}



ot_u8 auth_create_key(ot_uint* key_index, keytype_t type, ot_u32 lifetime, void* keydata, const id_tmpl* user_id) {
    uint64_t id64;
    idtype_t idtype;
    ot_int index;
    
    ///1. Input Checking
    if (key_index == NULL) {
        return 1;
    }
    if (type != KEY_AES128) {
        // Only type supported in this impl is AES128
        return 2;
    }
    if (lifetime < AUTH_MIN_LIFETIME) {
        return 3;
    }
    if (keydata == NULL) {
        return 4;
    }
    
    ///2. Convert user_id into id64 form.  
    ///   sub_make_id64 handles user_id == NULL and other special cases.
    idtype = sub_make_id64(&id64, user_id);
    
    ///3. Escape for local ID types, which are handled specially
    if (idtype != ID_normal) {
        return 5;
    }

#if (AUTH_NUM_ELEMENTS >= 0)
    ///4. Static Allocation 
    ///   If id64 and idtype already exists, replace that entry.
    ///   Else, add new key to end of list.
    
    index = sub_search_user(user_id, authmod);
    if (index >= 0) {
        // do update
    }
    else {
        // do add
    }
    
    //code from delete
    if (key_index < dlls_size) {
        ot_int i;
    
        memset((void*)dlls_ctx[i], 0, sizeof(authctx_t));
        dlls_info[i].flags = AUTH_KEYFLAGS_INVALID;
        dlls_size--;
        
        if (key_index >= 2) {
            dlls_size--;
            for (i=key_index; i<dlls_size; i++) {
                memcpy(&dlls_info[i], &dlls_info[i+1], sizeof(authinfo_t));
                memcpy(&dlls_ctx[i], &dlls_ctx[i+1], sizeof(authctx_t));
            }
        }
        
        return 0;
    }
    
#elif (AUTH_NUM_ELEMENTS < 0)
    ///@todo implement this
    
#endif

    return 255;
}



ot_u8 auth_delete_key(ot_uint key_index) {
/// This will delete a key from the table of keys.  It is up to the application
/// to maintain keys, and thus to use this function.
/// 
/// Static Allocation: Typically used with small key tables (e.g. for endpoints)
/// - wipe old key context and
/// - if key index is 0 or 1, these are special keys, don't displace them.
/// - else, shift table contents over deleted key
///
/// Dynamic Allocation: Typically used with large key tables (e.g. gateways)
/// - Wipe, delete, and free the key context & info
/// - reduce size and pointers of table accordingly

#if (AUTH_NUM_ELEMENTS >= 0)
    // Static allocation:
    if (key_index < dlls_size) {
        ot_int i;
    
        memset((void*)dlls_ctx[i], 0, sizeof(authctx_t));
        dlls_info[i].flags = AUTH_KEYFLAGS_INVALID;
        dlls_size--;
        
        if (key_index >= 2) {
            dlls_size--;
            for (i=key_index; i<dlls_size; i++) {
                memcpy(&dlls_info[i], &dlls_info[i+1], sizeof(authinfo_t));
                memcpy(&dlls_ctx[i], &dlls_ctx[i+1], sizeof(authctx_t));
            }
        }
        
        return 0;
    }
    
#elif (AUTH_NUM_ELEMENTS < 0)
    ///@todo implement this
    
#endif

    return 255;
}


