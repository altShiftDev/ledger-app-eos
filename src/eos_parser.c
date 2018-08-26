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

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "eos_types.h"
#include "eos_api.h"

#ifndef TEST
#include "os.h"
#endif


static const uint64_t ACTION_TRANSFER = 0xcdcd3c2d57000000;
static const uint64_t ACTION_BUYRAM = 0x3ebd734800000000;
static const uint64_t ACTION_BUYRAMBYTES = 0x3ebd7348fecab000;
static const uint64_t ACTION_SELLRAM = 0xc2a31b9a40000000;
static const uint64_t ACTION_DELEGATEBW = 0x4aa2a61b2a3f0000;
static const uint64_t ACTION_UNDELEGATEBW = 0xd4d2a8a986ca8fc0;
static const uint64_t ACTION_VOTEPRODUCER = 0xdd32aade89d21570;


uint16_t parse_authorization(uint8_t *buffer, int i) {
    uint16_t offset = 0;   

    // actor
    //memcpy(&opDetails->authorization[i], buffer + offset, sizeof(uint64_t));    
    offset += 8;

    //permission
    //memcpy(&opDetails->authorization[i], buffer + offset, sizeof(uint64_t));    
    offset += 8;

    return offset;
}

uint16_t parse_asset(uint8_t *buffer, asset_t* asset) {
    uint16_t offset = 0;   

    // amount
    memcpy(&asset->amount, buffer + offset, sizeof(int64_t));    
    //asset->amount = read_uint64_block(buffer + offset);    
    offset += 8;

    //symbol
    memcpy(&asset->symbol, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    return offset;
}

//     "from": "account_name",
//     "to": "account_name",
//     "quantity": "asset",
//     "memo": "string"
uint16_t parse_transfer(uint8_t *buffer, transfer_action_t *action, uint32_t* length) {
    uint16_t offset = 0;
    //unsigned char bts;

    // from
    memcpy(&action->from, buffer + offset, sizeof(uint64_t));    
    //transfer->from = read_uint64_block(buffer + offset);
    offset += 8;

    // to
    memcpy(&action->to, buffer + offset, sizeof(uint64_t));
    //transfer->to = read_uint64_block(buffer + offset);
    offset += 8;

    offset += parse_asset(buffer + offset, &action->quantity);

    //memo -- should we show?

    offset = *(length);
    return offset;
}

//     "payer": "account_name",
//     "receiver": "account_name",
//     "quant": "asset"
uint16_t parse_buyram(uint8_t *buffer, buyram_action_t *action, uint32_t* length) {
    uint16_t offset = 0;
    // payer
    memcpy(&action->payer, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    // receiver
    memcpy(&action->receiver, buffer + offset, sizeof(uint64_t));
    offset += 8;

    offset += parse_asset(buffer + offset, &action->quant);

    offset = *(length);
    return offset;
}

//     "payer": "account_name",
//     "receiver": "account_name",
//     "bytes": "uint32"
uint16_t parse_buyrambytes(uint8_t *buffer, buyrambytes_action_t *action, uint32_t* length) {
    uint16_t offset = 0;
    // payer
    memcpy(&action->payer, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    // receiver
    memcpy(&action->receiver, buffer + offset, sizeof(uint64_t));
    offset += 8;

    // bytes
    memcpy(&action->bytes, buffer + offset, sizeof(uint32_t));
    offset += 4;

    offset = *(length);
    return offset;
}

//     "account": "account_name",
//     "bytes": "uint64"
uint16_t parse_sellram(uint8_t *buffer, sellram_action_t *action, uint32_t* length) {
    uint16_t offset = 0;
    // to
    memcpy(&action->account, buffer + offset, sizeof(uint64_t));
    offset += 8;

    // bytes
    memcpy(&action->bytes, buffer + offset, sizeof(uint64_t));
    offset += 8;

    offset = *(length);
    return offset;
}

//     "from": "account_name",
//     "receiver": "account_name",
//     "stake_net_quantity": "asset",
//     "stake_cpu_quantity": "asset",
//     "transfer": "bool"
uint16_t parse_delegatebw(uint8_t *buffer, delegatebw_action_t *action, uint32_t* length) {
    uint16_t offset = 0;
    // from
    memcpy(&action->from, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    // receiver
    memcpy(&action->receiver, buffer + offset, sizeof(uint64_t));
    offset += 8;

    // stake_net_quantity 
    offset += parse_asset(buffer + offset, &action->stake_net_quantity);

    // stake_cpu_quantity 
    offset += parse_asset(buffer + offset, &action->stake_cpu_quantity);    

    memcpy(&action->transfer, buffer + offset, sizeof(int));    
    offset += sizeof(int);

    offset = *(length);
    return offset;
}

//     "from": "account_name",
//     "receiver": "account_name",
//     "unstake_net_quantity": "asset",
//     "unstake_cpu_quantity": "asset"
uint16_t parse_undelegatebw(uint8_t *buffer, undelegatebw_action_t *action, uint32_t* length) {
    uint16_t offset = 0;
    // from
    memcpy(&action->from, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    // receiver
    memcpy(&action->receiver, buffer + offset, sizeof(uint64_t));
    offset += 8;

    // stake_net_quantity 
    offset += parse_asset(buffer + offset, &action->unstake_net_quantity);

    // stake_cpu_quantity 
    offset += parse_asset(buffer + offset, &action->unstake_cpu_quantity);    

    offset = *(length);
    return offset;
}

//     "voter": "account_name",
//     "proxy": "account_name",
//     "producers": "account_name[]"
uint16_t parse_voteproducer(uint8_t *buffer, voteproducer_action_t *action, uint32_t* length) {
    uint16_t offset = 0;
    // voter
    memcpy(&action->voter, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    // proxy
    memcpy(&action->proxy, buffer + offset, sizeof(uint64_t));
    offset += 8;

    // parse array

    offset = *(length);
    return offset;
}

uint16_t parse_op(uint8_t *buffer, operation_details_t *opDetails) {
    uint16_t offset = 0;   
    //unsigned char out[13];
    unsigned char bts;

    // account
    memcpy(&opDetails->account, buffer + offset, sizeof(uint64_t));    
    //opDetails->account = read_uint64_block(buffer + offset);
    offset += 8;

    // type
    memcpy(&opDetails->name, buffer + offset, sizeof(uint64_t));
    //opDetails->name = read_uint64_block(buffer + offset);
    offset += 8;

    // authorizations length
    parse_varint32(buffer + offset, &bts);
    offset += bts;

    // authorizations
    int i;
    for (i = 0; i < bts; i++) {
        offset += parse_authorization(buffer + offset, i);
    }

    // data length
    uint32_t dataSize = parse_varint32(buffer + offset, &bts);
    offset += bts;

    #ifdef TEST
    printf("action: %" PRIx64 "\n", opDetails->name);
    #endif

    // parse data based on "name" field
    if (opDetails->name == ACTION_TRANSFER) {
        opDetails->type = 0;
        offset += parse_transfer(buffer + offset, &opDetails->op.transfer, &dataSize);
        return offset;
    }
    else if (opDetails->name == ACTION_BUYRAM) {
        opDetails->type = 1;
        offset += parse_buyram(buffer + offset, &opDetails->op.buyram, &dataSize);
        return offset;
    }
    else if (opDetails->name == ACTION_BUYRAMBYTES) {
        opDetails->type = 2;        
        offset += parse_buyrambytes(buffer + offset, &opDetails->op.buyrambytes, &dataSize);
        return offset;
    }
    else if (opDetails->name == ACTION_SELLRAM) {
        opDetails->type = 3;        
        offset += parse_sellram(buffer + offset, &opDetails->op.sellram, &dataSize);
        return offset;
    }
    else if (opDetails->name == ACTION_DELEGATEBW) {
        opDetails->type = 4;        
        offset += parse_delegatebw(buffer + offset, &opDetails->op.delegatebw, &dataSize);
        return offset;
    }
    else if (opDetails->name == ACTION_UNDELEGATEBW) {
        opDetails->type = 5;        
        offset += parse_undelegatebw(buffer + offset, &opDetails->op.undelegatebw, &dataSize);
        return offset;
    }
    else if (opDetails->name == ACTION_VOTEPRODUCER) {
        opDetails->type = 6;        
        offset += parse_voteproducer(buffer + offset, &opDetails->op.voteproducer, &dataSize);
        return offset;
    }
    else {
        THROW(0x6900); // unknown action type
        return offset;        
    }
}

void parse_tx(uint8_t *buffer, tx_context_t *txCtx) {
    uint16_t offset = txCtx->offset;
    unsigned char bts;    

    if (offset == 0) {
        MEMCLEAR(txCtx->txDetails);
        offset += 32;  // chain
        offset += 4;   // expiration
        offset += 2;   // ref_block_num
        offset += 4;   // ref_block_prefix        

        // net_usage_words (varint32)
        parse_varint32(buffer + offset, &bts);
        offset += bts;

        // max_cpu_usage_ms (varint32)
        parse_varint32(buffer + offset, &bts);
        offset += bts;

        // delay_sec (varint32)
        parse_varint32(buffer + offset, &bts);
        offset += bts;

        // skip context free actions (not yet supported)
        // context free actions length (varint32)
        parse_varint32(buffer + offset, &bts);
        offset += bts;

        // if length > 0 read through context free actions to get new offset
        // no need to show these for now.

        // read actions length (varint32)    
        txCtx->opCount = parse_varint32(buffer + offset, &bts);
        if (txCtx->opCount > MAX_OPS) {
            THROW(0x6e10);
        }        
        offset += bts;          
        txCtx->opIdx = 0;
    }

    offset += parse_op(buffer + offset, &txCtx->opDetails);

    txCtx->opIdx += 1;
    if (txCtx->opCount == txCtx->opIdx) {
        offset = 0; // start from beginning next time
    }
    txCtx->offset = offset;    
}
