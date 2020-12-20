#!/usr/bin/env python3
#coding: utf-8

"""
NlscanRootCA.cer is a self-signed digital certificate file, with no encoding error.
Its digital signature algorithm is sha1RSA with 2048-bit RSA keys.

It's final "blob" is the signature blob(the big number), starting at offset 0x37F,
with content 24 27 E1 ... 18 9E 14, which means a 2048-bit big number 
0x2427E1...189414 .

This program will calculate that big number with python code, and verify against
the .cer content to ensure that our calculation is correct.

Reference materials:
* https://cryptobook.nakov.com/digital-signatures/rsa-sign-verify-examples
* `PKCS#1, RSA Cryptography Specification` depicted by RFC2313, RFC2437, RFC3447, RFC8017.

"""

from Crypto.PublicKey import RSA
from Crypto.Signature.pkcs1_15 import PKCS115_SigScheme
from Crypto.Hash import SHA1
import binascii, codecs

def do_work():
	cerdump = open('NlscanRootCA.cer', 'rb').read()
	blob2hash = cerdump[4 : 4+4+867]
	sha1_to_sign = SHA1.new(blob2hash)
	
	print("SHA1 of the blob to sign: %s"%( 
		binascii.hexlify(sha1_to_sign.digest()).decode('ascii')
	)) # debug
	
	
	keypem = open('NlscanRootCA_key.pem', 'r').read()
	privatekey = RSA.import_key(keypem)
	
	signer = PKCS115_SigScheme(privatekey)
	signature = signer.sign(sha1_to_sign)
	
	assert( cerdump[0x37F:] == signature )
	
	print("Signature is: %s"%(
		binascii.hexlify(signature).decode('ascii')
	))
	print()
	print("Verify OK.")
	
if __name__=='__main__':
	do_work()

