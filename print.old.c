#include <stdio.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <ctype.h>
#include "main.h"
#include "calc.h"
#include "print.h"
// jvdh 2004-3-30

extern SMCDATA smcdata;

float psdim = 880;

void psdrawArc(FILE* f, float x0,float y0,float w,float h,float a0,float a1){
        if (w==h) {     //the easy one...
	  //                fprintf(stderr,"%.2f %.2f %.2f %.2f %.2f arc",x0,y0,w/2.,a0,a1);
                fprintf(f,"%.2f %.2f %.2f %.2f %.2f arc",x0,y0,w/2.,a0,a1);
                if (a0>a1) {
		  //                        fprintf(stderr,"n");         //clockwise arc - default is anticlockwise
                        fprintf(f,"n");         //clockwise arc - default is anticlockwise
                }
		//                fprintf(stderr,"\n");
                fprintf(f,"\n");
                
        } else {        //ellipse!!
	  //                fprintf(stderr,"elliptical arcs not implemented\n");
        }
}

void psdrawLine(FILE* f,float rerhoi, float imrhoi, float rerhof, float imrhof) {
  float x1, y1, x2, y2;
  //  fprintf(stderr,"psdrawLine %.2f %.2f %.2f %.2f %.2f\n",rerhoi, imrhoi, rerhof, imrhof,psdim*1.);
  x1=(rerhoi+1.)/2.*psdim; y1=(1.+imrhoi)/2.*psdim;
  x2=(rerhof+1.)/2.*psdim; y2=(1.+imrhof)/2.*psdim;
  //  fprintf(stderr,"%.2f %.2f %.2f %.2f\n",x1,y1,x2,y2);
  fprintf(f,"%.2f %.2f moveto\n",x1,y1);
  fprintf(f,"%.2f %.2f lineto\n",x2,y2);
   
}

void psdrawX(FILE* f, float X, float sa, float al) {
  float xc, yc, r;
  float x, y, w, h;

  //  fprintf(stderr,"psdrawX\n");
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
    //    fprintf(stderr,"%.2f %.2f moveto\n",x,y);
    //    fprintf(stderr,"%.2f %.2f lineto\n",w,h);
    fprintf(f,"%.2f %.2f moveto\n",x,y);
    fprintf(f,"%.2f %.2f lineto\n",w,h);
  }
}
        //arc of constant R
void psdrawR(FILE* f, float R, float sa, float al) {
  float xc, yc, r;
  float x, y, w, h;

                  
  xc=R/(1.+R); yc=0; r=1./(1.+R);
  x=((xc+1.)/2.*psdim); y=((1.-yc)/2.*psdim);
  w=(r*psdim); h=(r*psdim);
  //  fprintf(stderr,"psdrawR R=%.2f sa=%.2f al=%.2f\nr=%.2f psdim=%.2f\n",R,sa,al,r,psdim);

  psdrawArc(f,x, y, w, h, sa, al);
}


void psdrawG(FILE* f, float G, float sa, float al){
  float xc, yc, r;
  float x, y, w, h;
  //  fprintf(stderr,"psdrawG");
  xc=-G/(1.+G); yc=0; r=1./(1.+G);
  x=(int)((xc+1.)/2.*psdim); y=(int)((1.-yc)/2.*psdim);
  w=(int)(r*psdim); h=(int)(r*psdim);
  psdrawArc(f,x, y, w, h, sa, al);
}


void psdrawB(FILE* f,float B, float sa, float al) {
  float xc, yc, r;
  int x, y, w, h;

  //        fprintf(stderr,"drawB");
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

    //    fprintf(stderr,"%.2f %.2f moveto\n",x,y);
    //    fprintf(stderr,"%.2f %.2f lineto\n",w,h);
    fprintf(f,"%.2f %.2f moveto\n",x,y);
    fprintf(f,"%.2f %.2f lineto\n",w,h);
  }
}

void writePS(char* b){
  FILE *psf, *templ;
  int i, n, step;
  float R, X, G, B, sa, ea, a;
  float rer, imr, rer1, imr1, rer2, imr2;
  int x,y;
  char buff[256], fname[80];
  
  sprintf(fname, "%s", getenv("HOME"));
  strcat(fname, "/.gsmc/imped_admit_smithchart.ps");

  if((templ=fopen(fname, "r"))==NULL) {
    fprintf(stderr, 
	"Unable to open file %s for reading\n", fname);
    return;
  }
  if((psf=fopen(".out.ps.tmp", "w"))==NULL) {
    fprintf(stderr, "Unable to open file .out.ps.tmp for writing\n");
    return;
  }
  while(1){
    if(fgets(buff, 256, templ)==NULL)
      break;
    fputs(buff, psf);
  }  
  fclose(templ);
  
  //  system("cp imped_admit_smithchart.ps .out.ps.tmp -f",b); //#-) system call - not good...
  //system("cp empty.ps .out.ps.tmp -f",b); //#-) system call - not good...
  /*  if((psf=fopen(".out.ps.tmp", "a"))==NULL) {
    fprintf(stderr, "Unable to open file %s for writing\n", b);
    return;
  }
  */
    
  fprintf(psf, "\%\%PS file appended by GTK Smith Chart Calculator\n\n");
/***
  * Draw arcs
  */
  //fprintf(psf,"gsave\n3057 4289.5 translate\n2 setlinewidth\n");
  fprintf(psf,"gsave\n855 2092 translate\n5 5 scale\n2 setlinewidth\n");
  //fprintf(psf,"\n\n\ngsave\n5 5 translate\nnewpath\ngsave\n [1 4] 0 setdash\n");
    /*    for (x=0;x<=20; x++) {
                fprintf(psf,"\nnewpath\n%.2f %.2f moveto\n%.2f %.2f lineto\nstroke\nclosepath\n",x*psdim/20.,0.,x*psdim./20.,404.);
        }  
        for (y=0; y<=20;y++){
                fprintf(psf,"\nnewpath\n%.2f %.2f moveto\n%.2f %.2f lineto\nstroke\nclosepath\n",0.,y*psdim./20.,404.,y*psdim./20.);
        }
  fprintf(psf,"grestore\n");*/
  //  fprintf(stderr,"smcdata.ne = %d\n",smcdata.ne);
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
        fprintf(psf,"stroke\nclosepath\ngrestore\n\n\n"); 
  /**
   * end arcs
   */
  
        //final line of ps file
  fprintf(psf, "\ncleartomark end end pagesave restore showpage\n\%\%PageTrailer\n\%\%Trailer\n\%\%Pages: 1\n\%\%EOF\n");
//  fprintf(psf, "\nshowpage grestore grestore \n\%\%PageTrailer\n\%\%Trailer\n\%\%Pages: 1\n\%\%EOF\n");
  fclose(psf);
  rename (".out.ps.tmp",b);

  
  
  /*%% insert stuff here...
gsave
        3057 4289.5 translate     %set origin to (3057,4290)
        newpath
        0 0 150 0 90 arc
        stroke
        closepath
grestore


%% #-)
newpath
0 0 3057 4289.5 moveto
144 72 lineto
144 144 lineto
72 144 lineto
closepath
fill
%% #-)
  */    
        
}



