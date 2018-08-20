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

#include "os.h"
#include "cx.h"

int eos_is_canonical(const unsigned char WIDE *sig PLENGTH(sig_len), 
                           unsigned int sig_len);

unsigned short eos_hash_and_encode_base58(unsigned char WIDE *in PLENGTH(inlen), 
                                                unsigned short inlen, 
                                                unsigned char *out PLENGTH(outlen),
                                                unsigned short outlen);

unsigned short eos_signature_to_encoded_base58(unsigned char WIDE *in  PLENGTH(inlen), 
                                               unsigned short inlen, 
                                               unsigned char *out PLENGTH(outlen),
                                               unsigned short outlen,
                                               unsigned int *info PLENGTH(sizeof(unsigned int)));

unsigned short eos_compress_public_key(cx_ecfp_public_key_t *publicKey,
                                       uint8_t *out, uint32_t outlen);


