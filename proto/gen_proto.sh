#! /usr/bin/bash

OUT_DIR=../protocpp
DATA_FILE=./op_cmd/op_cmd
TEMPLATE_FILE=./op_cmd/template_cmd.hpp
TARGET_FILE=op_cmd.hpp

FILES=$(find ./ -name "*.proto")

for FILE in $FILES
do
   echo protoc --cpp_out=$OUT_DIR $FILE
   ./protos/protoc --cpp_out=$OUT_DIR $FILE
done

python gen_cmd.py --out_dir=$OUT_DIR --data_file=$DATA_FILE \
    --template_file=$TEMPLATE_FILE --target_file=$TARGET_FILE
