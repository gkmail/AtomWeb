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

#define T_COMMENT_BEGIN 256
#define T_HEAD_BEGIN    257
#define T_TAIL_BEGIN    258
#define T_MEMBER_BEGIN  259
#define T_INIT_BEGIN    260
#define T_RELEASE_BEGIN 261
#define T_EXPR_BEGIN    262
#define T_CODE_BEGIN    263
#define T_END           264
#define T_C_COMMENT_LINE  265
#define T_C_COMMENT_BLOCK 266

typedef struct Text_s   Text;
typedef struct Object_s Object;

typedef enum {
	TEXT_C_PROG,
	TEXT_C_EXPR,
	TEXT_OTHER
} TextType;

struct Text_s {
	Text     *prev;
	Text     *next;
	TextType  type;
	char     *text;
	int       line;
	int       column;
};

struct Object_s {
	Text *head;
	Text *member;
	Text *init;
	Text *code;
	Text *release;
	Text *tail;
};

static char  c_name_buf[PATH_MAX];
static char  output_name_buf[PATH_MAX];
static const char *c_name = NULL;
static const char *output_name = NULL;
static const char *input_name = NULL;
static const char *mime_type = NULL;
static int   bin_mode = 0;

static FILE *input = NULL;
static FILE *output = NULL;

static Object object;

static char input_buf[1024];
static int  input_pos = 0;;
static int  input_len = 0;
static char fetch_buf[3];
static int  fetch_len = 0;
static int  last_is_nl = 0;
static int  lineno = 1;
static int  column = 0;

static char text_buf[1024];
static int  text_len = 0;

static Text    **text_list_stack[2];
static TextType  text_type_stack[2];
static int       text_line;
static int       text_column;
static int       text_stack_top = 0;

static const char* usage_str =
"usage: aw-converter [OPTIONS] FILE\n"
"aw-converter: Atom Web Converter\n"
"options:\n"
"\t-o FILE     output file name\n"
"\t-c NAME     class name\n"
"\t-b          convert a binary file\n"
"\t-h          show this message\n"
;

static void
usage (void)
{
	fprintf(stderr, "%s", usage_str);
}

static void
parse_args (int argc, char **argv)
{
	int opt;

	while ((opt = getopt(argc, argv, "o:c:bh")) != -1) {
		switch (opt) {
			case 'o':
				output_name = optarg;
				break;
			case 'c':
				c_name = optarg;
				break;
			case 'h':
				usage();
				break;
			case 'b':
				bin_mode = 1;
				break;
			default:
				usage();
				exit(1);
				break;
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "no input file defined\n");
		exit(1);
	}

	input_name = argv[optind];
}

static void
output_line (int line, int tline, int tcol)
{
	if ((line != tline) || (tcol != 1)) {
		int n;

		fprintf(output, "\n#line %d \"%s\"\n",
					tline, input_name);

		for (n = 1; n < tcol; n++) {
			fprintf(output, " ");
		}
	}
}

static void
output_c_expr (const char *text, int line, int tline, int tcol)
{
	if (!text)
		return;

	fprintf(output, "\taw_session_printf(sess,\n");
	output_line(line + 1, tline, tcol);
	fprintf(output, "%s);\n", text);
}

static void
output_other (const char *text)
{
	const char *ptr;
	int size = 0;

	fprintf(output, "\taw_session_entity(sess, \"");

	ptr = text;
	while (*ptr) {
		switch (*ptr) {
			case '\\':
				fprintf(output, "\\\\");
				break;
			case '\'':
				fprintf(output, "\\\'");
				break;
			case '\"':
				fprintf(output, "\\\"");
				break;
			case '\t':
				fprintf(output, "\\t");
				break;
			case '\r':
				fprintf(output, "\\r");
				break;
			case '\v':
				fprintf(output, "\\v");
				break;
			case '\f':
				fprintf(output, "\\f");
				break;
			case '\a':
				fprintf(output, "\\a");
				break;
			case '\b':
				fprintf(output, "\\b");
				break;
			case '\n':
				fprintf(output, "\\n\"\n\t\t\"");
				break;
			default:
				if (isprint(*ptr)) {
					fprintf(output, "%c", *ptr);
				} else {
					fprintf(output, "\\x%02x", *ptr);
				}
				break;
		}

		ptr++;
		size++;
	}

	fprintf(output, "\"\n\t\t, %d);\n", size);
}

static int
has_valid_char (const char *str, int len)
{
	const char *end;

	if (len == -1)
		len = strlen(str);

	end = str + len;

	while (str < end) {
		if (!isspace(*str))
			return 1;

		str++;
	}

	return 0;
}

static void
gen_code (Text *text)
{
	Text *t = text;
	int line = 0;
	char *ptr, *begin;

	if (!t)
		return;

	do {
		switch (t->type) {
			case TEXT_C_PROG:
				output_line(line, t->line, t->column);
				fprintf(output, "%s", t->text);
				break;
			case TEXT_C_EXPR:
				output_c_expr(t->text, line, t->line, t->column);
				line ++;
				break;
			case TEXT_OTHER:
				output_other(t->text);
				break;
		}

		begin = t->text;
		do {
			ptr = strchr(begin, '\n');
			if (ptr) {
				line ++;
				begin = ptr + 1;
			}
		} while (ptr);

		t = t->next;
	} while (t != text);
}

static void
free_code (Text *text)
{
	Text *t = text;
	Text *n;

	if (!t)
		return;

	do {
		n = t->next;

		if (t->text)
			free(t->text);
		free(t);

		t = n;
	} while (t != text);
}

static int
getch_lower (void)
{
	int r, c;

	if (fetch_len) {
		return fetch_buf[--fetch_len];
	}
	
	if (input_len == 0) {
		input_pos = 0;

		r = fread(input_buf, 1, sizeof(input_buf), input);
		if (r < 0)
			return r;
		if (r == 0)
			return -1;

		input_len = r;
	}

	c = input_buf[input_pos];

	input_pos ++;
	input_len --;
	column ++;

	return c;
}

static int
getch (void)
{
	int c = getch_lower();

	if (last_is_nl) {
		lineno ++;
		column = 1;
		last_is_nl = 0;
	}

	if (c == '\n')
		last_is_nl = 1;

	return c;
}

static void
unget (int ch)
{
	if (ch == '\n')
		last_is_nl = 0;

	if ((ch >= 0) && (ch < 256)) {
		AW_ASSERT(fetch_len < sizeof(fetch_buf) / sizeof(char));
		fetch_buf[fetch_len++] = ch;
	}
}

static void
append_text ()
{
	Text **list;
	Text *t;

	if (!text_len)
		return;

	if (!has_valid_char(text_buf, text_len)) {
		text_len = 0;
		return;
	}

	t = (Text*)malloc(sizeof(Text));

	t->type = text_type_stack[text_stack_top - 1];
	t->line   = text_line;
	t->column = text_column;
	t->text = strndup(text_buf, text_len);

	list = text_list_stack[text_stack_top - 1];

	if (*list) {
		t->prev = (*list)->prev;
		t->next = (*list);
		(*list)->prev->next = t;
		(*list)->prev = t;
	} else {
		t->prev = t;
		t->next = t;
		*list = t;
	}

	text_len = 0;
}

static void
push (Text **list, TextType type)
{
	AW_ASSERT(text_stack_top < 2);

	append_text();

	text_list_stack[text_stack_top] = list;
	text_type_stack[text_stack_top] = type;
	text_line = 0;

	text_stack_top ++;
}

static void
pop ()
{
	AW_ASSERT(text_stack_top > 0);

	append_text();
	text_stack_top --;
	text_line = 0;
}

static void
append_char (int c)
{
	if (c < 0)
		return;

	if (text_len == sizeof(text_buf)) {
		append_text();
	}

	text_buf[text_len ++] = c;

	if (text_len == 1) {
		text_line   = lineno;
		text_column = column;
	}
}

static int
getch_other_upper (void)
{
	int c, r;

	c = getch();
	r = c;

	if (c == '<') {
		c = getch();

		if (c == '?') {
			c = getch();
			if (c == 'h') {
				return T_HEAD_BEGIN;
			} else if (c == 't') {
				return T_TAIL_BEGIN;
			} else if (c == 'm') {
				return T_MEMBER_BEGIN;
			} else if (c == 'i') {
				return T_INIT_BEGIN;
			} else if (c == 'r') {
				return T_RELEASE_BEGIN;
			} else if (c == '=') {
				return T_EXPR_BEGIN;
			} else if (c == 'c') {
				return T_CODE_BEGIN;
			} else {
				unget(c);
				unget('?');
			}
		} else if (c == '!') {
			c = getch();
			if (c == '-') {
				c = getch();
				if (c == '-') {
					return T_COMMENT_BEGIN;
				} else {
					unget(c);
					unget('-');
					unget('!');
				}
			} else {
				unget(c);
				unget('!');
			}
		} else {
			unget(c);
		}
	}

	append_char(r);
	return r;
}

static int
getch_c_upper (void)
{
	int c, r;

	c = getch();
	r = c;

	if (c == '/') {
		c = getch();
		if (c == '*') {
			append_char('/');
			append_char('*');
			return T_C_COMMENT_BLOCK;
		} else if (c == '/') {
			append_char('/');
			append_char('/');
			return T_C_COMMENT_LINE;
		} else {
			unget(c);
			append_char('/');
		}
	} else if (c == '?') {
		c = getch();
		if (c == '>')
			return T_END;
		else
			unget(c);
		append_char('?');
	} else {
		append_char(r);
	}

	return r;
}

static void
eatup_comment (void)
{
	int c;

	while (1) {
		c = getch();
		if (c == -1)
			return;

		if (c == '-') {
			c = getch();
			if (c == '-') {
				c = getch();
				if (c == '>')
					return;
				unget(c);
				unget('-');
			} else {
				unget(c);
			}
		}
	}
}

static void
scan_c ()
{
	int loop = 1;
	int c;

	while (loop) {
		c = getch_c_upper();
		if (c < 0)
			break;
		if (c == T_END)
			break;

		if (c == T_C_COMMENT_BLOCK) {
			int prev = 0;

			while (1) {
				c = getch_c_upper();
				if (c < 0)
					break;
				if (c == T_END) {
					loop = 0;
					break;
				}
				if ((c == '/') && (prev == '*')) {
					break;
				}
				prev = c;
			}
		} else if (c == T_C_COMMENT_LINE) {
			while (1) {
				c = getch_c_upper();
				if (c < 0)
					break;
				if (c == T_END) {
					loop = 0;
					break;
				}
				if (c == '\n') {
					break;
				}
			}
		}
	}
}

static void
scan_other ()
{
	int c;

	while (1) {
		c = getch_other_upper();
		if (c < 0)
			break;

		switch (c) {
			case T_COMMENT_BEGIN:
				eatup_comment();
				break;
			case T_HEAD_BEGIN:
				push(&object.head, TEXT_C_PROG);
				scan_c();
				pop();
				break;
			case T_TAIL_BEGIN:
				push(&object.tail, TEXT_C_PROG);
				scan_c();
				pop();
				break;
			case T_MEMBER_BEGIN:
				push(&object.member, TEXT_C_PROG);
				scan_c();
				pop();
				break;
			case T_INIT_BEGIN:
				push(&object.init, TEXT_C_PROG);
				scan_c();
				pop();
				break;
			case T_RELEASE_BEGIN:
				push(&object.release, TEXT_C_PROG);
				scan_c();
				pop();
				break;
			case T_EXPR_BEGIN:
				push(&object.code, TEXT_C_EXPR);
				scan_c();
				pop();
				break;
			case T_CODE_BEGIN:
				push(&object.code, TEXT_C_PROG);
				scan_c();
				pop();
				break;
			default:
				break;
		}
	}
}

static void
convert (void)
{
	memset(&object, 0, sizeof(object));

	push(&object.code, TEXT_OTHER);
	scan_other();
	append_text();

	fprintf(output, "/*Generated by aw-converter*/\n\n");
	fprintf(output, "#include <atomweb.h>\n\n");

	gen_code(object.head);

	fprintf(output, "typedef struct {\n");
	gen_code(object.member);
	fprintf(output, "} AW_UserData;\n\n");

	fprintf(output, "static AW_Ptr aw_create (AW_Session *sess) {\n");
	fprintf(output, "\tAW_Ptr ptr;\n");
	fprintf(output, "\tAW_UserData *udata;\n");
	fprintf(output, "\tudata = (AW_UserData*)malloc(sizeof(AW_UserData));\n");
	fprintf(output, "\tptr = (AW_Ptr)udata;\n");
	gen_code(object.init);
	fprintf(output, "\treturn ptr;\n");
	fprintf(output, "}\n\n");

	fprintf(output, "static AW_Result aw_run (AW_Session *sess, AW_Ptr ptr) {\n");
	fprintf(output, "\tAW_UserData *udata = (AW_UserData*)ptr;\n");
	gen_code(object.code);
	fprintf(output, "\tudata = udata;\n");
	fprintf(output, "\treturn AW_OK;\n");
	fprintf(output, "}\n\n");

	fprintf(output, "static void aw_release (AW_Ptr data) {\n");
	fprintf(output, "\tif (!data) return;\n");
	gen_code(object.release);
	fprintf(output, "\tfree(data);\n");
	fprintf(output, "}\n\n");


	gen_code(object.tail);

	fprintf(output, "const AW_Class aw_%s_class = {\n", c_name);
	fprintf(output, "\t%d,\n", 0);
	fprintf(output, "\t%s%s%s,\n", mime_type?"\"":"",
				mime_type?mime_type:"NULL",
				mime_type?"\"":"");
	fprintf(output, "\taw_create,\n");
	fprintf(output, "\taw_run,\n");
	fprintf(output, "\taw_release,\n");
	fprintf(output, "};\n\n");

	free_code(object.head);
	free_code(object.member);
	free_code(object.init);
	free_code(object.code);
	free_code(object.release);
	free_code(object.tail);
}

static void
convert_bin (void)
{
	int ch, size = 0;

	fprintf(output, "/*Generated by aw-converter*/\n\n");
	fprintf(output, "#include <atomweb.h>\n\n");

	fprintf(output, "static const AW_U8 aw_data[] = {\n");
	while (1) {
		ch = fgetc(input);
		if (ch == EOF)
			break;
		if (size) {
			fprintf(output, ",");

			if (!(size % 16)) {
				fprintf(output, "\n");
			}
		}

		fprintf(output, "0x%02x", ch);
		size ++;
	}
	fprintf(output, "};\n");
	
	fprintf(output, "const AW_Class aw_%s_class = {\n", c_name);
	fprintf(output, "\tAW_CLASS_FL_BINARY,\n");
	fprintf(output, "\t%s%s%s,\n", mime_type?"\"":"",
				mime_type?mime_type:"NULL",
				mime_type?"\"":"");
	fprintf(output, "\tNULL,\n");
	fprintf(output, "\tNULL,\n");
	fprintf(output, "\tNULL,\n");
	fprintf(output, "\taw_data,\n");
	fprintf(output, "\tsizeof(aw_data),\n");
	fprintf(output, "};\n\n");
}

int
main (int argc, char **argv)
{
	int r = 1;
	int len, slen;
	const char *ptr, *suffix;
	char *dir, *base;
	char path_buf[PATH_MAX];
	char dir_buf[PATH_MAX];
	char base_buf[PATH_MAX];

	/*Parse arguments*/
	parse_args(argc, argv);

	strcpy(dir_buf, input_name);
	strcpy(base_buf, input_name);
	suffix = ".aw";

	dir  = dirname(dir_buf);
	base = basename(base_buf);

	slen = strlen(suffix);
	len  = strlen(base);
	if ((len > slen) && !strcasecmp(base + len - slen, suffix)) {
		len -= slen;
		base[len] = 0;
	}

	if (!output_name) {
		sprintf(output_name_buf, "%s/%s.c", dir, base);
		output_name = output_name_buf;
	}

	if (!c_name) {
		sprintf(path_buf, "%s/%s", dir, base);
		c_name = aw_str_cname(path_buf, c_name_buf, sizeof(c_name_buf));
	}

	ptr = strrchr(base, '.');
	if (ptr) {
		suffix = ptr + 1;
	} else {
		suffix = NULL;
	}

	/*Open files*/
	input = fopen(input_name, "rb");
	if (!input) {
		fprintf(stderr, "cannot open \"%s\"\n", input_name);
		goto end;
	}

	output = fopen(output_name, "wb");
	if (!input) {
		fprintf(stderr, "cannot open \"%s\"\n", output_name);
		goto end;
	}

	/*Get MIME type*/
	if (suffix) {
		mime_type = aw_mime_get_type(suffix);
	}

	AW_INFO(("input: %s output: %s", input_name, output_name));

	if (bin_mode) {
		convert_bin();
	} else {
		convert();
	}

	r = 0;
end:
	if (input)
		fclose(input);
	if (output)
		fclose(output);
	return r;
}

