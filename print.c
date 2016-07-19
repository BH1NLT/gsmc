/*
 *   GSMC - A GTK Smith Chart Calulator for RF impedance matching
 * 
 *	(c) by Lapo Pieri IK5NAX  2003-2016,
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
#include <time.h>
#include "main.h"
#include "draw.h"
#include "calc.h"
#include "print.h"
// jvdh 2004-3-30

extern SMCDATA smcdata;
extern int Zcirc, Ycirc, RHOcirc, Qcirc;

float psdim=510.236; /* chart diameter=180mm */

void psdrawchart(FILE *f){
  int i, n, step;
  float R, X, G, B, sa, ea, a;
  float rer, imr, rer1, imr1, rer2, imr2;

  fprintf(f, "\n%%Chart border and j=0 line\nnewpath\n0 setlinewidth\n");
  psdrawArc(f, psdim/2, psdim/2, psdim, psdim, 0, 360);
  psdrawLine(f, -1., 0., 1., 0.);
  fprintf(f, "0. 0. 0. setrgbcolor\nstroke\n");

  if(Zcirc) {
    fprintf(f, "\n%%Constant Impedance circle\nnewpath\n");
    psdrawRk(f, 0.2); psdrawRk(f, 0.5); psdrawRk(f, 1); psdrawRk(f, 2);
    psdrawXk(f, 0.2); psdrawXk(f, 0.5);  psdrawXk(f, 1);  psdrawXk(f, 2);
    psdrawXk(f, -0.2); psdrawXk(f, -0.5); psdrawXk(f, -1); psdrawXk(f, -2);
    fprintf(f, "stroke\n");
  }

  if(Ycirc) {
    fprintf(f, "\n%%Constant Admittance circle\nnewpath\n");
    psdrawGk(f, 0.2); psdrawGk(f, 0.5); psdrawGk(f, 1); psdrawGk(f, 2);
    psdrawBk(f, 0.2); psdrawBk(f, 0.5); psdrawBk(f, 1); psdrawBk(f, 2);
    psdrawBk(f, -0.2); psdrawBk(f, -0.5); psdrawBk(f, -1); psdrawBk(f, -2);
    fprintf(f, "stroke\n");
  }


  if(RHOcirc) {
    fprintf(f, "\n%%Constant Coefficient of Reflection circle\nnewpath\n");
    psdrawRHOk(f, 0.25); psdrawRHOk(f, 0.5); psdrawRHOk(f, 0.75);
  }

  if(Qcirc) {
    fprintf(f, "\n%%Constant Q circle\nnewpath\n");
    psdrawQk(f, 0.5); psdrawQk(f, 1); psdrawQk(f, 2); psdrawQk(f, 5);
  }

  /* Start point */
  fprintf(f, "\n%%Start Point\nnewpath\n");
  psdrawSP(f, smcdata.rerhoSP, smcdata.imrhoSP);

}

void psdrawArc(FILE* f, float x0,float y0,float w,float h,float a0,float a1){
  if (w==h) {
    fprintf(f,"%.2f %.2f %.2f %.2f %.2f arc",x0,y0,w/2.,a0,a1);
    if (a0>a1) {
      fprintf(f,"n");         //clockwise arc - default is anticlockwise
    }
    fprintf(f,"\n");
  }
  else {        //ellipse!!
  }
}

void psdrawLine(FILE* f,float rerhoi, float imrhoi,
		float rerhof, float imrhof) {
  float x1, y1, x2, y2;

  x1=(rerhoi+1.)/2.*psdim; y1=(1.+imrhoi)/2.*psdim;
  x2=(rerhof+1.)/2.*psdim; y2=(1.+imrhof)/2.*psdim;
  fprintf(f,"%.2f %.2f moveto\n",x1,y1);
  fprintf(f,"%.2f %.2f lineto\n",x2,y2); 
}

void psdrawX(FILE* f, float X, float sa, float al) {
  float xc, yc, r;
  float x, y, w, h;

  if(fabs(X)>QZ) {
    xc=1.; yc=1./X; r=1./fabs(X);
    x=((xc+1.)/2.*psdim); y=((1.+yc)/2.*psdim);//#
    w=(r*psdim); h=(r*psdim);
    psdrawArc(f,x, y, w, h, sa, al);
  }
  
  else {
    yc=X/2.; y=((1.-yc)/2.*psdim);
    yc=0; h=((1.-yc)/2.*psdim);
    xc=sa; x=((xc+1.)/2.*psdim);
    xc=al; w=((xc+1.)/2.*psdim);
    fprintf(f,"%.2f %.2f moveto\n",x,y);
    fprintf(f,"%.2f %.2f lineto\n",w,h);
  }
}


void psdrawRk(FILE* f, float R) {
  float xc, yc, r;
  float x, y, w, h;
  
  xc=R/(1.+R); yc=0; r=1./(1.+R);
  x=((xc+1.)/2.*psdim); y=((1.-yc)/2.*psdim);
  w=(r*psdim); h=(r*psdim);
  
  fprintf(f, "%.2f %.2f moveto\n", x+w/2., y);
  fprintf(f, "%.2f %.2f %.2f 0.00 360.00 arc\n", x, y, w/2.);
  fprintf(f, "1.0 0. 0. setrgbcolor\n");
  fprintf(f, "/Helvetica findfont\n8 scalefont\nsetfont\n");
  fprintf(f, "%.2f %.2f moveto\n", x-h/2.+2., (float)(psdim/2.-8.));
  fprintf(f, "1.0 0. 0. setrgbcolor\n(%.1f) show\n", R*smcdata.z0);
}

void psdrawXk(FILE* f, float X){
  float xc, yc, r;
  float x, y, w, h;

  if(X!=0.) {
    xc=1.; yc=1./X; r=1./fabs(X);
    
    x=(int)((xc+1.)/2.*psdim); y=(int)((1-yc)/2.*psdim);
    w=(int)(r*psdim); h=(int)(r*psdim);
    if(X<0.){
      fprintf(f, "%.2f %.2f moveto\n", (float)psdim, (float)psdim/2.);
      fprintf(f, "%.2f %.2f %.2f %.2f %.2f arc\n",
	      x, y-h, h/2., 90., -2.*180./M_PI*atan(X)+90.);
    }
    else{
      fprintf(f, "%.2f %.2f moveto\n", (float)psdim, (float)psdim/2.);
      fprintf(f, "%.2f %.2f %.2f %.2f %.2f arcn\n",
	      x, y+h, h/2., 270., -2.*180./M_PI*atan(X)+270.);
      fprintf(f, "/Helvetica findfont\n8 scalefont\nsetfont\n");
      fprintf(f, "%.2f %.2f rmoveto\n", fabs(X)<1.?-24.:+6., +8.);
      fprintf(f, "(+j%.1f) show\n", X*smcdata.z0);

    }
  }
}


void psdrawGk(FILE* f, float G){
  float xc, yc, r;
  float x, y, w, h;

  xc=-G/(1.+G); yc=0; r=1./(1.+G);
  x=(int)((xc+1.)/2.*psdim); y=(int)((1.-yc)/2.*psdim);
  w=(int)(r*psdim); h=(int)(r*psdim);

  fprintf(f, "%.2f %.2f moveto\n", x-w/2., y);
  fprintf(f, "%.2f %.2f %.2f 0.00 360.00 arc\n", x, y, w/2.);
  fprintf(f, "0. 0. 1.0 setrgbcolor\n");
  fprintf(f, "/Helvetica findfont\n8 scalefont\nsetfont\n");
  fprintf(f, "%.2f %.2f moveto\n", x+h/2.-25., (float)(psdim/2.+2.));
  fprintf(f, "0. 0. 1.0 setrgbcolor\n(%.3f) show\n", G/smcdata.z0);
}


void psdrawBk(FILE* f,float B) {
  float xc, yc, r;
  float x, y, w, h;

  if(B!=0.) {
    xc=-1.; yc=1./B; r=1./fabs(B);
    
    x=(int)((xc+1.)/2.*psdim); y=(int)((1-yc)/2.*psdim);
    w=(int)(r*psdim); h=(int)(r*psdim);
    if(B<0.){
      fprintf(f, "%.2f %.2f moveto\n", 0., (float)psdim/2.);
      fprintf(f, "%.2f %.2f %.2f %.2f %.2f arcn\n",
	      x, y-h, h/2., 90., -2.*180./M_PI*atan(1./B)-90.);
      fprintf(f, "/Helvetica findfont\n8 scalefont\nsetfont\n");
      fprintf(f, "%.2f %.2f rmoveto\n", fabs(B)>1.?-24.:+6., -8.);
      fprintf(f, "(-j%.3f) show\n", fabs(B)/smcdata.z0);

    }
    else{
      fprintf(f, "%.2f %.2f moveto\n", 0., (float)psdim/2.);
      fprintf(f, "%.2f %.2f %.2f %.2f %.2f arc\n",
	      x, y+h, h/2., 270., -2.*180./M_PI*atan(1./B)+90.);

    }
  }

}


void psdrawRHOk(FILE* f, float RHO) {
  float xc, yc, r;
  float x, y, w, h;
  
  xc=0.; yc=0.; r=RHO;
  x=((xc+1.)/2.*psdim); y=((1.-yc)/2.*psdim);
  w=(r*psdim); h=(r*psdim);
  
  fprintf(f, "0.5 0.5 0.5 setrgbcolor\n");
  fprintf(f, "%.2f %.2f moveto\n", 0., psdim/2.);
  fprintf(f, "%.2f %.2f %.2f 0.00 360.00 arc\n", x, y, w/2.);
  fprintf(f, "/Helvetica findfont\n8 scalefont\nsetfont\n");
  fprintf(f, "%.2f %.2f moveto\n", x+w/2./sqrt(2.)+2., y+h/2./sqrt(2.)+2.);
  fprintf(f, "(%.2f) show\nstroke\n", RHO);
}

void psdrawQk(FILE* f, float Q) {
  float xc, yc, r;
  float x, y, w, h;
  
  xc=0.; yc=1./Q; r=sqrt(1.+1./Q/Q);
  x=((xc+1.)/2.*psdim); y=((1.-yc)/2.*psdim);
  w=(r*psdim); h=(r*psdim);
  
  fprintf(f, "0. 1.0 0. setrgbcolor\n");
  fprintf(f, "%.2f %.2f moveto\n", (float)psdim, (float)psdim/2.);
  fprintf(f, "%.2f %.2f %.2f %.2f %.2f arc\n", x, y, w/2.,
	  90.-180./M_PI*atan(Q), 90+180./M_PI*atan(Q));
  fprintf(f, "/Helvetica findfont\n8 scalefont\nsetfont\n");
  fprintf(f, "%.2f %.2f moveto\n", x-8., y+h/2.+2.);
  fprintf(f, "(%.1f) show\n", Q);

  xc=0.; yc=-1./Q; r=sqrt(1.+1./Q/Q);
  x=((xc+1.)/2.*psdim); y=((1.-yc)/2.*psdim);
  w=(r*psdim); h=(r*psdim);

  fprintf(f, "%.2f %.2f moveto\n", (float)psdim/2., (float)psdim);
  fprintf(f, "%.2f %.2f %.2f %.2f %.2f arc\n", x, y, w/2.,
	  270.-180./M_PI*atan(Q), 270.+180/M_PI*atan(Q));

  fprintf(f, "stroke\n");
}


void psdrawR(FILE* f, float R, float sa, float al) {
  float xc, yc, r;
  float x, y, w, h;
  
  xc=R/(1.+R); yc=0; r=1./(1.+R);
  x=((xc+1.)/2.*psdim); y=((1.-yc)/2.*psdim);
  w=(r*psdim); h=(r*psdim);
  
  psdrawArc(f,x, y, w, h, sa, al);
}



void psdrawG(FILE* f, float G, float sa, float al){
  float xc, yc, r;
  float x, y, w, h;

  xc=-G/(1.+G); yc=0; r=1./(1.+G);
  x=(int)((xc+1.)/2.*psdim); y=(int)((1.-yc)/2.*psdim);
  w=(int)(r*psdim); h=(int)(r*psdim);
  psdrawArc(f,x, y, w, h, sa, al);
}


void psdrawB(FILE* f,float B, float sa, float al) {
  float xc, yc, r;
  int x, y, w, h;

  if(fabs(B)>QZ) {
    xc=-1.; yc=-1./B; r=1./fabs(B);
    x=(int)((xc+1.)/2.*psdim); y=(int)((1.+yc)/2.*psdim);
    w=(int)(r*psdim); h=(int)(r*psdim);
    
    psdrawArc(f,x, y, w, h, sa, al);
  }

  else {
    yc=-B/2.; y=(int)((1.-yc)/2.*psdim);
    yc=0; h=(int)((1.-yc)/2.*psdim);
    xc=sa; x=(int)((xc+1.)/2.*psdim);
    xc=al; w=(int)((xc+1.)/2.*psdim);
    
    fprintf(f,"%.2f %.2f moveto\n",x,y);
    fprintf(f,"%.2f %.2f lineto\n",w,h);
  }
}


void psdrawSP(FILE *f, float rerho, float imrho) {
  float x, y, r, i;

  switch(smcdata.SPtype){
  case SPTYPE_Z: rho2z(rerho, imrho, &r, &i);  
    r*=smcdata.z0; i*=smcdata.z0; fprintf(f, "1.0 0. 0. setrgbcolor\n");
    break;
  case SPTYPE_Y: rho2y(rerho, imrho, &r, &i);  
    r/=smcdata.z0; i/=smcdata.z0; fprintf(f, "0. 0. 1.0 setrgbcolor\n");
    break;
  case SPTYPE_RHO: r=rerho; i=imrho; fprintf(f, "0.5 0.5 0.5 setrgbcolor\n");

    break;
  case SPTYPE_UNDEF:
  default:
    return;
  }

  x=((rerho+1.)/2.*psdim); y=((1.+imrho)/2.*psdim);
  fprintf(f, "%.2f %.2f moveto\n", x+2., y);
  fprintf(f, "%.2f %.2f 2. 0. 360. arc\nfill\n",
	  x, y);
  fprintf(f, "/Helvetica findfont\n8 scalefont\nsetfont\n");
  fprintf(f, "%.2f %.2f moveto\n", x-24., y-10.);
  fprintf(f, "(%5.3f%cj%5.3f) show\n", r, i<0.?'-':'+', fabs(i));
}


void writePS(char* b){
  FILE *psf, *templ;
  int i, n, step, eps;
  float R, X, G, B, sa, ea, a, x1, y1;
  float rer, imr, rer1, imr1, rer2, imr2;
  int x,y;
  char buff[256], buff2[256], fname[80];
  time_t t;

  if((psf=fopen(b, "w"))==NULL) {
    fprintf(stderr, "Unable to open file %s for writing\n", b);
    return;
  }

  if(strstr(b, ".eps")!=NULL)
    eps=1;
  else if (strstr(b, ".ps")!=NULL)
    eps=0;
  else{
    fprintf(stderr,
	    "Unable to determine file format for %s (.ps and .eps allowed)\n",
	    b);
    return;
  }

  /* Beginning of PS file */
  if(eps)
    fprintf(psf, "%%!PS-Adobe EPSF-3.0\n%%%%Title: %s\n", "GSMC printout");
  else
    fprintf(psf, "%%!PS-Adobe-3.0\n%%%%Title: %s\n", "GSMC printout");
  fprintf(psf, "%%%%BoundingBox: %d %d %d %d\n", 0, 0, 595, 842); /*A4 paper*/

  fprintf(psf, "%.2f %.2f translate\n", 50., (float)psdim/2.-30.);

  fprintf(psf, "%%Page heading\n");
  fprintf(psf, "/Helvetica findfont\n20 scalefont\nsetfont\n");
  fprintf(psf, "newpath\n%.2f %.2f moveto\n(GSMC printout) show\n",
	  (float)psdim/2.-70., (float)psdim+50.);
  fprintf(psf, "/Helvetica findfont\n10 scalefont\nsetfont\n");
  fprintf(psf, "%.2f %.2f moveto\n(source file: %s) show\n",
	  10., (float)psdim+20., smcdata.projectname);
  t=time(NULL);
  fprintf(psf, "%.2f %.2f moveto\n", 10., (float)psdim+5.);
  fprintf(psf, "(Date: %s) show\n", ctime(&t));

  psdrawchart(psf);

  fprintf(psf,
	  "\n%%Arcs\nnewpath\n2 setlinewidth\n1 setlinejoin\n1 setlinecap\n");

/***
  * Draw arcs
  */
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
	  psdrawX(psf,X, sa, ea);
	}
	else
	  psdrawX(psf,X, smcdata.rerhoIP[i-1], smcdata.rerhoIP[i]);
      }
      else if(smcdata.ELtype[i]=='l' || smcdata.ELtype[i]=='c') {
	sa=thetaonR(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], R)*180/M_PI;
	ea=thetaonR(smcdata.rerhoIP[i], smcdata.imrhoIP[i], R)*180/M_PI;
	if(ea<0.) ea+=360; if(sa<0.) sa+=360;
	psdrawR(psf, R, sa, ea);
      }
      break;
      
      /* Parallel placed element, that could be an R, L, or C */
      case'p':
	rho2y(smcdata.rerhoIP[i], smcdata.imrhoIP[i], &G, &B);
      if(smcdata.ELtype[i]=='r') {
	if(fabs(B)>QZ) {
	  sa=thetaonB(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], B)*180/M_PI;
	  ea=thetaonB(smcdata.rerhoIP[i], smcdata.imrhoIP[i], B)*180/M_PI;
	  psdrawB(psf,B, sa, ea);
	}
	else 
	  psdrawB(psf,B, smcdata.rerhoIP[i-1], smcdata.rerhoIP[i]);
	
      }
      else if(smcdata.ELtype[i]=='l' || smcdata.ELtype[i]=='c') {
	sa=thetaonG(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], G)*180/M_PI;
	ea=thetaonG(smcdata.rerhoIP[i], smcdata.imrhoIP[i], G)*180/M_PI;
	
	psdrawG(psf,G, sa, ea);
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
	psdrawLine(psf,rer1, imr1, rer2, imr2);
	rer1=rer2; imr1=imr2;
      }
      break;
    case 'c': 
    case 'o':
      rho2y(smcdata.rerhoIP[i], smcdata.imrhoIP[i], &G, &B);       
      sa=thetaonG(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], G)*180/M_PI;
      ea=thetaonG(smcdata.rerhoIP[i], smcdata.imrhoIP[i], G)*180/M_PI;
      psdrawG(psf,G, sa, ea);
      break;
    case 'l':
    case 'a':
      rho2z(smcdata.rerhoIP[i], smcdata.imrhoIP[i], &R, &X);
      sa=thetaonR(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], R)*180/M_PI;
      ea=thetaonR(smcdata.rerhoIP[i], smcdata.imrhoIP[i], R)*180/M_PI;
      if(ea<0.) ea+=360; if(sa<0.) sa+=360;
      psdrawR(psf,R, sa, ea);
      break;
    default:
      fprintf(stderr, "Unallowed connection\n");
    }

  }
  fprintf(psf,"0. 0. 0. setrgbcolor\nstroke\n"); 
  /**
   * end arcs
   */

  /* Intermediate point impedance value print as text*/
  for(i=1; i<=smcdata.ne; i++) {
    x1=((smcdata.rerhoIP[i]+1.)/2.*psdim);
    y1=((1.+smcdata.imrhoIP[i])/2.*psdim);
    fprintf(psf, "%.2f %.2f moveto\n", x1, y1);
    if(Zcirc){
      fprintf(psf, "1.0 0.0 0.0 setrgbcolor\n");
      rho2z(smcdata.rerhoIP[i], smcdata.imrhoIP[i], &rer, &imr);
      float2prefs(rer*smcdata.z0, buff, 0);
      float2prefs(fabs(imr*smcdata.z0), buff2, 0);
      fprintf(psf, "(%s%cj%s) show\n", buff, smcdata.imrhoIP[i]>0.?'+':'-',
	      buff2);      
    }    
    else if(Ycirc){
      fprintf(psf, "0.0 0.0 1.0 setrgbcolor\n");
      rho2y(smcdata.rerhoIP[i], smcdata.imrhoIP[i], &rer, &imr);
      float2prefs(rer/smcdata.z0, buff, 0);
      float2prefs(imr/smcdata.z0, buff2, 0);
      fprintf(psf, "(%s%cj%s) show\n", buff, smcdata.imrhoIP[i]>0.?'+':'-',
	      buff2);      
    }
    else if(RHOcirc){
      fprintf(psf, "0.5 0.5 0.5 setrgbcolor\n");
      float2prefs(smcdata.rerhoIP[i], buff, 0);
      float2prefs(smcdata.imrhoIP[i], buff2, 0);
      fprintf(psf, "(%s%cj%s) show\n", buff, smcdata.imrhoIP[i]>0.?'+':'-',
	      buff2);      
    }
    else{ /* If no chart arc are selcted point are written as impedance */
      fprintf(psf, "1.0 0.0 0.0 setrgbcolor\n");
      rho2z(smcdata.rerhoIP[i], smcdata.imrhoIP[i], &rer, &imr);
      float2prefs(rer*smcdata.z0, buff, 0);
      float2prefs(imr*smcdata.z0, buff2, 0);
      fprintf(psf, "(%s%cj%s) show\n", buff, smcdata.imrhoIP[i]>0.?'+':'-',
	      buff2);      
    }

  }

  /* Textual circuit description */
  pscircdesc(psf);

  if(!eps)
    fprintf(psf, "showpage\n");
  
  fclose(psf);
}


void pscircdesc(FILE *f) {
  int i;
  char b[40], t[80], b2[3];

  fprintf(f, "\n%%Textual circuit description\nnewpath\n");
  fprintf(f, "0.0 0.0 0.0 setrgbcolor\n0 setlinewidth\n");
  fprintf(f, "%.2f %.2f moveto\n%.2f %.2f rlineto\n",
	  0., -36., (float)psdim, 0.);
  fprintf(f, "%.2f %.2f moveto\n%.2f %.2f rlineto\n",
	  0., -36., 0., +10);
  fprintf(f, "%.2f %.2f moveto\n%.2f %.2f rlineto\n",
	  (float)psdim/2., -36., 0., +10);
  fprintf(f, "%.2f %.2f moveto\n%.2f %.2f rlineto\n",
	  (float)psdim, -36., 0., +10);

  float2prefs(smcdata.z0, b, 0);
  fprintf(f, "%.2f %.2f moveto\n(z0= %s) show\n",
	  0., -48., b);
  float2prefs(smcdata.f0, b, 0);
  fprintf(f, "%.2f %.2f moveto\n(f0= %s) show\n",
	  80., -48., b);

  for(i=1; i<=smcdata.ne; i++) {

    switch(smcdata.ELtype[i]) {
    case 'r':
    case 'l':
    case 'c':
      float2prefs(smcdata.ELval0[i], b, 0);
      sprintf(t, "#%d:   %c   %c   %s %s\n",  i, toupper(smcdata.ELtype[i]),
	      smcdata.ELplace[i], b, smcdata.ELlocked[i]==1?"  - locked -":"");
      fprintf(f, "%.2f %.2f moveto\n",(int)((i-1)/5)*120., -68.-10.*((i-1)%5));
      fprintf(f, "(%s) show\n", t);
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
      fprintf(f, "%.2f %.2f moveto\n",(int)((i-1)/5)*120., -68.-10.*((i-1)%5));
      fprintf(f, "(%s) show\n", t);
      break;

    }

  }

  fprintf(f, "stroke\n");
}

