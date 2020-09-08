#!/bin/bash

# $Revision: 12269 $ $Date:: 2019-10-25 #$ $Author: serge $

FL=users.dat

add_contact()
{
    local login=$1
    local gender=$2
    local name=$3
    local first_name=$4
    local company_name="Yoyodine"
    local phone=$5
    local timezone="Europe/Berlin"

    local lower_name=$( echo "$name" | awk '{print tolower($1)}' ) # '
    local lower_first_name=$( echo "$first_name" | awk '{print tolower($1)}' ) #'
    local lower_company_name=$( echo "$company_name" | awk '{print tolower($1)}' ) #'

    local email="$lower_first_name.$lower_name@$lower_company_name.com"

    ./user_management_tool add $FL 1 A $login "xxx" $gender "$name" "$first_name" "$company_name" "$email" "$phone" "$timezone"
}

[[ ! -f $FL ]] && echo "creating users.dat" && ./user_management_tool init $FL

add_contact test                M "User" "Test"         "+497676565765"

add_contact english_shopper     M "Doe" "John"          "+491234567890"
add_contact chinese_shopper     M "王" "张"             "+491234567890"
add_contact russian_shopper     M "Иванов" "Иван"       "+491234567890"
add_contact german_shopper      M "Müller" "Markus"     "+491234567890"

add_contact test_shopper_1      F "Müller"    "Laura"    "+4930987654321"
add_contact test_user_1         F "Schmidt"   "Emilia"   "+4940987654321"
add_contact test_shopper_2      F "Schneider" "Lina"     "+4989987654321"
add_contact test_user_2         F "Fischer"   "Lena"     "+49221987654321"
add_contact test_shopper_3      F "Weber"     "Julia"    "+49211987654321"
add_contact test_user_3         F "Meyer"     "Lea"      "+4969987654321"
add_contact test_shopper_4      F "Wagner"    "Anna"     "+49421987654321"
add_contact test_user_4         F "Becker"    "Lara"     "+49711987654321"
add_contact test_shopper_5      F "Schulz"    "Elena"    "+49351987654321"
add_contact test_user_5         F "Schwarz"   "Sarah"    "+49201987654321"

add_contact test_shopper_6      M "Hoffmann"  "Liam"     "+491511123456789"
add_contact test_user_6         M "Schäfer"   "Jonas"    "+491520123456789"
add_contact test_shopper_7      M "Koch"      "Julian"   "+491573123456789"
add_contact test_user_7         M "Bauer"     "Tim"      "+491590123456789"
add_contact test_shopper_8      M "Richter"   "Linus"    "+491512123456789"
add_contact test_user_8         M "Klein"     "Milan"    "+491522123456789"
add_contact test_shopper_9      M "Wolf"      "Elias"    "+491575123456789"
add_contact test_user_9         M "Schröder"  "Michael"  "+49176123456789"
add_contact test_shopper_10     M "Neumann"   "Luca"     "+49175123456789"
add_contact test_user_10        M "Zimmermann" "Levi"    "+49163123456789"
