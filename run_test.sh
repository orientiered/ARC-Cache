#!/bin/bash

TEST_FILE=$1
EXECUTABLE=$2

NAME="${TEST_FILE%.*}"

TEST_NAME=$(basename $TEST_FILE)

LOG_FILE=${NAME}.res
ANS_FILE=${NAME}.ans
eval ${EXECUTABLE} < ${TEST_FILE} > ${LOG_FILE}

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

DIFF=$(diff ${LOG_FILE} ${ANS_FILE})

if [ $? -ne 0 ]; then
  printf "${RED}FAILED: ${TEST_FILE}${NC}, see ${LOG_FILE}\n"
  printf "${DIFF}\n"
  exit 1
else
  rm ${LOG_FILE}
  printf "${GREEN}PASS: ${TEST_FILE}${NC}\n"
fi

