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
#include <aw_uri.h>

static void
query_free (AW_Ptr name, AW_Ptr value)
{
	if (name)
		free(name);
	if (value)
		free(value);
}

static void
uri_clear (AW_Uri *uri)
{
	if (uri->chars) {
		free(uri->chars);
		uri->chars = NULL;
	}
	if (uri->proto) {
		free(uri->proto);
		uri->proto = NULL;
	}
	if (uri->user) {
		free(uri->user);
		uri->user = NULL;
	}
	if (uri->host) {
		free(uri->host);
		uri->host = NULL;
	}
	if (uri->path) {
		free(uri->path);
		uri->path = NULL;
	}

	aw_hash_deinit(&uri->query_hash);
	aw_string_hash_init(&uri->query_hash, query_free);
}

static AW_U8
hex_value (AW_Char ch)
{
	if ((ch >= '0') || (ch <= '9'))
		return ch - '0';
	if ((ch >= 'a') || (ch <= 'f'))
		return ch - 'a' + 10;
	return ch - 'A' + 10;
}

static AW_Result
uri_decode (const AW_Char *str, AW_Size len, AW_Char **rstr)
{
	AW_Char *dstr;
	const AW_Char *src;
	AW_Char *dst;
	AW_Size left;

	if (len == -1)
		len = strlen(str);

	if (len == 0) {
		*rstr = NULL;
		return AW_OK;
	}

	dstr = (AW_Char*)malloc((len + 1) * sizeof(AW_Char));
	if (!dstr) {
		AW_FATAL(("not enough memory"));
	}

	src = str;
	dst = dstr;
	left = len;

	while (left) {
		if (*src == '%') {
			if ((left < 3) || !isxdigit(src[1]) || !isxdigit(src[2])) {
				AW_ERROR(("illegal character in URI"));
				goto error;
			}

			*dst = (hex_value(src[1]) << 4) | hex_value(src[2]);
			src += 3;
			left -= 3;
		} else {
			*dst = *src++;
			left--;
		}

		dst++;
	}

	*dst = 0;

	*rstr = dstr;
	return AW_OK;
error:
	free(dstr);
	return AW_ERR_SYNTAX;
}

static AW_Result
uri_parse_param (const AW_Char *str, AW_Size len, AW_Hash *hash)
{
	const AW_Char *equ, *end;
	AW_Char *key = NULL, *value = NULL;
	AW_Result r;

	if (len == 0)
		return AW_OK;

	end = str + len;

	equ = strchr(str, '=');
	if (equ && (equ < end)) {
		if ((r = uri_decode(str, equ - str, &key)) != AW_OK)
			goto error;
		if ((r = uri_decode(equ + 1, end - equ - 1, &value)) != AW_OK)
			goto error;
	} else {
		if ((r = uri_decode(str, end - str, &key)) != AW_OK)
			goto error;
		value = NULL;
	}

	r = aw_hash_add(hash, key, value);
	if (r < 0)
		goto error;
	if (r == AW_NONE) {
		AW_WARNING(("parameter \"%s\" has already been defined", key));
		r = AW_OK;
		goto error;
	}

	return AW_OK;
error:
	if (key)
		free(key);
	if (value)
		free(value);

	return r;
}

static AW_Result
uri_parse_params (const AW_Char *str, AW_Size len, AW_Hash *hash)
{
	const AW_Char *ch, *ptr, *end;
	AW_Size ilen;
	AW_Result r;

	if (len == -1)
		len = strlen(str);

	ch  = str;
	end = ch + len;

	while (1) {
		ptr = strchr(ch, '&');
		if (ptr && (ptr < end)) {
			ilen = ptr - ch;
			if ((r = uri_parse_param(ch, ilen, hash)) != AW_OK)
				return r;
			ch = ptr + 1;
		} else {
			ilen = end - ch;
			if ((r = uri_parse_param(ch, ilen, hash)) != AW_OK)
				return r;
			break;
		}
	}

	return AW_OK;
}

void
aw_uri_init (AW_Uri *uri)
{
	AW_ASSERT(uri);

	uri->chars = NULL;
	uri->proto = NULL;
	uri->user  = NULL;
	uri->host  = NULL;
	uri->path  = NULL;
	uri->port  = 0;

	aw_string_hash_init(&uri->query_hash, query_free);
}

void
aw_uri_deinit (AW_Uri *uri)
{
	AW_ASSERT(uri);

	uri_clear(uri);
}

AW_Result
aw_uri_parse (AW_Uri *uri, const AW_Char *str)
{
	const AW_Char *ch, *ptr;
	AW_Bool have_path = AW_TRUE;
	AW_Result r;

	AW_ASSERT(uri && str);

	uri->chars = strdup(str);

	ch = str;
	ptr = strstr(ch, "://");
	if (ptr) {
		if ((r = uri_decode(ch, ptr - ch, &uri->proto)) != AW_OK)
			goto error;
		ch = ptr + 3;

		ptr = strchr(ch, '@');
		if (ptr) {
			if ((r = uri_decode(ch, ptr - ch, &uri->user)) != AW_OK)
				goto error;
			ch = ptr + 1;
		}

		ptr = strpbrk(ch, ":/?#");

		if (ptr) {
			if (*ptr == ':') {
				AW_Char *end;
				long int port;

				if ((r = uri_decode(ch, ptr - ch, &uri->host)) != AW_OK)
					goto error;

				port = strtol(ptr + 1, &end, 10);
				if (port > 65535)
					goto error;

				uri->port = port;

				ch = end;
				ptr = strchr(end, '/');
				if (ptr)
					ch = ptr;

				ptr = strpbrk(ch, "?#");
			} else if (*ptr == '/') {
				if ((r = uri_decode(ch, ptr - ch, &uri->host)) != AW_OK)
					goto error;
				ch = ptr;
				ptr = strpbrk(ch, "?#");
			}
		} else {
			if ((r = uri_decode(ch, -1, &uri->host)) != AW_OK)
				goto error;

			have_path = AW_FALSE;
		}
	} else {
		ptr = strpbrk(ch, "?#");
	}

	if (have_path) {
		if (ptr) {
			if ((r = uri_decode(ch, ptr - ch, &uri->path)) != AW_OK)
				goto error;

			ch = ptr + 1;

			if (*ptr == '?') {
				AW_Size len;

				ptr = strchr(ch, '#');
				if (ptr) {
					len = ptr - ch;
				} else {
					len = -1;
				}

				r = uri_parse_params(ch, len, &uri->query_hash);
				if (r != AW_OK)
					goto error;
			}
		} else {
			if ((r = uri_decode(ch, -1, &uri->path)) != AW_OK)
				goto error;
		}
	}

	return AW_OK;

error:
	AW_ERROR(("illegal URI \"%s\"", str));
	uri_clear(uri);
	return AW_ERR_SYNTAX;
}

const AW_Char*
aw_uri_get_query (AW_Uri *uri, const AW_Char *name)
{
	AW_Ptr value;
	AW_Result r;

	AW_ASSERT(uri && name);

	r = aw_hash_lookup(&uri->query_hash, (AW_Ptr)name, &value);
	if (r <= 0)
		return NULL;

	return (const AW_Char*)value;
}

extern AW_Result
aw_parse_params (const AW_Char *str, AW_Hash *hash)
{
	return uri_parse_params(str, -1, hash);
}

