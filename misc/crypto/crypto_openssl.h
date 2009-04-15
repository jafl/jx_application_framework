/*	$KAME: crypto_openssl.h,v 1.23 2001/08/14 12:26:06 sakane Exp $	*/

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

#ifndef _CRYPTO_OPENSSL_H_
#define _CRYPTO_OPENSSL_H_

#include "vmbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* AES(RIJNDAEL) */
extern vchar_t *eay_aes_encrypt __P((vchar_t *, vchar_t *, vchar_t *));
extern vchar_t *eay_aes_decrypt __P((vchar_t *, vchar_t *, vchar_t *));

/* SHA2 functions */
extern caddr_t eay_sha2_512_init __P((void));
extern void eay_sha2_512_update __P((caddr_t, vchar_t *));
extern vchar_t *eay_sha2_512_final __P((caddr_t));
extern vchar_t *eay_sha2_512_one __P((vchar_t *));
extern int eay_sha2_512_hashlen __P((void));

extern caddr_t eay_sha2_384_init __P((void));
extern void eay_sha2_384_update __P((caddr_t, vchar_t *));
extern vchar_t *eay_sha2_384_final __P((caddr_t));
extern vchar_t *eay_sha2_384_one __P((vchar_t *));
extern int eay_sha2_384_hashlen __P((void));

extern caddr_t eay_sha2_256_init __P((void));
extern void eay_sha2_256_update __P((caddr_t, vchar_t *));
extern vchar_t *eay_sha2_256_final __P((caddr_t));
extern vchar_t *eay_sha2_256_one __P((vchar_t *));
extern int eay_sha2_256_hashlen __P((void));

/* SHA functions */
extern caddr_t eay_sha1_init __P((void));
extern void eay_sha1_update __P((caddr_t, vchar_t *));
extern vchar_t *eay_sha1_final __P((caddr_t));
extern vchar_t *eay_sha1_one __P((vchar_t *));
extern int eay_sha1_hashlen __P((void));

/* MD5 functions */
extern caddr_t eay_md5_init __P((void));
extern void eay_md5_update __P((caddr_t, vchar_t *));
extern vchar_t *eay_md5_final __P((caddr_t));
extern vchar_t *eay_md5_one __P((vchar_t *));
extern int eay_md5_hashlen __P((void));

#ifdef __cplusplus
}
#endif

#endif
