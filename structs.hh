#pragma once

typedef struct _SDT {
    UINT32* ServiceTable;
    UINT32* Count;
    UINT32  Limit;
    UINT32* ArgumentTable;
} SDT, * PSDT;