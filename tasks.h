#ifndef TASKS_H
#define TASKS_H

#include "queue.h"
#include "stdint.h"

void bluetooth(void);
void check_spi(void);
void clear_display(void);
void set_page(uint8_t page);
void set_column(uint8_t column);
void send_data(uint8_t byte);
void paintpixel(uint16_t x,uint16_t y);
void circle(void);
void square(void);
void triangle(void);
void smile(void);
void checkmark(void);
void error(void);
void display(void);
void sound(void);

#endif
