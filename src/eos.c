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

#include "eos_api.h"
#include "eos_types.h"
#include "eos_vars.h"
#include "eos_helpers.h"
#include "eos_ux.h"

uint8_t read_bip32(uint8_t *dataBuffer, uint32_t *bip32) {
    uint8_t bip32Len = dataBuffer[0];
    dataBuffer += 1;
    if (bip32Len < 0x01 || bip32Len > MAX_BIP32_LEN) {
        THROW(0x6a80);
    }
    uint8_t i;
    for (i = 0; i < bip32Len; i++) {
        bip32[i] = (dataBuffer[0] << 24) | (dataBuffer[1] << 16) | (dataBuffer[2] << 8) | (dataBuffer[3]);
        dataBuffer += 4;
    }
    return bip32Len;
}

void handle_get_app_configuration(volatile unsigned int *tx) {
    G_io_apdu_buffer[0] = ctx.hashSigning;
    G_io_apdu_buffer[1] = LEDGER_MAJOR_VERSION;
    G_io_apdu_buffer[2] = LEDGER_MINOR_VERSION;
    G_io_apdu_buffer[3] = LEDGER_PATCH_VERSION;
    *tx = 4;
    THROW(0x9000);
}

void handle_get_public_key(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx) {

    if ((p1 != P1_SIGNATURE) && (p1 != P1_NO_SIGNATURE)) {
        THROW(0x6B00);
    }
    if ((p2 != P2_CONFIRM) && (p2 != P2_NO_CONFIRM)) {
        THROW(0x6B00);
    }

    uint32_t bip32[MAX_BIP32_LEN];
    uint8_t bip32Len = read_bip32(dataBuffer, bip32);
    uint8_t privateKeyData[33];
    uint8_t compressedKey[33];    
    cx_ecfp_private_key_t privateKey;
    cx_ecfp_public_key_t publicKey;        
    
    os_memset(privateKeyData, 0, 33);    
    os_memset(compressedKey, 0, 33);    
    os_memset(ctx.req.pk.address, 0, 50);    

    os_perso_derive_node_bip32(CX_CURVE_256K1, bip32, bip32Len, privateKeyData, NULL);
    cx_ecfp_init_private_key(CX_CURVE_256K1, privateKeyData, 32, &privateKey);
    cx_ecfp_generate_pair(CX_CURVE_256K1, &publicKey, &privateKey, 1);    

    eos_compress_public_key(&publicKey, compressedKey, 33);
    eos_hash_and_encode_base58(compressedKey, 33, ctx.req.pk.address, 50);

    os_memset(compressedKey, 0, 33);
    os_memset(privateKeyData, 0, 33);    
    MEMCLEAR(privateKey);

    if (p2 & P2_CONFIRM) {
        ctx.reqType = CONFIRM_ADDRESS;
        ui_show_address_init();
        *flags |= IO_ASYNCH_REPLY;
    } else {
        *tx = set_result_get_public_key();
        THROW(0x9000);
    }
}

void handle_sign_tx(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags) {

    if ((p1 != P1_FIRST) && (p1 != P1_MORE)) {
        THROW(0x6B00);
    }
    if ((p2 != P2_LAST) && (p2 != P2_MORE)) {
        THROW(0x6B00);
    }

    if (p1 == P1_FIRST) {
        MEMCLEAR(ctx.req.tx);
        ctx.reqType = CONFIRM_TRANSACTION;
        // read the bip32 path
        ctx.req.tx.bip32Len = read_bip32(dataBuffer, ctx.req.tx.bip32);
        dataBuffer += 1 + ctx.req.tx.bip32Len * 4;
        dataLength -= 1 + ctx.req.tx.bip32Len * 4;

        // read raw tx data
        ctx.req.tx.rawLength = dataLength;
        os_memmove(ctx.req.tx.raw, dataBuffer, dataLength);
    } else {
        // read more raw tx data
        uint32_t offset = ctx.req.tx.rawLength;
        ctx.req.tx.rawLength += dataLength;
        if (ctx.req.tx.rawLength > MAX_RAW_TX) {
            THROW(0x6700);
        }
        os_memmove(ctx.req.tx.raw+offset, dataBuffer, dataLength);
    }

    if (p2 == P2_MORE) {
        THROW(0x9000);
    }

    // hash transaction
    cx_hash_sha256(ctx.req.tx.raw, ctx.req.tx.rawLength, ctx.req.tx.hash, 32);

    ui_approve_tx_init();

    *flags |= IO_ASYNCH_REPLY;
}

void handle_sign_tx_hash(uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags) {

    if (!ctx.hashSigning) {
        THROW(0x6c66);
    }

    MEMCLEAR(ctx.req.tx);
    ctx.reqType = CONFIRM_TRANSACTION;

    ctx.req.tx.bip32Len = read_bip32(dataBuffer, ctx.req.tx.bip32);
    dataBuffer += 1 + ctx.req.tx.bip32Len * 4;
    dataLength -= 1 + ctx.req.tx.bip32Len * 4;

    if (dataLength != 32) {
        THROW(0x6a80);
    }
    os_memmove(ctx.req.tx.hash, dataBuffer, dataLength);

    ui_approve_tx_hash_init();

    *flags |= IO_ASYNCH_REPLY;
}

void handle_keep_alive(volatile unsigned int *flags) {
    *flags |= IO_ASYNCH_REPLY;
}

unsigned int set_result_sign_tx(void) {
    uint8_t privateKeyData[32];
    uint8_t signature[164];
    uint8_t signatureLength;
    uint8_t eos_signature[164];
    cx_ecfp_private_key_t privateKey;
    uint32_t tx = 0;
    unsigned int info = 0;
    
    os_perso_derive_node_bip32(CX_CURVE_256K1, ctx.req.tx.bip32, ctx.req.tx.bip32Len, privateKeyData, NULL);
    cx_ecfp_init_private_key(CX_CURVE_256K1, privateKeyData, 32, &privateKey);

    MEMCLEAR(privateKeyData);
    MEMCLEAR(signature);
    MEMCLEAR(eos_signature);

    do {
        signatureLength = cx_ecdsa_sign(&privateKey, CX_RND_TRNG | CX_LAST, CX_SHA256, ctx.req.tx.hash, 32, signature, 164, &info);    
    }
    while (!eos_is_canonical(signature, signatureLength));

    MEMCLEAR(privateKey);

    tx = eos_signature_to_encoded_base58(signature, signatureLength, eos_signature, 164, &info);
    os_memmove(G_io_apdu_buffer, eos_signature, tx);
    return tx;
}

uint32_t set_result_get_public_key() {
    uint32_t tx = 0;
    
    uint32_t addressLength = strlen((const char *)ctx.req.pk.address);
    tx += addressLength;
    os_memmove(G_io_apdu_buffer, ctx.req.pk.address, addressLength);

    return tx;
}


