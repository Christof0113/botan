botan
========================================

:Subtitle: Botan command line util
:Manual section: 1

Outline
------------

The ``botan`` program is a command line tool for using a broad variety
of functions of the Botan library in the shell.

All commands follow the syntax ``botan <command> <command-options>``.

If ``botan`` is run with an unknown command, or without any command, or with the
``--help`` option, all available commands with their options will be printed.

Hash Function
----------------
``hash --algo=SHA-256 --buf-size=4096 files``
  Compute the *algo* digest over the data in any number of *files*. If
  no files are listed on the command line, the input source defaults
  to standard input.

Password Hash
----------------
``gen_bcrypt --work-factor=12 password``
  Calculate the bcrypt password digest of *password*. *work-factor* is an
  integer between 4 and 18.  A higher *work-factor* value results in a
  more expensive hash calculation.

``check_bcrypt password hash``
  Checks if the bcrypt hash of the passed *password* equals the passed *hash* value.

HMAC
----------------
``hmac --hash=SHA-256 --buf-size=4096 key files``
  Compute the HMAC tag with the cryptographic hash function *hash*
  using the key in file *key* over the data in *files*. *files*
  defaults to STDIN.

Public Key Cryptography
-------------------------------------
``keygen --algo=RSA --params= --passphrase= --pbe= --pbe-millis=300 --der-out``
  Generate a PKCS #8 *algo* private key. If *der-out* is passed, the pair is BER
  encoded.  Otherwise, PEM encoding is used. To protect the PKCS #8 formatted
  key, it is recommended to encrypt it with a provided *passphrase*. *pbe* is
  the name of the desired encryption algorithm, which uses *pbe-millis*
  milliseconds to derive the encryption key from the passed
  *passphrase*. Algorithm specific parameters, as the desired bitlength of an
  RSA key, can be passed with *params*.

    - For RSA *params* specifies the bit length of the RSA modulus. It defaults to 3072.
    - For DH *params* specifies the DH parameters. It defaults to modp/ietf/2048.
    - For DSA *params* specifies the DSA parameters. It defaults to dsa/botan/2048.
    - For EC algorithms *params* specifies the elliptic curve. It defaults to secp256r1.

``pkcs8 --pass-in= --pub-out --der-out --pass-out= --pbe= --pbe-millis=300 key``
  Open a PKCS #8 formatted key at *key*. If *key* is encrypted, the passphrase
  must be passed as *pass-in*. It is possible to (re)encrypt the read key with
  the passphrase passed as *pass-out*. The parameters *pbe-millis* and *pbe*
  work similarly to ``keygen``.

``sign --passphrase= --hash=SHA-256 --emsa= key file``
  Sign the data in *file* using the PKCS #8 private key *key*. If *key* is
  encrypted, the used passphrase must be passed as *pass-in*. *emsa* specifies
  the signature scheme and *hash* the cryptographic hash function used in the
  scheme.

    - For RSA signatures EMSA4 (RSA-PSS) is the default scheme.
    - For ECDSA and DSA *emsa* defaults to EMSA1 (signing the hash directly)

``verify --hash=SHA-256 --emsa= pubkey file signature``
  Verify the authenticity of the data in *file* with the provided signature
  *signature* and the public key *pubkey*. Similarly to the signing process,
  *emsa* specifies the signature scheme and *hash* the cryptographic hash
  function used in the scheme.

``gen_dl_group --pbits=1024 --qbits=0 --type=subgroup``
  Generate ANSI X9.42 encoded Diffie-Hellman group parameters.

    - If *type=subgroup* is passed, the size of the prime subgroup q is sampled
      as a prime of *qbits* length and p is *pbits* long. If *qbits* is not
      passed, its length is estimated from *pbits* as described in RFC 3766.
    - If *type=strong* is passed, p is sampled as a safe prime with length
      *pbits* and the prime subgroup has size q with *pbits*-1 length.

``dl_group_info --pem name``
  Print raw Diffie-Hellman parameters (p,g) of the standarized DH group
  *name*. If *pem* is set, the X9.42 encoded group is printed.

``ec_group_info --pem name``
  Print raw elliptic curve domain parameters of the standarized curve *name*. If
  *pem* is set, the encoded domain is printed.

X.509
----------------------------------------------

``gen_pkcs10 key CN --country= --organization= --email= --key-pass= --hash=SHA-256  --emsa=``
  Generate a PKCS #10 certificate signing request (CSR) using the passed PKCS #8
  private key *key*. If the private key is encrypted, the decryption passphrase
  *key-pass* has to be passed.*emsa* specifies the padding scheme to be used
  when calculating the signature.

    - For RSA keys EMSA4 (RSA-PSS) is the default scheme.
    - For ECDSA, DSA, ECGDSA, ECKCDSA and GOST-34.10 keys *emsa* defaults to EMSA1.

``gen_self_signed key CN --country= --dns= --organization= --email= --key-pass= --ca --hash=SHA-256 --emsa=``
  Generate a self signed X.509 certificate using the PKCS #8 private key
  *key*. If the private key is encrypted, the decryption passphrase *key-pass*
  has to be passed. If *ca* is passed, the certificate is marked for certificate
  authority (CA) usage. *emsa* specifies the padding scheme to be used when
  calculating the signature.

    - For RSA keys EMSA4 (RSA-PSS) is the default scheme.
    - For ECDSA, DSA, ECGDSA, ECKCDSA and GOST-34.10 keys *emsa* defaults to EMSA1.

``sign_cert --ca-key-pass= --hash=SHA-256 --duration=365 --emsa= ca_cert ca_key pkcs10_req``
  Create a CA signed X.509 certificate from the information contained in the
  PKCS #10 CSR *pkcs10_req*. The CA certificate is passed as *ca_cert* and the
  respective PKCS #8 private key as *ca_key*. If the private key is encrypted,
  the decryption passphrase *ca-key-pass* has to be passed. The created
  certificate has a validity period of *duration* days. *emsa* specifies the
  padding scheme to be used when calculating the signature. *emsa* defaults to
  the padding scheme used in the CA certificate.

``ocsp_check subject issuer``
  Verify an X.509 certificate against the issuers OCSP responder. Pass the
  certificate to validate as *subject* and the CA certificate as *issuer*.

``cert_info --fingerprint --ber file``
  Parse X.509 PEM certificate and display data fields. If ``--fingerprint`` is
  used, the certificate's fingerprint is also printed.

``cert_verify subject *ca_certs``
  Verify if the provided X.509 certificate *subject* can be sucessfully
  validated. The list of trusted CA certificates is passed with *ca_certs*,
  which is a list of one or more certificates.

TLS Server/Client
-----------------------

``tls_ciphers --policy=default --version=tls1.2``
  Prints the list of ciphersuites that will be offered under a particular
  policy/version. The policy can be any of the the strings "default", "suiteb",
  "strict", or "all" to denote built-in policies, or it can name a file from
  which a policy description will be read.

``tls_client host --port=443 --print-certs --policy= --tls1.0 --tls1.1 --tls1.2 --session-db= --session-db-pass= --next-protocols= --type=tcp``
  Implements a testing TLS client, which connects to *host* via TCP or UDP on
  port *port*. The TLS version can be set with the flags *tls1.0*, *tls1.1* and
  *tls1.2* of which the lowest specified version is automatically chosen.  If
  none of the TLS version flags is set, the latest supported version is
  chosen. The client honors the TLS policy defined in the *policy* file and
  prints all certificates in the chain, if *print-certs* is passed.
  *next-protocols* is a comma seperated list and specifies the protocols to
  advertise with Application-Layer Protocol Negotiation (ALPN).

``tls_server cert key --port=443 --type=tcp --policy=``
  Implements a testing TLS server, which allows TLS clients to connect. Binds to
  either TCP or UDP on port *port*. The server uses the certificate *cert* and
  the respective PKCS #8 private key *key*. The server honors the TLS policy
  defined in the *policy* file.

``tls_http_server cert key --port=443 --policy= --session-db --session-db-pass=``
  Only available if asio support was enabled. Provides a simple HTTP server
  which replies to all requests with an informational text output. The server
  honors the TLS policy defined in the *policy* file.

``tls_proxy listen_port target_host target_port server_cert server_key``
  Only available if asio support was enabled. Listens on a port and
  forwards all connects to a target server specified at
  ``target_host`` and ``target_port``.

Number Theory
-----------------------
``is_prime --prob=56 n``
  Test if the integer *n* is composite or prime with a Miller-Rabin primality test with *(prob+2)/2* iterations.

``factor n``
  Factor the integer *n* using a combination of trial division by small primes, and Pollard's Rho algorithm.

``gen_prime --count=1 bits``
  Samples *count* primes with a length of *bits* bits.

PSK Database
--------------------

Only available if sqlite3 support was compiled in.

``psk_set db db_key name psk``
  Using the PSK database named db and encrypting under the (hex) key ``db_key``,
  save the provided psk (also hex) under ``name``::

    $ botan psk_set psk.db deadba55 bunny f00fee

``psk_get db db_key name``
  Get back a value saved with ``psk_set``::

    $ botan psk_get psk.db deadba55 bunny
    f00fee

``psk_list db db_key``
  List all values saved to the database under the given key::

    $ botan psk_list psk.db deadba55
    bunny

Data Encoding/Decoding
------------------------

``base64_dec file``
  Encode *file* to Base64.

``base64_enc file``
  Decode Base64 encoded *file*.

``hex_dec file``
  Encode *file* to Hex.

``hex_enc file``
  Decode Hex encoded *file*.

Miscellaneous Commands
-------------------------------------
``version --full``
  Print the version number. If option ``-full`` is provided additional details are printed.

``config info_type``
  Prints build information, useful for applications which want to
  build against the library.  The ``info_type`` argument can be any of
  ``prefix``, ``cflags``, ``ldflags``, or ``libs``.

``cpuid``
  List available processor flags (aes_ni, SIMD extensions, ...).

``asn1print file``
  Decode and print *file* with ASN.1 Basic Encoding Rules (BER).

``http_get url``
  Retrieve ressource from the passed http/https *url*.

``speed --msec=100 --provider= --buf-size=4096 algos``
  Measures the speed of the passed *algos*. If no *algos* are passed
  all available speed tests are executed. *msec* (in milliseconds)
  sets the period of measurement for each algorithm.

``rng --system --rdrand bytes``
  Sample *bytes* random bytes from the specified random number generator. If
  *system* is set, the Botan System_RNG is used. If *system* is unset and
  *rdrand* is set, the hardware rng RDRAND_RNG is used.  If both are unset, the
  Botan AutoSeeded_RNG is used.

``cc_encrypt CC passphrase --tweak=``
  Encrypt the passed valid credit card number *CC* using FPE encryption and the
  passphrase *passphrase*. The key is derived from the passphrase using PBKDF2
  with SHA256. Due to the nature of FPE, the ciphertext is also a credit card
  number with a valid checksum. *tweak* is public and parameterizes the
  encryption function.

``cc_decrypt CC passphrase --tweak=``
  Decrypt the passed valid ciphertext *CC* using FPE decryption with
  the passphrase *passphrase* and the tweak *tweak*.
