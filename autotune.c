#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <math.h>
#include "main.h"
#include "autotune.h"
#include <unistd.h>


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
  float r, oldr, wf;

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
