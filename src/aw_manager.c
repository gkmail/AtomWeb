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
#include <aw_manager.h>

static void
ptr_free (AW_Ptr ptr)
{
	if (ptr)
		free(ptr);
}

static void
res_free (AW_Ptr key, AW_Ptr value)
{
	AW_FreeFunc free = (AW_FreeFunc)value;

	if (free)
		free(key);
}

void
aw_manager_init (AW_Manager *man)
{
	AW_ASSERT(man);

	aw_direct_hash_init(&man->res_hash, res_free);
}

void
aw_manager_deinit (AW_Manager *man)
{
	AW_ASSERT(man);

	aw_hash_deinit(&man->res_hash);
}

void
aw_manager_add_res (AW_Manager *man, AW_Ptr ptr, AW_FreeFunc free)
{
	AW_ASSERT(man);

	aw_hash_add(&man->res_hash, ptr, free);
}

void
aw_manager_remove_res (AW_Manager *man, AW_Ptr ptr)
{
	AW_ASSERT(man);

	aw_hash_remove(&man->res_hash, ptr);
}

AW_Ptr
aw_manager_alloc (AW_Manager *man, AW_Size size)
{
	AW_Ptr ptr;

	AW_ASSERT(man);

	ptr = (AW_Ptr)malloc(size);
	if (!ptr)
		return ptr;

	aw_manager_add_res(man, ptr, ptr_free);
	return ptr;
}

