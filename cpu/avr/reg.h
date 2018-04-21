 /*
 * Macros for hardware access, both direct and via the bit-band region.
 * @{
 *
 * \file
 * Header file with register manipulation macro definitions
 */
#ifndef REG_H_
#define REG_H_

#define REG(x)         (*((volatile unsigned long *)(x)))
#endif /* REG_H_ */