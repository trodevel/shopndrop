# Makefile for libsws
# Copyright (C) 2018 Sergey Kolevatov

###################################################################

SWS_PATH := $(shell echo $$SWS_PATH)

ifeq (,$(SWS_PATH))
    $(error 'please define path to Simple-Web-Server $$SWS_PATH')
endif

###################################################################

SWS_INC = -I$(SWS_PATH)
