#ifndef __YJIN_QUEUE__

#define __YJIN_QUEUE__

template <typename T>
struct MPSCQueueNodeData;

template <typename T>
struct MPSCQueueData;

template <typename T>
using MPSCQueue = MPSCQueueData<T>*;

template <typename T>
using MPSCQueueNode = MPSCQueueNodeData<T>*;

template <typename T>
struct MPSCQueueNodeData {
    MPSCQueueNode<T> next;
    T data;
};

template <typename T>
struct MPSCQueueData {
    MPSCQueueNode<T> head;
    MPSCQueueNode<T> tail;
};

template <typename T> MPSCQueue<T> create_queue(void);
template <typename T> void destroy_queue(MPSCQueue<T> queue);
template <typename T> bool is_empty(MPSCQueue<T> queue);
template <typename T> void enqueue(MPSCQueue<T> queue, T data);
template <typename T> bool dequeue(MPSCQueue<T> queue, T* dest);

#endif /* __YJIN_QUEUE__ */
