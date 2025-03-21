// Wokwi Custom Chip - For docs and examples see:
// https://docs.wokwi.com/chips-api/getting-started
//
// SPDX-License-Identifier: MIT
// Copyright 2023 A D

#include "wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

// #define DEBUGGING

void printBits(size_t const size, const void * ptr)
{
  unsigned char *b = (unsigned char*) ptr;
  unsigned char byte;
  int i, j;

  for (i = size-1; i >= 0; i--) {
    for (j = 7; j >= 0; j--) {
      byte = (b[i] >> j) & 1;
#ifdef DEBUGGING
      printf("%u", byte);
#endif
    }
#ifdef DEBUGGING
    printf(" ");
#endif
  }
#ifdef DEBUGGING
   printf("\n");
#endif
}

typedef struct {
  bool first_write;
  pin_t pins[8];
  buffer_t buffer;
} chip_state_t;

bool on_i2c_connect(void *user_data, uint32_t address, bool read) {
  // `address` parameter contains the 7-bit address that was received on the I2C bus.
  // `read` indicates whether this is a read request (true) or write request (false).
  
  return true; // true means ACK, false NACK
}

uint8_t on_i2c_read(void *user_data) {
  
  return 0; // The byte to be returned to the microcontroller
}

bool on_i2c_write(void *user_data, uint8_t data) {
  chip_state_t *chip = (chip_state_t *)user_data;
  if (((uint8_t)chip->first_write) == 0) {
    unsigned char *b = (unsigned char*)&data;
    unsigned char byte;
    int i, j;
    i = 0;

    for (j = 7; j >= 0; j--) {
      byte = (b[0] >> j) & 1;
      if (byte == 0) {
        char pin[3] = "";
        sprintf(pin, "P%d", i);
#ifdef DEBUGGING
        printf("%s - output\n", pin);
#endif
        chip->pins[j] = pin_init(pin, OUTPUT_LOW);
      } else {
        char pin[3] = "";
        sprintf(pin, "P%d", i);
#ifdef DEBUGGING
        printf("%s - input\n", pin);
#endif
        chip->pins[j] = pin_init(pin, INPUT);
      }
      i++;
    }
    chip->first_write = true;
  } else {
    unsigned char *b = (unsigned char*)&data;
    unsigned char byte;
    int i, j;
    i = 0;

    for (j = 7; j >= 0; j--) {
      byte = (b[0] >> j) & 1;
      if (byte == 0) {
        char pin[3] = "";
        sprintf(pin, "P%d", j);
#ifdef DEBUGGING
        printf("%s - LOW\n", pin);
#endif
        pin_write(chip->pins[i], LOW);
      } else {
        char pin[3] = "";
        sprintf(pin, "P%d", j);
#ifdef DEBUGGING
        printf("%s - HIGH\n", pin);
#endif
        pin_write(chip->pins[i], HIGH);
      }
      i++;
    }
  }

  // `data` is the byte received from the microcontroller
  return true; // true means ACK, false NACK
}

void on_i2c_disconnect(void *user_data) {
  // This method is optional. Useful if you need to know when the I2C transaction has concluded.
}

void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));

  const i2c_config_t i2c1 = {
    .address = 0x20,
    .scl = pin_init("SCL", INPUT_PULLUP),
    .sda = pin_init("SDA", INPUT_PULLUP),
    .connect = on_i2c_connect,
    .read = on_i2c_read,
    .write = on_i2c_write,
    .disconnect = on_i2c_disconnect,
    .user_data = chip,
  };

  i2c_init(&i2c1);
  // TODO: Initialize the chip, set up IO pins, create timers, etc.

  // printf("Hello from custom chip!\n");
}