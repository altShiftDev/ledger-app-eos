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
#ifndef TEST
#include "bolos_target.h"
#include "os.h"
#include "eos_helpers.h"
#endif
#include "eos_types.h"

#ifdef TARGET_NANOS

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#include "eos_format.h"
#include "eos_api.h"
#include "string.h"
#include "eos_ux.h"

char opCaption[20];
char detailCaption[20];
char detailValue[89];

uint32_t current_producer_index = 0;

volatile format_function_t formatter;

uint8_t decimals(uint64_t value) { 
    return value & 0xFF; 
}

uint64_t precision(uint64_t value) {
    uint64_t p10 = 1;
    uint64_t p = decimals(value);
    while( p > 0  ) {
        p10 *= 10; --p;
    }
    return p10;
}

void name(uint64_t value, char* buffer) {
    uint64_t v = value;
    v >>= 8;
    int index = 0;
    while (v > 0) {
        char c = v & 0xFF;
        buffer[index++] = c;
        v >>= 8;
    }
    buffer[index] = '\0';
}

void format_asset(asset_t* asset, char* out, uint8_t outlen) {
    int64_t abs_amount = llabs(asset->amount);        
    uint64_t sym = asset->symbol;
    uint64_t prec = precision(sym);    

    int64_t amt = abs_amount / prec;
    print_int(amt, out);    
    if (decimals(sym)) {
        int64_t frac = abs_amount % prec;
        char fracStr[10];     
        char* ptr;
        print_int(prec + frac, fracStr);    
        ptr = fracStr + 1;
        memmove(ptr + 1, ptr, sizeof(ptr) + 1);    
        ptr[0] = '.';            
        strcpy(out + strlen(out), ptr); 
    } 

    char sym_name[17];
    name(sym, sym_name);    

    memmove(sym_name + 1, sym_name, strlen(sym_name) + 1);
    sym_name[0] = ' ';
    strcpy(out + strlen(out), sym_name);
}

/* 
 * formatting for transfer action
 */
void format_transfer_quantity(tx_context_t *txCtx) {
    strcpy(detailCaption, " Quantity "); 
    char tmp[64];
    format_asset(&txCtx->opDetails.op.transfer.quantity, tmp, 64);
    print_summary(tmp,  detailValue, 6, 6);    
    formatter = NULL; //&format_confirm_transaction;
}

void format_transfer_to(tx_context_t *txCtx) {
    strcpy(detailCaption, "   To   ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.transfer.to, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_transfer_quantity;
}

void format_transfer_from(tx_context_t *txCtx) {
    strcpy(detailCaption, " Transfer ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.transfer.from, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_transfer_to;
}

void format_transfer(tx_context_t *txCtx) {
    formatter = &format_transfer_from;
}

/* 
 * formatting for buy ram action
 */
void format_buyram_quant(tx_context_t *txCtx) {
    strcpy(detailCaption, " Quantity ");    
    char tmp[64];
    format_asset(&txCtx->opDetails.op.buyram.quant, tmp, 64);
    print_summary(tmp,  detailValue, 6, 6);    
    formatter = NULL;
}

void format_buyram_receiver(tx_context_t *txCtx) {
    strcpy(detailCaption, " Receiver ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.buyram.receiver, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_buyram_quant;
}

void format_buyram_payer(tx_context_t *txCtx) {
    strcpy(detailCaption, " Buy RAM ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.buyram.payer, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_buyram_receiver;
}

void format_buyram(tx_context_t *txCtx) {
    formatter = format_buyram_payer;
}

/* 
 * formatting for buy ram bytes action
 */
void format_buyrambytes_bytes(tx_context_t *txCtx) {
    strcpy(detailCaption, "  Bytes  ");    
    char tmp[64];
    print_int(txCtx->opDetails.op.buyrambytes.bytes, tmp);
    print_summary(tmp,  detailValue, 6, 6);    
    formatter = NULL;
}

void format_buyrambytes_receiver(tx_context_t *txCtx) {
    strcpy(detailCaption, " Receiver ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.buyrambytes.receiver, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_buyrambytes_bytes;
}

void format_buyrambytes_payer(tx_context_t *txCtx) {
    strcpy(detailCaption, " Buy RAM ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.buyrambytes.payer, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_buyrambytes_receiver;
}

void format_buyrambytes(tx_context_t *txCtx) {
    formatter = &format_buyrambytes_payer;
}

/* 
 * formatting for sell ram action
 */
void format_sellram_bytes(tx_context_t *txCtx) {
    strcpy(detailCaption, "  Bytes  ");    
    char tmp[64];
    print_int(txCtx->opDetails.op.sellram.bytes, tmp);
    print_summary(tmp,  detailValue, 6, 6);    
    formatter = NULL;
}

void format_sellram_account(tx_context_t *txCtx) {
    strcpy(detailCaption, " Sell RAM ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.sellram.account, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_sellram_bytes;
}

void format_sellram(tx_context_t *txCtx) {
    formatter = &format_sellram_account;
}

/* 
 * formatting for delegate bw action
 */
void format_delegatebw_transfer(tx_context_t *txCtx) {
    strcpy(detailCaption, " Transfer ");    
    char tmp[64];
    print_int(txCtx->opDetails.op.delegatebw.transfer, tmp);
    print_summary(tmp,  detailValue, 6, 6);    
    formatter = NULL;
}

void format_delegatebw_stake_cpu_quantity(tx_context_t *txCtx) {
    strcpy(detailCaption, "   CPU  ");    
    char tmp[32];
    format_asset(&txCtx->opDetails.op.delegatebw.stake_cpu_quantity, tmp, 32);
    print_summary(tmp,  detailValue, 6, 6);    
    formatter = &format_delegatebw_transfer;
}

void format_delegatebw_stake_net_quantity(tx_context_t *txCtx) {
    strcpy(detailCaption, "   NET   ");    
    char tmp[32];
    format_asset(&txCtx->opDetails.op.delegatebw.stake_net_quantity, tmp, 32);
    print_summary(tmp,  detailValue, 6, 6);    
    formatter = &format_delegatebw_stake_cpu_quantity;
}

void format_delegatebw_receiver(tx_context_t *txCtx) {
    strcpy(detailCaption, " Receiver ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.delegatebw.receiver, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_delegatebw_stake_net_quantity;
}

void format_delegatebw_from(tx_context_t *txCtx) {
    strcpy(detailCaption, "Increase Stake");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.delegatebw.from, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_delegatebw_receiver;
}

void format_delegatebw(tx_context_t *txCtx) {
    formatter = &format_delegatebw_from;
}

/* 
 * formatting for undelegate action
 */
void format_undelegatebw_stake_cpu_quantity(tx_context_t *txCtx) {
    strcpy(detailCaption, "   CPU   ");    
    char tmp[64];
    format_asset(&txCtx->opDetails.op.undelegatebw.unstake_cpu_quantity, tmp, 64);
    print_summary(tmp,  detailValue, 6, 6);    
    formatter = NULL;
}

void format_undelegatebw_stake_net_quantity(tx_context_t *txCtx) {
    strcpy(detailCaption, "   NET   ");    
    char tmp[64];
    format_asset(&txCtx->opDetails.op.undelegatebw.unstake_net_quantity, tmp, 64);
    print_summary(tmp,  detailValue, 6, 6);    
    formatter = &format_undelegatebw_stake_cpu_quantity;
}

void format_undelegatebw_receiver(tx_context_t *txCtx) {
    strcpy(detailCaption, " Receiver ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.undelegatebw.receiver, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_undelegatebw_stake_net_quantity;
}

void format_undelegatebw_from(tx_context_t *txCtx) {
    strcpy(detailCaption, "Decrease Stake");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.undelegatebw.from, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_undelegatebw_receiver;
}

void format_undelegatebw(tx_context_t *txCtx) {
    formatter = &format_undelegatebw_from;
}

/* 
 * formatting for vote producer action
 */
void format_voteproducer_producers(tx_context_t *txCtx) {
    strcpy(detailCaption, " Producers ");    
    char tmp[13];

    uint64_t producer;
    memcpy(&producer, txCtx->opDetails.op.voteproducer.producers + (current_producer_index * 8), 8);
    parse_name(producer, tmp);
    print_summary(tmp, detailValue, 6, 6);        
    current_producer_index += 1;

    if (current_producer_index < txCtx->opDetails.op.voteproducer.num_producers) {
        formatter = &format_voteproducer_producers;
    }
    else {
        formatter = NULL;
    }
}

void format_voteproducer_proxy(tx_context_t *txCtx) {
    strcpy(detailCaption, " Set Proxy ");    
    char tmp[13];    
    parse_name(txCtx->opDetails.op.voteproducer.proxy, tmp);       
    print_summary(tmp, detailValue, 6, 6);
    formatter = NULL;
}

void setproxy_or_producers(tx_context_t *txCtx) {
    char tmp[13];
    parse_name(txCtx->opDetails.op.voteproducer.proxy, tmp); 

    if (tmp[0] != '\0') {
        formatter = &format_voteproducer_proxy;
    }
    else {
        formatter = &format_voteproducer_producers;
    }
}

void format_voteproducer_voter(tx_context_t *txCtx) {
    strcpy(detailCaption, "Vote Producer");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.voteproducer.voter, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    setproxy_or_producers(txCtx);
}

void format_voteproducer(tx_context_t *txCtx) {
    current_producer_index = 0;
    formatter = &format_voteproducer_voter;
}


/* 
 * formatting for new account action
 */
void format_newaccount_active(tx_context_t *txCtx) {
    strcpy(detailCaption, "  Active  ");    
    char tmp[64];
#ifdef TEST
    print_public_key(txCtx->opDetails.op.newaccount.active.public_key, detailValue, 8, 8 );
#else
    eos_hash_and_encode_base58(txCtx->opDetails.op.newaccount.active.public_key, 33, (unsigned char*)tmp, 64);
    print_summary(tmp, detailValue, 6, 6);
#endif    
    formatter = NULL;
}

void format_newaccount_owner(tx_context_t *txCtx) {
    strcpy(detailCaption, "  Owner   ");    
    char tmp[64];
#ifdef TEST
    print_public_key(txCtx->opDetails.op.newaccount.owner.public_key, detailValue, 8, 8 );
#else
    eos_hash_and_encode_base58(txCtx->opDetails.op.newaccount.owner.public_key, 33, (unsigned char*)tmp, 64);
    print_summary(tmp, detailValue, 6, 6);
#endif    
    formatter = &format_newaccount_active;
}

void format_newaccount_name(tx_context_t *txCtx) {
    strcpy(detailCaption, "  Name   ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.newaccount.name, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_newaccount_owner;
}

void format_transfer_creator(tx_context_t *txCtx) {
    strcpy(detailCaption, " New Account ");    
    char tmp[13];
    /*uint32_t length = */parse_name(txCtx->opDetails.op.newaccount.creator, tmp);    
    print_summary(tmp, detailValue, 6, 6);
    formatter = &format_newaccount_name;
}

void format_newaccount(tx_context_t *txCtx) {
    formatter = &format_transfer_creator;
}


/* 
 * formatting for full transaction
 */
const format_function_t formatters[8] = {
    &format_transfer,
    &format_buyram,
    &format_buyrambytes,    
    &format_sellram,
    &format_delegatebw,
    &format_undelegatebw,
    &format_voteproducer,
    &format_newaccount
};

void format_confirm_operation(tx_context_t *txCtx) {
    if (txCtx->opCount > 1) {
        strcpy(opCaption, "Operation ");
        print_uint(txCtx->opIdx, opCaption+strlen(opCaption));
        strcpy(opCaption+strlen(opCaption), " of ");
        print_uint(txCtx->opCount, opCaption+strlen(opCaption));
        formatter = ((format_function_t)PIC(formatters[txCtx->opDetails.type]));
    } else {
        ((format_function_t)PIC(formatters[txCtx->opDetails.type]))(txCtx);
    }    
}

void format_confirm_transaction_details(tx_context_t *txCtx) {
    formatter = NULL;
}

void format_confirm_transaction(tx_context_t *txCtx) {
    formatter = &format_confirm_operation;    
}


/* 
 * formatting for hashed transaction
 */
void format_confirm_hash_detail(tx_context_t *txCtx) {
    strcpy(detailCaption, "Hash");
    print_binary_summary(txCtx->hash, detailValue, 32);
    formatter = &format_confirm_hash;
}

void format_confirm_hash_warning(tx_context_t *txCtx) {
    memset(detailValue, 0, 89);    
    strcpy(detailCaption, "WARNING");
    strcpy(detailValue, "No details available");
    formatter = &format_confirm_hash_detail;
}

void format_confirm_hash(tx_context_t *txCtx) {
    formatter = &format_confirm_hash_warning;
}

#endif
