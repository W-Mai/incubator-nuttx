/****************************************************************************
 * arch/sim/src/sim/posix/sim_ioexpander.h
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

#ifndef __ARCH_SIM_SRC_POSIX_SIM_IOEXPANDER_H
#define __ARCH_SIM_SRC_POSIX_SIM_IOEXPANDER_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#ifdef __SIM__
#include "config.h"
#endif

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CODE

/* Pin definitions **********************************************************/

#define IOEXPANDER_DIRECTION_IN            0  /* float */
#define IOEXPANDER_DIRECTION_IN_PULLUP     1
#define IOEXPANDER_DIRECTION_IN_PULLDOWN   2
#define IOEXPANDER_DIRECTION_OUT           3  /* push-pull */
#define IOEXPANDER_DIRECTION_OUT_OPENDRAIN 4

#define PINSET_ALL                 (~((ioe_pinset_t)0))

/* Pin options */

#define IOEXPANDER_OPTION_INVERT   1  /* Set the "active" level for a pin */
#  define IOEXPANDER_VAL_NORMAL    0  /* Normal, no inversion */
#  define IOEXPANDER_VAL_INVERT    1  /* Inverted */

#define IOEXPANDER_OPTION_INTCFG   2  /* Configure interrupt for a pin */
#  define IOEXPANDER_VAL_DISABLE   0  /* 0000 Disable pin  interrupts */
#  define IOEXPANDER_VAL_LEVEL     1  /* xx01 Interrupt on level (vs. edge) */
#    define IOEXPANDER_VAL_HIGH    5  /* 0101 Interrupt on high level */
#    define IOEXPANDER_VAL_LOW     9  /* 1001 Interrupt on low level */
#  define IOEXPANDER_VAL_EDGE      2  /* xx10 Interrupt on edge (vs. level) */
#    define IOEXPANDER_VAL_RISING  6  /* 0110 Interrupt on rising edge */
#    define IOEXPANDER_VAL_FALLING 10 /* 1010 Interrupt on falling edge */
#    define IOEXPANDER_VAL_BOTH    14 /* 1110 Interrupt on both edges */

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* This type represents a bitmap of pins
 *
 * For IOE NPINS greater than 64, ioe_pinset_t represent one interrupt pin
 * number instead of a bitmap of pins.
 */

#if CONFIG_IOEXPANDER_NPINS <= 8
typedef uint8_t ioe_pinset_t;
#elif CONFIG_IOEXPANDER_NPINS <= 16
typedef uint16_t ioe_pinset_t;
#elif CONFIG_IOEXPANDER_NPINS <= 32
typedef uint32_t ioe_pinset_t;
#elif CONFIG_IOEXPANDER_NPINS <= 64
typedef uint64_t ioe_pinset_t;
#else
typedef uint8_t ioe_pinset_t;
#endif

#ifdef CONFIG_IOEXPANDER_INT_ENABLE
/* This type represents a pin interrupt callback function */

struct ioexpander_dev_s;
typedef CODE int (*ioe_callback_t)(FAR struct ioexpander_dev_s *dev,
                                   ioe_pinset_t pinset, FAR void *arg);
#endif /* CONFIG_IOEXPANDER_INT_ENABLE */

/* I/O expander interface methods */

struct ioexpander_dev_s;
struct ioexpander_ops_s
{
  CODE int (*ioe_direction)(FAR struct ioexpander_dev_s *dev, uint8_t pin,
                          int direction);
  CODE int (*ioe_option)(FAR struct ioexpander_dev_s *dev, uint8_t pin,
                       int opt, FAR void *val);
  CODE int (*ioe_writepin)(FAR struct ioexpander_dev_s *dev, uint8_t pin,
                         bool value);
  CODE int (*ioe_readpin)(FAR struct ioexpander_dev_s *dev, uint8_t pin,
                        FAR bool *value);
  CODE int (*ioe_readbuf)(FAR struct ioexpander_dev_s *dev, uint8_t pin,
                        FAR bool *value);
#ifdef CONFIG_IOEXPANDER_MULTIPIN
    CODE int (*ioe_multiwritepin)(FAR struct ioexpander_dev_s *dev,
                                FAR uint8_t *pins, FAR bool *values,
                                int count);
  CODE int (*ioe_multireadpin)(FAR struct ioexpander_dev_s *dev,
                               FAR uint8_t *pins, FAR bool *values,
                               int count);
  CODE int (*ioe_multireadbuf)(FAR struct ioexpander_dev_s *dev,
                               FAR uint8_t *pins, FAR bool *values,
                               int count);
#endif
#ifdef CONFIG_IOEXPANDER_INT_ENABLE
    CODE FAR void *(*ioe_attach)(FAR struct ioexpander_dev_s *dev,
                               ioe_pinset_t pinset,
                               ioe_callback_t callback, FAR void *arg);
    CODE int (*ioe_detach)(FAR struct ioexpander_dev_s *dev, FAR void *handle);
#endif
};

struct ioexpander_dev_s
{
    /* "Lower half" operations provided by the I/O expander lower half */

    FAR const struct ioexpander_ops_s *ops;

    /* Internal storage used by the I/O expander may (internal to the I/O
     * expander implementation).
     */
};

#endif //__ARCH_SIM_SRC_POSIX_SIM_IOEXPANDER_H
