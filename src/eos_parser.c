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
static const uint64_t ACTION_NEWACCOUNT = 0x9ab864229a9e4000;

// Need to get constants for these
static const uint64_t ACTION_PREPARE = 0x9ab864229a9e4000;
static const uint64_t ACTION_ACTIVATE = 0x9ab864229a9e4000;

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
    offset += 8;

    //symbol
    memcpy(&asset->symbol, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    return offset;
}

uint16_t parse_transfer(uint8_t *buffer, transfer_action_t *action, uint32_t* length) {
    uint16_t offset = 0;

    // from
    memcpy(&action->from, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    // to
    memcpy(&action->to, buffer + offset, sizeof(uint64_t));
    offset += 8;

    //parse_asset
    uint8_t asset_length = parse_asset(buffer + offset, &action->quantity);
    offset += asset_length;

    //memo -- should we show?
    memcpy(&action->memo, buffer + offset, sizeof(uint64_t));
//    offset += *(length) - (16 + asset_length);

    offset = *(length);
    return offset;
}

uint16_t parse_prepare(uint8_t *buffer, transfer_action_t *action, uint32_t* length) {
    uint16_t offset = 0;
    memcpy(&action->memo, buffer + offset, sizeof(uint64_t));

    offset = *(length);
    return offset;
}

uint16_t parse_activate(uint8_t *buffer, transfer_action_t *action, uint32_t* length) {
    uint16_t offset = 0;
    memcpy(&action->memo, buffer + offset, sizeof(uint64_t));

    offset = *(length);
    return offset;
}

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

uint16_t parse_voteproducer(uint8_t *buffer, voteproducer_action_t *action, uint32_t* length) {
    uint16_t offset = 0;
    unsigned char bts;

    // voter
    memcpy(&action->voter, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    // proxy
    memcpy(&action->proxy, buffer + offset, sizeof(uint64_t));
    offset += 8;

    // length of producers
    uint32_t num_producers = parse_varint32(buffer + offset, &bts);
    action->num_producers = num_producers;
    offset += bts;    

    // set to first producer
    action->producers = buffer + offset;

    offset = *(length);
    return offset;
}

// typedef struct {
//     uint64_t account_name;
//     uint64_t permission;
// } permission_level_t;

// typedef struct {
//     uint32_t wait_sec;
//     uint16_t weight;
// } wait_weight_t;

// typedef struct {
//     permission_level_t permission;
//     uint16_t weight;
// } permission_level_weight_t;

// typedef struct {
//     uint8_t public_key[33];
//     uint16_t weight;
// } key_weight_t;

// typedef struct {
//     uint32_t threshold;
//     key_weight_t *key_weights;
//     permission_level_weight_t *accounts;
//     wait_weight_t *waits;
// } authority_t;

// typedef struct {
//     uint64_t creator;
//     uint64_t name;
//     authority_t owner;   
//     authority_t active;       
// } newaccount_action_t;

// uint16_t parse_key_weight(uint8_t *buffer, authority_t* authority, uint32_t num_key_weights) {

//     // only parse out public key for displaying

//     uint16_t offset = 0;  
//     memcpy(authority->public_key, buffer + offset, 33);
//     offset += 35;
//     return offset;
// }

uint16_t parse_authority(uint8_t *buffer, authority_t* authority) {
    uint16_t offset = 0;   
    unsigned char bts;    

    // amount
    memcpy(&authority->threshold, buffer + offset, sizeof(int32_t));    
    offset += 4;

    //length of key_weights
    uint32_t num_key_weights = parse_varint32(buffer + offset, &bts);
    //authority->num_key_weights = num_key_weights;
    offset += bts;  

    // We will only parse out the first key weight into public key for displaying
    //parse_key_weight(buffer + offset, authority, num_key_weights);
    /*uint32_t pos = */parse_varint32(buffer + offset, &bts);
    offset += bts;  

    memcpy(authority->public_key, buffer + offset, 33);

    // 35 is size of key_weight
    //offset += num_key_weights * 35;
    offset += 35;    

    //length of accounts
    uint32_t num_accounts = parse_varint32(buffer + offset, &bts);
    //authority->num_accounts = num_accounts;
    offset += bts;    

    // 18 is size of account
    offset += num_accounts * 18;

    //length of waits
    uint32_t num_waits = parse_varint32(buffer + offset, &bts);
    //authority->num_waits = num_waits;
    offset += bts;   

    // 6 is size of wait
    offset += num_waits * 6;

    return offset;
}

uint16_t parse_newaccount(uint8_t *buffer, newaccount_action_t *action, uint32_t* length) {
    uint16_t offset = 0;

    // creator
    memcpy(&action->creator, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    // name
    memcpy(&action->name, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    // owner
    offset += parse_authority(buffer + offset, &action->owner);

    // active
    offset += parse_authority(buffer + offset, &action->active);

    offset = *(length);
    return offset;
}

uint16_t parse_op(uint8_t *buffer, operation_details_t *opDetails) {
    uint16_t offset = 0;   
    unsigned char bts;

    // account
    memcpy(&opDetails->account, buffer + offset, sizeof(uint64_t));    
    offset += 8;

    // type
    memcpy(&opDetails->name, buffer + offset, sizeof(uint64_t));
    offset += 8;

    // authorizations length
    uint32_t num_authorization = parse_varint32(buffer + offset, &bts);
    offset += bts;

    // authorizations
    uint32_t i;
    for (i = 0; i < num_authorization; i++) {
        offset += parse_authorization(buffer + offset, i);
    }

    // data length
    uint32_t dataSize = parse_varint32(buffer + offset, &bts);
    offset += bts;

    #ifdef TEST
    printf("action: %" PRIx64 "\n", opDetails->name);
    #endif

    // parse action based on "name" field
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
    else if (opDetails->name == ACTION_NEWACCOUNT) {
        opDetails->type = 7;        
        offset += parse_newaccount(buffer + offset, &opDetails->op.newaccount, &dataSize);
        return offset;
    }
    else if (opDetails->name == ACTION_PREPARE) {
        opDetails->type = 8;        
        offset += parse_prepare(buffer + offset, &opDetails->op.newaccount, &dataSize);
        return offset;
    }
    else if (opDetails->name == ACTION_ACTIVATE) {
        opDetails->type = 9;        
        offset += parse_activate(buffer + offset, &opDetails->op.newaccount, &dataSize);
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
        // should be empty so 0
        parse_varint32(buffer + offset, &bts);
        offset += bts;

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
