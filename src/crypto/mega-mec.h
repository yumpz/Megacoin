// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 Bitcoin Developers
// Copyright (c) 2014 Talkcoin Developers
// Copyright (c) 2019 Limxtec developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MEGACOIN_CRYPTO_MEGA_MEC_H
#define MEGACOIN_CRYPTO_MEGA_MEC_H

#include <uint256.h>
#include <algorithm>    // std::next_permutation

#include <crypto/sph_blake.h>
#include <crypto/sph_bmw.h>
#include <crypto/sph_groestl.h>
#include <crypto/sph_jh.h>
#include <crypto/sph_keccak.h>
#include <crypto/sph_skein.h>
#include <crypto/sph_luffa.h>
#include <crypto/sph_cubehash.h>
#include <crypto/sph_shavite.h>
#include <crypto/sph_simd.h>
#include <crypto/sph_echo.h>
#include <crypto/sph_hamsi.h>
#include <crypto/sph_fugue.h>
#include <crypto/sph_shabal.h>
#include <crypto/sph_whirlpool.h>
extern "C" {
#include <crypto/sph_sha2.h>
//#include <crypto/sph_tiger.h>
}
#include "lyra2.h"
//#include "gost_streebog.h"

#define HASH_FUNC_BASE_TIMESTAMP 1492973331 // BitCore: Genesis Timestamp
#define HASH_FUNC_COUNT_1 8
#define HASH_FUNC_COUNT_2 8
#define HASH_FUNC_COUNT_3 7
#define HASH_FUNC_VAR_1 2100
#define HASH_FUNC_VAR_2 2100
#define HASH_FUNC_COUNT_PERMUTATIONS_7 5040
#define HASH_FUNC_COUNT_PERMUTATIONS 40320 



template<typename T1>
inline uint256 Mega_Mec(const T1 pbegin, const T1 pend,uint32_t timestamp)
{
//	static std::chrono::duration<double>[16];
    int hashSelection;

    sph_blake512_context     ctx_blake;      //0
    sph_bmw512_context       ctx_bmw;        //1
    sph_groestl512_context   ctx_groestl;    //2
    sph_jh512_context        ctx_jh;         //3
    sph_keccak512_context    ctx_keccak;     //4
    sph_skein512_context     ctx_skein;      //5
    sph_luffa512_context     ctx_luffa;      //6
    sph_cubehash512_context  ctx_cubehash;   //7
    sph_shavite512_context   ctx_shavite;    //8
    sph_simd512_context      ctx_simd;       //9
    sph_echo512_context      ctx_echo;       //A
    sph_hamsi512_context     ctx_hamsi;      //B
    sph_fugue512_context     ctx_fugue;      //C
    sph_shabal512_context    ctx_shabal;     //D
    sph_whirlpool_context    ctx_whirlpool;  //E
    sph_sha512_context        ctx_sha512;
   // sph_tiger_context         ctx_tiger;

    static unsigned char pblank[1];
    arith_uint512 hash[23];
    uint32_t permutation_1[HASH_FUNC_COUNT_1];
	uint32_t permutation_2[HASH_FUNC_COUNT_2 + HASH_FUNC_COUNT_1];
	uint32_t permutation_3[HASH_FUNC_COUNT_3 + HASH_FUNC_COUNT_2 + HASH_FUNC_COUNT_1];
    

    			//Init1
			for (uint32_t i = 1; i < HASH_FUNC_COUNT_1; i++) {
				permutation_1[i] = i;
				//std::cout << "Init1: " << permutation_1[i] << " \n";
			}

			//Init2
			for (uint32_t i = HASH_FUNC_COUNT_1; i < HASH_FUNC_COUNT_2 + HASH_FUNC_COUNT_1; i++) {
				permutation_2[i] = i;
				//std::cout << "Init2: " << permutation_2[i] << " \n";
			}

			//Init3
			for (uint32_t i = HASH_FUNC_COUNT_1 + HASH_FUNC_COUNT_2; i < HASH_FUNC_COUNT_3 + HASH_FUNC_COUNT_2 + HASH_FUNC_COUNT_1; i++) {
				permutation_3[i] = i;
				//std::cout << "Init3: " << permutation_3[i] << " \n";
			}
			
			uint32_t steps_1 = (timestamp - HASH_FUNC_BASE_TIMESTAMP) % HASH_FUNC_COUNT_PERMUTATIONS_7;
			for (uint32_t i = 0; i < steps_1; i++) {
				std::next_permutation(permutation_1, permutation_1 + HASH_FUNC_COUNT_1);
			}
			
			uint32_t steps_2 = (timestamp+ HASH_FUNC_VAR_1 - HASH_FUNC_BASE_TIMESTAMP) % HASH_FUNC_COUNT_PERMUTATIONS;
			for (uint32_t i = 0; i < steps_2; i++) {
				std::next_permutation(permutation_2 + HASH_FUNC_COUNT_1, permutation_2 + HASH_FUNC_COUNT_1 + HASH_FUNC_COUNT_2);
			}

			uint32_t steps_3 = (timestamp+ HASH_FUNC_VAR_2 - HASH_FUNC_BASE_TIMESTAMP) % HASH_FUNC_COUNT_PERMUTATIONS_7;
			for (uint32_t i = 0; i < steps_3; i++) {
				std::next_permutation(permutation_3 + HASH_FUNC_COUNT_1 + HASH_FUNC_COUNT_2, permutation_3 + HASH_FUNC_COUNT_1 + HASH_FUNC_COUNT_2 + HASH_FUNC_COUNT_3);
			}

            int lenToHash = 64;
            int lenToHashinit = (pend - pbegin) * sizeof(pbegin[0]);
            const void *toHash; 
            const void *toHashinit = (pbegin == pend ? pblank : static_cast<const void*>(&pbegin[0]));

			sph_blake512_init(&ctx_blake);
            sph_blake512 (&ctx_blake, toHashinit, lenToHashinit);
			sph_blake512_close(&ctx_blake, static_cast<void*>(&hash[0]));

			for (int i = 1; i < HASH_FUNC_COUNT_1; i++) {
            toHash = static_cast<const void*>(&hash[i]);;
				switch (permutation_1[i]) {
				case 1:
					sph_blake512_init(&ctx_blake);
					sph_blake512(&ctx_blake, toHash, lenToHash);
					sph_blake512_close(&ctx_blake, static_cast<void*>(&hash[i]));
					break;
				case 2:
					sph_bmw512_init(&ctx_bmw);
					sph_bmw512(&ctx_bmw, toHash, lenToHash);
					sph_bmw512_close(&ctx_bmw, static_cast<void*>(&hash[i]));
					break;
				case 3:
					sph_groestl512_init(&ctx_groestl);
					sph_groestl512(&ctx_groestl, toHash, lenToHash);
					sph_groestl512_close(&ctx_groestl, static_cast<void*>(&hash[i]));
					break;
				case 4:
					sph_jh512_init(&ctx_jh);
					sph_jh512(&ctx_jh, toHash, lenToHash);
					sph_jh512_close(&ctx_jh, static_cast<void*>(&hash[i]));
					break;
				case 5:
					sph_keccak512_init(&ctx_keccak);
					sph_keccak512(&ctx_keccak, toHash, lenToHash);
					sph_keccak512_close(&ctx_keccak, static_cast<void*>(&hash[i]));
					break;
				case 6:
					sph_skein512_init(&ctx_skein);
					sph_skein512(&ctx_skein, toHash, lenToHash);
					sph_skein512_close(&ctx_skein, static_cast<void*>(&hash[i]));
					break;
				case 7:
					sph_luffa512_init(&ctx_luffa);
					sph_luffa512(&ctx_luffa,  toHash, lenToHash);
					sph_luffa512_close(&ctx_luffa, static_cast<void*>(&hash[i]));
					break;
				}
			}
			for (int i = HASH_FUNC_COUNT_1; i < HASH_FUNC_COUNT_1 + HASH_FUNC_COUNT_2; i++) {
            toHash = static_cast<const void*>(&hash[i]);;
				switch (permutation_2[i]) {
				case 8:
					sph_cubehash512_init(&ctx_cubehash);
					sph_cubehash512(&ctx_cubehash, toHash, lenToHash);
					sph_cubehash512_close(&ctx_cubehash, static_cast<void*>(&hash[i]));
					break;
				case 9:
					sph_shavite512_init(&ctx_shavite);
					sph_shavite512(&ctx_shavite, toHash, lenToHash);
					sph_shavite512_close(&ctx_shavite, static_cast<void*>(&hash[i]));
					break;
				case 10:
					sph_simd512_init(&ctx_simd);
					sph_simd512(&ctx_simd, toHash, lenToHash);
					sph_simd512_close(&ctx_simd, static_cast<void*>(&hash[i]));
					break;
				case 11:
					sph_echo512_init(&ctx_echo);
					sph_echo512(&ctx_echo, toHash, lenToHash);
					sph_echo512_close(&ctx_echo, static_cast<void*>(&hash[i]));
					break;
				case 12:
					sph_hamsi512_init(&ctx_hamsi);
					sph_hamsi512(&ctx_hamsi, toHash, lenToHash);
					sph_hamsi512_close(&ctx_hamsi, static_cast<void*>(&hash[i]));
					break;
				case 13:
					sph_fugue512_init(&ctx_fugue);
					sph_fugue512(&ctx_fugue, toHash, lenToHash);
					sph_fugue512_close(&ctx_fugue, static_cast<void*>(&hash[i]));
					break;
				case 14:
					sph_shabal512_init(&ctx_shabal);
					sph_shabal512(&ctx_shabal, toHash, lenToHash);
					sph_shabal512_close(&ctx_shabal, static_cast<void*>(&hash[i]));
					break;
				case 15:
					sph_whirlpool_init(&ctx_whirlpool);
					sph_whirlpool(&ctx_whirlpool, toHash, lenToHash);
					sph_whirlpool_close(&ctx_whirlpool, static_cast<void*>(&hash[i]));
					break;
				}
			}

			for (int i = HASH_FUNC_COUNT_2; i < HASH_FUNC_COUNT_1 + HASH_FUNC_COUNT_2 + HASH_FUNC_COUNT_3; i++) {
				switch (permutation_3[i]) {
                toHash = static_cast<const void*>(&hash[i]);;
				case 16:
					//sph_tiger_init(&ctx_tiger);
					//sph_tiger(&ctx_tiger, toHash, lenToHash);
					//sph_tiger_close(&ctx_tiger, static_cast<void*>(&hash[i]));

					//sph_sha512_init(&ctx_sha512);
					//sph_sha512(&ctx_sha512, static_cast<const void*>(&hash[i]), 64);
					//sph_sha512_close(&ctx_sha512, static_cast<void*>(&hash[i]));

					sph_sha512_init(&ctx_sha512);
					sph_sha512(&ctx_sha512, toHash, lenToHash);;
					sph_sha512_close(&ctx_sha512, static_cast<void*>(&hash[i]));
					break;
				case 17:
					sph_sha512_init(&ctx_sha512);
					sph_sha512(&ctx_sha512, toHash, lenToHash);;
					sph_sha512_close(&ctx_sha512, static_cast<void*>(&hash[i]));
					break;
				case 18:
					sph_sha512_init(&ctx_sha512);
					sph_sha512(&ctx_sha512, toHash, lenToHash);;
					sph_sha512_close(&ctx_sha512, static_cast<void*>(&hash[i]));
					break;
				case 19:
					sph_sha512_init(&ctx_sha512);
					sph_sha512(&ctx_sha512, toHash, lenToHash);;
					sph_sha512_close(&ctx_sha512, static_cast<void*>(&hash[i]));
					break;
				case 20:
					sph_sha512_init(&ctx_sha512);
					sph_sha512(&ctx_sha512, toHash, lenToHash);;
					sph_sha512_close(&ctx_sha512, static_cast<void*>(&hash[i]));
					break;
				case 21:
					sph_sha512_init(&ctx_sha512);
					sph_sha512(&ctx_sha512, toHash, lenToHash);;
					sph_sha512_close(&ctx_sha512, static_cast<void*>(&hash[i]));
					break;
				case 22:
					sph_sha512_init(&ctx_sha512);
					sph_sha512(&ctx_sha512, toHash, lenToHash);;
					sph_sha512_close(&ctx_sha512, static_cast<void*>(&hash[i]));
					break;
				}

                LYRA2(static_cast<void*>(&hash[23]), 32, static_cast<const void*>(&hash[23]), 32, static_cast<const void*>(&hash[23]), 32, 1, 4, 4);

            return hash[23].trim256();
			}

}

#endif // MEGACOIN_CRYPTO_MEGA_MEC_H