@echo off
pushd bin
call cl /MT /Zi /EHsc ../src/teste.cpp /I../include
popd