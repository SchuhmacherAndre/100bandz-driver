build release/debug x64 - should be automatically set-up

map with kdmapper for testing purposes

currently has SSDT, AES256 support - hooks to be implemented soon.

## THOUGHTS
| Function Name | Usage Status | Reasoning |
|--------------|--------------|-----------|
| `NtReadVirtualMemory` | ❌ DO NOT USE | - same reason as most of the read/write mem functions are going to be, under the hood it uses KeStackAttachProcess, which has been detected and is bannable! (not flaggable, bannable by eac!) |
| `MmCopyVirtualMemory` | ❌ DO NOT USE | - KeStackAttachProcess |
| `MiDoMapped` | ❌ DO NOT USE | - KeStackAttachProcess |
| `MiDoPoolCopy` | ❌ DO NOT USE | - KeStackAttachProcess |

Reading physical memory might be the move.

going to add more stuff as i dive deeper.


# Credits
- [SchuhmacherAndre](https://github.com/SchuhmacherAndre)
- [n0verify](https://github.com/n0verify)
