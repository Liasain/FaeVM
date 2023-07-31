# FaeVM

FaeVM is a virtual machine (VM) that runs FaePL, an 8-bit, stack language.

## How-To

Build using `make`.

`$ ./main ": CR 10 EMIT ; : . 10 /MOD DUP 0 = IF DROP ELSE . THEN 48 + EMIT ; 139 . CR"`

`139 `

Main runs the compiler on the input, and then has the machine execute the code.

## Status

Use at your own peril. FaeVM is made for fun and learning.

To see the state of the program, set `vm->log` to 1.

