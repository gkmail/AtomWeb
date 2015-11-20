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

#ifndef _AW_SERVER_H_
#define _AW_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_types.h"
#include "aw_manager.h"
#include "aw_session.h"

/**\brief Socket receive function*/
typedef AW_Size (*AW_RecvFunc) (AW_Ptr sock, AW_Char *buf, AW_Size size);
/**\brief Socket send function*/
typedef AW_Size (*AW_SendFunc) (AW_Ptr sock, const AW_Char *buf, AW_Size size);
/**\brief HTTP method function*/
typedef AW_Result (*AW_MethodFunc) (AW_Session *sess, AW_Method method);

/**\brief HTTP server*/
struct AW_Server_s {
	AW_Manager    man;  /**< Resource manager*/
	const AW_Map *map;  /**< Object lookup map*/
	AW_RecvFunc   recv; /**< Socket receive function*/
	AW_SendFunc   send; /**< Socket send function*/
	/**\brief Receive data limits in bytes(<=0 is unlimited)*/
	AW_Size       recv_limit;
	/**\brief Send data limits in bytes(<=0 is unlimited)*/
	AW_Size       send_limit;
	AW_MethodFunc methods[AW_METHOD_COUNT]; /**< HTTP method functions*/
};

/**
 * \brief Create a new HTTP server
 * \return The new server
 */
extern AW_Server* aw_server_create (void);

/**
 * \brief Release a unused HTTP server
 * \param[in] serv The server to be released
 */
extern void       aw_server_destroy (AW_Server *serv);

/**
 * \brief Set the server's send and receive functions
 * \param[in] serv The server
 * \param recv Socket receive function
 * \param send Socket send function
 */
extern void       aw_server_set_io_funcs (AW_Server *serv,
					AW_RecvFunc recv,
					AW_SendFunc send);

/**
 * \brief Set the HTTP method function
 * \param[in] serv The server
 * \param method Method type
 * \param func Method function
 */
extern void       aw_server_set_method_func (AW_Server *serv,
					AW_Method method,
					AW_MethodFunc func);

/**
 * \brief Set the web object lookup map
 * \param[in] serv The server
 * \param[in] map The object lookup map
 */
extern void       aw_server_set_map (AW_Server *serv, const AW_Map *map);

/**
 * \brief Set sending data limits in bytes
 * \param[in] serv The server
 * \param lim The maximum data size should be sent per session
 * lim<=0 means unlimited.
 */
extern void       aw_server_set_send_limit (AW_Server *serv, AW_Size lim);

/**
 * \brief Set receiving data limits in bytes
 * \param[in] serv The server
 * \param lim The maximum data size should be received per session
 * lim<=0 means unlimited.
 */
extern void       aw_server_set_recv_limit (AW_Server *serv, AW_Size lim);

/**
 * \brief Default socket receive function
 * \param sock The socket
 * \param[out] buf Receiving data buffer
 * \param size The size of the buffer
 * \return Received data count in bytes
 * \retval <0 On error
 * \retval ==0 If the socket is closed
 */
extern AW_Size    aw_default_recv (AW_Ptr sock,
					AW_Char *buf,
					AW_Size size);

/**
 * \brief Default socket send function
 * \param sock The socket
 * \param[in] buf Sending data buffer
 * \param size The size of the buffer
 * \return Sent data count in bytes
 * \retval <0 On error
 */
extern AW_Size    aw_default_send (AW_Ptr sock,
					const AW_Char *buf,
					AW_Size size);

/**
 * \brief Default HTTP get method function
 * \param[in] sess The session
 * \param method AW_METHOD_GET
 * \retval AW_OK On success
 * \retval <0 On error
 */
extern AW_Result  aw_default_get (AW_Session *sess, AW_Method method);

/**
 * \brief Default HTTP post method function
 * \param[in] sess The session
 * \param method AW_METHOD_POST
 * \retval AW_OK On success
 * \retval <0 On error
 */
extern AW_Result  aw_default_post (AW_Session *sess, AW_Method method);

#ifdef __cplusplus
}
#endif

#endif

