/*
 * PrimosSerial.c
 * 
 * Copyright 2019 José Manuel Miranda Villagrán <miranda@ThinkPad>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>

#define N 1000000


#ifdef __TENON__
#define sysconf(_SC_CLK_TCK) 60
#endif 

#ifdef __MCC__
#define clock() TimeNow()
#define CLOCKS_PER_SEC CLOCK_TICK
#else
#endif 

#ifdef __sun__
#define CLOCKS_PER_SEC 1000000
#endif

#ifndef __MCC__ /* Todos excepto parsytec */
static double  timetick;
static double  tstart = 0.0;
static double  ucpustart = 0.0, scpustart = 0.0;
static int     first = 1;

void ctimer(double *, double *, double *);
#endif


void ctimer(double *elapsed, double *ucpu, double *scpu)
{ int             i;
  struct tms      cpu;
  struct timeval  tp;
  struct timezone tzp;
  if(first) {
    /* Initialize clock */
    timetick = 1.0 / (double)(sysconf(_SC_CLK_TCK));
    first = 0;
    gettimeofday(&tp, &tzp);
    tstart = (double)tp.tv_sec + (double)tp.tv_usec * 1.0e-6;
    /* Initialize CPU time */
    i = times(&cpu);
    ucpustart = ((double)(cpu.tms_utime + cpu.tms_cutime)) * timetick;
    scpustart = ((double)(cpu.tms_stime + cpu.tms_cstime)) * timetick;
    /* Return values */
    *elapsed = 0.0e0;
    *ucpu = 0.0e0;
    *scpu = 0.0e0;
  }
  else  {
    /* Get clock time */
    gettimeofday(&tp, &tzp);
    *elapsed = (double)tp.tv_sec + (double)tp.tv_usec * 1.0e-6 - tstart;
    /* Get CPU time */
    i=times(&cpu);
    *ucpu = ((double)(cpu.tms_utime + cpu.tms_cutime)) * timetick - ucpustart;
    *scpu = ((double)(cpu.tms_stime + cpu.tms_cstime)) * timetick - scpustart; 
  }
  return;
}

int main(int argc, char **argv){
  int primos[N];
  int i,j;
  
  for(i = 0; i < N/2; i++)
    primos[i] = 0;
    
  double laps,user,syst;
  srand(time(NULL));
  ctimer(&laps,&user,&syst);
    
  primos[0] = 2;
  
  for(i = 3; i < N; i++){
    for(j = 0; j < N/2; j++){
      if(i % primos[j] == 0)
        break;
      if(primos[j+1] == 0){
        primos[j+1] = i;
        break;
      }
    }
  }
  ctimer(&laps,&user,&syst);
  printf("Enlapsed=%f User=%f System=%f\n", laps,user,syst);
  
  return 0;
}