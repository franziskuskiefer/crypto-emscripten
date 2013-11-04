EMCC= `./find-emcc.py`/emcc

GCRYPT_BUILD= build/libgcrypt-1.5.3
LIBS= -L./build/lib
GCRYPT= -lgcrypt -lgpg-error
OTR= -lotr
NODE= --pre-js ./src/pre-node.js
HTML= --post-js ./src/post.js
OPTIMISATION= -O2 --llvm-opts 1 --llvm-lto 0 -s ASM_JS=1 --closure 1 --js-library src/library_gcrypt.js \
    -s LINKABLE=1 -s EXPORTED_FUNCTIONS="['_main','__gcry_mpi_new','__gcry_mpi_set','__gcry_mpi_release','__gcry_mpi_print','__gcry_mpi_scan','__gcry_strerror']"
HTML_OPTIMISATION= -O2 --llvm-opts 1 --llvm-lto 0 -s ASM_JS=1 --closure 0 --js-library src/library_gcrypt.js \
    -s LINKABLE=1 -s EXPORTED_FUNCTIONS="['_main','__gcry_mpi_new','__gcry_mpi_set','__gcry_mpi_release','__gcry_mpi_print','__gcry_mpi_scan','__gcry_strerror','_mul','_add','_sub','_modPow','_inv','_spakeNext','_genGcryptRand']"

TEST_OBJS=benchmark.js basic.js pubkey.js keygen.js prime.js ac-data.js ac.js ac-schemes.js curves.js \
    fips186-dsa.js fipsdrv.js hmac.js mpitests.js pkcs1v2.js random.js register.js rsacvt.js t-kdf.js \
    t-mpi-bit.js tsexp.o version.js

all: libotr-test test

test: basic.js pubkey.js random.js
	
test-all: $(TEST_OBJS)

libotr-test:
	mkdir -p tests/
	$(EMCC) src/libotr-test.c -o tests/libotr-test.js -I./build/include \
         $(LIBS) $(OTR) $(GCRYPT) $(NODE) $(OPTIMISATION)

run-test-all: $(TEST_OBJS:.js=.run-silent)

run-test: basic.run pubkey.run random.run

clean:
	rm -fr tests/*

%.js:
	mkdir -p tests/
	$(EMCC) $(GCRYPT_BUILD)/tests/$(@:.js=.o) -o tests/$(@) $(LIBS) $(GCRYPT) $(NODE) $(OPTIMISATION)

%.run:
	node tests/$(@:.run=) --verbose

%.run-silent:
	node tests/$(@:.run-silent=)

web:
	mkdir -p tests/
	$(EMCC) $(GCRYPT_BUILD)/tests/basic.o -o tests/basic.html $(LIBS) $(GCRYPT) $(OPTIMISATION) --shell-file ./src/shell.html

bench:
	mkdir -p tests/
	$(EMCC) $(GCRYPT_BUILD)/tests/benchmark.o -o tests/benchmark.js $(LIBS) $(GCRYPT) $(NODE) $(OPTIMISATION)

# POW (SPAKE)

build-pow: util.pow spake.pow test.pow

%.pow:
	cd pow/; \
	emcc -I. -I..  -I../src -I../src  -I/home/franziskus/Workspace/github/crypto-emscripten/build/include -m32 -fvisibility=hidden -Wall -MT $(@:.pow=.o) -MD -MP -c -o $(@:.pow=.o) $(@:.pow=.c)

pow: build-pow
	$(EMCC) pow/util.o pow/spake.o pow/test.o -o pow/test.js $(LIBS) $(GCRYPT) $(NODE) $(HTML_OPTIMISATION)

pow-js: build-pow
	$(EMCC) pow/util.o pow/spake.o pow/test.o -o pow/gcrypt.js $(LIBS) $(GCRYPT) $(HTML_OPTIMISATION) --shell-file ./pow/shell.js

pow-html: build-pow
	$(EMCC) pow/util.o pow/spake.o pow/test.o -o pow/test.html $(LIBS) $(GCRYPT) $(HTML_OPTIMISATION) --shell-file ./pow/shell.html

run-pow:
	node pow/test.js --verbose

# Own simple tests

build-my-test:
	cd my-test/; \
	EMCC_DEBUG=1 emcc -I. -I.. -I./build/include -I/home/franziskus/Workspace/github/crypto-emscripten/build/include -m32 -fvisibility=hidden -Wall -MT test.o -MD -MP -MF .deps/test.Tpo -c -o test.o test.c

my-test: build-my-test
	EMCC_DEBUG=1 $(EMCC) my-test/test.o -o my-test/test.js $(LIBS) $(GCRYPT) $(NODE) $(OPTIMISATION)

run-my-test:
	node my-test/test.js --verbose
