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
#include <aw_object.h>
#include <aw_session.h>
#include <aw_manager.h>

static void
obj_free (AW_Ptr ptr)
{
	AW_Object *obj = (AW_Object*)ptr;

	free(obj);
}

AW_Object*
aw_object_create (AW_Session *sess, const AW_Class *clazz)
{
	AW_Object *obj;
	AW_Ptr data;

	AW_ASSERT(sess && clazz);

	obj = malloc(sizeof(AW_Object));
	if (!obj) {
		AW_FATAL(("not enough memory"));
	}

	obj->clazz = clazz;
	obj->sess  = sess;
	obj->data  = NULL;

	aw_manager_add_res(&sess->man, obj, obj_free);

	if (clazz->create) {
		data = clazz->create(sess);
		if (data) {
			obj->data = data;

			if (clazz->release) {
				aw_manager_add_res(&sess->man, data, clazz->release);
			}
		}
	}

	return obj;
}

void
aw_object_destroy (AW_Object *obj)
{
	AW_ASSERT(obj);

	aw_manager_remove_res(&obj->sess->man, obj);
}

AW_Ptr
aw_object_get_data (AW_Object *obj)
{
	AW_ASSERT(obj);

	return obj->data;
}

AW_Result
aw_object_run (AW_Object *obj)
{
	AW_ASSERT(obj);

	return obj->clazz->run(obj->sess, obj->data);
}

