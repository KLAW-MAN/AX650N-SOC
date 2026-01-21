1. This test case is used to generate two pub hash values, RSA2048 and RSA3072. According to the signature encryption algorithm used by the user, the generated hash is a fuse image used for subsequent verification of the signature;

2. If the user has generated the public and private keys by himself, he needs to replace the key_n_2048[256] or key_n_2048[256] array by running the command openssl rsa -modulus -in private.pem -text
Get key information;
#: openssl rsa -modulus -in private.pem -text
RSA Private-Key: (2048 bit, 2 primes)
modulus:
    00:90:e1:31:81:d2:22:72:c3:13:63:b4:04:54:a4:
    f5:45:95:e6:36:86:d7:27:77:c8:18:68:b9:09:59:
    a9:fa:4a:9a:eb:3b:8b:dc:2c:7c:cd:1d:6d:be:0e:
    5e:ae:ff:4f:9f:f0:40:90:e1:31:81:d2:22:72:c3:
    13:63:b4:04:54:a4:f5:45:95:e6:36:86:d7:27:77:
    c8:18:68:b9:09:59:a9:fa:4a:9a:eb:3b:8b:dc:2c:
    7c:cd:1d:6d:be:0e:5e:ae:ff:4f:9f:f0:40:90:e1:
    31:81:d2:22:72:c3:13:63:b4:04:54:a4:f5:45:95:
    e6:36:86:d7:27:77:c8:1b:d9:38:e8:98:47:f7:a7:
    57:06:b6:66:15:c5:75:24:d4:84:33:e3:93:42:f2:
    a2:52:01:b1:61:10:c0:70:1f:cf:7f:2e:de:8e:3d:
    ed:9d:4c:fc:ac:5c:0b:bb:6b:1a:ca:7a:29:d9:89:
    38:e8:98:47:f7:a7:57:06:b6:66:15:c5:75:24:d4:
    84:33:e3:93:42:f2:a2:52:01:b1:61:10:c0:70:1f:
    cf:7f:2e:de:8e:3d:ed:9d:4c:fc:ac:5c:0b:bb:6b:
    1a:ca:7a:29:d9:89:38:e8:98:47:f7:a7:57:06:b6:
    66:15:c5:75:24:d4:84:33:e3:93:42:f2:a2:52:06:
    be:2b
publicExponent: 65537 (0x10001)

3. Operation method:
#ax_pubkey_hash_write

4. Obtain running results:
digest_3072[8] = {
	0x6C1DDA30, 0x53CD22C1, 0x97BEB790, 0xAAEDA709,
	0xEF49ED74, 0xEC82D080, 0x90175EFB, 0x637DD4E5
};

digest_2048[8] = {
	0x78DDFE9E, 0x698A9288, 0xCCAD1548, 0x5CA04574,
	0x9F1AA945, 0xF57EB822, 0xB807DB67, 0x29140398,
}

