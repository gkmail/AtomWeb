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
#include <aw_session.h>
#include <aw_server.h>
#include <aw_hash.h>
#include <aw_uri.h>
#include <aw_object.h>
#include <aw_map.h>

#ifndef AW_INPUT_BUFFER_SIZE
	#define AW_INPUT_BUFFER_SIZE 1024
#endif

static void
buf_resize (AW_Session *sess, AW_Size size)
{
	AW_Size nsize;
	AW_Char *nbuf;
	
	nsize = AW_MAX(sess->buf_size * 2, AW_INPUT_BUFFER_SIZE);

	if (size > 0)
		nsize = AW_MAX(size, nsize);

	nbuf = realloc(sess->buf, nsize * sizeof(AW_Char));
	if (!nbuf) {
		AW_FATAL(("not enough memory"));
	}

	sess->buf = nbuf;
	sess->buf_size = nsize;
}

static void
append_char (AW_Session *sess, AW_Char ch)
{
	AW_Size last = sess->pos + sess->len;

	if (last == sess->buf_size) {
		if (sess->pos) {
			memmove(sess->buf, sess->buf + sess->pos, sess->len);
			sess->pos = 0;
		} else {
			buf_resize(sess, sess->buf_size + 1);
		}
	}

	sess->buf[last] = ch;
	sess->len++;
}

static AW_Result
recv_data (AW_Session *sess)
{
	AW_Size left, cnt;

	if (sess->flags & AW_SESSION_FL_ERROR)
		return AW_FAILED;
	if (sess->flags & AW_SESSION_FL_END)
		return 0;

	cnt  = sess->len;
	left = sess->buf_size - cnt;

	if (cnt && sess->pos) {
		memmove(sess->buf, sess->buf + sess->pos, cnt);
		sess->pos = 0;
	}

	if (left == 0) {
		buf_resize(sess, 0);
		left = sess->buf_size - sess->len;
	}

	cnt = aw_session_recv(sess, sess->buf + sess->len, left);
	if (cnt <= 0) {
		sess->flags |= AW_SESSION_FL_END;

		if (cnt < 0)
			sess->flags |= AW_SESSION_FL_ERROR;

		return cnt;
	}

	sess->len += cnt;

	return AW_OK;
}

static AW_Result
recv_size (AW_Session *sess, AW_Size size)
{
	AW_Result r;

	while (sess->len < size) {
		r = recv_data(sess);
		if (r <= 0)
			return r;
	}

	return AW_OK;
}

static AW_Char*
str_nchar (AW_Char *ch, AW_Char csearch, AW_Size left)
{
	while ((*ch != csearch) && left) {
		ch ++;
		left --;
	}

	return left ? ch : NULL;
}

static AW_Char*
str_lookup_space (AW_Char *ch)
{
	while (!isspace(*ch))
		ch++;

	return ch;
}

static AW_Char*
str_eatup_space (AW_Char *ch)
{
	while (isspace(*ch))
		ch++;

	return ch;
}

static AW_Char*
str_trim (AW_Session *sess, AW_Char *str, AW_Size len)
{
	AW_Char *begin, *end;

	end = str + len;
	while (1) {
		if (!isspace(end[-1]))
			break;
		end--;
	}

	begin = str;
	while (begin < end) {
		if (!isspace(*begin))
			break;
		begin++;
	}

	*end = 0;
	return begin;
}

static AW_Char*
get_line (AW_Session *sess)
{
	AW_Char *pbegin, *ptr;
	AW_Size begin, pos, cnt, last;

	begin = sess->pos;
	pos   = begin;

	while (1) {
		if (sess->len) {
			pbegin = sess->buf + pos;

			ptr = str_nchar(pbegin, '\n', sess->len);
			if (ptr) {
				if ((ptr > pbegin) && (ptr[-1] == '\r')) {
					ptr[-1] = 0;
				} else {
					ptr[0] = 0;
				}

				cnt = ptr - pbegin + 1;
				sess->pos += cnt;
				sess->len -= cnt;
				return str_trim(sess, sess->buf + begin, cnt);
			}
		}

		last = sess->len;
		cnt  = recv_data(sess);
		if (cnt < 0)
			return NULL;

		if (cnt == 0) {
			return NULL;
		}

		pos = last;
	}

	return NULL;
}

static AW_Result
parse_request_line (AW_Session *sess, AW_Char *line)
{
	AW_Char *begin, *ptr;
	AW_Result r;

	begin = line;
	ptr = str_lookup_space(begin);
	if (!ptr)
		return AW_ERR_SYNTAX;

	*ptr = 0;

	if (!strcasecmp(begin, "options")) {
		sess->method = AW_METHOD_OPTIONS;
	} else if (!strcasecmp(begin, "get")) {
		sess->method = AW_METHOD_GET;
	} else if (!strcasecmp(begin, "head")) {
		sess->method = AW_METHOD_HEAD;
	} else if (!strcasecmp(begin, "post")) {
		sess->method = AW_METHOD_POST;
	} else if (!strcasecmp(begin, "delete")) {
		sess->method = AW_METHOD_DELETE;
	} else if (!strcasecmp(begin, "trace")) {
		sess->method = AW_METHOD_TRACE;
	} else if (!strcasecmp(begin, "connect")) {
		sess->method = AW_METHOD_CONNECT;
	} else {
		AW_ERROR(("unknown method \"%s\"", begin));
		return AW_ERR_SYNTAX;
	}

	AW_INFO(("method: %s", begin));

	begin = str_eatup_space(ptr + 1);
	if (!begin) {
		AW_ERROR(("no URI"));
		return AW_ERR_SYNTAX;
	}

	ptr = str_lookup_space(begin);
	if (!ptr) {
		AW_ERROR(("no HTTP version"));
		return AW_ERR_SYNTAX;
	}
	*ptr = 0;

	AW_INFO(("URI: %s", begin));

	if ((r = aw_uri_parse(&sess->uri, begin)) != AW_OK)
		return r;

	return AW_OK;
}

static AW_Result
parse_header_line (AW_Session *sess, AW_Char *line)
{
	AW_Char *key, *ptr, *value;
	AW_Result r;

	key = line;
	ptr = strchr(key, ':');

	if (!ptr) {
		AW_ERROR(("illegal header line \"%s\"", line));
		return AW_ERR_SYNTAX;
	}

	*ptr = 0;

	value = str_eatup_space(ptr + 1);

	key   = strdup(key);
	if (value)
		value = strdup(value);

	r = aw_hash_add(&sess->req_hash, key, value);
	if (r <= 0) {
		free(key);

		if (value)
			free(value);
	}

	AW_INFO(("header line: %s : %s", key, value));

	return r;
}

static const AW_Char*
get_reason (AW_U16 code)
{
	switch(code) {
		case 100: return "Continue";
		case 101: return "Switching Protocols";
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";
		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy";
		case 307: return "Temporary Redirect";
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Time-out";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Request Entity Too Large";
		case 414: return "Request-URI Too Large";
		case 415: return "Unsupported Media Type";
		case 416: return "Requested range not satisfiable";
		case 417: return "Expectation Failed";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Time-out";
		case 505: return "HTTP Version not supported";
	}

	return NULL;
}

static void
send_resp (AW_Session *sess, AW_U16 code)
{
	const AW_Char *reason;
	char buf[32];
	int len;

	aw_session_send(sess, "HTTP/1.1 ", -1);
	len = sprintf(buf, "%03d ", code);
	aw_session_send(sess, buf, len);

	reason = get_reason(code);
	if (reason) {
		aw_session_send(sess, reason, -1);
	}

	aw_session_send(sess, "\r\n", 2);

	if (code >= 400) {
		aw_session_send(sess, "\r\n", 2);
		aw_session_send(sess, "<html>Atom Web Error: ", -1);
		aw_session_send(sess, buf, len);
		aw_session_send(sess, " (", 2);
		aw_session_send(sess, reason, -1);
		aw_session_send(sess, ")</html>", -1);
	}
}

static void
send_resp_header (AW_Ptr key, AW_Ptr value, AW_Ptr data)
{
	AW_Session *sess = (AW_Session*)data;

	aw_session_send(sess, key, strlen(key));
	aw_session_send(sess, ": ", 2);

	if (value) {
		aw_session_send(sess, value, strlen(value));
	}

	aw_session_send(sess, "\r\n", 2);
}

static void
str_entry_free (AW_Ptr key, AW_Ptr value)
{
	if (key)
		free(key);
	if (value)
		free(value);
}

AW_Session*
aw_session_create (AW_Server *serv, AW_Ptr sock)
{
	AW_Session *sess;

	AW_ASSERT(serv);

	sess = (AW_Session*)malloc(sizeof(AW_Session));
	if (!sess) {
		AW_FATAL(("not enough memory"));
	}

	sess->serv = serv;
	sess->sock = sock;
	sess->buf  = NULL;
	sess->len  = 0;
	sess->pos  = 0;
	sess->buf_size = 0;
	sess->flags = 0;

	aw_manager_init(&sess->man);
	aw_uri_init(&sess->uri);
	aw_case_string_hash_init(&sess->req_hash, str_entry_free);
	aw_case_string_hash_init(&sess->resp_hash, str_entry_free);
	aw_string_hash_init(&sess->post_hash, str_entry_free);

	return sess;
}

AW_Result
aw_session_run (AW_Session *sess)
{
	AW_MethodFunc func;
	AW_Char *line;
	AW_Result r;

	AW_ASSERT(sess);

	if (!(line = get_line(sess))) {
		return AW_ERR_SYNTAX;
	}

	if ((r = parse_request_line(sess, line)) != AW_OK)
		return r;

	while (1) {
		if (!(line = get_line(sess))) {
			return AW_ERR_SYNTAX;
		}

		if (line[0] == 0)
			break;

		if ((r = parse_header_line(sess, line)) != AW_OK)
			return r;
	}

	func = sess->serv->methods[sess->method];
	if (func) {
		r = func(sess, sess->method);
	} else {
		AW_ERROR(("do not support the method %d", sess->method));
		r = AW_ERR_NOTSUPP;
	}

	return r;
}

void
aw_session_destroy (AW_Session *sess)
{
	AW_ASSERT(sess);

	if (sess->buf)
		free(sess->buf);

	aw_manager_deinit(&sess->man);
	aw_uri_deinit(&sess->uri);
	aw_hash_deinit(&sess->req_hash);
	aw_hash_deinit(&sess->resp_hash);
	aw_hash_deinit(&sess->post_hash);

	free(sess);
}

const AW_Char*
aw_session_get_request (AW_Session *sess, const AW_Char *name)
{
	AW_Ptr value;
	AW_Result r;

	AW_ASSERT(sess && name);

	r = aw_hash_lookup(&sess->req_hash, (AW_Ptr)name, &value);
	if (r <= 0)
		return NULL;

	return (const AW_Char*)value;
}

const AW_Char*
aw_session_get_post (AW_Session *sess, const AW_Char *name)
{
	AW_Ptr value;
	AW_Result r;

	AW_ASSERT(sess && name);

	r = aw_hash_lookup(&sess->post_hash, (AW_Ptr)name, &value);
	if (r <= 0)
		return NULL;

	return (const AW_Char*)value;
}

const AW_Char*
aw_session_get_query (AW_Session *sess, const AW_Char *name)
{
	AW_ASSERT(sess && name);

	return aw_uri_get_query(&sess->uri, name);;
}

const AW_Char*
aw_session_get_param (AW_Session *sess, const AW_Char *name)
{
	AW_ASSERT(sess);

	if (sess->method == AW_METHOD_POST) {
		return aw_session_get_post(sess, name);
	} else {
		return aw_session_get_query(sess, name);
	}
}

AW_Result
aw_session_add_response (AW_Session *sess, const AW_Char *key,
			const AW_Char *value)
{
	AW_Char *k = NULL, *v = NULL;
	AW_Result r;

	AW_ASSERT(sess && key);

	k = strdup(key);
	if (value)
		v = strdup(value);

	r = aw_hash_add(&sess->resp_hash, k, v);
	if (r <= 0) {
		if (k)
			free(k);
		if (v)
			free(v);
	}

	return r;
}

AW_Size
aw_session_recv (AW_Session *sess, AW_Char *buf, AW_Size size)
{
	AW_ASSERT(sess);

	if (sess->flags & AW_SESSION_FL_ERROR)
		return AW_FAILED;
	if (sess->flags & AW_SESSION_FL_END)
		return 0;

	return sess->serv->recv(sess->sock, buf, size);
}

AW_Size
aw_session_send (AW_Session *sess, const AW_Char *buf, AW_Size size)
{
	AW_ASSERT(sess);

	if (sess->flags & AW_SESSION_FL_ERROR)
		return AW_FAILED;
	if (sess->flags & AW_SESSION_FL_END)
		return 0;

	if (size == -1)
		size = strlen(buf);

	return sess->serv->send(sess->sock, buf, size);
}

AW_Result
aw_default_get (AW_Session *sess, AW_Method method)
{
	char buf[64];
	const AW_Char *path = NULL;
	const AW_Class *clazz = NULL;
	AW_Object *obj;
	AW_Size begin, len;
	AW_Result r;

	path = sess->uri.path;

	if (sess->serv->map) {
		clazz = aw_map_lookup(sess->serv->map, path);
	}

	sess->pos = 0;
	sess->len = 0;

	if (!clazz) {
		AW_ERROR(("cannot find \"%s\"", path));
		send_resp(sess, 404);
		return AW_FAILED;
	}

	obj = aw_object_create(sess, clazz);
	if ((r = aw_object_run(obj)) != AW_OK) {
		send_resp(sess, 500);
		return r;
	}

	send_resp(sess, 200);
	aw_hash_for_each(&sess->resp_hash, send_resp_header, sess);

	begin = sess->pos;
	len   = sess->len;
	sprintf(buf, "%d", len);
	send_resp_header("Content-Length", buf, sess);

	aw_session_send(sess, "\r\n\r\n", 4);

	if (sess->len) {
		aw_session_send(sess, sess->buf + begin, len);
	}

	return AW_OK;
}

AW_Result
aw_default_post (AW_Session *sess, AW_Method method)
{
	const AW_Char *len_str, *data_str;
	AW_Size len;
	AW_Result r;

	len_str = aw_session_get_request(sess, "Content-Length");
	if (!len_str) {
		AW_ERROR(("cannot get content length"));
		send_resp(sess, 411);
		return AW_ERR_SYNTAX;
	}

	len = strtol(len_str, NULL, 10);
	if (len == LONG_MAX) {
		AW_ERROR(("illegal content length"));
		send_resp(sess, 400);
		return AW_ERR_SYNTAX;
	}

	r = recv_size(sess, len);
	if (r <= 0) {
		AW_ERROR(("receive post data failed"));
		send_resp(sess, 400);
		return AW_ERR_SYNTAX;
	}

	append_char(sess, 0);

	data_str = sess->buf + sess->pos;

	if ((r = aw_parse_params(data_str, &sess->post_hash)) != AW_OK) {
		send_resp(sess, 400);
		return r;
	}

	return aw_default_get(sess, AW_METHOD_GET);
}

AW_Result
aw_session_entity (AW_Session *sess, const AW_Char *buf, AW_Size size)
{
	AW_ASSERT(sess);

	if (!size)
		return AW_OK;

	if (sess->len == sess->buf_size) {
		buf_resize(sess, sess->len + size);
	}

	memcpy(sess->buf + sess->len, buf, size);
	sess->len += size;

	return AW_OK;
}

AW_Result
aw_session_printf (AW_Session *sess, const AW_Char *fmt, ...)
{
	AW_Char *ptr;
	AW_Size left, size;
	va_list ap;

	ptr  = sess->buf + sess->len;
	left = sess->buf_size - sess->len;

	va_start(ap, fmt);
	size = vsnprintf(ptr, left, fmt, ap);
	va_end(ap);

	if (size > left) {
		buf_resize(sess, sess->len + size);

		va_start(ap, fmt);
		size = vsnprintf(ptr, left, fmt, ap);
		va_end(ap);
	}

	sess->len += size;

	return AW_OK;
}

