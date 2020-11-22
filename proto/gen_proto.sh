#! /usr/bin/bash

OUT_DIR=../protocpp
DATA_FILE=./op_cmd/op_cmd
TEMPLATE_CMD_FILE=./op_cmd/template_cmd.hpp
TARGET_CMD_FILE=op_cmd.hpp
TEMPLATE_CMD_MANAGER_FILE=./op_cmd/template_cmd_manager.cc
TARGET_CMD_MANAGER_FILE=cmd_manager.cc
COPY_FILE=./op_cmd/cmd_manager.hpp
COPY_TARGET_FILE=cmd_manager.hpp
PROTOC=/usr/local/integration_server_third/bin/protoc

FILES=$(find ./ -name "*.proto")

for FILE in $FILES
do
   echo $PROTOC --cpp_out=$OUT_DIR $FILE
   $PROTOC --cpp_out=$OUT_DIR $FILE
done

#生成op_cmd.hpp
python gen_cmd.py --out_dir=$OUT_DIR --data_file=$DATA_FILE \
    --template_file=$TEMPLATE_CMD_FILE --target_file=$TARGET_CMD_FILE

#生成cmd_manager.cc
python gen_cmd.py --out_dir=$OUT_DIR --data_file=$DATA_FILE \
    --template_file=$TEMPLATE_CMD_MANAGER_FILE --target_file=$TARGET_CMD_MANAGER_FILE

#拷贝cmd_manager.hpp
cp $COPY_FILE $OUT_DIR/$COPY_TARGET_FILE
echo "cp $COPY_FILE $OUT_DIR/$COPY_TARGET_FILE"
