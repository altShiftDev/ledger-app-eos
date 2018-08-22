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
#ifndef _EOS_TYPES_H_
#define _EOS_TYPES_H_

#include "os.h"
#include "cx.h"
#include <stdint.h>
#include <stdbool.h>

// ------------------------------------------------------------------------- //
//                       REQUEST PARSING CONSTANTS                           //
// ------------------------------------------------------------------------- //

#define CLA 0xE0
#define INS_GET_PUBLIC_KEY 0x02
#define INS_SIGN_TX 0x04
#define INS_GET_APP_CONFIGURATION 0x06
#define INS_SIGN_TX_HASH 0x08
#define INS_KEEP_ALIVE 0x10
#define P1_NO_SIGNATURE 0x00
#define P1_SIGNATURE 0x01
#define P2_NO_CONFIRM 0x00
#define P2_CONFIRM 0x01
#define P1_FIRST 0x00
#define P1_MORE 0x80
#define P2_LAST 0x00
#define P2_MORE 0x80

#define OFFSET_CLA 0
#define OFFSET_INS 1
#define OFFSET_P1 2
#define OFFSET_P2 3
#define OFFSET_LC 4
#define OFFSET_CDATA 5

/* Max transaction size is 1.5Kb */
#define MAX_RAW_TX 1540
//#define MAX_RAW_TX 2048
/* For sure not more than 35 operations will fit in that */
#define MAX_OPS 35
/* Although SEP-0005 only allows 3 bip32 path elements we support more */
#define MAX_BIP32_LEN 10

/* max amount is max int64 scaled down: "922337203685.4775807" */
#define AMOUNT_MAX_SIZE 21


// ------------------------------------------------------------------------- //
//                       TRANSACTION PARSING CONSTANTS                       //
// ------------------------------------------------------------------------- //


// ------------------------------------------------------------------------- //
//                             DISPLAY CONSTANTS                             //
// ------------------------------------------------------------------------- //

/*
 * Longest string will be "Operation ii of nn"
 */
#define OPERATION_CAPTION_MAX_SIZE 20

/*
 * Captions don't scroll so there is no use in having more capacity than can fit on screen at once.
 */
#define DETAIL_CAPTION_MAX_SIZE 20

/*
 * DETAIL_VALUE_MAX_SIZE value of 89 is due to the maximum length of managed data value which can be 64 bytes long.
 * Managed data values are displayed as base64 encoded strings, which are 4*((len+2)/3) characters long.
 * (An additional slot is required for the end-of-string character of course)
 */
#define DETAIL_VALUE_MAX_SIZE 89

static const char* NETWORK_NAMES[3] = { "Public", "Test", "Unknown" };

// ------------------------------------------------------------------------- //
//                              UTILITIES                                    //
// ------------------------------------------------------------------------- //

#ifdef TEST
#include <stdio.h>
#define THROW(code) { printf("error: %d", code); }
#define PRINTF(msg, arg) printf(msg, arg)
#define PIC(code) code
//#define TARGET_NANOS 1
#define TARGET_BLUE 1
#define MEMCLEAR(dest) { memset(&dest, 0, sizeof(dest)); }
#else
#define MEMCLEAR(dest) { os_memset(&dest, 0, sizeof(dest)); }
#include "bolos_target.h"
#endif // TEST

// ------------------------------------------------------------------------- //
//                           DISPLAY CONSTANTS                               //
// ------------------------------------------------------------------------- //

#ifdef TARGET_BLUE
#define COLOR_BG_1 0xF9F9F9
#define COLOR_APP 0x07a2cc
#define COLOR_APP_LIGHT 0xd4eef7
#define BAGL_FONT_OPEN_SANS_LIGHT_16_22PX_AVG_WIDTH 10
#define BAGL_FONT_OPEN_SANS_REGULAR_10_13PX_AVG_WIDTH 8
#define MAX_CHAR_PER_LINE 28
#endif

// ------------------------------------------------------------------------- //
//                           TYPE DEFINITIONS                                //
// ------------------------------------------------------------------------- //
typedef struct {
    uint64_t actor;
    uint64_t permission;
} authorization_t;

typedef struct {
    uint64_t from;
    uint64_t to;
    uint64_t quantity;
    uint64_t memo;    
} transfer_action_t;

typedef struct {
    uint64_t from;
} buyram_action_t;

typedef struct {
    uint64_t from;
} sellram_action_t;

typedef struct {
    uint64_t from;
} stakeup_action_t;

typedef struct {
    uint64_t from;
} stakedown_action_t;

typedef struct {
    uint64_t from;    
} vote_action_t;

typedef struct {
    uint64_t account;        
    uint64_t type;    
    //authorization_t authorization[5];    
    union {
        transfer_action_t transfer;
        buyram_action_t buyram;
        sellram_action_t sellram;
        stakeup_action_t stakeup;
        stakedown_action_t stakedown;
        vote_action_t vote;
    } op;
} operation_details_t;

typedef struct {
    uint32_t expiration;
    uint16_t ref_block_num;
    uint32_t ref_block_prefix;
    uint8_t net_usage_words;
    uint8_t max_cpu_usage_ms;
    uint8_t delay_sec;
    int64_t sequenceNumber;
} tx_details_t;

typedef struct {
    cx_ecfp_public_key_t publicKey;
    unsigned char address[50];
    uint8_t chainCode[32];
    bool getChaincode;    
} pk_context_t;

typedef struct {
    uint8_t bip32Len;
    uint32_t bip32[MAX_BIP32_LEN];
    uint8_t raw[MAX_RAW_TX];
    uint32_t rawLength;
    uint8_t hash[32];
    uint16_t offset;
    operation_details_t opDetails;
    tx_details_t txDetails;
    uint8_t opCount;
    uint8_t opIdx;
} tx_context_t;

enum request_type_t {
    CONFIRM_ADDRESS,
    CONFIRM_TRANSACTION
};

typedef struct {
    union {
        pk_context_t pk;
        tx_context_t tx;
    } req;
    enum request_type_t reqType;
    uint16_t u2fTimer;
    uint8_t hashSigning;
} eos_context_t;

typedef struct {
    uint8_t initialized;
} eos_nv_state_t;

#endif