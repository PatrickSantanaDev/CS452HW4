#include "deq.h"

typedef void* Mdeq;
void mdeq_del(Mdeq, DeqMapF);
Mdeq mdeq_new(int);

void mdeq_tail_put(Mdeq, Data);
void mdeq_head_put(Mdeq, Data);

Data mdeq_head_get(Mdeq);
Data mdeq_tail_get(Mdeq);
Data mdeq_head_ith(Mdeq, int);
Data mdeq_tail_ith(Mdeq, int);

Data mdeq_tail_rem(Mdeq, Data);
Data mdeq_head_rem(Mdeq, Data);