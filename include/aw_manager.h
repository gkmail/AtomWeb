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

#ifndef _AW_MANAGER_H_
#define _AW_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_types.h"
#include "aw_hash.h"

/**\brief Resource manager*/
struct AW_Manager_s {
	AW_Hash  res_hash; /**< Resource hash table*/
};

/**
 * \brief Resource manager initialize
 * \param[in] man Resource manager
 */
extern void   aw_manager_init (AW_Manager *man);

/**
 * \brief Resource manager release
 * \param[in] man Resource manager
 */
extern void   aw_manager_deinit (AW_Manager *man);

/**
 * \brief Add a resource object into the manager
 * \param[in] man Resource manager
 * \param[in] ptr Pointer of the resource
 * \param Resource release function
 */
extern void   aw_manager_add_res (AW_Manager *man,
				AW_Ptr ptr,
				AW_FreeFunc release);

/**
 * \brief Release a resource object in the manager
 * \param[in] man Resource manager
 * \param[in] ptr Pointer of the resource
 */
extern void   aw_manager_remove_res (AW_Manager *man,
				AW_Ptr ptr);

/**
 * \brief Allocate a memory buffer and store it in the manager
 * \param[in] man Resource manager
 * \param size Buffer size in bytes
 */
extern AW_Ptr aw_manager_alloc (AW_Manager *man,
				AW_Size size);

#ifdef __cplusplus
}
#endif

#endif

