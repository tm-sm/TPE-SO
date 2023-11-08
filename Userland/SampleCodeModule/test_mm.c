#include <test_util.h>
#include <system.h>
#include <stdlib.h>
#include <string.h>
#include <standardLib.h>

#define MAX_BLOCKS 40

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(int argc, char *argv[]) {

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 2)
    return -1;

  if ((max_memory = satoi(argv[1])) <= 0)
    return -1;

  while (1) {
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = alloc(mm_rqs[rq].size);
      if (mm_rqs[rq].address != NULL) {
          printFormat("Reserved: %d bytes | ",mm_rqs[rq].size);
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        memset(mm_rqs[i].address, i, mm_rqs[i].size/2);

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size/2)) {
          printFormat("test_mm ERROR\n");
          return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address) {
          printFormat("Freed: %d bytes from %x | ",mm_rqs[i].size,mm_rqs[i].address);
          dealloc(mm_rqs[i].address);
      }
    wait(1000);
  }
}
