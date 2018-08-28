/*******************************************************************************
 *   Ledger Eos App
 *   (c) 2018-2019 Ledger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#include "eos_helpers.h"
#include "eos_base58.h"
#include <stdbool.h>


int eos_is_canonical(const unsigned char WIDE *sig PLENGTH(sig_len), unsigned int sig_len) {
    uint8_t rLength, sLength, rOffset, sOffset;
    unsigned char r[33];
    unsigned char s[33];

    rLength = sig[3];
    sLength = sig[4 + rLength + 1];
    rOffset = (rLength == 33 ? 1 : 0);
    sOffset = (sLength == 33 ? 1 : 0);

    os_memmove(r, sig + 4 + rOffset, rLength);
    os_memmove(s, sig + 4 + rLength + 2 + sOffset, sLength);

    int canonical = !(r[0] & 0x80)
        && !(r[0] == 0 && !(r[1] & 0x80))
        && !(s[0] & 0x80)
        && !(s[0] == 0 && !(s[1] & 0x80));

    return canonical;
 }

unsigned short eos_hash_and_encode_base58(unsigned char WIDE *in PLENGTH(inlen),
                                                unsigned short inlen,
                                                unsigned char *out PLENGTH(outlen) ,
                                                unsigned short outlen) {
    unsigned char tmpBuffer[inlen + 4];
    unsigned char checksumBuffer[32];

    os_memmove(tmpBuffer, in, inlen);

    cx_ripemd160_t ripemd;
    cx_ripemd160_init(&ripemd);
    cx_hash(&ripemd.header, CX_LAST, in, inlen, checksumBuffer, 32);
    os_memmove(tmpBuffer + inlen, checksumBuffer, 4);
    
    unsigned int olen;
    b58enc(out, &olen, tmpBuffer, inlen + 4);
    return olen;
    //return eos_encode_base58(tmpBuffer, inlen + 4, out, outlen);
}

unsigned short eos_signature_to_encoded_base58(unsigned char WIDE *signature PLENGTH(signaturelen),
                                               unsigned short signaturelen,
                                               unsigned char *out PLENGTH(outlen),
                                               unsigned short outlen,
                                               unsigned int *info PLENGTH(sizeof(unsigned int))) {
    unsigned char tempBuffer[65 + 2];
    //unsigned char sigBuffer[164];
    //unsigned char keyType[2] = "K1";
    //unsigned char sigType[7] = "SIG_K1_";   
    uint8_t rLength;
    uint8_t sLength;
    uint8_t rOffset;
    uint8_t sOffset;
    uint32_t sigLen = 0; 

    os_memset(tempBuffer, 0, sizeof(tempBuffer)); 

    tempBuffer[0] = 27;
    if ((*info) & CX_ECCINFO_PARITY_ODD) {
       tempBuffer[0]++;
    }

    rLength = signature[3];
    sLength = signature[4 + rLength + 1];
    rOffset = (rLength == 33 ? 1 : 0);
    sOffset = (sLength == 33 ? 1 : 0);

    os_memmove(tempBuffer + 1, signature + 4 + rOffset, 32);
    os_memmove(tempBuffer + 33, signature + 4 + rLength + 2 + sOffset, 32);

    /* Should do below, but doesn't work for some reason, will fix */
    // os_memmove(tempBuffer + 65, keyType, 2);
    // os_memmove(out, sigType, 7);
    // sigLen = 7;
    // /*sigLen += */eos_hash_and_encode_base58(tempBuffer, 67, out + 7, sizeof(out) - 7);
    // sigLen += strlen((unsigned char*)out);
    
    // this will end up returning compact signature
    sigLen = 65;
    os_memmove(out, tempBuffer, 65);

    return sigLen;
}

unsigned short eos_compress_public_key(cx_ecfp_public_key_t *publicKey,
                                       uint8_t *out, uint32_t outlen) {
    if (outlen < 33) {
        THROW(EXCEPTION_OVERFLOW);
    }
    out[0] = ((publicKey->W[64] & 1) ? 0x03 : 0x02);
    os_memmove(out + 1, publicKey->W + 1, 32);
    return 33;
}
