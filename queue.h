#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include <stdint.h>
#define Q_SIZE 10
typedef struct {uint16_t head;
    uint16_t tail;
    const uint16_t bufSize;
		int16_t buffer[Q_SIZE+1]; 
                } queue_t;
//API
bool get_q(queue_t *q0, int16_t *datum);
bool put_q(queue_t *q0, const int16_t *datum);

#endif
