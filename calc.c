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


#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdio.h>
#include <math.h>
#include "main.h"
#include "calc.h"

extern SMCDATA smcdata;

void rhoz1z2(float *rerho, float *imrho, float z1, float z2) {
  float A, B, x, y;
  A=z1-z2; B=z1+z2;
  x=*rerho; y=*imrho;
  *rerho=(A*B*(1+x*x+y*y)+x*(A*A+B*B))/((B+A*x)*(B+A*x)+A*A*y*y);
  *imrho=(y*(B*B-A*A))/((B+A*x)*(B+A*x)+A*A*y*y);
}


void z2rho(float r, float x, float *rerho, float *imrho) {

  *rerho=(r*r+x*x-1)/((r+1)*(r+1)+x*x);
  *imrho=2*x/((r+1)*(r+1)+x*x);
}

void y2rho(float g, float b, float *rerho, float *imrho) {

  *rerho=(1-g*g-b*b)/((1+g)*(1+g)+b*b);
  *imrho=-2*b/((1+g)*(1+g)+b*b);
}

void rho2z(float rerho, float imrho, float *r, float *x) {

  *r=(1-rerho*rerho-imrho*imrho)/((1-rerho)*(1-rerho)+imrho*imrho);  
  *x=2*imrho/((1-rerho)*(1-rerho)+imrho*imrho);
}

void rho2y(float rerho, float imrho, float *g, float *b) {

  *g=(1-rerho*rerho-imrho*imrho)/((1+rerho)*(1+rerho)+imrho*imrho);  
  *b=-2*imrho/((1+rerho)*(1+rerho)+imrho*imrho);
}

float thetaonR(float rerho, float imrho, float R) {

  return (atan2(imrho, rerho-R/(1.+R)));
}

float thetaonX(float rerho, float imrho, float X) {
    if(X!=0.)
      return atan2(imrho-1./X, rerho-1.);
    else	  
      return 0;
}

float thetaonG(float rerho, float imrho, float G) {

  return (atan2(imrho, rerho+G/(1.+G)));
}

float thetaonB(float rerho, float imrho, float B) {
    if(B!=0.)
      return atan2(imrho+1./B, rerho+1.);
    else	  
      return 0;
}

/* Polar to rectangular conversion; angle in degrees */
void polar2rect(float *a, float *b) {
  float re, im;

  re=*a*cos(*b/180.*M_PI); im=*a*sin(*b/180.*M_PI);
  *a=re; *b=im;
}

/* Polar to rectangular conversion; angle in degrees */
void rect2polar(float *a, float *b) {
  float mod, arg;

  mod=sqrt(*a**a+*b**b); arg=atan2(*b, *a);
  *a=mod; *b=arg;
}


/* Do recalculation when some of data in smcdata are changed */
void recalc(void){ 
  int i;
  float reap, imap;


  /* Recalculate start point rho */
  switch(smcdata.SPtype) {
  case SPTYPE_Z: z2rho(smcdata.reentrySP/smcdata.z0,
		       smcdata.imentrySP/smcdata.z0, 
		       &smcdata.rerhoSP, &smcdata.imrhoSP); break;
  
  case SPTYPE_Y: y2rho(smcdata.reentrySP*smcdata.z0,
		       smcdata.imentrySP*smcdata.z0, 
		       &smcdata.rerhoSP, &smcdata.imrhoSP); break;

  case SPTYPE_RHO: smcdata.rerhoSP=smcdata.reentrySP;
    smcdata.imrhoSP=smcdata.imentrySP; break;
  }

  /* Recalculation of impedance when component value are given */
  smcdata.rerhoIP[0]=smcdata.rerhoSP; smcdata.imrhoIP[0]=smcdata.imrhoSP;
  for(i=1; i<=smcdata.ne; i++) {
    switch(smcdata.ELtype[i]) {
    case 'r':
      if(smcdata.ELplace[i]=='s') {
        rho2z(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], &reap, &imap);
	reap+=smcdata.ELval0[i]/smcdata.z0;
	z2rho(reap, imap, &smcdata.rerhoIP[i], &smcdata.imrhoIP[i]);
      }
      else if(smcdata.ELplace[i]=='p') {
	rho2y(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], &reap, &imap);
	reap+=smcdata.z0/smcdata.ELval0[i];
	y2rho(reap, imap, &smcdata.rerhoIP[i], &smcdata.imrhoIP[i]);
      }
      else { fprintf(stderr, "Impossible connection (%c)\n", 
		     smcdata.ELplace[i]); }
      break;
    case 'l':
      if(smcdata.ELplace[i]=='s') {
	rho2z(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], &reap, &imap);
	imap+=2*M_PI*smcdata.f0*smcdata.ELval0[i]/smcdata.z0;
	z2rho(reap, imap, &smcdata.rerhoIP[i], &smcdata.imrhoIP[i]);
      }
      else if(smcdata.ELplace[i]=='p') {
	rho2y(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], &reap, &imap);
	imap-=smcdata.z0/(2*M_PI*smcdata.f0*smcdata.ELval0[i]);
	y2rho(reap, imap, &smcdata.rerhoIP[i], &smcdata.imrhoIP[i]);
      }
      else { fprintf(stderr, "Impossible connection (%c)\n", 
		     smcdata.ELplace[i]); }
      break;
    case 'c':
      if(smcdata.ELplace[i]=='s') {
	rho2z(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], &reap, &imap);
	imap-=1./(2*M_PI*smcdata.f0*smcdata.ELval0[i])/smcdata.z0;
	z2rho(reap, imap, &smcdata.rerhoIP[i], &smcdata.imrhoIP[i]);
      }
      else if(smcdata.ELplace[i]=='p') {
	rho2y(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], &reap, &imap);
	imap+=2*M_PI*smcdata.f0*smcdata.ELval0[i]*smcdata.z0;
	y2rho(reap, imap, &smcdata.rerhoIP[i], &smcdata.imrhoIP[i]);
      }
      else { fprintf(stderr, "Impossible connection (%c)\n", 
		     smcdata.ELplace[i]); }
      break;

    case 't':
      switch(smcdata.ELplace[i]) {
      case 't':                                         /* Trasmission line */
	reap=smcdata.rerhoIP[i-1]; imap=smcdata.imrhoIP[i-1];
	rhoz1z2(&reap, &imap, smcdata.z0, smcdata.ELval1[i]);
	smcdata.rerhoIP[i]=reap*cos(2.*smcdata.ELval0[i]/180.*M_PI)+
	  imap*sin(2.*smcdata.ELval0[i]/180.*M_PI);
	smcdata.imrhoIP[i]=-reap*sin(2.*smcdata.ELval0[i]/180.*M_PI)+
	  imap*cos(2.*smcdata.ELval0[i]/180.*M_PI);
	rhoz1z2(&smcdata.rerhoIP[i], &smcdata.imrhoIP[i], 
		smcdata.ELval1[i], smcdata.z0);
	break;
      case 'o':                                       /* Open parallel stub */
	rho2y(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], &reap, &imap);
	imap+=tan(smcdata.ELval0[i]/180.*M_PI)/smcdata.ELval1[i]*smcdata.z0;
	y2rho(reap, imap, &smcdata.rerhoIP[i], &smcdata.imrhoIP[i]);
	break;
      case 'c':                                      /* Short parallel stub */
	rho2y(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], &reap, &imap);
	imap+=-1./tan(smcdata.ELval0[i]/180.*M_PI)/
	  smcdata.ELval1[i]*smcdata.z0;
	y2rho(reap, imap, &smcdata.rerhoIP[i], &smcdata.imrhoIP[i]);
	break;
      case 'l':                                      /* Short serial stub */
        rho2z(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], &reap, &imap);
	imap+=smcdata.ELval1[i]*tan(smcdata.ELval0[i]/180.*M_PI)/smcdata.z0;
	z2rho(reap, imap, &smcdata.rerhoIP[i], &smcdata.imrhoIP[i]);
	break;
      case 'a':                                      /* Open serial stub */
        rho2z(smcdata.rerhoIP[i-1], smcdata.imrhoIP[i-1], &reap, &imap);
	imap+=-smcdata.ELval1[i]/tan(smcdata.ELval0[i]/180.*M_PI)/smcdata.z0;
	z2rho(reap, imap, &smcdata.rerhoIP[i], &smcdata.imrhoIP[i]);
	break;
      default:
	fprintf(stderr, "Impossible connection (%c)\n", 
		smcdata.ELplace[i]);
      }
    }
  }

  /*
    for(i=0; i<=smcdata.ne; i++) 
      printf("%f %f\n", smcdata.rerhoIP[i], smcdata.imrhoIP[i]);
    printf("\n");
  */

}



void float2prefs(float v, char *b, int isspice) {
  float p;
  
  if(v!=0.){
    p=log10(fabs(v));
    if(p>=-18. && p<-15.) sprintf(b, "%6.2fa", v*1e18);
    else if(p>=-15. && p<-12.) sprintf(b, "%6.2ff", v*1e15);
    else if(p>=-12. && p<-9.) sprintf(b, "%6.2fp", v*1e12);
    else if(p>=-9. && p<-6.) sprintf(b, "%6.2fn", v*1e9);
    else if(p>=-6. && p<-3.) sprintf(b, "%6.2fu", v*1e6);
    else if(p>=-3. && p<0.) sprintf(b, "%6.2fm", v*1e3);
    else if(p>=0. && p<3.) sprintf(b, "%6.2f ", v);
    else if(p>=3. && p<6.) sprintf(b, "%6.2fk", v*1e-3);
    else if(p>=6. && p<9.) {
      if(isspice) sprintf(b, "%6.2fMEG", v*1e-6);
      else sprintf(b, "%6.2fM", v*1e-6);
    }
    else if(p>=9. && p<12.) sprintf(b, "%6.2fG", v*1e-9);
    else if(p>=12. && p<15.) sprintf(b, "%6.2fT", v*1e-12);

    else sprintf(b, "%6.2g", v);
  }
  else sprintf(b, "0");
}




