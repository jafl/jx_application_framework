/*	$KAME: crypto_openssl.c,v 1.68 2001/08/20 06:46:28 itojun Exp $	*/

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
#include <limits.h>
#include <string.h>

#include "rijndael-api-fst.h"
#include "sha2.h"

#include "sha1.h"
#include "md5.h"

#include "crypto_openssl.h"

// glue

#define SHA_CTX				struct sha1_ctxt
#define SHA1_Init			SHA1Init
#define SHA1_Update			SHA1Update
#define SHA1_Final			SHA1Final
#define SHA_DIGEST_LENGTH	SHA1_RESULTLEN

#define MD5_Init	MD5Init
#define MD5_Update	MD5Update
#define MD5_Final	MD5Final

/*
 * AES(RIJNDAEL)-CBC
 */
vchar_t *
eay_aes_encrypt(data, key, iv)
	vchar_t *data, *key, *iv;
{
	vchar_t *res;
	keyInstance k;
	cipherInstance c;

	memset(&k, 0, sizeof(k));
	if (rijndael_makeKey(&k, DIR_ENCRYPT, key->l << 3, key->v) < 0)
		return NULL;

	/* allocate buffer for result */
	if ((res = vmalloc(data->l)) == NULL)
		return NULL;

	/* encryption data */
	memset(&c, 0, sizeof(c));
	if (rijndael_cipherInit(&c, MODE_CBC, iv->v) < 0)
		return NULL;
	if (rijndael_blockEncrypt(&c, &k, data->v, data->l << 3, res->v) < 0)
		return NULL;

	return res;
}

vchar_t *
eay_aes_decrypt(data, key, iv)
	vchar_t *data, *key, *iv;
{
	vchar_t *res;
	keyInstance k;
	cipherInstance c;

	memset(&k, 0, sizeof(k));
	if (rijndael_makeKey(&k, DIR_DECRYPT, key->l << 3, key->v) < 0)
		return NULL;

	/* allocate buffer for result */
	if ((res = vmalloc(data->l)) == NULL)
		return NULL;

	/* decryption data */
	memset(&c, 0, sizeof(c));
	if (rijndael_cipherInit(&c, MODE_CBC, iv->v) < 0)
		return NULL;
	if (rijndael_blockDecrypt(&c, &k, data->v, data->l << 3, res->v) < 0)
		return NULL;

	return res;
}

/*
 * SHA2-512 functions
 */
caddr_t
eay_sha2_512_init()
{
	SHA512_CTX *c = malloc(sizeof(*c));

	SHA512_Init(c);

	return((caddr_t)c);
}

void
eay_sha2_512_update(c, data)
	caddr_t c;
	vchar_t *data;
{
	SHA512_Update((SHA512_CTX *)c, data->v, data->l);

	return;
}

vchar_t *
eay_sha2_512_final(c)
	caddr_t c;
{
	vchar_t *res;

	if ((res = vmalloc(SHA512_DIGEST_LENGTH)) == 0)
		return(0);

	SHA512_Final(res->v, (SHA512_CTX *)c);
	(void)free(c);

	return(res);
}

vchar_t *
eay_sha2_512_one(data)
	vchar_t *data;
{
	caddr_t ctx;
	vchar_t *res;

	ctx = eay_sha2_512_init();
	eay_sha2_512_update(ctx, data);
	res = eay_sha2_512_final(ctx);

	return(res);
}

int
eay_sha2_512_hashlen()
{
	return SHA512_DIGEST_LENGTH << 3;
}

/*
 * SHA2-384 functions
 */
caddr_t
eay_sha2_384_init()
{
	SHA384_CTX *c = malloc(sizeof(*c));

	SHA384_Init(c);

	return((caddr_t)c);
}

void
eay_sha2_384_update(c, data)
	caddr_t c;
	vchar_t *data;
{
	SHA384_Update((SHA384_CTX *)c, data->v, data->l);

	return;
}

vchar_t *
eay_sha2_384_final(c)
	caddr_t c;
{
	vchar_t *res;

	if ((res = vmalloc(SHA384_DIGEST_LENGTH)) == 0)
		return(0);

	SHA384_Final(res->v, (SHA384_CTX *)c);
	(void)free(c);

	return(res);
}

vchar_t *
eay_sha2_384_one(data)
	vchar_t *data;
{
	caddr_t ctx;
	vchar_t *res;

	ctx = eay_sha2_384_init();
	eay_sha2_384_update(ctx, data);
	res = eay_sha2_384_final(ctx);

	return(res);
}

int
eay_sha2_384_hashlen()
{
	return SHA384_DIGEST_LENGTH << 3;
}

/*
 * SHA2-256 functions
 */
caddr_t
eay_sha2_256_init()
{
	SHA256_CTX *c = malloc(sizeof(*c));

	SHA256_Init(c);

	return((caddr_t)c);
}

void
eay_sha2_256_update(c, data)
	caddr_t c;
	vchar_t *data;
{
	SHA256_Update((SHA256_CTX *)c, data->v, data->l);

	return;
}

vchar_t *
eay_sha2_256_final(c)
	caddr_t c;
{
	vchar_t *res;

	if ((res = vmalloc(SHA256_DIGEST_LENGTH)) == 0)
		return(0);

	SHA256_Final(res->v, (SHA256_CTX *)c);
	(void)free(c);

	return(res);
}

vchar_t *
eay_sha2_256_one(data)
	vchar_t *data;
{
	caddr_t ctx;
	vchar_t *res;

	ctx = eay_sha2_256_init();
	eay_sha2_256_update(ctx, data);
	res = eay_sha2_256_final(ctx);

	return(res);
}

int
eay_sha2_256_hashlen()
{
	return SHA256_DIGEST_LENGTH << 3;
}

/*
 * SHA functions
 */
caddr_t
eay_sha1_init()
{
	SHA_CTX *c = malloc(sizeof(*c));

	SHA1_Init(c);

	return((caddr_t)c);
}

void
eay_sha1_update(c, data)
	caddr_t c;
	vchar_t *data;
{
	SHA1_Update((SHA_CTX *)c, data->v, data->l);

	return;
}

vchar_t *
eay_sha1_final(c)
	caddr_t c;
{
	vchar_t *res;

	if ((res = vmalloc(SHA_DIGEST_LENGTH)) == 0)
		return(0);

	SHA1_Final(res->v, (SHA_CTX *)c);
	(void)free(c);

	return(res);
}

vchar_t *
eay_sha1_one(data)
	vchar_t *data;
{
	caddr_t ctx;
	vchar_t *res;

	ctx = eay_sha1_init();
	eay_sha1_update(ctx, data);
	res = eay_sha1_final(ctx);

	return(res);
}

int
eay_sha1_hashlen()
{
	return SHA_DIGEST_LENGTH << 3;
}

/*
 * MD5 functions
 */
caddr_t
eay_md5_init()
{
	MD5_CTX *c = malloc(sizeof(*c));

	MD5_Init(c);

	return((caddr_t)c);
}

void
eay_md5_update(c, data)
	caddr_t c;
	vchar_t *data;
{
	MD5_Update((MD5_CTX *)c, data->v, data->l);

	return;
}

vchar_t *
eay_md5_final(c)
	caddr_t c;
{
	vchar_t *res;

	if ((res = vmalloc(MD5_DIGEST_LENGTH)) == 0)
		return(0);

	MD5_Final(res->v, (MD5_CTX *)c);
	(void)free(c);

	return(res);
}

vchar_t *
eay_md5_one(data)
	vchar_t *data;
{
	caddr_t ctx;
	vchar_t *res;

	ctx = eay_md5_init();
	eay_md5_update(ctx, data);
	res = eay_md5_final(ctx);

	return(res);
}

int
eay_md5_hashlen()
{
	return MD5_DIGEST_LENGTH << 3;
}
