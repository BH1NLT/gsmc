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
#include <math.h>
#include <unistd.h>
#include "main.h"
#include "calc.h"
#include "draw.h"
#include "autotune.h"


extern SMCDATA smcdata;
extern GtkWidget *scarea;
extern float tunestep;

#define MAXAUTOITER 50

float weigthfu(void){
  int i;
  float m, qm=0., q;
  const float p=0.6;

    m=sqrt((smcdata.rerhoIP[smcdata.ne]*smcdata.rerhoIP[smcdata.ne])+
	   (smcdata.imrhoIP[smcdata.ne]*smcdata.imrhoIP[smcdata.ne]));
 
    for(i=0; i<smcdata.ne; i++){
      q=fabs(smcdata.imrhoIP[i]/smcdata.rerhoIP[i]);
      qm+=q*q;
    }    

  return p*m+(1-p)*sqrt(qm);
}

/* Very simple "screwdriver" strategy for tuning, i.e.: adjust (aka rotate)
   each value and then pass to the next and so on restarting from the first
   if the result is not good enough. Algritm stops when error function 
   cannot go lower; error function is a weighted mean of matching and
   lowered Q.

   I don't think this could be a good algorithm, but it work in most case and
   is usefull if some manual adjust are taken; multiple invoking of autotune
   can be usefull as well as tunestep varying.

   TO BE IMPROVED!
*/
void autotune(void){
  int i, j;
  float r, oldr;

  for(i=1; i<=smcdata.ne; i++){
    if(smcdata.ELlocked[i]==1)
      continue;

    r=sqrt((smcdata.rerhoIP[smcdata.ne]*smcdata.rerhoIP[smcdata.ne])+
	   (smcdata.imrhoIP[smcdata.ne]*smcdata.imrhoIP[smcdata.ne]));
    for(j=0; j<MAXAUTOITER; j++){
      oldr=r;
      smcdata.ELval0[i]*=tunestep;
      recalc();
      r=sqrt((smcdata.rerhoIP[smcdata.ne]*smcdata.rerhoIP[smcdata.ne])+
	     (smcdata.imrhoIP[smcdata.ne]*smcdata.imrhoIP[smcdata.ne]));
      if(r>oldr){
	smcdata.ELval0[i]/=tunestep;
	break;
      }
    }
    
    oldr=r;
    for(j=0; j<MAXAUTOITER; j++){
      oldr=r;
      smcdata.ELval0[i]/=tunestep;
      recalc();
      r=sqrt((smcdata.rerhoIP[smcdata.ne]*smcdata.rerhoIP[smcdata.ne])+
	     (smcdata.imrhoIP[smcdata.ne]*smcdata.imrhoIP[smcdata.ne]));
      if(r>oldr){
	smcdata.ELval0[i]*=tunestep;
	break;
      }
    }
  }
  
  drawchart(); gtk_widget_draw(scarea, NULL);
}
