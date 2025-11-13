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

#include "trackfil.h"

#include <stdlib.h>

typedef struct TrackedFile {
    FILE *fp;
    const char *name;   /* original name (pre-munge) - memory owned by caller */
    struct TrackedFile *next;
} TrackedFile;

static TrackedFile *tf_head = NULL;

void trackfile_initialise(alloc_fn *allocator)
{
    tf_head = NULL;

    // Ignore the allocator - there's no free, and system allocators are better.
}

static TrackedFile *trackfile_add(const char *name)
{
    TrackedFile *node = (TrackedFile *)malloc(sizeof(TrackedFile));
    if (!node) return NULL;

    node->name = name;
    node->next = tf_head;
    tf_head = node;

    return node;
}

void trackfile_close(FILE *f)
{
    TrackedFile *p, *t;

    if (f == NULL)
        return;

    /* Close the stream regardless of whether we track it */
    fclose(f);

    p = tf_head;
    if (p == NULL)
        return;

    /* If the head node matches, free it */
    if (p->fp == f) {
        tf_head = p->next;
        free(p);
        return;
    }

    /* Find the predecessor of the node to delete */
    while (p != NULL && p->next != NULL && p->next->fp != f)
        p = p->next;

    /* If found, free */
    if (p != NULL && p->next != NULL) {
        t = p->next;
        p->next = t->next;
        free(t);
    }
}

FILE *trackfile_open(const char *fname, const char *mode)
{
    FILE *f;
    TrackedFile *node;

    if (!fname)
        return NULL;

    node = trackfile_add(fname);
    if (!node)
        return NULL;

    f = fopen(fname, mode);

    node->fp = f;
    if (!f) {
        // node will be tf_head.
        tf_head = tf_head->next;
        free(node);
    }

    return f;
}

void trackfile_finalise(void)
{
    while (tf_head != NULL) {
        TrackedFile *next = tf_head->next;

        if (tf_head->fp)
            fclose(tf_head->fp);

        free(tf_head);

        tf_head = next;
    }
}
