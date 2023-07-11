# FaeVM

FaeVM is a virtual machine (VM) that runs FaeTH, an 8-bit, stack language.

## How-To

Build using `make`.

`$ ./main 1 2 + . CR`

`3`

`$ ./main 2 3 \* . CR`

`6`

`$ ./main ": A 2 ; : B 3 ; B A * . CR"`

`6`

To enable more output between steps of execution, add `1 LOG` or `2 LOG` to the beginning of the program.

`$ ./main "2 LOG 1 2 +"

The program prints nothing, but the stack is displayed between steps.

## Status

Use at your own peril. FaeVM is made for fun and learning.

Separation of levels is poorly done and needs a rethink.

FaeTH is a placeholder, in the event of the arrival of a better pun.

FaeTH: Like FORTH, which it immitates poorly, possibly squatted.

Faelang: Phalanx, feeling, possibly squatted.

FaePL: Fable.

