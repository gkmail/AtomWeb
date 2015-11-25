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
#include <aw_map.h>

static const AW_MapNode*
find_node (const AW_Map *map, const AW_Char *name, AW_Size len)
{
	const AW_MapNode *n;
	const AW_MapLink *l;
	const AW_Char *ch, *chend;
	int nid = 0;
	int lid;

	ch  = name;
	chend = ch + len;

next_node:

	n   = &map->nodes[nid];
	lid = n->links;

	while (lid != -1) {
		l = &map->links[lid];

		if (l->chr == *ch) {
			ch ++;
			nid = l->node;

			if (ch < chend)
				goto next_node;
			else
				break;
		}

		lid = l->next;
	}

	if (!n->name)
		return NULL;

	if (strncasecmp(n->name, name, len))
		return NULL;

	return n;
}

const AW_Class*
aw_map_lookup (const AW_Map *map, const AW_Char *path)
{
	const AW_Char *begin, *end, *ptr;
	const AW_Map *base;
	const AW_MapNode *ent;
	AW_Size len;

	AW_ASSERT(map);

	if (!path) {
		path = "index.html";
	}

	len = strlen(path);
	begin = path;
	end   = begin + len;

	if (*begin == '/')
		begin++;

	base = map;

	while (1) {
		ptr = strchr(begin, '/');
		if (ptr) {
			len = ptr - begin;

			ent = find_node(base, begin, len);
			if (!ent || (ent->map_id == -1))
				return NULL;

			base  = &map[ent->map_id];
			begin = ptr + 1;
		} else {
			len = end - begin;

			if (len == 0) {
				begin = "index.html";
				len = strlen(begin);
			}

			ent = find_node(base, begin, len);
			if (!ent || !ent->clazz)
				return NULL;

			return ent->clazz;
		}
	}

	return NULL;
}

