#!/bin/bash
mkdir -p obj
gcc test/parsertest.c src/eos_utils.c src/eos_parser.c src/eos_format_nanos.c src/eos_varint.c test/test_utils.c -o obj/parsertest -I src/ -I test/ -D TARGET_NANOS -D TEST
./obj/parsertest test/tx_transfer.hex

gcc test/parsertest.c src/eos_utils.c src/eos_parser.c src/eos_format_nanos.c src/eos_varint.c test/test_utils.c -o obj/parsertest -I src/ -I test/ -D TARGET_NANOS -D TEST
./obj/parsertest test/tx_buyram.hex

gcc test/parsertest.c src/eos_utils.c src/eos_parser.c src/eos_format_nanos.c src/eos_varint.c test/test_utils.c -o obj/parsertest -I src/ -I test/ -D TARGET_NANOS -D TEST
./obj/parsertest test/tx_buyrambytes.hex

gcc test/parsertest.c src/eos_utils.c src/eos_parser.c src/eos_format_nanos.c src/eos_varint.c test/test_utils.c -o obj/parsertest -I src/ -I test/ -D TARGET_NANOS -D TEST
./obj/parsertest test/tx_sellram.hex

gcc test/parsertest.c src/eos_utils.c src/eos_parser.c src/eos_format_nanos.c src/eos_varint.c test/test_utils.c -o obj/parsertest -I src/ -I test/ -D TARGET_NANOS -D TEST
./obj/parsertest test/tx_delegatebw.hex

gcc test/parsertest.c src/eos_utils.c src/eos_parser.c src/eos_format_nanos.c src/eos_varint.c test/test_utils.c -o obj/parsertest -I src/ -I test/ -D TARGET_NANOS -D TEST
./obj/parsertest test/tx_undelegatebw.hex

gcc test/parsertest.c src/eos_utils.c src/eos_parser.c src/eos_format_nanos.c src/eos_varint.c test/test_utils.c -o obj/parsertest -I src/ -I test/ -D TARGET_NANOS -D TEST
./obj/parsertest test/tx_voteproducer.hex

gcc test/parsertest.c src/eos_utils.c src/eos_parser.c src/eos_format_nanos.c src/eos_varint.c test/test_utils.c -o obj/parsertest -I src/ -I test/ -D TARGET_NANOS -D TEST
./obj/parsertest test/tx_voteproducer_array.hex

gcc test/parsertest.c src/eos_utils.c src/eos_parser.c src/eos_format_nanos.c src/eos_varint.c test/test_utils.c -o obj/parsertest -I src/ -I test/ -D TARGET_NANOS -D TEST
./obj/parsertest test/tx_newaccount.hex



# gcc test/utilstest.c src/eos_utils.c src/eos_varint.c test/test_utils.c -o obj/utilstest -I src/ -I test/ -D TEST -D TARGET_NANOS
# ./obj/utilstest
