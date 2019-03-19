#!/bin/sh

for f in *.crt
do
  openssl x509 -inform der -in $f -out $f.pem
done

# cat *.pem > ../certs.pem
