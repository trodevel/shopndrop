#!/bin/bash

# $Revision: 13757 $ $Date:: 2020-09-08 #$ $Author: serge $

RUNTIME_DIR="runtime"

[[ ! -d ../$RUNTIME_DIR ]] && mkdir ../$RUNTIME_DIR

# create directories

DIRS="logs status cred"

for s in $DIRS
do
    [[ ! -d ../$RUNTIME_DIR/$s ]] && mkdir ../$RUNTIME_DIR/$s
done

# link files

[[ ! -d ../$RUNTIME_DIR/resources ]] && ln -sf ../../persek_res ../$RUNTIME_DIR/resources

FILES="shopndrop.ini example"

for s in $FILES
do
    [[ ! -f ../$RUNTIME_DIR/$s ]] && ln -sf ../$s ../$RUNTIME_DIR
done

# copy files

FILES="users.dat server.crt server.key"

for s in $FILES
do
    [[ ! -f ../$RUNTIME_DIR/cred/$s ]] && cp $s ../$RUNTIME_DIR/cred
done
