/**
 * @file compiler.h
 *
 * @brief defines non standard c things like packing to avoid issues with different pragmas etc on different compilers
 *
 * @author OT
 *
 * @date Jan 2013
 *
 */


#ifndef __COMPILER__
#define __COMPILER__


#if defined(__CC_ARM)
	/* realview (keil) compiler */

	// packing
	#define packed_start _Pragma("push") _Pragma("pack(1)")     // unused (see packed)
	#define packed_end _Pragma("pop")                           // unused (see packed)
	#define packed(type) type __attribute__ ((packed))      	// pack this object (ie struct etc)

	// placement (ram/rom/etc */
	#define at_symbol(sym) __attribute__((section(sym)))        // place a object (ie struct var etc) at a position in code

	// inlining
	#define force_inline __forceinline                          // force this function to inline

	// weak functions
	#define weak() __weak

#elif (__GNUC__)
	/* gcc compiler */

	// packing
	#define packed_start                                            // unused (see packed)
	#define packed_end                                              // unused (see packed)
	#define packed(type) type __attribute__ ((packed))      		// pack this object (ie struct etc)

	// placement (ram/rom/etc */
	#define at_symbol(sym) __attribute__((section(sym)))            // place a object (ie struct var etc) at a position in code

	// inlining
	#define force_inline __attribute__((always_inline))             // force this function to inline

	// weak functions
	#define weak __attribute((weak))

#elif (__IAR_SYSTEMS_ICC__)
	/* iar compiler */

	// packing
	#define packed_start                                            // unused (see packed)
	#define packed_end                                              // unused (see packed)
	#define packed(type) __packed type                              // pack this object (ie struct etc)

	// placement (ram/rom/etc)
	#define at_symbol(sym)                                          // place a object (ie struct var etc) at a position in code (not available yet)

	// inlining
	#define force_inline                                            // not supported

	// weak functions
	#define weak() __weak

#endif


#endif

