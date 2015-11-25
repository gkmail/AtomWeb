/******************************************************************************
 *                AtomWeb: An embedded web server                             *
 *                                                                            *
 * Copyright (c) 2015 Gong Ke                                                 *
 * All rights reserved.                                                       *
 *                                                                            *
 * Redistribution and use in source and binary forms, with or without         *
 * modification, are permitted provided that the following conditions are     *
 * met:                                                                       *
 * 1.Redistributions of source code must retain the above copyright notice,   *
 * this list of conditions and the following disclaimer.                      *
 * 2.Redistributions in binary form must reproduce the above copyright        *
 * notice, this list of conditions and the following disclaimer in the        *
 * documentation and/or other materials provided with the distribution.       *
 * 3.Neither the name of the Gong Ke; nor the names of its contributors may   *
 * be used to endorse or promote products derived from this software without  *
 * specific prior written permission.                                         *
 *                                                                            *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS    *
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  *
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR           *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,      *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,        *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING       *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               *
 *****************************************************************************/

#ifndef _AW_HASH_H_
#define _AW_HASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_types.h"

/**\brief Hash table entry free function*/
typedef void (*AW_HashFreeFunc)(AW_Ptr key, AW_Ptr value);
/**\brief Hash table entry traverse function*/
typedef void (*AW_HashForEachFunc) (AW_Ptr key, AW_Ptr value, AW_Ptr data);

/**\brief Hash table entry*/
struct AW_HashEntry_s {
	AW_HashEntry *next;  /**< The next entry in the list*/
	AW_Ptr        key;   /**< Key*/
	AW_Ptr        value; /**< Value*/
};

/**\brief Hash table*/
struct AW_Hash_s {
	AW_KeyFunc      key;     /**< Key calculation function*/
	AW_EqualFunc    equ;     /**< Key equal compare function*/
	AW_HashFreeFunc release; /**< Entry release function*/
	AW_HashEntry  **entries; /**< Entry list buffer*/
	AW_Size         count;   /**< Entries count in the hash table*/
	AW_Size         bucket;  /**< List count in the hash table*/
};

/**\brief Direct key function*/
extern AW_U32    aw_direct_key (AW_Ptr key);
/**\brief String key function*/
extern AW_U32    aw_string_key (AW_Ptr key);
/**\brief Case-insensitive key function*/
extern AW_U32    aw_case_string_key (AW_Ptr key);

/**\brief Direct key compare function*/
extern AW_Bool   aw_direct_equal (AW_Ptr k1, AW_Ptr k2);
/**\brief String key compare function*/
extern AW_Bool   aw_string_equal (AW_Ptr k1, AW_Ptr k2);
/**\brief Case-insensitive key compare function*/
extern AW_Bool   aw_case_string_equal (AW_Ptr k1, AW_Ptr k2);

/**
 * \brief Hash table initialize
 * \param[in] hash Hash table
 * \param key Key calulation function
 * \param equ Key compare function
 * \param release Entry release function
 */
extern void      aw_hash_init (AW_Hash *hash,
					AW_KeyFunc key,
					AW_EqualFunc equ,
					AW_HashFreeFunc release);

#define aw_direct_hash_init(hash, free)\
	aw_hash_init(hash, aw_direct_key, aw_direct_equal, free)
#define aw_string_hash_init(hash, free)\
	aw_hash_init(hash, aw_string_key, aw_string_equal, free)
#define aw_case_string_hash_init(hash, free)\
	aw_hash_init(hash, aw_case_string_key, aw_case_string_equal, free)

/**
 * \brief Release a hash table
 */
extern void      aw_hash_deinit (AW_Hash *hash);

/**
 * \brief Add an entry into the hash table
 * \param[in] hash Hash table
 * \param key Key
 * \param value Value
 * \retval AW_OK The entry is added
 * \retval AW_NONE An old entry with the same key is already added
 * \retval <0 On error
 */
extern AW_Result aw_hash_add (AW_Hash *hash,
					AW_Ptr key,
					AW_Ptr value);

/**
 * \brief Lookup an entry in the hash table with the key
 * \param[in] hash The hash table
 * \param key Key
 * \param[out] value Return the value
 * \retval AW_OK The entry is found
 * \retval AW_NONE Cannot find the entry
 * \retval <0 On error
 */
extern AW_Result aw_hash_lookup (AW_Hash *hash,
					AW_Ptr key,
					AW_Ptr *value);

/**
 * \brief Remove an entry from the hash table
 * \param[in] hash The hash table
 * \param key The entry's key to be removed
 * \retval AW_OK The entry has been removed
 * \retval AW_NONE Cannot find the entry
 * \retval <0 On error
 */
extern AW_Result aw_hash_remove (AW_Hash *hash,
					AW_Ptr key);

/**
 * \brief Hash table entries traverse
 * \param[in] hash The hash table
 * \param func The function invoked for each entry
 * \param data User defined parameter of func
 */
extern void      aw_hash_for_each (AW_Hash *hash,
					AW_HashForEachFunc func,
					AW_Ptr data);

#ifdef __cplusplus
}
#endif

#endif

