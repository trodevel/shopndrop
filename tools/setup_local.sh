#!/bin/bash

# $Revision: 13938 $ $Date:: 2020-10-03 #$ $Author: serge $

RUNTIME_DIR="runtime"

[[ ! -d ../$RUNTIME_DIR ]] && mkdir ../$RUNTIME_DIR

# create directories

DIRS="logs status cred resources"

for s in $DIRS
do
    [[ ! -d ../$RUNTIME_DIR/$s ]] && mkdir ../$RUNTIME_DIR/$s
done

# link files

[[ ! -f ../$RUNTIME_DIR/resources/date_time_zonespec.csv ]] && ln -sf ../../../persek_res/date_time_zonespec.csv ../$RUNTIME_DIR/resources
[[ ! -f ../$RUNTIME_DIR/resources/registration_template.txt ]] && ln -sf ../../registration_template.txt ../$RUNTIME_DIR/resources

FILES="shopndrop.ini example"

for s in $FILES
do
    [[ ! -f ../$RUNTIME_DIR/$s ]] && ln -sf ../$s ../$RUNTIME_DIR
done

# copy files

FILES="users.dat"

for s in $FILES
do
    [[ ! -f ../$RUNTIME_DIR/status/$s ]] && cp $s ../$RUNTIME_DIR/status
done

# credentials

FILES="server.crt server.key user_reg_email_credentials.ini"

for s in $FILES
do
    [[ ! -f ../$RUNTIME_DIR/cred/$s ]] && cp $s ../$RUNTIME_DIR/cred
done
