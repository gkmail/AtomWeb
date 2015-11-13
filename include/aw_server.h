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

typedef AW_Size (*AW_RecvFunc) (AW_Ptr sock, AW_Char *buf, AW_Size size);
typedef AW_Size (*AW_SendFunc) (AW_Ptr sock, const AW_Char *buf, AW_Size size);
typedef AW_Result (*AW_MethodFunc) (AW_Session *sess, AW_Method method);

struct AW_Server_s {
	AW_Manager    man;
	const AW_Map *map;
	AW_RecvFunc   recv;
	AW_SendFunc   send;
	AW_MethodFunc methods[AW_METHOD_COUNT];
};

extern AW_Server* aw_server_create (void);
extern void       aw_server_destroy (AW_Server *serv);
extern void       aw_server_set_io_funcs (AW_Server *serv,
					AW_RecvFunc recv,
					AW_SendFunc send);
extern void       aw_server_set_method_func (AW_Server *serv,
					AW_Method method,
					AW_MethodFunc func);
extern void       aw_server_set_map (AW_Server *serv, const AW_Map *map);

extern AW_Size    aw_default_recv (AW_Ptr sock,
					AW_Char *buf,
					AW_Size size);
extern AW_Size    aw_default_send (AW_Ptr sock,
					const AW_Char *buf,
					AW_Size size);

extern AW_Result  aw_default_get (AW_Session *sess, AW_Method method);
extern AW_Result  aw_default_post (AW_Session *sess, AW_Method method);

#ifdef __cplusplus
}
#endif

#endif

