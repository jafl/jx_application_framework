/*	$KAME: vmbuf.c,v 1.10 2001/04/03 15:51:57 thorpej Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/param.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vmbuf.h"

vchar_t*
vmalloc
	(
	const size_t size
	)
{
	vchar_t *var;

	if ((var = (vchar_t *)malloc(sizeof(*var))) == NULL)
		return NULL;

	var->l = size;
	var->v = (caddr_t)calloc(1, size);
	if (var->v == NULL) {
		free(var);
		return NULL;
	}

	return var;
}

vchar_t*
vrealloc
	(
	vchar_t*		ptr,
	const size_t	size
	)
{
	caddr_t v;

	if (ptr != NULL) {
		if ((v = (caddr_t)realloc(ptr->v, size)) == NULL) {
			vfree(ptr);
			return NULL;
		}
		memset(v + ptr->l, 0, size - ptr->l);
		ptr->v = v;
		ptr->l = size;
	} else {
		if ((ptr = vmalloc(size)) == NULL)
			return NULL;
	}

	return ptr;
}

void
vfree
	(
	vchar_t* var
	)
{
	if (var == NULL)
		return;

	if (var->v)
		free(var->v);

	free(var);
}

vchar_t*
vdup
	(
	const vchar_t* src
	)
{
	vchar_t *new;

	if ((new = vmalloc(src->l)) == NULL)
		return NULL;

	memcpy(new->v, src->v, src->l);

	return new;
}

vchar_t*
vcopy
	(
	const void*		src,
	const size_t	size
	)
{
	vchar_t *new;

	if ((new = vmalloc(size)) == NULL)
		return NULL;

	memcpy(new->v, src, size);

	return new;
}

char*
vstr
	(
	const vchar_t* src
	)
{
	char* new;

	if ((new = calloc(1, src->l+1)) == NULL)
		return NULL;

	memcpy(new, src->v, src->l);

	return new;
}

int
vcmp
	(
	const vchar_t* v1,
	const vchar_t* v2
	)
{
	if (v1->l != v2->l)
		{
		return 1;
		}
	else
		{
		return memcmp(v1->v, v2->v, v1->l);
		}
}
