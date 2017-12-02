//************************************************************************
// hoser.c  
//
// Based on BezierDoc.cpp by Chris Daelman
// which was based on the hoser spreadsheet by John VanZwieten Jr. and III
//************************************************************************

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

// Suppress all those pesty unused errors
#define UNUSED(x) (void)(x)

void hoseout(void);

//************************************************************************
// Adds 2 Points. Don't Just Use '+' ;)
//************************************************************************
/* r = a + b */
void V3Add(float r[4], float a[4], float b[4])
{
   r[0] = a[0] + b[0];
   r[1] = a[1] + b[1];
   r[2] = a[2] + b[2];
}

//************************************************************************
/* r += a */
void V3Sum(float r[4], float a[4])
{
   r[0] += a[0];
   r[1] += a[1];
   r[2] += a[2];
}

//************************************************************************
// Multiplies a Point and a Constant. Don't Just Use '*'
//************************************************************************
/* r = ta */
void V3Scale(float r[4], float a[4], double t)
{
   r[0] = t * a[0];
   r[1] = t * a[1];
   r[2] = t * a[2];
}

//************************************************************************
// Calculate the 3rd degree polynomial based on an array of 4 points and 
// a variable (u) which is generally between 0 And 1.  By stepping u in 
// equal increments from 0 to 1 we get a nice approximation of the curve.   
//************************************************************************
void Bernstein(float r[4], float u, float m[4][4]) 
{
  float a[4], b[4], c[4], d[4];

#if 0
  V3Scale(a, m[0], pow(u,3));		// m[0] is the 1st control point
  V3Scale(b, m[1], 3*pow(u,2)*(1-u));	// m[1] is the 2nd control point
  V3Scale(c, m[2], 3*u*pow((1-u),2));	// m[2] is the 3rd control point
  V3Scale(d, m[3], pow((1-u),3));	// m[3] is the 4th control point
#else
  // I dont know why, but I must reverse the order of the control points.
  V3Scale(a, m[3], pow(u,3));		// m[3] is the 1st control point
  V3Scale(b, m[2], 3*pow(u,2)*(1-u));	// m[2] is the 2nd control point
  V3Scale(c, m[1], 3*u*pow((1-u),2));	// m[1] is the 3rd control point
  V3Scale(d, m[0], pow((1-u),3));	// m[0] is the 4th control point
#endif

  V3Add(r, a, b);
  V3Sum(r, c);
  V3Sum(r, d);
}

/************************************************************************
0 Example:
0 Start with a 750.dat (or 752.dat) End piece,
0 Next comes a 755.dat End Section,
0 Then a string of approximately 25 754.dat Link Sections,
0 Place a 756.dat Center Section,
0 Then another string of 754.dats facing the opposite way,
0 Then a 755.dat End Section,
0 Finish with a 750.dat (or 752.dat) End piece.
*/

//************************************************************************
#ifdef TESTING
char maintext[100000];
char helper[256];
#else
char maintext[256];
char helper[256];
#endif

//************************************************************************
// The user inserts 2 colored end parts (754.dat) at the correct locations.
// Then insert 2 hose parts (754.dat) for intermediate control points.
// Key in the hoser command key 'H' followed by the number of steps,
// and press <Enter> to send the goodies to this fn, which hoses it.
//************************************************************************
void hoser(float m[4][4], int color, int steps, int drawline,
	   char *parttext, char *firstparttext)
{
  float m1[4][4];
  float p[4],prevp[4],nextp[4],nextnextp[4];
  double Xaxisrotation,Yaxisrotation,pi,pidiv2;
  double pointtopointdistance,nextpointtopointdistance;
  double cumulatedptpdistance,cumulatedptpdistanceperstep;
  double NTV,nextNTV,PrevNTV; // NTV = NormalizedTimeValue;
  double RatioofDistancetoIdeal,nextRatioofDistancetoIdeal;
  double cumulatedITV,ITV; // ITV = IntermedTimeValue;
  double Prevpointtopointdistance,PrevRatioofDistancetoIdeal;
  char *segname;
  int i;

  pi=acos(-1);
  pidiv2=pi/2;

#ifdef TESTING
  // Start out with comment lines letting us know about the control points.
  sprintf(maintext,"0 Bezier ");
  sprintf(helper,"0 %g %g %g " ,m[0][0] ,m[0][1], m[0][2]);
  strcat(maintext, helper);
  sprintf(helper,"0 %g %g %g " ,m[1][0] ,m[1][1], m[1][2]);
  strcat(maintext, helper);
  sprintf(helper,"0 %g %g %g " ,m[2][0] ,m[2][1], m[2][2]);
  strcat(maintext, helper);
  sprintf(helper,"0 %g %g %g " ,m[3][0] ,m[3][1], m[3][2]);
  strcat(maintext, helper);
  sprintf(helper,"0 %d %s %s %d %d\n" ,color ,parttext, firstparttext, steps, drawline);
  strcat(maintext, helper);
#endif

  // For some reason it works better to calculate distance on one less step.
  // Gotta think about this.
  steps--;
  cumulatedptpdistance=0;
  for (i = 0; i < steps-1; i++)
  {
    Bernstein(p, ((float)i / steps), m);
    Bernstein(nextp, ((float)(i+1) / steps), m);
    pointtopointdistance=sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+
			      ((nextp[1]-p[1])*(nextp[1]-p[1]))+
			      ((nextp[2]-p[2])*(nextp[2]-p[2])));
    cumulatedptpdistance += pointtopointdistance;
  }			

  cumulatedptpdistanceperstep=cumulatedptpdistance/steps;
  cumulatedITV = 0;
  for (i = 0; i < steps; i++)
  {
    Bernstein(p, ((float)i / steps), m);
    Bernstein(nextp, ((float)(i+1) / steps), m);
    pointtopointdistance=sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+
			      ((nextp[1]-p[1])*(nextp[1]-p[1]))+
			      ((nextp[2]-p[2])*(nextp[2]-p[2])));
    RatioofDistancetoIdeal=pointtopointdistance/cumulatedptpdistanceperstep;
    cumulatedITV += ((1/(double)steps)/RatioofDistancetoIdeal);
  }

  ITV = 0;
  NTV = 0;
  PrevNTV = 0;
  steps++;
  for (i =0; i < steps; i++)
  {
    Bernstein(p, ((float)i / steps), m);
    Bernstein(nextp, ((float)(i+1) / steps), m);
    Bernstein(nextnextp, ((float)(i+2) / steps), m);
    Bernstein(prevp, ((float)(i-1) / steps), m);
    Prevpointtopointdistance=sqrt(((p[0]-prevp[0])*(p[0]-prevp[0]))+
				  ((p[1]-prevp[1])*(p[1]-prevp[1]))+
				  ((p[2]-prevp[2])*(p[2]-prevp[2])));
    pointtopointdistance=sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+
			      ((nextp[1]-p[1])*(nextp[1]-p[1]))+
			      ((nextp[2]-p[2])*(nextp[2]-p[2])));
    nextpointtopointdistance=sqrt(((nextnextp[0]-nextp[0])*(nextnextp[0]-nextp[0]))+
				  ((nextnextp[1]-nextp[1])*(nextnextp[1]-nextp[1]))+
				  ((nextnextp[2]-nextp[2])*(nextnextp[2]-nextp[2])));

    PrevRatioofDistancetoIdeal=Prevpointtopointdistance/cumulatedptpdistanceperstep;
    RatioofDistancetoIdeal=pointtopointdistance/cumulatedptpdistanceperstep;
    nextRatioofDistancetoIdeal=nextpointtopointdistance/cumulatedptpdistanceperstep;

    if (i>0)
      NTV=PrevNTV+((1/(double)steps)/PrevRatioofDistancetoIdeal/cumulatedITV); 
    else 
      NTV=0;
    nextNTV=NTV+((1/(double)steps)/RatioofDistancetoIdeal/cumulatedITV);

    Bernstein(p, NTV, m);
    Bernstein(nextp, nextNTV, m);

    // Now we need to create the rotation matrix for this hose step.
    // Im sure theres an easier to understand way to write this, but since
    // I cant follow it, I cant make it easier for someone else to read. 

    // Perhaps a quaternion would help here.
    // We already have the location vector p for this segment.
    // We need to calculate the tangent to the curve here and
    // convert it into a rotation matrix.
    // I guess we use prevp and nextp to get the tangent.
    // Whats with all the special cases?
    if ((nextp[0]<p[0]) && (nextp[1]<p[1]) && (nextp[2]<p[2])) {
      Xaxisrotation=-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=(atan((nextp[0]-p[0])/(nextp[2]-p[2])));}
    if ((nextp[0]<p[0]) && (nextp[1]<p[1]) && (nextp[2]==p[2])) {
      Xaxisrotation=-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=pidiv2;}
    if ((nextp[0]<p[0]) && (nextp[1]<p[1]) && (nextp[2]>p[2])) {
      Xaxisrotation=-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=(-pi+(atan((nextp[0]-p[0])/(nextp[2]-p[2]))));}
    if ((nextp[0]<p[0]) && (nextp[1]==p[1]) && (nextp[2]<p[2])) {
      Xaxisrotation=pidiv2;
      Yaxisrotation=(pi+(atan((nextp[0]-p[0])/(nextp[2]-p[2]))));}
    if ((nextp[0]<p[0]) && (nextp[1]==p[1]) && (nextp[2]==p[2])) {
      Xaxisrotation=pidiv2;Yaxisrotation=pidiv2;}
    if ((nextp[0]<p[0]) && (nextp[1]==p[1]) && (nextp[2]>p[2])) {
      Xaxisrotation=pidiv2;Yaxisrotation=(pi+(atan((nextp[0]-p[0])/(nextp[2]-p[2]))));}
    if ((nextp[0]<p[0]) && (nextp[1]>p[1]) && (nextp[2]<p[2])) {
      Xaxisrotation=pi-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=(atan((nextp[0]-p[0])/(nextp[2]-p[2])));}
    if ((nextp[0]<p[0]) && (nextp[1]>p[1]) && (nextp[2]==p[2])) {
      Xaxisrotation=(-pi+(atan((nextp[0]-p[0])/(nextp[1]-p[1]))));
      Yaxisrotation=pidiv2;}
    if ((nextp[0]<p[0]) && (nextp[1]>p[1]) && (nextp[2]>p[2])) {
      Xaxisrotation=pi-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=(-pi+(atan((nextp[0]-p[0])/(nextp[2]-p[2]))));}
    if ((nextp[0]==p[0]) && (nextp[1]<p[1]) && (nextp[2]<p[2])) {
      Xaxisrotation=-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=0;}
    if ((nextp[0]==p[0]) && (nextp[1]<p[1]) && (nextp[2]==p[2])) {
      Xaxisrotation=0;
      Yaxisrotation=pi;}
    if ((nextp[0]==p[0]) && (nextp[1]<p[1]) && (nextp[2]>p[2])) {
      Xaxisrotation=(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=0;}
    if ((nextp[0]==p[0]) && (nextp[1]==p[1]) && (nextp[2]<p[2])) {
      Xaxisrotation=pidiv2;
      Yaxisrotation=0;}
    if ((nextp[0]==p[0]) && (nextp[1]==p[1]) && (nextp[2]==p[2])) {
      Xaxisrotation=pi;
      Yaxisrotation=pi;}
    if ((nextp[0]==p[0]) && (nextp[1]==p[1]) && (nextp[2]>p[2])) {
      Xaxisrotation=-pidiv2;
      Yaxisrotation=0;}
    if ((nextp[0]==p[0]) && (nextp[1]>p[1]) && (nextp[2]<p[2])) {
      Xaxisrotation=pi-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=0;}
    if ((nextp[0]==p[0]) && (nextp[1]>p[1]) && (nextp[2]==p[2])) {
      Xaxisrotation=pi;
      Yaxisrotation=pi;}
    if ((nextp[0]==p[0]) && (nextp[1]>p[1]) && (nextp[2]>p[2])) {
      Xaxisrotation=-pi+(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=0;}
    if ((nextp[0]>p[0]) && (nextp[1]<p[1]) && (nextp[2]<p[2])) {
      Xaxisrotation=-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=(atan((nextp[0]-p[0])/(nextp[2]-p[2])));}
    if ((nextp[0]>p[0]) && (nextp[1]<p[1]) && (nextp[2]==p[2])) {
      Xaxisrotation=(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=pidiv2;}
    if ((nextp[0]>p[0]) && (nextp[1]<p[1]) && (nextp[2]>p[2])) {
      Xaxisrotation=-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=(-pi+(atan((nextp[0]-p[0])/(nextp[2]-p[2]))));}
    if ((nextp[0]>p[0]) && (nextp[1]==p[1]) && (nextp[2]<p[2])) {
      Xaxisrotation=-pidiv2;
      Yaxisrotation=(atan((nextp[0]-p[0])/(nextp[2]-p[2])));}
    if ((nextp[0]>p[0]) && (nextp[1]==p[1]) && (nextp[2]==p[2])) {
      Xaxisrotation=-pidiv2;
      Yaxisrotation=-pidiv2;}
    if ((nextp[0]>p[0]) && (nextp[1]==p[1]) && (nextp[2]>p[2])) {
      Xaxisrotation=-pidiv2;
      Yaxisrotation=(atan((nextp[0]-p[0])/(nextp[2]-p[2])));}
    if ((nextp[0]>p[0]) && (nextp[1]>p[1]) && (nextp[2]<p[2])) {
      Xaxisrotation=pi-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=(atan((nextp[0]-p[0])/(nextp[2]-p[2])));}
    if ((nextp[0]>p[0]) && (nextp[1]>p[1]) && (nextp[2]==p[2])) {
      Xaxisrotation=-pi+(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=pidiv2;}
    if ((nextp[0]>p[0]) && (nextp[1]>p[1]) && (nextp[2]>p[2])) {
      Xaxisrotation=pi-(atan(sqrt(((nextp[0]-p[0])*(nextp[0]-p[0]))+((nextp[2]-p[2])*(nextp[2]-p[2])))/(nextp[1]-p[1])));
      Yaxisrotation=(-pi+(atan((nextp[0]-p[0])/(nextp[2]-p[2]))));}

    if ((i==0) || (i==(steps-1))) 
      segname = firstparttext;
    else 
      segname = parttext;
    if (!stricmp(parttext, "209.dat")) // Chain links
    {
      // Turn each link 90 degrees from the previous link.
      // NOTE: The center point on each link is not the part center.
      // Its the center of the half torus at one end of the link.
      // This makes the last link troublesome to place.  We ought to
      // set the end point half a link further out from the end part,
      // and then skip the 180 turn for the last link.
    }
    else
    if (i==(steps-1))
    {
      // Turn the last part 180 degrees to make it face out of the hose.
      if (Xaxisrotation < 0)
	  Xaxisrotation += pi; 
      else
	  Xaxisrotation -= pi; 
    }

    // Format the new hose segment.
#ifdef TESTING
    sprintf(helper,"1 %d " ,16); // color
    strcat(maintext, helper);
    sprintf(helper,"%g %g %g " ,p[0] ,p[1], p[2]);
    strcat(maintext, helper);
    sprintf(helper,"%g %g %g " , cos(Yaxisrotation),
	    (sin(Xaxisrotation)*sin(Yaxisrotation)),
	    (cos(Xaxisrotation)*sin(Yaxisrotation)));
    strcat(maintext, helper);
    sprintf(helper,"0 %g %g " ,cos(Xaxisrotation) ,-sin(Xaxisrotation));
    strcat(maintext, helper);
    sprintf(helper,"%g %g %g " ,-sin(Yaxisrotation),
	    (sin(Xaxisrotation)*cos(Yaxisrotation)),
	    (cos(Xaxisrotation)*cos(Yaxisrotation)));
    strcat(maintext, helper);
    sprintf(helper,"%s\n" ,segname);
#else
    m1[0][0] = cos(Yaxisrotation);
    m1[0][1] = (sin(Xaxisrotation)*sin(Yaxisrotation));
    m1[0][2] = (cos(Xaxisrotation)*sin(Yaxisrotation));
    m1[0][3] = p[0];
    m1[1][0] = 0;
    m1[1][1] = cos(Xaxisrotation);
    m1[1][2] = -sin(Xaxisrotation);
    m1[1][3] = p[1];
    m1[2][0] = -sin(Yaxisrotation);
    m1[2][1] = (sin(Xaxisrotation)*cos(Yaxisrotation));
    m1[2][2] = (cos(Xaxisrotation)*cos(Yaxisrotation));
    m1[2][3] = p[2];
    m1[3][0] = 0;
    m1[3][1] = 0;
    m1[3][2] = 0;
    m1[3][3] = 1;
    if (((i & 1) == 0) && (!stricmp(parttext, "209.dat")))
    {
      // Roll alternate chain links 90 degrees around Y axis
      // (before applying the hoser matrix).
      float m0[4][4] = {
	{1.0,0.0,0.0,0.0},
	{0.0,1.0,0.0,0.0},
	{0.0,0.0,1.0,0.0},
	{0.0,0.0,0.0,1.0}
      };
      float m2[4][4] = {
	{1.0,0.0,0.0,0.0},
	{0.0,1.0,0.0,0.0},
	{0.0,0.0,1.0,0.0},
	{0.0,0.0,0.0,1.0}
      };
      m0[0][0] = 0.0;  //(float)cos(pidiv2);
      m0[0][2] = 1.0;  //(float)sin(pidiv2);
      m0[2][0] = -1.0; //(float)(-1.0*sin(pidiv2));
      m0[2][2] = 0.0;  //(float)cos(pidiv2);
      M4M4Mul(m2,m1,m0);
      memcpy(m1, m2, sizeof(m1));

    }
    hoseseg(segname, color, m1);
#endif

    p[0] = nextp[0]; 
    p[1] = nextp[1]; 
    p[2] = nextp[2];
    PrevNTV = NTV;
  }
  
#ifdef TESTING
  sprintf(helper,"0 Bezier curve end\n");
  strcat(maintext, helper);
#else
  sprintf(maintext, "");
#endif

  if (drawline) 
  {	
    sprintf(helper,"2 %d %g %g %g %g %g %g\n",color,
	    m[0][0],m[0][1],m[0][2],
	    m[1][0],m[1][1],m[1][2]);
    strcat(maintext, helper);
    sprintf(helper,"2 %d %g %g %g %g %g %g\n",color,
	    m[2][0],m[2][1],m[2][2],
	    m[3][0],m[3][1],m[3][2]);
    strcat(maintext, helper);
  }

#ifdef TESTING
  sprintf(helper,"0\n");
  strcat(maintext, helper);	

  hoseout();
#else
  if (drawline) 
    hoseout();
#endif

}

/***************************************************************/
#include <stdio.h>
void hoseout(void)
  {
    FILE *fp;
    int bytes = 0;
    UNUSED(bytes);
    fp = fopen("hoseout.dat","w+");
    fprintf(fp, maintext);
    fclose(fp);

    printf(maintext);
  }
    
#ifdef TESTING
/***************************************************************/
int 
main(int argc, char **argv)
{
  float m[4][4];
  int color = 16;
  int steps = 50;
  int drawline = 1;
  char parttext[256] = "754.dat";  // The hose parts
  char firstparttext[256] = "756.dat";  // The end parts (first and last)

  // Get the 4 control points from the part locations.
  m[0][0] = -37; // X
  m[0][1] = -78; // Y
  m[0][2] = 150; // Z
  m[0][3] = 0; // This is just filler in the 4x4 matrix.

  m[1][0] = 120;
  m[1][1] = 30;
  m[1][2] = -1;
  m[1][3] = 0;
  
  m[2][0] = 30;
  m[2][1] = 100;
  m[2][2] = 0;
  m[2][3] = 0;
  
  m[3][0] = -60;
  m[3][1] = -61;
  m[3][2] = 240;
  m[3][3] = 0;

  // Fix the middle control points.  (I don't know why.)
  // Subtract the 2nd control point from the first.
  V3Scale(m[1], m[1], -1);
  V3Sum(m[1], m[0]);
  // Subtract the 3rd control point from the last.
  V3Scale(m[2], m[2], -1);
  V3Sum(m[2], m[3]);

  hoser( m, color, steps, drawline, parttext, firstparttext );
}

#endif    
















