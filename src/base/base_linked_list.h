#ifndef BASE_LINKED_LIST_H
#define BASE_LINKED_LIST_H


#define CheckNil(nil,p) ((p) == 0 || (p) == nil)
#define SetNil(nil, p) ((p)=nil)

#define AppendLast(first,last,next,value) (CheckNil(0, last))?\
((first)=(value),(last)=(value)):\
((next)=(value),(last)=(value))

#define SLLPush(f,l,n) (f == 0) ?\
  (f)=(l)=(n):\
  ((l)->next=(n), (l)=(n), SetNil(0,(n)->next))
  
// Base Singly-Linked-List Queue Macros
#define SLLQueuePush_NZ(nil,f,l,n,next) (CheckNil(nil,f)?\
((f)=(l)=(n),SetNil(nil,(n)->next)):\
((l)->next=(n),(l)=(n),SetNil(nil,(n)->next)))

#define SLLQueuePop_NZ(nil,f,l,next) ((f)==(l)?\
(SetNil(nil,f),SetNil(nil,l)):\
((f)=(f)->next))

#define SLLQueuePushFront_NZ(nil,f,l,n,next) (CheckNil(nil,f)?\
((f)=(l)=(n),SetNil(nil,(n)->next)):\
((n)->next=(f),(f)=(n)))

#define SLLQueuePush(f,l,n) SLLQueuePush_NZ(0,f,l,n,next)

#define SLLStackPush_N(f,n,field) ((n)->field=(f), (f)=(n))
#define SLLStackPop_N(f,field) ((f)=(f)->field)
#define SLLQueuePushFront(f,l,n) SLLQueuePushFront_NZ(0,f,l,n,next)


#endif