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

#include <aw_debug.h>
#include <aw_server.h>

AW_Size
aw_default_recv (AW_Ptr data, AW_Char *buf, AW_Size size)
{
	int sock = (AW_IntPtr)data;
	AW_Size r;

#ifdef AW_MINGW
	r = recv(sock, buf, size, 0);
#else
	r = read(sock, buf, size);
#endif

	return r;
}

AW_Size
aw_default_send (AW_Ptr data, const AW_Char *buf, AW_Size size)
{
	int sock = (AW_IntPtr)data;
	AW_Size r;

#ifdef AW_MINGW
	r = send(sock, buf, size, 0);
#else
	r = write(sock, buf, size);
#endif

	return r;
}

AW_Server*
aw_server_create (void)
{
	AW_Server *serv;

	serv = (AW_Server*)malloc(sizeof(AW_Server));
	if (!serv) {
		AW_FATAL(("not enough memory"));
	}

	memset(serv, 0, sizeof(AW_Server));

	serv->recv = aw_default_recv;
	serv->send = aw_default_send;

	serv->send_limit = -1;
	serv->recv_limit = -1;

	serv->methods[AW_METHOD_GET]  = aw_default_get;
	serv->methods[AW_METHOD_POST] = aw_default_post;

	aw_manager_init(&serv->man);

	return serv;
}

void
aw_server_destroy (AW_Server *serv)
{
	AW_ASSERT(serv);

	aw_manager_deinit(&serv->man);
	free(serv);
}

void
aw_server_set_io_funcs (AW_Server *serv, AW_RecvFunc recv,
			AW_SendFunc send)
{
	AW_ASSERT(serv && recv && send);

	serv->recv = recv;
	serv->send = send;
}

void
aw_server_set_method_func (AW_Server *serv, AW_Method method,
			AW_MethodFunc func)
{
	AW_ASSERT(serv && (method >= 0) && (method < AW_METHOD_COUNT));

	serv->methods[method] = func;
}

void
aw_server_set_map (AW_Server *serv, const AW_Map *map)
{
	AW_ASSERT(serv);

	serv->map = map;
}

void
aw_server_set_send_limit (AW_Server *serv, AW_Size lim)
{
	AW_ASSERT(serv);

	serv->send_limit = lim;
}

void
aw_server_set_recv_limit (AW_Server *serv, AW_Size lim)
{
	AW_ASSERT(serv);

	serv->recv_limit = lim;
}

