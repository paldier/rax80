/* vi: set sw=4 ts=4: */
/*
 * Copyright 1989 - 1991, Julianne Frances Haugh <jockgrrl@austin.rr.com>
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
 * 3. Neither the name of Julianne F. Haugh nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JULIE HAUGH AND CONTRIBUTORS ''AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL JULIE HAUGH OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "libbb.h"
#include <openssl/evp.h>
int evp_sha256(char * in_str, char * digest)
{
    EVP_MD_CTX *evp_ctx;
    const EVP_MD *md;
    unsigned char dige_value[EVP_MAX_MD_SIZE];
    int md_len, i;
    char * str_idx;

    if (digest == NULL || in_str == NULL ) {
        digest[0] = '\0';
        return -1;
    }

    OpenSSL_add_all_digests();
    md = EVP_get_digestbyname("SHA256");
    evp_ctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(evp_ctx, md, NULL);
    EVP_DigestUpdate(evp_ctx, in_str, strlen(in_str));
    EVP_DigestFinal_ex(evp_ctx, dige_value, &md_len);
    EVP_MD_CTX_destroy(evp_ctx);

    str_idx = digest;
    for(i = 0; i < md_len; i++) {
        sprintf(str_idx, "%02x", dige_value[i]);
        str_idx += 2;
    }

    /* Call this once before exit. */
    EVP_cleanup();
}


#define SHADOW_BUFSIZE 256

/* Retrieve encrypted password string for pw.
 * If pw == NULL, return a string which fails password check against any
 * password.
 */
#if !ENABLE_FEATURE_SHADOWPASSWDS
#define get_passwd(pw, buffer) get_passwd(pw)
#endif
static const char *get_passwd(const struct passwd *pw, char buffer[SHADOW_BUFSIZE])
{
	const char *pass;

	if (!pw)
		return "aa"; /* "aa" will never match */

	pass = pw->pw_passwd;
#if ENABLE_FEATURE_SHADOWPASSWDS
	/* Using _r function to avoid pulling in static buffers */
	if ((pass[0] == 'x' || pass[0] == '*') && !pass[1]) {
		struct spwd spw;
		int r;
		/* getspnam_r may return 0 yet set result to NULL.
		 * At least glibc 2.4 does this. Be extra paranoid here. */
		struct spwd *result = NULL;
		r = getspnam_r(pw->pw_name, &spw, buffer, SHADOW_BUFSIZE, &result);
		pass = (r || !result) ? "aa" : result->sp_pwdp;
	}
#endif
	return pass;
}

/*
 * Return CHECKPASS_PW_HAS_EMPTY_PASSWORD if PW has an empty password.
 * Return 1 if the user gives the correct password for entry PW,
 * 0 if not.
 * NULL pw means "just fake it for login with bad username"
 */
int FAST_FUNC check_password(const struct passwd *pw, const char *plaintext)
{
	IF_FEATURE_SHADOWPASSWDS(char buffer[SHADOW_BUFSIZE];)
	char *encrypted;
	const char *pw_pass;
	int r;

	pw_pass = get_passwd(pw, buffer);
	if (!pw_pass[0]) { /* empty password field? */
		return CHECKPASS_PW_HAS_EMPTY_PASSWORD;
	}

#if 0
	encrypted = pw_encrypt(plaintext, /*salt:*/ pw_pass, 1);
	r = (strcmp(encrypted, pw_pass) == 0);
	free(encrypted);
#else
    {
        char sha256_digest[128]="";
        evp_sha256(plaintext, sha256_digest);
        //memset(unencrypted, 0, strlen(unencrypted));
        return strcmp(sha256_digest, pw_pass) == 0;
    }
#endif 
	return r;
}


/* Ask the user for a password.
 * Return CHECKPASS_PW_HAS_EMPTY_PASSWORD without asking if PW has an empty password.
 * Return -1 on EOF, error while reading input, or timeout.
 * Return 1 if the user gives the correct password for entry PW,
 * 0 if not.
 *
 * NULL pw means "just fake it for login with bad username"
 */
int FAST_FUNC ask_and_check_password_extended(const struct passwd *pw,
		int timeout, const char *prompt)
{
	IF_FEATURE_SHADOWPASSWDS(char buffer[SHADOW_BUFSIZE];)
	char *plaintext;
	const char *pw_pass;
	int r;

	pw_pass = get_passwd(pw, buffer);
	if (!pw_pass[0]) /* empty password field? */
		return CHECKPASS_PW_HAS_EMPTY_PASSWORD;

	plaintext = bb_ask(STDIN_FILENO, timeout, prompt);
	if (!plaintext) {
		/* EOF (such as ^D) or error (such as ^C) or timeout */
		return -1;
	}

	r = check_password(pw, plaintext);
	nuke_str(plaintext);
	return r;
}

int FAST_FUNC ask_and_check_password(const struct passwd *pw)
{
	return ask_and_check_password_extended(pw, 0, "Password: ");
}
