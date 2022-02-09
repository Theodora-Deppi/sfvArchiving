# SFV Hashing tool

## Created for personal archiving reason, hence the name.

Currently, only CRC hashing is supported.

Uses [Fast CRC](https://github.com/stbrumme/crc32) & [MIO](https://github.com/mandreyel/mio).

Speed Tests
[Tested with Intel(R) Core(TM) i9-10980HK CPU @ 2.40GHz
 & PM981a NVMe SAMSUNG 2048GB & 32 GB DDR4 Ram]

| Test Type   | Input                       | This    | QuickSFV |
|-------------|-----------------------------|---------|----------|
| Writing SFV | 1GB File                    | 236 ms  | 2829 ms  |
| Writing SFV | 5GB File                    | 1108 ms | 13999 ms |
| Writing SFV | 10GB File                   | 2081 ms | 38771 ms |
| Reading SFV | 1GB File                    | 242 ms  | 2853 ms  |
| Reading SFV | 5GB File                    | 1108 ms | 14050 ms |
| Reading SFV | 10GB File                   | 2105 ms | 28917 ms |