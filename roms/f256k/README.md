# ROM Files for the F256Ke

This directory contains BIN files for programming the F256Ke flash memory with the Foenix Toolbox.

## How to Install

Currently, the toolbox must be installed by bulk programming the flash memory.
Using the FoenixMgr Python script, this can be done with the following command (substitute the device path or name for your F256K's USB debug port):

```
python FoenixMgr.zip --port {debug device name} --flash-bulk toolbox.csv
```
