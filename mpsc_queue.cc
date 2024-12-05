/*
 * MPSC Queue 
 * (Developed by Yeonjin Park of Seoul National University
 * for the implementation and evaluation of upcoming SIGMOD 2026 paper)
 */

#include "grace_queue.h"

/* 
 * head=tail
 *  |
 *  A -> nullptr
 */
template <typename T>
MPSCQueue<T> create_queue(void) {
    MPSCQueue<T> queue = (MPSCQueue<T>) MALLOC(sizeof(MPSCQueueData<T>));
    queue->head = queue->tail 
        = (MPSCQueueNode<T>) MALLOC(sizeof(MPSCQueueNodeData<T>));
    queue->head->next = nullptr;
    queue->head->data = 0;
    return queue;
}

template <typename T>
void destroy_queue(MPSCQueue<T> queue) {
    MPSCQueueNode<T> current = queue->head;
    MPSCQueueNode<T> next = current->next;
    for (; current; current = next) {
        next = current->next;
        FREE(current);
    }
    FREE(queue);
    return;
}

/* 
 * head=tail
 *  |
 *  A -> nullptr
 */

template <typename T>
bool is_empty(MPSCQueue<T> queue) {
    return queue->head->next == nullptr;
}

/* 
 * head        tail                   head                 tail
 *  |           |                ==>   |                    |
 *  A -> ... -> B -> nullptr           A -> ... -> B -> 'new_node' -> nullptr
 */
template <typename T>
void enqueue(MPSCQueue<T> queue, T data) {
    MPSCQueueNode<T> new_node 
                    = (MPSCQueueNode<T>) MALLOC(sizeof(MPSCQueueNodeData<T>));
    new_node->data = data;
    new_node->next = nullptr;

    /* 
     * Step 1. Logical enqueue (competition - other threads are trying CAS)
     * head        tail                 head        tail
     *  |           |              ==>   |           |
     *  A -> ... -> B -> nullptr         A -> ... -> B -> 'new_node' -> nullptr
     */
    while (true) {
        bool is_enqueued = 
        __sync_bool_compare_and_swap(&queue->tail->next, nullptr, new_node);
        if (is_enqueued) {
            break;
        }
        else {
            // TODO : yield to reduce the waste of CPU cycle
            // No problem in integrity 
            // pthread_yield()
        }
    }
    /* 
     * Step 2. Physical enqueue (no competition here)
     *  head               tail
     *   |                  |
     *   A -> ... -> B -> 'new_node' -> nullptr
     */
    queue->tail = new_node;
}

/* Dequeues data to dest if non-empty queue
   Returns true if data is dequeued, false if queue is empty.
 */
template <typename T>
bool dequeue(MPSCQueue<T> queue, T* dest) {
    MPSCQueueNode<T> head = queue->head;
    MPSCQueueNode<T> dequeue_target = head->next;

    /* 
    * Case 1:
    * head=tail
    *  |
    *  A -> nullptr
    */
    if (dequeue_target == nullptr) {
        return false;
    }

    /* 
    * Case 2: 
    * head
    *  |
    *  A -> 'dequeue_target' -> ...
    */

    queue->head = dequeue_target;
    FREE(head);

    *dest = dequeue_target->data;
    return true;
}
 
