## Current Features
Build release/debug x64 - should automatically be set-up.  
SSDT TABLE LOOKUP  
PHYSICAL MEMORY R&W
AES ENCRYPTION  

## Roadmap
* `rewrite physical memory operations` current implementation is 50% pasted, I'm currently reading up on translation of virt->phys, and how it exactly works. next up is a complete non-pasted rewrite.
* `vuln chinese driver` i'll try finding a vulnerable preferably chinese driver, admin->kernel exploit, question is, how should we go about mapping our driving using their kernel driver? maybe it doesnt even have to be vulnerable, make we can simply patch its memory AND patch it on disk after we load our driver (or `disable eac integrity check`) eac shouldnt doesnt check memory of signed modules, so we should be good there

# TODO Once bypass is complete
* `patch/check performance impact of cr3 shuffle` if theres no RPM/WPM delay then theres no point in patching it

## THOUGHTS
| Function Name | Usage Status | Reasoning |
|--------------|--------------|-----------|
| `NtReadVirtualMemory` | ❌ DO NOT USE | - same reason as most of the read/write mem functions are going to be, under the hood it uses KeStackAttachProcess, which has been detected and is bannable! (not flaggable, bannable by eac!) |
| `MmCopyVirtualMemory` | ❌ DO NOT USE | - KeStackAttachProcess |
| `MiDoMapped` | ❌ DO NOT USE | - KeStackAttachProcess |
| `MiDoPoolCopy` | ❌ DO NOT USE | - KeStackAttachProcess |




# Credits
- [SchuhmacherAndre](https://github.com/SchuhmacherAndre)
- [n0verify](https://github.com/n0verify)
