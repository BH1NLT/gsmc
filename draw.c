/*
 *   GSMC - A GTK Smith Chart Calulator for RF impedance matching
 * 
 *	(c) by Lapo Pieri IK5NAX  2003-2004,
 *          and Johannes van der Horst
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
 *  Send bugs reports, comments, critique, etc, to ik5nax@amsat.org or
 *  lapo_pieri@virgilio.it
 */


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <ctype.h>
#include "main.h"
#include "calc.h"
#include "draw.h"

extern SMCDATA smcdata;
extern GtkWidget *statarr[20];
extern const char statlabelname[20][15];
extern GtkTextBuffer *txtcircbuff;
extern GtkWidget *maindrawingwidget, *win_main, *textcircdesc, *scarea;
extern GdkPixmap *mainpixmap;
extern gint scdim;
GdkGC *impgc, *admgc, *qgc, *rhogc, *spgc, *arcgc;

extern int Zcirc, Ycirc, RHOcirc, Qcirc;
extern float tunestep, freqstep;

void setup_gcs(void) {

  /* Constant impedance circle */
  impgc=gdk_gc_new(mainpixmap);
  gdk_rgb_gc_set_foreground(impgc, 0xff0000);

  /* Constant admittance circle */
  admgc=gdk_gc_new(mainpixmap);
  gdk_rgb_gc_set_foreground(admgc, 0x0000ff);

  /* Constant Q circle */
  qgc=gdk_gc_new(mainpixmap);
  gdk_rgb_gc_set_foreground(qgc, 0x00ff00);

  /* Constant refl coeff circle */
  rhogc=gdk_gc_new(mainpixmap);
  gdk_rgb_gc_set_foreground(rhogc, 0x808080);

  /* Start point */
  spgc=gdk_gc_new(mainpixmap);
  gdk_rgb_gc_set_foreground(spgc, 0xf000f0);

  /* Arcs */
  arcgc=gdk_gc_new(mainpixmap);
  gdk_rgb_gc_set_foreground(arcgc, 0x000000);
  gdk_gc_set_line_attributes(arcgc, 2, GDK_LINE_SOLID,
			    GDK_CAP_ROUND, GDK_JOIN_ROUND);
}

void drawGk(float G) {
  float xc, yc, r;
  int x, y, w, h;

  xc=-G/(1.+G); yc=0; r=1./(1.+G);
  x=(int)((xc+1.)/2.*scdim); y=(int)((1-yc)/2.*scdim);
  w=(int)(r*scdim); h=(int)(r*scdim);

  gdk_draw_arc(mainpixmap,
	       admgc, FALSE,
	       x-w/2, y-h/2, w, h,
               0, 23040);
}

void drawRk(float R) {
  float xc, yc, r;
  int x, y, w, h;

  xc=R/(1.+R); yc=0; r=1./(1.+R);
  x=(int)((xc+1.)/2.*scdim); y=(int)((1-yc)/2.*scdim);
  w=(int)(r*scdim); h=(int)(r*scdim);

  gdk_draw_arc(mainpixmap,
	       impgc, FALSE,
	       x-w/2, y-h/2, w, h,
               0, 23040);
}


void drawXk(float X) {
  float xc, yc, r;
  int x, y, w, h;

  if(X!=0.) {
    xc=1.; yc=1./X; r=1./fabs(X);
    
    x=(int)((xc+1.)/2.*scdim); y=(int)((1-yc)/2.*scdim);
    w=(int)(r*scdim); h=(int)(r*scdim);
    if(X<0.)
      gdk_draw_arc(mainpixmap,
		   impgc, FALSE,
		   x-w/2, y-h/2, w, h,
		   90*64, (int)(-64.*2.*180./M_PI*atan(X)));
    else
      gdk_draw_arc(mainpixmap,
		   impgc, FALSE,
		   x-w/2, y-h/2, w, h,
		   -90*64, (int)(-64.*2.*180./M_PI*atan(X)));
  }
}

void drawBk(float B) {
  float xc, yc, r;
  int x, y, w, h;

  if(B!=0.) {
    xc=-1.; yc=1./B; r=1./fabs(B);    

    x=(int)((xc+1.)/2.*scdim); y=(int)((1-yc)/2.*scdim);
    w=(int)(r*scdim); h=(int)(r*scdim);
    if(B<0.)
      gdk_draw_arc(mainpixmap,
		   admgc, FALSE,
		   x-w/2, y-h/2, w, h,
		   90*64, (int)(64.*2.*180./M_PI*atan(B)));

    else
      gdk_draw_arc(mainpixmap,
		   admgc, FALSE,
		   x-w/2, y-h/2, w, h,
		   -90*64, (int)(64.*2.*180./M_PI*atan(B)));
  }
}


void drawRHOk(float RHO) {
  float xc, yc, r;
  int x, y, w, h;

  xc=0; yc=0; r=RHO;
  x=(int)((xc+1.)/2.*scdim); y=(int)((1-yc)/2.*scdim);
  w=(int)(r*scdim); h=(int)(r*scdim);

  gdk_draw_arc(mainpixmap,
	       rhogc, FALSE,
	       x-w/2, y-h/2, w, h,
               0, 23040);
}

void drawQk(float Q) {
  float xc, yc, r;
  int x, y, w, h;

  xc=0.; yc=1./Q; r=sqrt(1.+1./Q/Q);
  x=(int)((xc+1.)/2.*scdim); y=(int)((1-yc)/2.*scdim);
  w=(int)(r*scdim); h=(int)(r*scdim);
  
  gdk_draw_arc(mainpixmap,
	       qgc, FALSE,
	       x-w/2, y-h/2, w, h,
             (int)(64*(-90+180/M_PI*atan(Q))),(int)(-64*(180/M_PI*2*atan(Q))));
  
  xc=0.; yc=-1./Q; r=sqrt(1.+1./Q/Q);
  x=(int)((xc+1.)/2.*scdim); y=(int)((1-yc)/2.*scdim);
  w=(int)(r*scdim); h=(int)(r*scdim);

  gdk_draw_arc(mainpixmap,
	       qgc, FALSE,
	       x-w/2, y-h/2, w, h,
               (int)(64*(90-180/M_PI*atan(Q))),(int)(64*(180/M_PI*2*atan(Q))));
}

void drawSP(float rerho, float imrho) {
  int x, y;
  
  x=(int)((rerho+1.)/2.*scdim); y=(int)((1.-imrho)/2.*scdim);

  gdk_draw_arc(mainpixmap,
	       spgc, TRUE, x-2, y-2, 4, 4, 0, 23040);
}



void drawchart(void) {
  int i, n, step;
  float R, X, G, B, sa, ea, a;
  float rer, imr, rer1, imr1, rer2, imr2;

  gdk_draw_rectangle (mainpixmap,
                      scarea->style->white_gc,
                      TRUE,
                      0, 0,
                      scarea->allocation.width,
                      scarea->allocation.height);
  
  gdk_draw_arc(mainpixmap,
	       scarea->style->black_gc, FALSE, 0, 0,
	       scdim, scdim,
	       0, 23040);

  gdk_draw_line(mainpixmap,
		scarea->style->black_gc,
		0, scarea->allocation.width/2, 
		scdim, scdim/2);
  

  gdk_draw_rectangle(mainpixmap,
		     scarea->style->white_gc,
		     TRUE,
		     0, 0,
		     scdim, scdim);
  
  gdk_draw_arc(mainpixmap,
	       scarea->style->black_gc, FALSE, 0, 0,
	       scdim, scdim,
	       100, 36000);
  
  gdk_draw_line(mainpixmap,
		scarea->style->black_gc,
		0, scdim/2, scdim, scdim/2);
  
  
  if(Zcirc) {
    drawRk(0.2); drawRk(0.5); drawRk(1); drawRk(2);
    drawXk(0.2); drawXk(0.5); drawXk(1); drawXk(2);
    drawXk(-0.2); drawXk(-0.5); drawXk(-1); drawXk(-2);
  }

  if(Ycirc) {
    drawGk(0.2); drawGk(0.5); drawGk(1); drawGk(2);
    drawBk(0.2); drawBk(0.5); drawBk(1); drawBk(2);
    drawBk(-0.2); drawBk(-0.5); drawBk(-1); drawBk(-2);
  }

  if(RHOcirc) {
    drawRHOk(0.25); drawRHOk(0.5); drawRHOk(0.75);
  }

  if(Qcirc) {
    drawQk(0.5); drawQk(1); drawQk(2); drawQk(5);
  }


  drawSP(smcdata.rerhoSP, smcdata.imrhoSP);

 /* Draw arcs */
 for(i=1; i<=smcdata.ne; i++) {

   /* Series placed element, that could be an R, L, or C */
     switch(smcdata.ELplace[i]) {
     case 's':
       rho2z(smcdata.rerhoIP[i], smcdata.imrhoIP[i], &R, &X);
       if(smcdata.ELtype[i]=='r') {
	 if(fabs(X)>QZ) {
	   sa=thetaonX(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], X)*180/M_PI;
	   ea=thetaonX(smcdata.rerhoIP[i], smcdata.imrhoIP[i], X)*180/M_PI;
	   if(ea<0.) ea+=360; if(sa<0.) sa+=360;
	   drawX(X, sa, ea-sa);
	 }
	 else
	   drawX(X, smcdata.rerhoIP[i-1], smcdata.rerhoIP[i]);
       }
       else if(smcdata.ELtype[i]=='l' || smcdata.ELtype[i]=='c') {
	 sa=thetaonR(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], R)*180/M_PI;
	 ea=thetaonR(smcdata.rerhoIP[i], smcdata.imrhoIP[i], R)*180/M_PI;
	 if(ea<0.) ea+=360; if(sa<0.) sa+=360;
	 drawR(R, sa, ea-sa);
       }
       break;

       /* Parallel placed element, that could be an R, L, or C */
       case'p':
	 rho2y(smcdata.rerhoIP[i], smcdata.imrhoIP[i], &G, &B);
       if(smcdata.ELtype[i]=='r') {
	 if(fabs(B)>QZ) {
	   sa=thetaonB(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], B)*180/M_PI;
	   ea=thetaonB(smcdata.rerhoIP[i], smcdata.imrhoIP[i], B)*180/M_PI;
	   drawB(B, sa, ea-sa);
	 }
	 else 
	   drawB(B, smcdata.rerhoIP[i-1], smcdata.rerhoIP[i]);
	 
       }
       else if(smcdata.ELtype[i]=='l' || smcdata.ELtype[i]=='c') {
	 sa=thetaonG(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], G)*180/M_PI;
	 ea=thetaonG(smcdata.rerhoIP[i], smcdata.imrhoIP[i], G)*180/M_PI;
	 
	 drawG(G, sa, ea-sa);
       }
       break;
       
     /* The rest are for transmission line */
     case 't':
       rer1=smcdata.rerhoIP[i-1]; imr1=smcdata.imrhoIP[i-1];
       step=36*(1+(int)(smcdata.ELval0[i]/360));
       a=smcdata.ELval0[i]/180.*M_PI/step;
       for(n=0; n<step; n++) {
	 rhoz1z2(&rer1, &imr1, smcdata.z0, smcdata.ELval1[i]);
	 rer=rer1; imr=imr1;
	 rer2=rer*cos(2.*a)+imr*sin(2.*a);
	 imr2=-rer*sin(2.*a)+imr*cos(2.*a);
	 rhoz1z2(&rer1, &imr1, smcdata.ELval1[i], smcdata.z0);
	 rhoz1z2(&rer2, &imr2, smcdata.ELval1[i], smcdata.z0);
	 drawLINE(rer1, imr1, rer2, imr2);
	 rer1=rer2; imr1=imr2;
       }
       break;
     case 'c': 
     case 'o':
	 rho2y(smcdata.rerhoIP[i], smcdata.imrhoIP[i], &G, &B);       
	 sa=thetaonG(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], G)*180/M_PI;
	 ea=thetaonG(smcdata.rerhoIP[i], smcdata.imrhoIP[i], G)*180/M_PI;
	 drawG(G, sa, ea-sa);
       break;
     case 'l':
     case 'a':
       rho2z(smcdata.rerhoIP[i], smcdata.imrhoIP[i], &R, &X);
       sa=thetaonR(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], R)*180/M_PI;
       ea=thetaonR(smcdata.rerhoIP[i], smcdata.imrhoIP[i], R)*180/M_PI;
       if(ea<0.) ea+=360; if(sa<0.) sa+=360;
       drawR(R, sa, ea-sa);
       break;
       
     default:
       fprintf(stderr, "Unallowed connection\n");
     }
 }

}

void drawR(float R, float sa, float al) {

  float xc, yc, r;
  int x, y, w, h;

  xc=R/(1.+R); yc=0; r=1./(1.+R);
  x=(int)((xc+1.)/2.*scdim); y=(int)((1.-yc)/2.*scdim);
  w=(int)(r*scdim); h=(int)(r*scdim);

  gdk_draw_arc(mainpixmap, arcgc, FALSE, 
	       x-w/2, y-h/2, w, h, sa*64, al*64);
}


void drawX(float X, float sa, float al) {
  float xc, yc, r;
  int x, y, w, h;

  if(fabs(X)>QZ) {
    xc=1.; yc=1./X; r=1./fabs(X);
    x=(int)((xc+1.)/2.*scdim); y=(int)((1.-yc)/2.*scdim);
    w=(int)(r*scdim); h=(int)(r*scdim);

    gdk_draw_arc(mainpixmap, arcgc, FALSE, 
		 x-w/2, y-h/2, w, h, sa*64, al*64);
  }

  else {
    yc=X/2.; y=(int)((1.-yc)/2.*scdim);
    yc=0; h=(int)((1.-yc)/2.*scdim);
    xc=sa; x=(int)((xc+1.)/2.*scdim);
    xc=al; w=(int)((xc+1.)/2.*scdim);
    gdk_draw_line(mainpixmap, arcgc, x, y, w, h);
  }

}

void drawG(float G, float sa, float al){
  float xc, yc, r;
  int x, y, w, h;

  xc=-G/(1.+G); yc=0; r=1./(1.+G);
  x=(int)((xc+1.)/2.*scdim); y=(int)((1.-yc)/2.*scdim);
  w=(int)(r*scdim); h=(int)(r*scdim);


  gdk_draw_arc(mainpixmap, arcgc, FALSE, 
	       x-w/2, y-h/2, w, h, sa*64, al*64);
}


void drawB(float B, float sa, float al) {
  float xc, yc, r;
  int x, y, w, h;


  if(fabs(B)>QZ) {
    xc=-1.; yc=-1./B; r=1./fabs(B);
    x=(int)((xc+1.)/2.*scdim); y=(int)((1.-yc)/2.*scdim);
    w=(int)(r*scdim); h=(int)(r*scdim);
    
    gdk_draw_arc(mainpixmap, arcgc, FALSE, 
		 x-w/2, y-h/2, w, h, sa*64, al*64);
  }

  else {
    yc=-B/2.; y=(int)((1.-yc)/2.*scdim);
    yc=0; h=(int)((1.-yc)/2.*scdim);
    xc=sa; x=(int)((xc+1.)/2.*scdim);
    xc=al; w=(int)((xc+1.)/2.*scdim);

    gdk_draw_line(mainpixmap, arcgc, x, y, w, h);
  }
}


void drawRHO(float RHO, float sa, float ea) {
  float xc, yc, r;
  int x, y, w, h;

  xc=0; yc=0; r=RHO;
  x=(int)((xc+1.)/2.*scdim); y=(int)((yc+1.)/2.*scdim);
  w=(int)(r*scdim); h=(int)(r*scdim);

  gdk_draw_arc(mainpixmap, arcgc, FALSE, 
	   x-w/2, y-h/2, w, h, sa*64, ea*64);

}

void drawLINE(float rerhoi, float imrhoi, float rerhof, float imrhof) {
  int x1, y1a, x2, y2;

  x1=(int)((rerhoi+1.)/2.*scdim); y1a=(int)((1.-imrhoi)/2.*scdim);
  x2=(int)((rerhof+1.)/2.*scdim); y2=(int)((1.-imrhof)/2.*scdim);

  gdk_draw_line(mainpixmap, arcgc, x1, y1a, x2, y2);
}



void showtextstat(void) {
  char b[80], b2[80];
  float r, j;

  /* Display general data */
  float2prefs(smcdata.z0, b2, 0); sprintf(b, statlabelname[labelZ0], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelZ0]), b);

  float2prefs(smcdata.f0, b2, 0); sprintf(b, statlabelname[labelf0], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelf0]), b);

  /* Start point */
  rho2z(smcdata.rerhoIP[0], smcdata.imrhoIP[0], &r, &j);
  r*=smcdata.z0; j*=smcdata.z0;
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);
  strcpy(b2, b); sprintf(b, statlabelname[labelzSP], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelzSP]), b);

  rho2y(smcdata.rerhoIP[0], smcdata.imrhoIP[0], &r, &j);
  r/=smcdata.z0; j/=smcdata.z0;
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);
  strcpy(b2, b); sprintf(b, statlabelname[labelySP], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelySP]), b);

  r=smcdata.rerhoIP[0]; j=smcdata.imrhoIP[0];
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);
  strcpy(b2, b); sprintf(b, statlabelname[labelrhoSP], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelrhoSP]), b);

  r=sqrt((smcdata.rerhoIP[0]*smcdata.rerhoIP[0])+
	 (smcdata.imrhoIP[0]*smcdata.imrhoIP[0]));
  sprintf(b, " %5.2f", (1.+r)/(1.-r));
  strcpy(b2, b); sprintf(b, statlabelname[labelSWRSP], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelSWRSP]), b);


  /* End point */
  rho2z(smcdata.rerhoIP[smcdata.ne], smcdata.imrhoIP[smcdata.ne], &r, &j);
  r*=smcdata.z0; j*=smcdata.z0;
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);
  strcpy(b2, b); sprintf(b, statlabelname[labelzEP], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelzEP]), b);

  rho2y(smcdata.rerhoIP[smcdata.ne], smcdata.imrhoIP[smcdata.ne], &r, &j);
  r/=smcdata.z0; j/=smcdata.z0;
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);  
  strcpy(b2, b); sprintf(b, statlabelname[labelyEP], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelyEP]), b);

  r=smcdata.rerhoIP[smcdata.ne]; j=smcdata.imrhoIP[smcdata.ne];
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);
  strcpy(b2, b); sprintf(b, statlabelname[labelrhoEP], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelrhoEP]), b);

  r=sqrt((smcdata.rerhoIP[smcdata.ne]*smcdata.rerhoIP[smcdata.ne])+
	 (smcdata.imrhoIP[smcdata.ne]*smcdata.imrhoIP[smcdata.ne]));
  sprintf(b, "%5.2f", (1.+r)/(1.-r));
  strcpy(b2, b); sprintf(b, statlabelname[labelSWREP], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelSWREP]), b);

  /* Other data */
  sprintf(b2, "%3.0f", (tunestep-1.)*100.);
  sprintf(b, statlabelname[labeltunestep], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labeltunestep]), b);
  sprintf(b2, "%3.1f", freqstep*100.);
  sprintf(b, statlabelname[labelfreqstep], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelfreqstep]), b);

  /* Cursor point */
  rho2z(smcdata.rerhoCU, smcdata.imrhoCU, &r, &j);
  r*=smcdata.z0; j*=smcdata.z0;
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);
  strcpy(b2, b); sprintf(b, statlabelname[labelzCU], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelzCU]), b);

  rho2y(smcdata.rerhoCU, smcdata.imrhoCU, &r, &j);
  r/=smcdata.z0; j/=smcdata.z0;
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);  
  strcpy(b2, b); sprintf(b, statlabelname[labelyCU], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelyCU]), b);

  r=smcdata.rerhoCU; j=smcdata.imrhoCU;
  float2prefs(r, b, 0); sprintf(b2, "%cj", j>0. ? '+' : '-'); strcat(b, b2);
  float2prefs(fabs(j), b2, 0); strcat(b, b2);
  strcpy(b2, b); sprintf(b, statlabelname[labelrhoCU], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelrhoCU]), b);

  r=sqrt((smcdata.rerhoCU*smcdata.rerhoCU)+
	 (smcdata.imrhoCU*smcdata.imrhoCU));
  sprintf(b, "%5.2f", (1.+r)/(1.-r));
  strcpy(b2, b); sprintf(b, statlabelname[labelSWRCU], b2);
  gtk_label_set_text(GTK_LABEL(statarr[labelSWRCU]), b);

}

void showcircdesc(void) {
  int i;
  char b[40], t[80], b2[3];
  GtkTextIter dummy, tep;

  gtk_text_buffer_set_text(txtcircbuff, "\0", -1);
  gtk_text_buffer_get_bounds(txtcircbuff, &dummy, &tep);

  for(i=1; i<=smcdata.ne; i++) {

    switch(smcdata.ELtype[i]) {
    case 'r':
    case 'l':
    case 'c':
      float2prefs(smcdata.ELval0[i], b, 0);
      sprintf(t, "#%d:   %c   %c   %s %s\n",  i, toupper(smcdata.ELtype[i]),
	      smcdata.ELplace[i], b, smcdata.ELlocked[i]==1?"  - locked -":"");
      if(i==smcdata.neidx){
	gtk_text_buffer_insert_with_tags_by_name(GTK_TEXT_BUFFER(txtcircbuff),
			       &tep,
			       t, -1, "highlighted", NULL);
	gtk_text_buffer_get_bounds(txtcircbuff, &dummy, &tep);
      }

      else{
	gtk_text_buffer_insert(GTK_TEXT_BUFFER(txtcircbuff),
			       &tep,
			       t, -1);
	gtk_text_buffer_get_bounds(txtcircbuff, &dummy, &tep);
      }

      break;
    case 't':
      float2prefs(smcdata.ELval0[i], b, 0);
      exptlpln(smcdata.ELplace[i], b2);
      sprintf(t, "#%d:   %c   %s   %s", i, toupper(smcdata.ELtype[i]),
	      b2, b); 
      float2prefs(smcdata.ELval1[i], b, 0); strcat(t, b); 
      if(smcdata.ELlocked[i]==1)
	strcat(t,"  - locked -");
      strcat(t, "\n");
      if(i==smcdata.neidx){
	gtk_text_buffer_insert_with_tags_by_name(GTK_TEXT_BUFFER(txtcircbuff),
					 &tep,
					 t, -1, "highlighted", NULL);
	gtk_text_buffer_get_bounds(txtcircbuff, &dummy, &tep);
      }

      else{
	gtk_text_buffer_insert_with_tags(GTK_TEXT_BUFFER(txtcircbuff),
					 &tep,
					 t, -1, NULL);
	gtk_text_buffer_get_bounds(txtcircbuff, &dummy, &tep);
      }

      break;

    }

  }

}

void exptlpln(char ep, char *rv){

  switch(ep){
  case 't': strcpy(rv, "tr"); break;
  case 'a': strcpy(rv, "os"); break;
  case 'l': strcpy(rv, "ss"); break;
  case 'o': strcpy(rv, "op"); break;
  case 'c': strcpy(rv, "sp"); break;
  }

}


