## What is this?
100bandz is a .sys driver for windows, originaly intented to be used as an "educational EAC anticheat bypass", but I have lost most interest in this project, this can serve as a good base to learn from expand upon.  
there are currently a lot of features missing, like the communication implementation betweem UM and KM, but as i wrote in the roadmap, this shouldnt be hard to do - either map a rw memory area of like 2 bytes, should be plenty to communicate OR map enough for an encrypted aes string.

## Current Features
Build release/debug x64 - should automatically be set-up.  
SSDT TABLE LOOKUP  
PHYSICAL MEMORY R&W  
AES ENCRYPTION  
LOGGING HEADER  

## Roadmap
* `rewrite physical memory operations` I'm currently reading up on translation of virt->phys, and how it exactly works. next up is a complete rewrite.
* `vuln chinese driver` i'll try finding a vulnerable preferably chinese driver, admin->kernel exploit, question is, how should we go about mapping our driving using their kernel driver? maybe it doesnt even have to be vulnerable, make we can simply patch its memory <- hollow it out, AND patch it on disk after we load our driver (or `disable eac integrity check`) eac shouldnt be checking memory of signed modules, so we should be good there
* `usermode -> driver communications` honestly i think a shared memory pool with AES256 encryption is good enough, no need to overcomplicate it

# TODO Once bypass is complete
* `patch/check performance impact of cr3 shuffle` if theres no RPM/WPM delay then theres no point in patching it
* `tested delay` current read opeartion on notepad takes 10ms with reading cr3
* `custom builds` both ddriver and usermode app will get custom built on windows build server, and then packed
  
## THOUGHTS
| Function Name | Usage Status | Reasoning |
|--------------|--------------|-----------|
| `NtReadVirtualMemory` | ❌ DO NOT USE | - same reason as most of the read/write mem functions are going to be, under the hood it uses KeStackAttachProcess, which has been detected and is bannable! (not flaggable, bannable by eac!) |
| `MmCopyVirtualMemory` | ❌ DO NOT USE | - KeStackAttachProcess |
| `MiDoMapped` | ❌ DO NOT USE | - KeStackAttachProcess |
| `MiDoPoolCopy` | ❌ DO NOT USE | - KeStackAttachProcess |

## Logger Showcase
![Logger SS](https://i.ibb.co/X5cSZpd/image.png)


# Credits
- [SchuhmacherAndre](https://github.com/SchuhmacherAndre)
- [n0verify](https://github.com/n0verify)
