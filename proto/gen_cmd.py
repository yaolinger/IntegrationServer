#! /usr/bin/python
# -*- encoding=UTF-8 -*-

import os, re, sys, argparse
from mako.template import Template

#输入参数
parser = argparse.ArgumentParser(description='Auto build op_cmd')
parser.add_argument("--out_dir", type=str)
parser.add_argument("--data_file", type=str)
parser.add_argument("--template_file", type=str)
parser.add_argument("--target_file", type=str)

args = parser.parse_args()

#导出文件目录
outDir = args.out_dir
#数据文件
dataFile = args.data_file
#模板文件
templateFile = args.template_file
#目标文件
targetFile = args.target_file

#传入参数检测
if outDir is None:
    print "--out_dir is None"
    sys.exit()
if dataFile is None:
    print "--data_file is None"
    sys.exit()
if templateFile is None:
    print "--template_file is None"
    sys.exit()
if targetFile is None:
    print "--target_file is None"
    sys.exit()

#目录检测
if os.path.exists(outDir) == False:
    os.makedirs(outDir)

cmd_param = {}

#数据采集
def get_data(fileName):
    if os.path.exists(fileName) == False:
        print "File:", fileName, "not exist."
        sys.exit()
    readFile = open(fileName, 'rb')
    content = readFile.read().split('\n')
    readFile.close()
    index = 0
    for line in content:
        if len(line) == 0:
            continue
        if re.search("_BEGIN", line):
            value = line.split('=')
            index = int(value[1])
            continue
        elif re.search("_END", line) or re.search("//", line):
            continue
        else:
             index = index + 1
             cmd_param[index] = line

#构建文件
def create_file(inputFile, outputFile):
    if os.path.exists(inputFile) == False:
        print "TemplateFile :", inputFile, "not exist."
        sys.exit()
    dataTemplate = Template(filename = inputFile, input_encoding = 'UTF-8', output_encoding = 'UTF-8')
    content = dataTemplate.render(Msgs = cmd_param.items())
    tgFile = os.path.join(outDir, outputFile)
    writeFile = open(tgFile, 'wb')
    writeFile.write(content)
    writeFile.close()

get_data(dataFile)
create_file(templateFile, targetFile)

print "Build ", targetFile, "successful, out_dir:", outDir
