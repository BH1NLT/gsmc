/*
 *   GSMC - A GTK Smith Chart Calulator for RF impedance matching
 * 
 *	(c) by Lapo Pieri IK5NAX  2003-2016,
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Send bugs reports, comments, critique, etc, to
 *
 *        lapo_CHANGE_THIS_AT_CHANGE_THIS_radioteknos.it
 *        ik5nax_CHANGE_THIS_AT_CHANGE_THIS_radioteknos.it
 */


#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "dataconv.h"
#include "calc.h"

int entrystringdecode(char *is, float *re, float *im){
  int i, fn, err;
  char *b;

  /* Remove initial spaces, it will be usefull later */
  b=is; while(b[0]==' ' || b[0]=='\t') b++;
  
  err=0;
  for(i=0; i<(int)strlen(b); i++) {
    /* If a 'j' is present than swap it with the sign and replace it
       with blank, e.g.: "12+j56" --> "12 +56" */ 
    if(b[i]=='j' || b[i]=='i') {
      /* Except if 'j' is the first character as in "j78.34" */
      if(i!=0) {
	b[i]=b[i-1]; b[i-1]=' '; }
      else
	b[0]=' ';
      /* The string is ready to be read by a scanf, but if only one field
	 is found this mean that it's a pure immaginary number */
      if(sscanf(b, "%f %f", re, im)<2) { *im=*re; *re=0; }
      break;
    }
  }
  /* If no 'j' are found there may be one or more field separated by
     space or tab*/
  if(i==(int)strlen(b)) {
    if(strlen(b)==0) 
      err=1;
    else 
      fn=1;
    
    /* Search for field delimiter */
    for(i=0; i<(int)strlen(b); i++) {
      /* To do better: to detect multiple spaces or tab or trailing ones */
      if(b[i]==' ') fn++;
    }
    
    switch(fn) {
    case 1:                                 /* Single field, a real number */
      if(sscanf(b, "%f", re)==0)
	err=1;
      *im=0; break;
      
    case 2:                             /* Complex number in polar format */ 
      if(sscanf(b, "%f %f", re, im)==0)
	err=1;
      polar2rect(re, im); break;
      
    case 3:                         /* In futuro per le linee (Z0, E, Att) */
      break;
      
    default:
      err=1;
    }    
  }
  
  return err;
}

/* Decode values of component from string received by dialogwin() */
int componentvaluestringdecode(char *is, float *val0, float *val1,
			       float *val2, char *place) {
  int ni;
  char f0[10], f1[10], f2[10], f3[10];

  ni=sscanf(is, "%s %s %s %s", f0, f1, f2, f3);
  switch(ni) {
  case 0: fprintf(stderr, "Invalid component value\n"); return 1;

  case 1: *place=' '; *val0=psdec(f0); *val1=0.; *val2=0.; break;

  case 2: *place=f1[0]; *val0=psdec(f0); *val1=0.; *val2=0.; break;

  case 3: *place=f2[0]; *val0=psdec(f0); *val1=psdec(f1); *val2=0; break;

  default: fprintf(stderr, "Invalid component value\n"); return 1;
  }

  return 0;
}

/* Decode float value with unit of measure prefix (but not unit of measure
   symbol) */
float psdec(char *b) {
  int i;
  float pf=1., v;
  char b2[40];

  for(i=0; i<(int)strlen(b); i++)
    if((b[i]>'9' || b[i]<'0') && b[i]!='.' && b[i]!='-' && b[i]!='+') break;
  
  if(i!=(int)strlen(b))
    switch(b[i]) {
    case 'f': pf=1.e-15; break;
    case 'p': pf=1.e-12; break;
    case 'n': pf=1.e-9; break;
    case 'u': pf=1.e-6; break;
    case 'm': pf=1.e-3; break;
    case 'k': pf=1.e+3; break;
    case 'M': pf=1.e+6; break;
    case 'G': pf=1.e+9; break;
    case 'T': pf=1.e+12; break;
    case ' ': pf=1.; break;
    default: fprintf(stderr, "Invalid prefix (%c)\n", b[i]); return(0.);
    }

  strncpy(b2, b, i+2);
  sscanf(b2, "%f", &v);

  return (v*pf);
}
