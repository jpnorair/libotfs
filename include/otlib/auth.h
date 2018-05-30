/*  Copyright 2013, JP Norair
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
  * @file       /otlib/auth.h
  * @author     JP Norair
  * @version    R102
  * @date       21 Aug 2014
  * @brief      Authentication & Cryptography Functionality
  * @defgroup   Authentication (Authentication Module)
  * @ingroup    Authentication
  *
  * The crypto module includes a table that stores key matches, and functions
  * that are mostly hooks to application or platform-level crypto functions.
  * The requirement is that the crypto table is wiped whenever power goes off
  * or the chip is reset.  In this implementation, that requirement is met by
  * putting the table in volatile RAM.
  ******************************************************************************
  */


#ifndef __AUTH_H
#define __AUTH_H

#include <otstd.h>
#include <otlib/queue.h>
#include <otlib/alp.h>

/// Default user types
#define AUTH_GUEST  (id_tmpl*)auth_guest
#define AUTH_USER   (id_tmpl*)auth_user
#define AUTH_ROOT   NULL

#define AUTH_FLAG_ISGLOBAL  0x80
#define AUTH_FLAG_ISROOT    0x40


extern const id_tmpl*   auth_root;      // this is self-root, uses local root key
extern const id_tmpl*   auth_user;      // this is self-user, uses local user key
extern const id_tmpl*   auth_guest;






/** High Level Cryptographic Interface Functions <BR>
  * ========================================================================<BR>
  * init, encrypt, decrypt
  */

/** @brief Authentication Module Initializer
  * @param None
  * @retval None
  * @ingroup Authentication
  *
  * Call once during startup, or whenever user-code decides it is a good idea
  * to refresh the Key table.
  */
void auth_init(void);



/** @brief Deinitialize Authentication Module
  * @param None
  * @retval None
  * @ingroup Authentication
  *
  * This will wipe and deallocate all internal data used by the auth module.
  * If your implementation does not use dynamic memory allocation, you can
  * ignore this function, however it also wipes the RAM for security purposes.
  */
void auth_deinit(void);



/** @brief Encrypts the data in a ot_queue, in-place
  * @param q            (ot_queue*) input q containing encrypted data
  * @param key_index    (ot_uint) Key Index to use for encryption
  * @param options      (ot_uint) Decryption options specific to type of Crypto
  * @retval None
  * @ingroup auth
  * @sa auth_decrypt_q
  * @sa auth_decrypt
  * @sa auth_encrypt
  *
  * It is important to align the front of the input queue per specification of
  * the crypto format to be used.  There are some protections built-into this
  * function, but if you get it wrong the decrypted output will probably be
  * wrong as well.
  *
  * Encryption is performed on the entire queue, between front and putcursor.
  */
//ot_int auth_encrypt_q(ot_queue* q, ot_uint key_index, ot_uint options);


/** @brief Decrypts the data in a ot_queue, in-place
  * @param q            (ot_queue*) input q containing encrypted data
  * @param key_index    (ot_uint) Key Index to use for decryption
  * @param options      (ot_uint) Decryption options specific to type of Crypto
  * @retval None
  * @ingroup auth
  * @sa auth_encrypt_q
  * @sa auth_decrypt
  * @sa auth_encrypt
  *
  * It is important to align the front of the input queue per specification of
  * the crypto format to be used.  There are some protections built-into this
  * function, but if you get it wrong the decrypted output will probably be
  * wrong as well.
  *
  * Decryption is performed on the entire queue, between front and putcursor.
  */
//ot_int auth_decrypt_q(ot_queue* q, ot_uint key_index, ot_uint options);




/** @brief Writes a 32 bit nonce to destination in memory, and advances nonce.
  * @param dst          (void*) destination in memory to write nonce
  * @param total_size   (ot_uint) total bytes to put to destination
  * @retval None
  * @ingroup Authentication
  * @sa auth_putnonce_q()
  * @sa auth_getnonce()
  * 
  * If the "total_size" input is less than 4, then only part of the nonce will be 
  * written.  If "total_size" input is greater than 4, then (total_size - 4)
  * bytes will skipped from dst, before writing the nonce.
  */
void auth_putnonce(void* dst, ot_uint total_size);


/** @brief Writes a pre-padded 32 bit nonce to a queue
  * @param q            (ot_queue*) destination in memory to write nonce
  * @param total_size   (ot_uint) total bytes to put to queue
  * @retval None
  * @ingroup Authentication
  * @sa auth_putnonce()
  * 
  * Identical to auth_putnonce() except with queue output.
  */
void auth_putnonce_q(ot_queue* q, ot_uint total_size);


/** @brief Returns a 32 bit nonce, and advances nonce.
  * @param None
  * @retval ot_u32      Internal 32 bit nonce
  * @ingroup Authentication
  * @sa auth_putnonce()
  * 
  * This function returns the internal nonce for you to do with as you wish.
  */
ot_u32 auth_getnonce(void);



/** @brief Encrypts a datastream, in-place
  * @param iv           (void*) A Cryptographic init vector (IV)
  * @param data         (void*) stream for in-place encryption
  * @param datalen      (ot_uint) length of stream in bytes
  * @param key_index    (ot_uint) Key Index to use for encryption
  * @retval ot_int      number of bytes added to stream as result of encryption,
  *                       or negative on error.
  * @ingroup auth
  * @sa auth_decrypt
  * @sa auth_putnonce
  * @sa auth_getnonce
  *
  * Usage of this function should be mostly self-explanatory, however some 
  * information should be provided about the usage of "nonce".
  *
  * The "iv" input will take 7 bytes from the iv pointer and use this as the iv.
  * If your device cannot address at the byte level, zero pad the 8th byte.
  *
  * In typical usage, auth_putnonce() is used before this function to write the
  * nonce to a packet buffer, then the raw data is put onto the packet buffer, 
  * then this function is called with iv pointing to 3 bytes ahead of the nonce
  * and data pointing to the start of the raw data.
  *
  * The data is encrypted IN PLACE.  There is no double bufferring required.
  */
ot_int auth_encrypt(void* iv, void* data, ot_uint datalen, ot_uint key_index);



/** @brief Decrypts a datastream, in-place
  * @param iv           (void*) A Cryptographic init vector (IV)
  * @param data         (void*) stream for in-place decryption
  * @param datalen      (ot_uint) length of stream in bytes
  * @param key_index    (ot_uint) Key Index to use for decryption
  * @retval ot_int      number of bytes removed from stream as result of
  *                       decryption, or negative on error.
  * @ingroup auth
  * @sa auth_encrypt
  *
  * This function is very similar to auth_encrypt(), but it should be used in 
  * decryption processes.  The major difference is that the decryption process
  * will validate the authentication tag footer and return 4 on success, 
  * whereas encryption adds this footer.
  */
ot_int auth_decrypt(void* iv, void* data, ot_uint datalen, ot_uint key_index);



/** @brief Returns Decryption-Key DATA of a given key index, but no Auth/Sec metadata
  * @param key      (void**) Is loaded with pointer to expanded key.  Always word-aligned.
  * @param index    (ot_uint) Key Index input
  * @retval ot_int  returns sizeof the key data, or negative on error.
  * @ingroup auth
  * @sa auth_get_enckey
  *
  * For expert use only.
  *
  * @note for the standard EAX crypto implementation, there is only one key for 
  *       both encryption and decryption.
  */
ot_int auth_get_deckey(void** key, ot_uint index);



/** @brief Returns Encryption-Key DATA of a given key index, but no Auth/Sec metadata
  * @param key      (void**) Is loaded with pointer to expanded key.  Always word-aligned.
  * @param index    (ot_uint) Key Index input
  * @retval ot_int  returns sizeof the key data, or negative on error.
  * @ingroup auth
  * @sa auth_get_enckey
  *
  * For expert use only.
  *
  * @note for the standard EAX crypto implementation, there is only one key for 
  *       both encryption and decryption.
  */
ot_int auth_get_enckey(void** key, ot_uint index);









/** User Authentication Routines <BR>
  * ========================================================================<BR>
  * Specifically, the Auth-Sec ALP should have hooks into these functions.
  */

/** @brief Returns True if the supplied ID has root access
  * @param user_id      (id_tmpl*) pointer to a UID/VID template
  * @retval ot_bool     True when ID is root
  * @ingroup Authentication
  */
ot_bool auth_isroot(id_tmpl* user_id);



/** @brief Returns True if the supplied ID has user access (root passes this)
  * @param user_id      (id_tmpl*) pointer to a UID/VID template
  * @retval ot_bool     True when ID is user or root
  * @ingroup Authentication
  */
ot_bool auth_isuser(id_tmpl* user_id);



/** @brief Checks the authentication data per supplied user, and provides yes or no
  * @param data_mod     (ot_u8) Veelite Mod value of the desired data element
  * @param req_mod      (ot_u8) Requested Mod for operation (e.g. read or write)
  * @param user_id      (id_tmpl*) pointer to a UID/VID template
  * @retval ot_u8       Non-zero when authentication is OK
  * @ingroup Authentication
  */
ot_u8 auth_check(ot_u8 data_mod, ot_u8 req_mod, id_tmpl* user_id);











/** Functions Typically Used with ALP <BR>
  * ========================================================================<BR>
  * Specifically, the Auth-Sec ALP should have hooks into these functions.
  */

/** @brief Finds a Key-Index, given a User ID
  * @param key_index    (ot_uint*) returns a key index
  * @param user_id      (id_tmpl*) input user id
  * @retval ot_u8       Zero (0) on success, else an error code
  * @ingroup Authentication
  */
ot_u8 auth_find_keyindex(ot_uint* key_index, id_tmpl* user_id);


/** @brief Finds an Auth Handle, given a User ID
  * @param handle       (void*) Output handle for key being read
  * @param user_id      (id_tmpl*) input user id
  * @retval ot_u8       Zero (0) on success, else an error code
  * @ingroup Authentication
  */
ot_u8 auth_find_key(void* handle, id_tmpl* user_id);


/** @brief Reads an Auth/Sec Key Element, given key index
  * @param handle       (void*) Output handle for key being read
  * @param key_index    (ot_uint) input key index to read
  * @retval ot_u8       Zero (0) on success, else an error code
  * @ingroup Authentication
  */
ot_u8 auth_read_key(void* handle, ot_uint key_index);


/** @brief Updates an Auth/Sec Key Element, given
  * @param handle       (void*) Input Key Information
  * @param key_index    (ot_uint) Input Key Index
  * @retval ot_u8       Zero (0) on success, else an error code
  * @ingroup Authentication
  *
  * Sub-elements in "handle" that are set to NULL will be skipped during the
  * update procedure.
  */
ot_u8 auth_update_key(void* handle, ot_uint key_index);


/** @brief Create a Key given Auth/Sec information
  * @param key_index    (ot_uint*) Output Key Index
  * @param handle       (void*) Input Key & Auth/Sec Information
  * @retval ot_u8       Zero (0) on success, else an error code
  * @ingroup Authentication
  */
ot_u8 auth_create_key(ot_uint* key_index, void* handle);


/** @brief Deletes an Auth/Sec Key Element, given key index
  * @param key_index    (ot_uint) input key index to delete
  * @retval ot_u8       Zero (0) on success, else an error code
  * @ingroup Authentication
  */
ot_u8 auth_delete_key(ot_uint key_index);







/* @brief Adds a new key entry and associated key data to the Crypto_Heap.
  * @param handle       (void*) Output handle for this new entry
  * @param new_user     (id_tmpl*) ID information for new user
  * @param new_info     (auth_info*) Auth/Sec parameters
  * @param new_key      (ot_u8*) cryptographic key data
  * @retval ot_u8       Zero (0) on success, else an error code
  * @ingroup Authentication
  *
  * If a new key is added, but there is no room left, the oldest key will be
  * deleted to make room for this new key.

ot_u8 auth_new_nlsuser(void* handle, id_tmpl* new_user, auth_info* new_info, ot_u8* new_key);
*/


/* @brief Searches and returns a key based on UID or VID (if UID is NULL).
  * @param handle       (void*) Output handle for this new entry
  * @param user_id      (id_tmpl*) Device ID of user to find auth/sec data
  * @param mod_flags    (ot_u8) extra user flags
  * @retval ot_u8       Zero (0) on success, else an error code
  * @ingroup Authentication
*/
//ot_u8 auth_search_user(void* handle, id_tmpl* user_id, ot_u8 mod_flags);
ot_int auth_search_user(id_tmpl* user_id, ot_u8 mod_flags);

const id_tmpl* auth_get_user(ot_uint user_index);


#endif





