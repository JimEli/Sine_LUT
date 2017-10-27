/*************************************************************************
 * Title: Sine LUT
 * File: SineLUT.c
 * Author: James Eli
 * Date: 7/2/2017
 *
 * This program demonstrates a simplistic trigonometric sine lookup table
 * or LUT.
 *
 * Notes:
 *   (1) Compiled with GCC 5.3.0.
 *************************************************************************
 * Change Log:
 *   07/02/2017: Initial release. JME
 *************************************************************************/
//#include "stdhdr.h" // Include to turn off assertions.
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
//#include <assert.h>
#include <math.h>
#include <fenv.h>
#include <time.h>

// Float validity checks.
bool isNaN( float val ) { return val != val; }
bool isNotNaN( float val ) { return val == val; }
bool isNaNOrInfinity( float val ) { return (val*0) != 0; }
bool isNotNaNOrInfinity( float val ) { return (val*0) == 0; }

#define DEGTORAD(d) ( d*(M_PI/180.) )
#define RADTDDEG(r) ( r*(180./M_PI) )

// Table of sin values [0-89 degrees].
const float fSine[ 90 ] = {
	0.000000, 0.017452, 0.034899, 0.052336, 0.069756, 0.087156, 0.104528, 0.121869, 0.139173,
	0.156434, 0.173648, 0.190809, 0.207912, 0.224951, 0.241922, 0.258819, 0.275637, 0.292372,
	0.309017, 0.325568, 0.342020, 0.358368, 0.374607, 0.390731, 0.406737, 0.422618, 0.438371,
	0.453990, 0.469472, 0.484810, 0.500000, 0.515038, 0.529919, 0.544639, 0.559193, 0.573576,
	0.587785, 0.601815, 0.615661, 0.629320, 0.642788, 0.656059, 0.669131, 0.681998, 0.694658,
	0.707107, 0.719340, 0.731354, 0.743145, 0.754710, 0.766044, 0.777146, 0.788011, 0.798636,
	0.809017, 0.819152, 0.829038, 0.838671, 0.848048, 0.857167, 0.866025, 0.874620, 0.882948,
	0.891007, 0.898794, 0.906308, 0.913545, 0.920505, 0.927184, 0.933580, 0.939693, 0.945519,
	0.951057, 0.956305, 0.961262, 0.965926, 0.970296, 0.974370, 0.978148, 0.981627, 0.984808,
	0.987688, 0.990268, 0.992546, 0.994522, 0.996195, 0.997564, 0.998630, 0.999391, 0.999848,
	//1.000000
};

// Imprecise method, which does not guarantee v = v1 when t = 1, due to floating-point arithmetic error.
// This form may be used when the hardware has a native fused multiply-add instruction.
//float lerp(float v0, float v1, float t) {
//  return v0 + t * (v1 - v0);
//}
// Precise method, which guarantees v = v1 when t = 1.
float lerp( float v0, float v1, float t ) {
  return (1 - t)*v0 + t*v1;
}

// Integer sin LUT.
float Sine( int a ) {
  if ( a >= 0 && a <= 89 )
	return fSine[a];
  else
    return FP_NAN;
}

float f;
float const two_right_angles = 180.0f;
void SinAsm( float degree ) {
	  // Convert angle from degrees to radians, then calculate sin value.
	  __asm__ __volatile__ (
	    "fld  %1;"
	    "fld  %2;"
	    "fldpi  ;"
	    "fmulp  ;"
	    "fdivp  ;"
	    "fsin   ;"
	    "fstp %0;"
	    : "=m" (f)
	    : "m" (two_right_angles), "g" (degree)
		: "memory"
	  );
}

int main(void) {
  // Required to make eclipse console output work properly.
  setvbuf( stdout, NULL, _IONBF, 0 );
  fflush( stdout );

#if 0
  // Clear floating point exceptions.
  feclearexcept( FE_ALL_EXCEPT );
  for ( int i=0; i<90; i++ )
    printf( "%f, %s", sin( DEGTORAD( i ) ), (i+1)%9 ? "" : "\n" );
  int feRaised = fetestexcept( FE_ALL_EXCEPT );
  if ( feRaised ) {
    printf( "A floating point exception (#%d) occurred.\n", feRaised );
    exit( EXIT_FAILURE );
  }
#endif

  // Time this.
  clock_t start, end;
  double cpu_time_used;

  //
  // Sine table function.
  //
  start = clock();
  for ( int j=0; j<1000; j++ )
    for ( int i=0; i<90; i++ )
      //printf( "%d = %f\n", i, Sine( i ) );
      f = Sine( i );
  printf("%f\n", f); // Do something with f.
  // Calculate and display execution time.
  end = clock();
  cpu_time_used = ((double)(end - start))/CLOCKS_PER_SEC;
  printf( "Sine time elapsed: %f\n", cpu_time_used );

  //
  // inline assembly fsin.
  //
  start = clock();
  for ( int j=0; j<1000; j++ ) {
    for ( int i=0; i<90; i++ ) {
  	  // Convert angle from degrees to radians, then calculate sin value.
      float const rightAngleX2 = 180.0f;
  	  __asm__ __volatile__ (
  	    "fld  %1;"
  	    "fild %2;"
  	    "fldpi  ;"
  	    "fmulp  ;"
  	    "fdivp  ;"
  	    "fsin   ;"
  	    "fstp %0;"
  	    : "=m" (f)
//  	    : "m"(two_right_angles), "m" (i)
  	    : "g"(rightAngleX2), "m" (i)
		: "memory"
  	  );
    }
  }
  printf( "%f\n", f );
  // Calculate and display execution time.
  end = clock();
  cpu_time_used = ((double)(end - start))/CLOCKS_PER_SEC;
  printf( "fsin inline time elapsed: %f\n", cpu_time_used );

  //
  // inline assembly fsin function.
  //
  start = clock();
  for ( int j=0; j<1000; j++ )
    for ( int i=0; i<90; i++ )
        SinAsm( i );
  printf( "%f\n", f );
  // Calculate and display execution time.
  end = clock();
  cpu_time_used = ((double)(end - start))/CLOCKS_PER_SEC;
  printf( "SinAsm time elapsed: %f\n", cpu_time_used );

  //
  // math library sin function.
  //
  start = clock();
  for ( int j=0; j<10000; j++ )
    for ( int i=0; i<90; i++ )
      //printf( "%d = %f\n", i, sin( i ) );
        f = sin( i );
  printf("%f\n", f); // Do something with f.
  // Calculate and display execution time.
  end = clock();
  cpu_time_used = ((double)(end - start))/CLOCKS_PER_SEC;
  printf( "Sin time elapsed: %f\n", cpu_time_used );

  return 0;
}

