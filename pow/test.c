#include "spake.h"
#include <emscripten.h>

unsigned char* mul(unsigned char* x, unsigned char* y){
	size_t scanned;
	unsigned char *result;
	gcry_mpi_t a = gcry_mpi_new(0);
	gcry_mpi_t b = gcry_mpi_new(0);
	gcry_mpi_scan(&a, GCRYMPI_FMT_HEX, x, 0, &scanned);
	gcry_mpi_scan(&b, GCRYMPI_FMT_HEX, y, 0, &scanned);
	gcry_mpi_mul(a, a, b);
	gcry_mpi_aprint(GCRYMPI_FMT_HEX, &result, NULL, a);
	return result;
}

unsigned char* add(unsigned char* x, unsigned char* y){
	size_t scanned;
	unsigned char *result;
	gcry_mpi_t a = gcry_mpi_new(0);
	gcry_mpi_t b = gcry_mpi_new(0);
	gcry_mpi_scan(&a, GCRYMPI_FMT_HEX, x, 0, &scanned);
	gcry_mpi_scan(&b, GCRYMPI_FMT_HEX, y, 0, &scanned);
	gcry_mpi_add(a, a, b);
	gcry_mpi_aprint(GCRYMPI_FMT_HEX, &result, NULL, a);
	return result;
}

unsigned char* sub(unsigned char* x, unsigned char* y){
	size_t scanned;
	unsigned char *result;
	gcry_mpi_t a = gcry_mpi_new(0);
	gcry_mpi_t b = gcry_mpi_new(0);
	gcry_mpi_scan(&a, GCRYMPI_FMT_HEX, x, 0, &scanned);
	gcry_mpi_scan(&b, GCRYMPI_FMT_HEX, y, 0, &scanned);
	gcry_mpi_sub(a, a, b);
	gcry_mpi_aprint(GCRYMPI_FMT_HEX, &result, NULL, a);
	return result;
}

unsigned char* modPow(unsigned char* x, unsigned char* y, unsigned char* z){
	size_t scanned;
	unsigned char *result;
	gcry_mpi_t a = gcry_mpi_new(0);
	gcry_mpi_t b = gcry_mpi_new(0);
	gcry_mpi_t c = gcry_mpi_new(0);
	gcry_mpi_scan(&a, GCRYMPI_FMT_HEX, x, 0, &scanned);
	gcry_mpi_scan(&b, GCRYMPI_FMT_HEX, y, 0, &scanned);
	gcry_mpi_scan(&c, GCRYMPI_FMT_HEX, z, 0, &scanned);
	gcry_mpi_powm(a, a, b, c);
	gcry_mpi_aprint(GCRYMPI_FMT_HEX, &result, NULL, a);
	return result;
}

unsigned char* inv(unsigned char* x, unsigned char* y){
	size_t scanned;
	unsigned char *result;
	gcry_mpi_t a = gcry_mpi_new(0);
	gcry_mpi_t b = gcry_mpi_new(0);
	gcry_mpi_scan(&a, GCRYMPI_FMT_HEX, x, 0, &scanned);
	gcry_mpi_scan(&b, GCRYMPI_FMT_HEX, y, 0, &scanned);
	gcry_mpi_invm(a, a, b);
	gcry_mpi_aprint(GCRYMPI_FMT_HEX, &result, NULL, a);
	return result;
}

const char* concat(const char *s1, const char *s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    const char *result = malloc(len1+len2+1);
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2+1);
    return result;
}

const char* spakeNext(unsigned char* mIn, unsigned char* passHash){
	size_t scanned;
	unsigned char *skString;
	unsigned char *mString;
	gcry_mpi_t a = gcry_mpi_new(0);
	gcry_mpi_t sk = gcry_mpi_new(0);
	gcry_mpi_t m = gcry_mpi_new(0);
	gcry_mpi_t X = gcry_mpi_new(0);
	gcry_mpi_scan(&m, GCRYMPI_FMT_HEX, mIn, 0, &scanned);
	gcry_mpi_scan(&a, GCRYMPI_FMT_HEX, passHash, 0, &scanned);
	spake_next_client(&sk, &X, m, a);
	gcry_mpi_aprint(GCRYMPI_FMT_HEX, &skString, NULL, sk);
	gcry_mpi_aprint(GCRYMPI_FMT_HEX, &mString, NULL, X);
	return concat(concat(skString, ","), mString);
}

const char* genGcryptRand(unsigned char* x){
	size_t scanned;
	unsigned char *result;
	gcry_mpi_t a = gcry_mpi_new(0);
	gcry_mpi_t r = gcry_mpi_new(0);
	gcry_mpi_scan(&a, GCRYMPI_FMT_HEX, x, 0, &scanned);
	gen_rand(r, a);
	gcry_mpi_aprint(GCRYMPI_FMT_HEX, &result, NULL, r);
	return result;
}


int main(int argc, char *argv[]) {
	/* disable secmem, not available in emscripten */
	gcry_control (GCRYCTL_DISABLE_SECMEM, 0);
	
	return 0;
}
