/****************************************************************************
 * arch/sim/src/sim/posix/sim_hostioexpander.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/


#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <linux/gpio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "sim_internal.h"

/* Pin definitions **********************************************************/

#define IOEXPANDER_DIRECTION_IN            0  /* float */
#define IOEXPANDER_DIRECTION_IN_PULLUP     1
#define IOEXPANDER_DIRECTION_IN_PULLDOWN   2
#define IOEXPANDER_DIRECTION_OUT           3  /* push-pull */
#define IOEXPANDER_DIRECTION_OUT_OPENDRAIN 4

int host_ioe_open(const char *filename)
{
  return open(filename, O_RDONLY);
}

int host_ioe_close(int fd)
{
  return close(fd);
}

int host_ioe_direction(int fd, uint8_t pin, int direction)
{
  switch (direction)
    {
      case IOEXPANDER_DIRECTION_IN:
      case IOEXPANDER_DIRECTION_IN_PULLUP:
      case IOEXPANDER_DIRECTION_IN_PULLDOWN:
        {
          host_ioe_readpin(fd, pin, NULL);
          break;
        }
      case IOEXPANDER_DIRECTION_OUT:
      case IOEXPANDER_DIRECTION_OUT_OPENDRAIN:
        {
          host_ioe_writepin(fd, pin, 0);
          break;
        }
      default:break;
    }

  return 0;
}

int host_ioe_writepin(int fd, uint8_t pin, bool value)
{
  struct gpiohandle_request rq;
  struct gpiohandle_data    data;

  int ret;

  rq.lineoffsets[0] = pin;
  rq.flags = GPIOHANDLE_REQUEST_OUTPUT;
  rq.lines = 1;
  ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);

  if (ret == -1)
    {
      return errno;
    }
  data.values[0] = value;
  ret = ioctl(rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);

  if (ret == -1)
    {
      return errno;
    }

  close(rq.fd);
  return ret;
}

int host_ioe_readpin(int fd, uint8_t pin, bool *value)
{
  struct gpiohandle_request rq;
  struct gpiohandle_data    data;

  int ret;

  if (value == NULL)
    {
      return 0;
    }

  rq.lineoffsets[0] = pin;
  rq.flags = GPIOHANDLE_REQUEST_INPUT;
  rq.lines = 1;
  ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);

  if (ret == -1)
    {
      return errno;
    }

  ret = ioctl(rq.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
  if (ret == -1)
    {
      return errno;
    }

  *value = data.values[0];
  close(rq.fd);
  return ret;
}
