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

#ifndef _AW_URI_H_
#define _AW_URI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_types.h"
#include "aw_hash.h"

/**\brief URI*/
struct AW_Uri_s {
	AW_Char *chars; /**< URI string*/
	AW_Char *proto; /**< Prototype*/
	AW_Char *user;  /**< User and password*/
	AW_Char *host;  /**< Host name*/
	AW_Char *path;  /**< Path*/
	AW_U16   port;  /**< Port number*/
	AW_Hash  query_hash; /**< Query parameters hash table*/
};

/**
 * \brief URI data initialize
 * \param[in] uri URI data
 */
extern void      aw_uri_init (AW_Uri *uri);

/**
 * \brief URI data release
 * \param[in] uri URI data
 */
extern void      aw_uri_deinit (AW_Uri *uri);

/**
 * \brief Parse the string to URI value
 * \param[in] uri URI data
 * \param[in] str URI string
 */
extern AW_Result aw_uri_parse (AW_Uri *uri, const AW_Char *str);

/**
 * \brief Get parameter value from query hash table
 * \param[in] uri URI data
 * \param[in] name The parameter's name
 * \return The parameter's value
 * \retval NULL If the parameter has not been defined
 */
extern const AW_Char* aw_uri_get_query (AW_Uri *uri,
						const AW_Char *name);

extern AW_Result aw_parse_params (const AW_Char *str, AW_Hash *hash);

#ifdef __cplusplus
}
#endif

#endif

