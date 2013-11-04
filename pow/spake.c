/*
 * spake.c
 *
 *  Created on: 13.10.2013
 *      Author: Franziskus Kiefer
 */
#include "spake.h"
#include <time.h>

/*
 * initialise spake session
 */
struct spake_session spake_init(int role, gcry_mpi_t g, gcry_mpi_t p, gcry_mpi_t M, gcry_mpi_t N, gcry_mpi_t pwd, int keySize) {
	struct spake_session session;
	gcry_mpi_t t;

	t = gcry_mpi_new(0);
	session.x = gcry_mpi_new(0);
	session.X = gcry_mpi_new(0);

	session.role = role;
	session.g = g;
	session.p = p;
	session.M = M;
	session.N = N;
	session.pwd = pwd;
	session.keySize = keySize;

	/* make dh key-pair */
	gen_rand(session.x, p);
	gcry_mpi_powm(session.X, g, session.x, p);

	/* create X* */
	if (role == 0) { /* client */
		gcry_mpi_powm(t, M, pwd, p);
		gcry_mpi_mulm(session.X, session.X, t, p);
	} else { /* server */
		gcry_mpi_powm(t, N, pwd, p);
		gcry_mpi_mulm(session.X, session.X, t, p);
	}

	return session;
}

/*
 * generate session key
 */
void spake_next(struct spake_session* session, gcry_mpi_t Y) {
	gcry_mpi_t k;
	k = gcry_mpi_new(0);
	unsigned char *tmp;
	size_t tmpSize;
	gcry_mpi_t t;

	t = gcry_mpi_new(0);

	/* calculate key */
	if (session->role == 0) { /* client */
		gcry_mpi_powm(t, session->N, session->pwd, session->p);
	} else { /* server */
		gcry_mpi_powm(t, session->M, session->pwd, session->p);
	}
	gcry_mpi_invm(t, t, session->p);
	gcry_mpi_mulm(k, Y, t, session->p);
	gcry_mpi_powm(k, k, session->x, session->p);

	/* hash it */
	gcry_md_hd_t hd;
    gcry_md_open(&hd, GCRY_MD_SHA256, 0);
 
	gcry_md_write(hd, "C", 1);
	gcry_md_write(hd, "S", 1);

	if (session->role == 0) { /* client */
		gcry_mpi_aprint(GCRYMPI_FMT_HEX, &tmp, &tmpSize, session->X);
		gcry_md_write(hd, tmp, tmpSize);

		gcry_mpi_aprint(GCRYMPI_FMT_HEX, &tmp, &tmpSize, Y);
		gcry_md_write(hd, tmp, tmpSize);
	} else { /* server */
		gcry_mpi_aprint(GCRYMPI_FMT_HEX, &tmp, &tmpSize, Y);
		gcry_md_write(hd, tmp, tmpSize);
	
		gcry_mpi_aprint(GCRYMPI_FMT_HEX, &tmp, &tmpSize, session->X);
		gcry_md_write(hd, tmp, tmpSize);
	}

	gcry_mpi_aprint(GCRYMPI_FMT_HEX, &tmp, &tmpSize, session->pwd);
	gcry_md_write(hd, tmp, tmpSize);
	
	gcry_mpi_aprint(GCRYMPI_FMT_HEX, &tmp, &tmpSize, k);
	gcry_md_write(hd, tmp, tmpSize);

	unsigned char* pi = gcry_md_read(hd, GCRY_MD_SHA256);
	session->k = calloc(session->keySize, sizeof(unsigned char));
    memcpy((char*)session->k, pi, session->keySize);
    gcry_md_close(hd);
	
}

void print_mpi (const char *name, gcry_mpi_t a) {
	unsigned char *buf;
	gcry_mpi_aprint (GCRYMPI_FMT_HEX, &buf, NULL, a);
	printf ("%s: %s\n", name, buf);
	gcry_free (buf);
}


void spake_next_client(gcry_mpi_t *sk, gcry_mpi_t *X, gcry_mpi_t m, gcry_mpi_t pwd) {
	gcry_mpi_t t = gcry_mpi_new(0);
	
	gcry_mpi_t g = gcry_mpi_new(0);
	gcry_mpi_t p = gcry_mpi_new(0);
	
	unsigned char* gString = "2";
	unsigned char* pString = "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF6955817183995497CEA956AE515D2261898FA051015728E5A8AACAA68FFFFFFFFFFFFFFFF";
	gcry_mpi_scan(&g, GCRYMPI_FMT_HEX, gString, 0, NULL);
	gcry_mpi_scan(&p, GCRYMPI_FMT_HEX, pString, 0, NULL);
	
	gcry_mpi_t M = gcry_mpi_new(0);
	gcry_mpi_t N = gcry_mpi_new(0);
	
	unsigned char* mString = "3d941d6d9cd4c77719840d6b391a63ca6ded5b5cf6aafeefb9ea530f523039e9c372736a79b7eb022e50029f7f2cb4fb16fd1def75657288eca90d2c880f306be76fe0341b3c8961ae6e61aabbb60e416069d97eeada2f1408f2017449dddcd5ac927f164b1a379727941bd7f2170d02ef12ef3ec801fae585ac7b9d4079f50feced64687128208d46e3e10c5d78eb05832f5322c07a3b4e14c6f595206fde99115e8eea19b5fb13dd434332ec3eccb41a4baa54a14183c3416313678697db8507abdcfc6a97c86099fa5172316d784c6997fc2e74e8e59c7c1bc90426164682f5bfbf6373b13ea90d7e13fbffd65e10c4ad96c38ccbf8e8def28d76746729dc";
	unsigned char* nString = "49bb3b5f7a3d9b500d36366e9935c11b7e159d36696b93d22d8dd742f1386f5d352caeca7e0891acfae0ed5856362e09428bf728f07f4a20092a4473497bebc58f5a1bf2a789f391ab3253cc23ec3f282fdb86a155c82d0f2fd8ba17f6a68d4a8afddaeef8bf155d3bca4b30de36be6a8652ee59f7239dbbc3f12c8c40998f3192271520414096ed726569f7b6b18f2198e87db6bcdf0b8bfd3da77f613a7db1e032b14bed73aa3d4c14cb4cc3f2f7b35feaf82685eb67a73217fec27c62e7e88523ea1c51352d5f42cb63dc1e9d128fe5a95252c01d2eda32b9843779dbaa332f557f0619265dfbaa920f9ce8003062275a367742b776999edc8ab1f766ba6c";
	gcry_mpi_scan(&M, GCRYMPI_FMT_HEX, mString, 0, NULL);
	gcry_mpi_scan(&N, GCRYMPI_FMT_HEX, nString, 0, NULL);
	
	gcry_mpi_t x = gcry_mpi_new(0);

time_t start = time(NULL);
	gen_rand(x, p);
time_t end = time(NULL);
printf("gen_rand time: %f\n", (start-end));
printf("--- fuuu ---\n");
	//gcry_mpi_scan(&x, GCRYMPI_FMT_HEX, "1234567890", 0, NULL);
	gcry_mpi_powm(*X, g, x, p);
	
	gcry_mpi_powm(t, N, pwd, p);
	gcry_mpi_mulm(*X, *X, t, p); // X <- publicKey

	gcry_mpi_powm(t, M, pwd, p);
	gcry_mpi_invm(t, t, p);
	gcry_mpi_mulm(*sk, m, t, p);
	gcry_mpi_powm(*sk, *sk, x, p); // sk <- session key
}
