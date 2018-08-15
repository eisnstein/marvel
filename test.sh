#!/bin/bash
set -e

URL="gateway.marvel.com"
ENDPOINT="v1/public/comics"
TS="1234567788"
API_KEY="5e97dd7d1e320e04480f74c2fe33f4ab"
HASH="be526e5bbe05d03055322202c54e9bc0"

CURL="http://$URL/$ENDPOINT?ts=$TS&apikey=$API_KEY&hash=$HASH"

echo "running curl ..."

curl $CURL -vvv

echo
