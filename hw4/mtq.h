#include "deq.h"

typedef void* Mtq;
void mtq_del(Mtq, DeqMapF);
Mtq mtq_new(int);

void mtq_tail_put(Mtq, Data);
void mtq_head_put(Mtq, Data);

Data mtq_head_get(Mtq);
Data mtq_tail_get(Mtq);
Data mtq_head_ith(Mtq, int);
Data mtq_tail_ith(Mtq, int);

Data mtq_tail_rem(Mtq, Data);
Data mtq_head_rem(Mtq, Data);