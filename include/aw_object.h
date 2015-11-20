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

#ifndef _AW_OBJECT_H_
#define _AW_OBJECT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_session.h"
#include "aw_manager.h"

/**\brief The object is binary data*/
#define AW_CLASS_FL_BINARY 1

/**\brief The class of the object*/
struct AW_Class_s {
	AW_U32    flags; /**< Flags*/
	char     *mime;  /**< MIME type string*/
	/**\brief Object create function*/
	AW_Ptr    (*create) (AW_Session *sess);
	/**\brief Object running function*/
	AW_Result (*run) (AW_Session *sess, AW_Ptr data);
	/**\brief Object release function*/
	void      (*release) (AW_Ptr data);
	const AW_U8 *data; /**< Binary data*/
	AW_Size   size;    /**< Binary data size in bytes*/
};

/**\brief Object*/
struct AW_Object_s {
	const AW_Class *clazz; /**< The class of the object*/
	AW_Session     *sess;  /**< The session*/
	AW_Ptr          data;  /**< User data allocated by 'create' function*/
};

/**
 * \brief Create a new object
 * \param[in] sess The session
 * \param[in] clazz The class of the object
 * \return The allocated object
 */
extern AW_Object* aw_object_create (AW_Session *sess, const AW_Class *clazz);

/**
 * \brief Relase an unused object
 * \param[in] obj The object to be released
 */
extern void       aw_object_destroy (AW_Object *obj);

/**
 * \brief Get the user defined data pointer
 * \param[in] obj The object
 * \return The used defined data pointer
 */
extern AW_Ptr     aw_object_get_data (AW_Object *obj);

/**
 * \brief Run the object to generate response data
 * \param[in] obj The object
 * \retval AW_OK On success
 * \retval <0 On failed
 */
extern AW_Result  aw_object_run (AW_Object *obj);

#ifdef __cplusplus
}
#endif

#endif

