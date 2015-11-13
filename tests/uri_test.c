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

#include <stdarg.h>
#include <aw_uri.h>

static int test_count = 0;
static int passed_count = 0;

static AW_Bool
check_str (const char *s1, const char *s2)
{
	if (!s1 && !s2)
		return AW_TRUE;

	if (!s1 || !s2)
		return AW_FALSE;

	return strcmp(s1, s2) ? AW_FALSE : AW_TRUE;
}

static void
test (const char *chars, const char *proto, const char *user,
			const char *host, AW_U16 port, const char *path,
			...)
{
	AW_Uri uri;
	const char *key, *value, *rvalue;
	va_list ap;
	AW_Result r;

	test_count++;

	aw_uri_init(&uri);

	printf("test \"%s\": ", chars);

	r = aw_uri_parse(&uri, chars);
	if (r != AW_OK) {
		fprintf(stderr, "parse failed\n");
		goto end;
	}

	r = AW_FAILED;

#define FIELD_CHECK(field)\
	if (!check_str(uri.field, field)) {\
		fprintf(stderr, "%s mismatch, expect \"%s\", but get \"%s\"\n",\
				#field, field, uri.field);\
		goto end;\
	}

	FIELD_CHECK(chars);
	FIELD_CHECK(proto);
	FIELD_CHECK(user);
	FIELD_CHECK(host);
	FIELD_CHECK(path);

	if (port != uri.port) {
		fprintf(stderr, "port mismatch, expect \"%d\", but get \"%d\"\n",
					port, uri.port);
		goto end;
	}

	va_start(ap, path);

	while (1) {
		key = va_arg(ap, char *);
		if (!key)
			break;

		value = va_arg(ap, char *);

		rvalue = aw_uri_get_query(&uri, key);

		if (!check_str(rvalue, value)) {
			fprintf(stderr, "\"%s\" mismatch, expect \"%s\", but get \"%s\"",
						key, value, rvalue);
			goto end;
		}
	}

	va_end(ap);

	r = AW_OK;
end:
	aw_uri_deinit(&uri);

	if (r == AW_OK) {
		printf("passed\n");
		passed_count++;
	} else {
		printf("failed\n");
	}
}

int
main (int argc, char **argv)
{
	test("http://xxx.xxx.xxx", "http", NULL, "xxx.xxx.xxx", 0, NULL, NULL);
	test("http://user@xxx.xxx.xxx", "http", "user", "xxx.xxx.xxx", 0, NULL, NULL);
	test("http://user:passwd@xxx.xxx.xxx", "http", "user:passwd", "xxx.xxx.xxx", 0, NULL, NULL);
	test("http://user:passwd@xxx.xxx.xxx/path/to/target", "http", "user:passwd", "xxx.xxx.xxx", 0, "/path/to/target", NULL);
	test("http://user:passwd@xxx.xxx.xxx:8080/path/to/target", "http", "user:passwd", "xxx.xxx.xxx", 8080, "/path/to/target", NULL);
	test("http://user:passwd@xxx.xxx.xxx:8080/", "http", "user:passwd", "xxx.xxx.xxx", 8080, "/", NULL);
	test("http://user:passwd@xxx.xxx.xxx:8080/test.html?", "http", "user:passwd", "xxx.xxx.xxx", 8080, "/test.html", NULL);
	test("http://user:passwd@xxx.xxx.xxx:8080/test.html?p1&p2&p3", "http", "user:passwd", "xxx.xxx.xxx", 8080, "/test.html", "p1", NULL, "p2", NULL, "p3", NULL, NULL);
	test("http://user:passwd@xxx.xxx.xxx:8080/test.html?p1=1&p2=test&p3=12345", "http", "user:passwd", "xxx.xxx.xxx", 8080, "/test.html", "p1", "1", "p2", "test", "p3", "12345", NULL);

	test("/", NULL, NULL, NULL, 0, "/", NULL);
	test("/path/to/target", NULL, NULL, NULL, 0, "/path/to/target", NULL);
	test("/path/to%20/target", NULL, NULL, NULL, 0, "/path/to /target", NULL);
	test("/path/to%20/target?p1=%20&p2=hello", NULL, NULL, NULL, 0, "/path/to /target", "p1", " ", "p2", "hello", NULL);

	printf("tests: %d passed: %d failed: %d\n",
				test_count,
				passed_count,
				test_count - passed_count);
	return 0;
}

