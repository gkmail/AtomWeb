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

#ifndef _AW_MAP_H_
#define _AW_MAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_types.h"

/**\brief Map node link*/
typedef struct AW_MapLink_s AW_MapLink;
/**\brief Map node*/
typedef struct AW_MapNode_s AW_MapNode;

/**\brief Map node link*/
struct AW_MapLink_s {
	AW_Char   chr;  /**< The character*/
	int       next; /**< The next link in the list*/
	int       node; /**< The destination node*/
};

/**\brief Map node*/
struct AW_MapNode_s {
	int       links;  /**< Links list*/
	int       map_id; /**< The sub directory's map index*/
	const char     *name;  /**< Name of the object*/
	const AW_Class *clazz; /**< The class of the object*/
};

/**
 * \brief Web object map
 * Use the map to lookup object by the path name.
 */
struct AW_Map_s {
	const AW_MapNode *nodes; /**< Nodes array*/
	const AW_MapLink *links; /**< Links array*/
};

/**
 * \brief Lookup a web object use the map
 * \param[in] map The web objects' map
 * \param[in] path The object's path name
 */
extern const AW_Class* aw_map_lookup (const AW_Map *map, const AW_Char *path);

#ifdef __cplusplus
}
#endif

#endif

