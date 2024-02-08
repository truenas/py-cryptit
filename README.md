cryptit
======

**Python bindings to the crypt_r syscall**

**QUICK HOWTO:**

Generate a SHA512 hashed password, with rounds set to 656,000 using a
cryptographically secure 16 char salt using the modular crypt format
found on most *nix based systems.
```
from secrets import token_hex
from cryptit import cryptit

>>> SHA512_PREFIX = '$6'
>>> rounds = 656_000
>>> salt = token_hex()[:16]
>>> passwd = 'your password'
>>> settings = f'{SHA512_PREFIX}$rounds={rounds}${salt}'
>>> hash = cryptit(passwd, settings)
>>> hash
'$6$rounds=656000$215bf4d450b99d32$W0MeOWckrgYhSKVpRFuxMtWm7OUGblzH91HAOyJMeGB7aitE4w/5trbfX9aI7P5GukcQlcihOEv3qfTfzP5V//'
>>>
```

Generating a secure hash is 50% of the problem. One must verify the password
to make sure it is correct. Using the `hash` from the above example, this
demonstrates how to securely verify if the password is correct against a given
`user_hash`
```
from secrets import compare_digest
from cryptit import cryptit

>>> passwd = 'your password'
>>> hash = '$6$rounds=656000$215bf4d450b99d32$W0MeOWckrgYhSKVpRFuxMtWm7OUGblzH91HAOyJMeGB7aitE4w/5trbfX9aI7P5GukcQlcihOEv3qfTfzP5V//'
>>> user_hash = hash
# the `user_hash` variable is expected to be found/provided/given
# to this function by whatever means necessary (stored in a db, from shadow file, etc)
>>> compare_digest(cryptit(passwd, hash), user_hash)
True  # means the password matches

# given the same example, if we were to scramble the `user_hash` to ensure the validation
# fails it would look like this
user_hash = 'does not match'
>>> compare_digest(cryptit(passwd, hash), user_hash)
False
```
