/*
 * atomic_functions.c
 *
 *  Created on: Sep 26, 2016
 *      Author: aliu
 */

/*
 * STD libs
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/*
 * Timing Libraries
 */
//#include <time.h>
#include <sys/time.h>

/*
 * GSL libraries and Math
 */
#include <gsl/gsl_spline.h>
#include <gsl/gsl_errno.h>
#include <math.h>

/*
 * Posix Thread library
 */
#include <pthread.h>

/*
 * Serial COM libraries
 */
#include<unistd.h>
#include<fcntl.h>
#include<termios.h>

/*
 * libmodbus library
 */
#include <modbus/modbus.h>

/*
 * File io library
 */
#include <dirent.h>



