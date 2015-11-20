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

#ifndef _AW_SESSION_H_
#define _AW_SESSION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_manager.h"
#include "aw_uri.h"
#include "aw_hash.h"

/**\brief HTTP method type*/
typedef enum {
	AW_METHOD_OPTIONS, /**< OPTIONS*/
	AW_METHOD_GET,     /**< GET*/
	AW_METHOD_HEAD,    /**< HEAD*/
	AW_METHOD_POST,    /**< POST*/
	AW_METHOD_PUT,     /**< PUT*/
	AW_METHOD_DELETE,  /**< DELETE*/
	AW_METHOD_TRACE,   /**< TRACE*/
	AW_METHOD_CONNECT, /**< CONNECT*/
	AW_METHOD_COUNT    /**< Methods count*/
} AW_Method;

/**\brief The session is been closed*/
#define AW_SESSION_FL_END   1
/**\brief The session has some error*/
#define AW_SESSION_FL_ERROR 2

/**\brief HTTP session*/
struct AW_Session_s {
	AW_Manager man;       /**< Resource manager*/
	AW_Server *serv;      /**< HTTP server*/
	AW_Ptr     sock;      /**< The socket*/
	AW_Hash    req_hash;  /**< Request header hash table*/
	AW_Hash    resp_hash; /**< Response header hash table*/
	AW_Hash    post_hash; /**< Post parameters hash table*/
	AW_Method  method;    /**< Method type*/
	AW_Uri     uri;       /**< Request URI*/
	AW_Char   *buf;       /**< Data buffer*/
	AW_Size    buf_size;  /**< The data buffer's size in bytes*/
	AW_Size    pos;       /**< The begin position of the data buffer*/
	AW_Size    len;       /**< Valid data length in the buffer*/
	AW_U32     flags;     /**< Session flags*/
	AW_Size    send;      /**< Data count has been sent*/
	AW_Size    recv;      /**< Data count has been received*/
};

/**
 * \brief Create a new session
 * \param[in] serv The HTTP server
 * \param sock The socket
 * \return The new session
 */
extern AW_Session* aw_session_create (AW_Server *serv, AW_Ptr sock);

/**
 * \brief Run the session
 * \param[in] sess The session
 * \retval AW_OK On success
 * \retval <0 On error
 */
extern AW_Result   aw_session_run (AW_Session *sess);

/**
 * \brief Release an unused session
 * \param[in] sess The session te be released
 */
extern void        aw_session_destroy (AW_Session *sess);

/**
 * \brief Get the request header's value
 * \param[in] sess The session
 * \param[in] name The request header's name
 * \return The header's value
 * \retval NULL When the header has not been defined
 */
extern const AW_Char* aw_session_get_request (AW_Session *sess,
						const AW_Char *name);

/**
 * \brief Get the post parameter's value
 * \param[in] sess The session
 * \param[in] name The parameter's name
 * \return The parameter's value
 * \retval NULL When the parameter has not been defined
 */
extern const AW_Char* aw_session_get_post (AW_Session *sess,
						const AW_Char *name);

/**
 * \brief Get the URI query parameter's value
 * \param[in] sess The session
 * \param[in] name The parameter's name
 * \return The parameter's value
 * \retval NULL When the parameter has not been defined
 */
extern const AW_Char* aw_session_get_query (AW_Session *sess,
						const AW_Char *name);

/**
 * \brief Get the parameter's value
 * The parameter is in the URI query field or post data.
 * \param[in] sess The session
 * \param[in] name The parameter's name
 * \return The parameter's value
 * \retval NULL When the parameter has not been defined
 */
extern const AW_Char* aw_session_get_param (AW_Session *sess,
						const AW_Char *name);

/**
 * \brief Add a reposonse header
 * \param[in] sess The session
 * \param[in] key The response header's name
 * \param[in] value The response header's value
 * \retval AW_OK On success
 * \retval <0 On error
 */
extern AW_Result   aw_session_add_response (AW_Session *sess,
						const AW_Char *key,
						const AW_Char *value);

/**
 * \brief Receive data from the session
 * The function is the wrapper of server socket receive function.
 * \param[in] sess The session
 * \param[out] buf The receiving data buffer
 * \param size The buffer size in bytes
 * \return Received data size in bytes
 * \retval <0 On error
 * \retval ==0 If the socket is closed
 */
extern AW_Size     aw_session_recv (AW_Session *sess,
						AW_Char *buf,
						AW_Size size);

/**
 * \brief Send data through the session
 * The function is the wrapper of server socket send function.
 * Do not invoke it directly.
 * Use 'aw_session_entity' or 'aw_session_printf' instead.
 * \param[in] sess The session
 * \param[out] buf The sending data buffer
 * \param size The buffer size in bytes
 * \return Sent data size in bytes
 * \retval <0 On error
 */
extern AW_Size     aw_session_send (AW_Session *sess,
						const AW_Char *buf,
						AW_Size size);

/**
 * \brief Append entity data to the session output buffer
 * \param[in] sess The session
 * \param[in] buf Append data buffer
 * \param size Buffer size in bytes
 * \retval AW_OK On success
 * \retval <0 On error
 */
extern AW_Result   aw_session_entity (AW_Session *sess,
						const AW_Char *buf,
						AW_Size size);

/**
 * \brief Append formatted string to the session output buffer
 * \param[in] sess The session
 * \param[in] fmt Format description string
 * \retval AW_OK On success
 * \retval <0 On error
 */
extern AW_Result   aw_session_printf (AW_Session *sess,
						const AW_Char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

