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
#endif
#include "eos_types.h"

#ifdef TARGET_NANOS

#include "eos_format.h"
#include "eos_api.h"
#include "string.h"
#include "eos_ux.h"

char opCaption[20];
char detailCaption[20];
char detailValue[89];

volatile format_function_t formatter;


void format_confirm_operation(tx_context_t *txCtx) {
}

void format_confirm_transaction(tx_context_t *txCtx) {
}

void format_confirm_hash_detail(tx_context_t *txCtx) {
    strcpy(detailCaption, "Hash");
    print_binary_summary(txCtx->hash, detailValue, 32);
    formatter = &format_confirm_hash;
}

void format_confirm_hash_warning(tx_context_t *txCtx) {
    strcpy(detailCaption, "WARNING");
    strcpy(detailValue, "No details available");
    formatter = &format_confirm_hash_detail;
}

void format_confirm_hash(tx_context_t *txCtx) {
    formatter = &format_confirm_hash_warning;
}

#endif
