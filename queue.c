//Queue code written by Dr. Carroll.

#include "queue.h"

bool get_q(queue_t *q0, int16_t *datum){
    // writable parameter = datum;
    bool error = false;
    if (q0->head == q0->tail){ // empty
      datum = 0;
      return true; //{datum==0,error == true}
    }else{
         *datum = q0->buffer[q0->head];
         q0->head++;
         if (q0->head >= q0->bufSize) q0->head=0;
    }
    return error;
}

bool put_q(queue_t *q0, const int16_t *datum){
    uint16_t new_tail = q0->tail + 1;
    if (new_tail >= q0->bufSize) new_tail = 0;
    if (new_tail == q0->head) return true; //full-error
    else{
        q0->buffer[q0->tail] = *datum;
        q0->tail = new_tail;
    }
    return false;
}
