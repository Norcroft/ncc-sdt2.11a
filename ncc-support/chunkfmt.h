/* Copyright 2025 Piers Wombwell
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>

// Generic chunked-file container used by AOF (OBJ_*). Matches the Acorn spec.

// "ChunkFileId".
#define CF_MAGIC        0xC3CBC6C5u

typedef struct cf_entry {
    char  cfe_key[8];   // chunkId, eg. "OBJ_HEAD", "OBJ_AREA".
    int32 cfe_offset;   // file offset of chunk, or 0 for unused chunk.
    int32 cfe_size;     // exact size of chunk in bytes.
} cf_entry;

typedef struct cf_header {
    int32    cf_magic;      // ChunkField/ChunkFileId = CF_MAGIC.
    int32    cf_maxchunks;  // number of entries in the header.
    int32    cf_numchunks;  // number of used entries (0 to maxChunks).
    cf_entry cf_chunks[1];  // variable length table.
} cf_header;
