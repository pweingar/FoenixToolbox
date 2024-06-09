# Foenix Toolbox ABI for 65816 Machines

Toolbox functions are compiled using Calypsi's `simple_call` convention. In this convention, the first parameter is passed in register, while the remaining parameters are passed on the stack. Results are returned in the A, C, or X:C registers according to the size of the return value. Note that registers C, X, Y, and the direct page pseudo-registers 0 - 7 may be altered by the function.

| Type                                | Size | Register |
| ----                                | ---- | -------- |
| char                                | 8    | A        |
| int, short, tiny, and near pointers | 16   | C        |
| long, float, far and huge pointers  | 32   | X:C      |

Please see the Calypsi manual for further details about the `simple_call` convention.

All toolbox functions are available through a jumptable starting at $00:F000 and are far-call subroutines, so the caller must use the `JSL` instruction to call them.
