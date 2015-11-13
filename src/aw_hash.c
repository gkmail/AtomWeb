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
#include <aw_hash.h>

AW_U32
aw_direct_key (AW_Ptr key)
{
	return (AW_UIntPtr)key;
}

AW_U32
aw_string_key (AW_Ptr key)
{
	AW_Char *ch = (AW_Char*)key;
	AW_U32 v;

	if (!ch)
		return 0;

	v = 1978;
	while (*ch) {
		v = (v << 5) + v + *ch;
		ch ++;
	}

	return v;
}

AW_U32
aw_case_string_key (AW_Ptr key)
{
	AW_Char *ch = (AW_Char*)key;
	AW_U32 v;

	if (!ch)
		return 0;

	v = 1978;
	while (*ch) {
		v = (v << 5) + v + tolower(*ch);
		ch ++;
	}

	return v;
}

AW_Bool
aw_direct_equal (AW_Ptr k1, AW_Ptr k2)
{
	return (k1 == k2);
}

AW_Bool
aw_string_equal (AW_Ptr k1, AW_Ptr k2)
{
	return strcmp(k1, k2) ? AW_FALSE : AW_TRUE;
}

AW_Bool
aw_case_string_equal (AW_Ptr k1, AW_Ptr k2)
{
	return strcasecmp(k1, k2) ? AW_FALSE : AW_TRUE;
}

void
aw_hash_init (AW_Hash *hash, AW_KeyFunc key, AW_EqualFunc equ,
			AW_HashFreeFunc release)
{
	AW_ASSERT(hash && key && equ);

	hash->key = key;
	hash->equ = equ;
	hash->release = release;
	hash->entries = NULL;
	hash->count   = 0;
	hash->bucket  = 0;
}

void
aw_hash_deinit (AW_Hash *hash)
{
	AW_Size i;
	AW_HashEntry *ent, *enext;

	AW_ASSERT(hash);

	if (!hash->entries)
		return;

	for (i = 0; i < hash->bucket; i++) {
		for (ent = hash->entries[i]; ent; ent = enext) {
			enext = ent->next;

			if (hash->release)
				hash->release(ent->key, ent->value);

			free(ent);
		}
	}

	free(hash->entries);
}

AW_Result
aw_hash_add (AW_Hash *hash, AW_Ptr key, AW_Ptr value)
{
	AW_HashEntry *ent;
	AW_U32 kv;

	AW_ASSERT(hash);

	kv = hash->key(key);

	if (hash->count > 0) {
		for (ent = hash->entries[kv % hash->bucket];
					ent;
					ent = ent->next) {
			if (hash->equ(key, ent->key)) {
				return AW_NONE;
			}
		}
	}

	if (hash->bucket * 3 >= hash->count) {
		AW_HashEntry **buf, *enext;
		AW_Size nsize, i;
		AW_U32 k;

		nsize = AW_MAX(hash->count + 1, 9);
		buf   = malloc(nsize * sizeof(AW_HashEntry*));
		if (!buf) {
			AW_FATAL(("not enough memory"));
		}

		memset(buf, 0, nsize * sizeof(AW_HashEntry*));

		for (i = 0; i < hash->bucket; i++) {
			for (ent = hash->entries[i]; ent; ent = enext) {
				enext = ent->next;

				k = hash->key(ent->key) % nsize;
				ent->next = buf[k];
				buf[k] = ent;
			}
		}

		if (hash->entries)
			free(hash->entries);

		hash->entries = buf;
		hash->bucket  = nsize;
	}

	ent = (AW_HashEntry*)malloc(sizeof(AW_HashEntry));
	if (!ent) {
		AW_FATAL(("not enough memory"));
	}

	kv %= hash->bucket;

	ent->key   = key;
	ent->value = value;
	ent->next  = hash->entries[kv];
	hash->entries[kv] = ent;

	hash->count ++;

	return AW_OK;
}

AW_Result
aw_hash_lookup (AW_Hash *hash, AW_Ptr key, AW_Ptr *value)
{
	AW_HashEntry *ent;
	AW_U32 kv;

	AW_ASSERT(hash);

	if (hash->count == 0)
		return AW_NONE;

	kv = hash->key(key) % hash->bucket;

	for (ent = hash->entries[kv % hash->bucket];
				ent;
				ent = ent->next) {
		if (hash->equ(key, ent->key)) {
			if (value)
				*value = ent->value;
			return AW_OK;
		}
	}

	return AW_NONE;
}

AW_Result
aw_hash_remove (AW_Hash *hash, AW_Ptr key)
{
	AW_HashEntry **eprev, *ent;
	AW_U32 kv;

	AW_ASSERT(hash);

	if (hash->count == 0)
		return AW_NONE;

	kv = hash->key(key) % hash->bucket;

	eprev = &hash->entries[kv];
	for (ent = *eprev; ent; ent = ent->next) {
		if (hash->equ(key, ent->key)) {
			*eprev = ent->next;

			if (hash->release)
				hash->release(ent->key, ent->value);

			free(ent);
			
			hash->count --;
			return AW_OK;
		}
	}

	return AW_NONE;
}

void
aw_hash_for_each (AW_Hash *hash, AW_HashForEachFunc func,
			AW_Ptr data)
{
	AW_Size i;
	AW_HashEntry *ent;

	AW_ASSERT(hash && func);

	for (i = 0; i < hash->bucket; i++) {
		for (ent = hash->entries[i]; ent; ent = ent->next) {
			func(ent->key, ent->value, data);
		}
	}
}

