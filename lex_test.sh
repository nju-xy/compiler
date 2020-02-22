#!/bin/sh
echo "lexical test"

flex -o ./Code/lex.yy.c ./Code/lexical.l 

gcc ./Code/main.c ./Code/lex.yy.c -lfl -o ./Code/scanner

echo "测试输出："

./Code/scanner ./Test/test.cmm