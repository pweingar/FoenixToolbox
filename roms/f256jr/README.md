# ROM Files for the F256JRe

This directory contains BIN files for programming the F256JRe flash memory with the Foenix Toolbox.

## How to Install

Currently, the toolbox must be installed by bulk programming the flash memory.
Using the FoenixMgr Python script, this can be done with the following command (substitute the device path or name for your F256JR's USB debug port):

```
python FoenixMgr.zip --port {debug device name} --flash-bulk toolbox.csv
```
