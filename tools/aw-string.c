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

#include "aw-tools.h"

void
aw_str_concat (char **ptr, char *add)
{
	if (!*ptr) {
		*ptr = strdup(add);
	} else {
		char *nptr;

		nptr = strcat(*ptr, add);

		free (*ptr);
		*ptr = nptr;
	}
}

void
aw_str_printf (char **ptr, const char *fmt, ...)
{
	va_list ap;
	int size;

	va_start(ap, fmt);
	size = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	if (size > 0) {
		char *buf;
		int olen, nlen;

		olen = *ptr ? strlen(*ptr) : 0;
		nlen = olen + size + 1;

		buf = (char*)malloc(nlen * sizeof(char));

		if (*ptr) {
			memcpy(buf, *ptr, olen);
			free(*ptr);
		}

		va_start(ap, fmt);
		vsnprintf(buf + olen, size + 1, fmt, ap);
		va_end(ap);

		*ptr = buf;
	}
}

char*
aw_str_cname (const char *lname, char *cbuf, int size)
{
	int len;
	char *pcname, *ptr;

	len = strlen(lname);
	if ((len > 3) && !strcasecmp(lname + len - 3, ".aw")) {
		strncpy(cbuf + 1, lname, len - 3);
		cbuf[len - 2] = 0;
	} else {
		strcpy(cbuf + 1, lname);
	}

	pcname = cbuf + 1;

	ptr = pcname;
	if (!isalpha(*ptr) && (*ptr != '_')) {
		cbuf[0] = '_';
		pcname = cbuf;
	}

	while (*ptr) {
		if (!isalnum(*ptr) && (*ptr != '_')) {
			*ptr = '_';
		}

		ptr++;
	}

	return pcname;
}
