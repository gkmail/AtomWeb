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

typedef enum {
	AW_METHOD_OPTIONS,
	AW_METHOD_GET,
	AW_METHOD_HEAD,
	AW_METHOD_POST,
	AW_METHOD_PUT,
	AW_METHOD_DELETE,
	AW_METHOD_TRACE,
	AW_METHOD_CONNECT,
	AW_METHOD_COUNT
} AW_Method;

#define AW_SESSION_FL_END   1
#define AW_SESSION_FL_ERROR 2

struct AW_Session_s {
	AW_Manager man;
	AW_Server *serv;
	AW_Ptr     sock;
	AW_Hash    req_hash;
	AW_Hash    resp_hash;
	AW_Hash    post_hash;
	AW_Method  method;
	AW_Uri     uri;
	AW_Char   *buf;
	AW_Size    buf_size;
	AW_Size    pos;
	AW_Size    len;
	AW_U32     flags;
};

extern AW_Session* aw_session_create (AW_Server *serv, AW_Ptr sock);
extern AW_Result   aw_session_run (AW_Session *sess);
extern void        aw_session_destroy (AW_Session *sess);

extern const AW_Char* aw_session_get_request (AW_Session *sess,
						const AW_Char *name);
extern const AW_Char* aw_session_get_post (AW_Session *sess,
						const AW_Char *name);
extern const AW_Char* aw_session_get_query (AW_Session *sess,
						const AW_Char *name);
extern const AW_Char* aw_session_get_param (AW_Session *sess,
						const AW_Char *name);

extern AW_Result   aw_session_add_response (AW_Session *sess,
						const AW_Char *key,
						const AW_Char *value);

extern AW_Size     aw_session_recv (AW_Session *sess,
						AW_Char *buf,
						AW_Size size);
extern AW_Size     aw_session_send (AW_Session *sess,
						const AW_Char *buf,
						AW_Size size);

extern AW_Result   aw_session_entity (AW_Session *sess,
						const AW_Char *buf,
						AW_Size size);
extern AW_Result   aw_session_printf (AW_Session *sess,
						const AW_Char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

