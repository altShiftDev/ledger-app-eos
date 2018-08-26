/*******************************************************************************
 *   Ledger EOS App
 *   (c) 2018 - 2019 Ledger
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
#include <stdio.h>

#include "eos_api.h"
#include "eos_format.h"
#include "test_utils.h"
#include <string.h>

int main(int argc, char *argv[]) {

    if(argc != 2 ) {
        printf("Expected argument missing\n");
        return 1;
    }

    char *filename = argv[1];
    uint8_t buffer[4096];
    tx_context_t txCtx;

    memset(&txCtx, 0, sizeof(txCtx));

    printf("==== %s ====\n", filename);
    int read = read_file(filename, buffer, 4096);
    //printHexBlocks(buffer, read/2);
    if (read) {
        //printHexBlocks(buffer, read/2);
        uint8_t count = 0;
        do {
            if (!formatter) {
                if (txCtx.opIdx > 0 && txCtx.opIdx == txCtx.opCount) {
                    txCtx.opIdx = 0;
                    formatter = &format_confirm_transaction_details;
                    count++;
                } else {
                    parse_tx(buffer, &txCtx);
                    if (txCtx.opIdx == 1) {
                        formatter = &format_confirm_transaction;
                        count++;
                        printf("\n");
                    } else {            
                        formatter = &format_confirm_operation;
                    }
                }
            }
            MEMCLEAR(opCaption);
            MEMCLEAR(detailCaption);
            MEMCLEAR(detailValue);
            formatter(&txCtx);
            if (opCaption[0] != '\0') {
                printf("Operation: %s\n", opCaption);
            }
            if (detailCaption[0] != '\0') {
                printf("%s - %s\n", detailCaption, detailValue);
            }
        } while (count <= 1);
    }
    printf("\n");
    return 0;

}
