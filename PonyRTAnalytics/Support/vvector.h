#ifndef GIM_LINEAR_H_INCLUDED
#define GIM_LINEAR_H_INCLUDED

#include <math.h>
#include <stdio.h>

static int p[512];
static int permutation[] = { 151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,
    21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
    74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,
    230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,
    80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,
    164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,
    118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,
    183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
    172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,
    218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,
    145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,
    115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,
    141,128,195,78,66,215,61,156,180
};

/* Function declarations */
static inline double   fade(double t);
static inline double   lerp(double t, double a, double b);
static inline double   grad(int hash, double x, double y, double z); 
static inline void     init_noise(void);
static inline double   pnoise(double x, double y, double z);

static inline void init_noise()
{
    static int inited = 0;
    
    if(!inited)
    {
        int i;
        for(i = 0; i < 256 ; i++) 
            p[256+i] = p[i] = permutation[i]; 
        
        inited = 1;
    }
}

static inline double pnoise(double x, double y, double z)
{
    init_noise();
    
    int   X = (int)floor(x) & 255,             /* FIND UNIT CUBE THAT */
    Y = (int)floor(y) & 255,             /* CONTAINS POINT.     */
    Z = (int)floor(z) & 255;
    x -= floor(x);                             /* FIND RELATIVE X,Y,Z */
    y -= floor(y);                             /* OF POINT IN CUBE.   */
    z -= floor(z);
    double  u = fade(x),                       /* COMPUTE FADE CURVES */
    v = fade(y),                       /* FOR EACH OF X,Y,Z.  */
    w = fade(z);
    int  A = p[X]+Y, 
    AA = p[A]+Z, 
    AB = p[A+1]+Z, /* HASH COORDINATES OF */
    B = p[X+1]+Y, 
    BA = p[B]+Z, 
    BB = p[B+1]+Z; /* THE 8 CUBE CORNERS, */
    
    return lerp(w,lerp(v,lerp(u, grad(p[AA  ], x, y, z),   /* AND ADD */
                              grad(p[BA  ], x-1, y, z)),        /* BLENDED */
                       lerp(u, grad(p[AB  ], x, y-1, z),         /* RESULTS */
                            grad(p[BB  ], x-1, y-1, z))),     /* FROM  8 */
                lerp(v, lerp(u, grad(p[AA+1], x, y, z-1 ),/* CORNERS */
                             grad(p[BA+1], x-1, y, z-1)),      /* OF CUBE */
                     lerp(u, grad(p[AB+1], x, y-1, z-1),
                          grad(p[BB+1], x-1, y-1, z-1))));
}

static inline double fade(double t){ return t * t * t * (t * (t * 6 - 15) + 10); }
static inline double lerp(double t, double a, double b){ return a + t * (b - a); }
static inline double grad(int hash, double x, double y, double z) 
{
    int     h = hash & 15;       /* CONVERT LO 4 BITS OF HASH CODE */
    double  u = h < 8 ? x : y,   /* INTO 12 GRADIENT DIRECTIONS.   */
    v = h < 4 ? y : h==12||h==14 ? x : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

#pragma mark -

static inline int side(float  *a, float * b, float * p)
{
	return (b[0] - a[0]) * (p[1] - a[1]) - (b[1] - a[1]) * (p[0] - a[0]);
}

static inline void closest_point_on_line_from_point(float  *a, float * b, float * p, float * r)
{
	float vx = b[0] - a[0];
	float vy = b[1] - a[1];
	float wx = p[0] - a[0];
	float wy = p[1] - a[1];
	
	float c1 = vx * wx + vy * wy;
	float c2 = vx * vx + vy * vy;
	
	float ratio = c1 / c2;
	
	r[0] = a[0] + ratio * vx;
	r[1] = a[1] + ratio * vy;
}

static inline void mirror(float * a, float * b, float * p, float * r)
{
	closest_point_on_line_from_point(a,b,p,r);
	r[0] = p[0] + 2.0f * (r[0] - p[0]);
	r[1] = p[1] + 2.0f * (r[1] - p[1]);
}

/*
static inline void gluPerspective(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
	GLfloat xmin, xmax, ymin, ymax;
	
	ymax = zNear * tan(fovy * M_PI / 360.0f);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
	
	frustum(xmin, xmax, ymin, ymax, zNear, zFar);
}*/

static inline float calculateFOV(float size, float dist)
{
	float radtheta, degtheta;

	radtheta = 2.0 * atan2( size*0.5f, dist );
	degtheta = (180.0 * radtheta) / M_PI;
	return degtheta;
}

static inline bool lineSegmentIntersection(
							 float Ax, float Ay,
							 float Bx, float By,
							 float Cx, float Cy,
							 float Dx, float Dy,
							 float *X, float *Y)
{
	
	float  distAB, theCos, theSin, newX, ABpos ;
	
	//  Fail if either line segment is zero-length.
	if ((Ax==Bx && Ay==By) || (Cx==Dx && Cy==Dy)) return NO;
	
	//  Fail if the segments share an end-point.
	if ((Ax==Cx && Ay==Cy) || (Bx==Cx && By==Cy)
		||  (Ax==Dx && Ay==Dy) || (Bx==Dx && By==Dy))
	{
		return NO;
	}
	
	//  (1) Translate the system so that point A is on the origin.
	Bx-=Ax; By-=Ay;
	Cx-=Ax; Cy-=Ay;
	Dx-=Ax; Dy-=Ay;
	
	//  Discover the length of segment A-B.
	distAB=sqrt(Bx*Bx+By*By);
	
	//  (2) Rotate the system so that point B is on the positive X axis.
	theCos=Bx/distAB;
	theSin=By/distAB;
	newX=Cx*theCos+Cy*theSin;
	Cy  =Cy*theCos-Cx*theSin; Cx=newX;
	newX=Dx*theCos+Dy*theSin;
	Dy  =Dy*theCos-Dx*theSin; Dx=newX;
	
	//  Fail if segment C-D doesn't cross line A-B.
	if ((Cy<0. && Dy<0.) || (Cy>=0. && Dy>=0.)) return NO;
	
	//  (3) Discover the position of the intersection point along line A-B.
	ABpos=Dx+(Cx-Dx)*Dy/(Dy-Cy);
	
	//  Fail if segment C-D crosses line A-B outside of segment A-B.
	if (ABpos<0. || ABpos>distAB) return NO;
	
	//  (4) Apply the discovered position to line A-B in the original coordinate system.
	*X=Ax+ABpos*theCos;
	*Y=Ay+ABpos*theSin;
	
	//  Success.
	return YES;
}



#define VEC_WIGGLE_1(r,amp,t,s)                 \
{                                               \
    float t2 = (t);                             \
    t2 = fmod(fabs(t), 10.0f) * 0.1f;                 \
    (r) = pnoise(t, s, 0) * (amp);           \
}                                               \

#define VEC_XLERP_IN_1(r,a,b,t)					\
{												\
	float t2 = t*t;								\
	(r) = (a) * (1.0f - t2) + (b) * t2;			\
}

#define VEC_XLERP_IN_2(r,a,b,t)						\
{													\
	float t2 = t*t;									\
	(r)[0] = (a)[0] * (1.0f - t2) + (b)[0] * t2;		\
	(r)[1] = (a)[1] * (1.0f - t2) + (b)[1] * t2;		\
}

#define VEC_XLERP_IN(r,a,b,t)							\
{													\
	float t2 = t*t;									\
	(r)[0] = (a)[0] * (1.0f - t2) + (b)[0] * t2;		\
	(r)[1] = (a)[1] * (1.0f - t2) + (b)[1] * t2;		\
	(r)[2] = (a)[2] * (1.0f - t2) + (b)[2] * t2;		\
}

#define VEC_XLERP_IN_4(r,a,b,t)						\
{													\
	float t2 = t*t;									\
	(r)[0] = (a)[0] * (1.0f - t2) + (b)[0] * t2;		\
	(r)[1] = (a)[1] * (1.0f - t2) + (b)[1] * t2;		\
	(r)[2] = (a)[2] * (1.0f - t2) + (b)[2] * t2;		\
	(r)[3] = (a)[3] * (1.0f - t2) + (b)[3] * t2;		\
}


#define VEC_XLERP_OUT_1(r,a,b,t)						\
{														\
	float t2 = -(t * t) + 2*t;							\
	(r) = (a) * (1.0f - t2) + (b) * t2;					\
}

#define VEC_XLERP_OUT_2(r,a,b,t)						\
{														\
	float t2 = -(t * t) + 2*t;							\
	(r)[0] = (a)[0] * (1.0f - t2) + (b)[0] * t2;		\
	(r)[1] = (a)[1] * (1.0f - t2) + (b)[1] * t2;		\
}

#define VEC_XLERP_OUT(r,a,b,t)							\
{														\
	float t2 = -(t * t) + 2*t;							\
	(r)[0] = (a)[0] * (1.0f - t2) + (b)[0] * t2;		\
	(r)[1] = (a)[1] * (1.0f - t2) + (b)[1] * t2;		\
	(r)[2] = (a)[2] * (1.0f - t2) + (b)[2] * t2;		\
}

#define VEC_XLERP_OUT_4(r,a,b,t)						\
{														\
	float t2 = -(t * t) + 2*t;							\
	(r)[0] = (a)[0] * (1.0f - t2) + (b)[0] * t2;		\
	(r)[1] = (a)[1] * (1.0f - t2) + (b)[1] * t2;		\
	(r)[2] = (a)[2] * (1.0f - t2) + (b)[2] * t2;		\
	(r)[3] = (a)[3] * (1.0f - t2) + (b)[3] * t2;		\
}



#define VEC_XLERP_BOTH_1(r,a,b,t)						\
{														\
	float t2 = t*t;										\
	if ((t2) < 1)										\
		(r) = ((b) - (a)) / 2 * t2 * t2 + (a);			\
	else													\
		(r) = ((a) - (b)) / 2 * ((t2-1) * (t2-3) - 1) + (a);	\
}

#define VEC_XLERP_BOTH_2(r,a,b,t)						\
{														\
	float t2 = t*2;										\
	if ((t2) < 1) {													\
		(r)[0] = ((b)[0] - (a)[0]) / 2 * t2 * t2 + (a)[0];				\
		(r)[1] = ((b)[1] - (a)[1]) / 2 * t2 * t2 + (a)[1];				\
	} else{																\
		(r)[0] = ((a)[0] - (b)[0]) / 2 * ((t2-1) * (t2-3) - 1) + (a)[0];	\
		(r)[1] = ((a)[1] - (b)[1]) / 2 * ((t2-1) * (t2-3) - 1) + (a)[1];	\
	}																	\
}

#define VEC_XLERP_BOTH(r,a,b,t)							\
{														\
	float t2 = t*2;										\
	if ((t2) < 1) {													\
		(r)[0] = ((b)[0] - (a)[0]) / 2 * t2 * t2 + (a)[0];				\
		(r)[1] = ((b)[1] - (a)[1]) / 2 * t2 * t2 + (a)[1];				\
		(r)[2] = ((b)[2] - (a)[2]) / 2 * t2 * t2 + (a)[2];				\
	} else{																\
		(r)[0] = ((a)[0] - (b)[0]) / 2 * ((t2-1) * (t2-3) - 1) + (a)[0];	\
		(r)[1] = ((a)[1] - (b)[1]) / 2 * ((t2-1) * (t2-3) - 1) + (a)[1];	\
		(r)[2] = ((a)[2] - (b)[2]) / 2 * ((t2-1) * (t2-3) - 1) + (a)[2];	\
	}																	\
}

#define VEC_XLERP_BOTH_4(r,a,b,t)						\
{														\
	float t2 = t*2;										\
	if ((t2) < 1) {													\
		(r)[0] = ((b)[0] - (a)[0]) / 2 * t2 * t2 + (a)[0];				\
		(r)[1] = ((b)[1] - (a)[1]) / 2 * t2 * t2 + (a)[1];				\
		(r)[2] = ((b)[2] - (a)[2]) / 2 * t2 * t2 + (a)[2];				\
		(r)[3] = ((b)[3] - (a)[3]) / 2 * t2 * t2 + (a)[3];				\
	} else{																\
		(r)[0] = ((a)[0] - (b)[0]) / 2 * ((t2-1) * (t2-3) - 1) + (a)[0];	\
		(r)[1] = ((a)[1] - (b)[1]) / 2 * ((t2-1) * (t2-3) - 1) + (a)[1];	\
		(r)[2] = ((a)[2] - (b)[2]) / 2 * ((t2-1) * (t2-3) - 1) + (a)[2];	\
		(r)[3] = ((a)[3] - (b)[3]) / 2 * ((t2-1) * (t2-3) - 1) + (a)[3];	\
	}																	\
}






#define VEC_LERP_1(r,a,b,t)					\
{											\
	(r) = (((b) - (a)) * t) + (a);	\
}

#define VEC_LERP_2(r,a,b,t)					\
{											\
	(r)[0] = (((b)[0] - (a)[0]) * t) + (a)[0];	\
	(r)[1] = (((b)[1] - (a)[1]) * t) + (a)[1];	\
}

#define VEC_LERP(r,a,b,t)					\
{											\
	(r)[0] = (((b)[0] - (a)[0]) * t) + (a)[0];	\
	(r)[1] = (((b)[1] - (a)[1]) * t) + (a)[1];	\
	(r)[2] = (((b)[2] - (a)[2]) * t) + (a)[2];	\
}

#define VEC_LERP_4(r,a,b,t)					\
{											\
	(r)[0] = (((b)[0] - (a)[0]) * t) + (a)[0];	\
	(r)[1] = (((b)[1] - (a)[1]) * t) + (a)[1];	\
	(r)[2] = (((b)[2] - (a)[2]) * t) + (a)[2];	\
	(r)[3] = (((b)[3] - (a)[3]) * t) + (a)[3];	\
}

#define VEC_BEZIER_2(r,p1,p2,p3,mu)			\
{											\
	float mum1,mum12,mu2;					\
	mu2 = mu * mu;							\
	mum1 = 1 - mu;							\
	mum12 = mum1 * mum1;					\
	(r)[0] = (p1)[0] * mum12 + 2 * (p2)[0] * mum1 * mu + (p3)[0] * mu2;	\
	(r)[1] = (p1)[1] * mum12 + 2 * (p2)[1] * mum1 * mu + (p3)[1] * mu2;	\
}

#define VEC_BEZIER(r,p1,p2,p3,mu)			\
{											\
	float mum1,mum12,mu2;					\
	mu2 = mu * mu;							\
	mum1 = 1 - mu;							\
	mum12 = mum1 * mum1;					\
	(r)[0] = (p1)[0] * mum12 + 2 * (p2)[0] * mum1 * mu + (p3)[0] * mu2;	\
	(r)[1] = (p1)[1] * mum12 + 2 * (p2)[1] * mum1 * mu + (p3)[1] * mu2;	\
	(r)[2] = (p1)[2] * mum12 + 2 * (p2)[2] * mum1 * mu + (p3)[2] * mu2;	\
}

#define VEC_BEZIER_4(r,p1,p2,p3,mu)			\
{											\
	float mum1,mum12,mu2;					\
	mu2 = mu * mu;							\
	mum1 = 1 - mu;							\
	mum12 = mum1 * mum1;					\
	(r)[0] = (p1)[0] * mum12 + 2 * (p2)[0] * mum1 * mu + (p3)[0] * mu2;	\
	(r)[1] = (p1)[1] * mum12 + 2 * (p2)[1] * mum1 * mu + (p3)[1] * mu2;	\
	(r)[2] = (p1)[2] * mum12 + 2 * (p2)[2] * mum1 * mu + (p3)[2] * mu2;	\
	(r)[3] = (p1)[3] * mum12 + 2 * (p2)[3] * mum1 * mu + (p3)[3] * mu2;	\
}

#pragma mark -

#define VEC_ROT_Z_2(v,n,q)									\
{															\
v[0] = (n)[0]*cos(q)-(n)[1]*sin(q);						\
v[1] = (n)[0]*sin(q)+(n)[1]*cos(q);						\
}



#define VEC_ROT_X(v,n,q)									\
{															\
	v[0] = (n)[0];											\
	v[1] = (n)[1]*cos(q)-(n)[2]*sin(q);						\
	v[2] = (n)[1]*sin(q)+(n)[2]*cos(q);						\
}

#define VEC_ROT_Y(v,n,q)									\
{															\
	v[0] = (n)[2]*sin(q)+(n)[0]*cos(q);						\
	v[1] = (n)[1];											\
	v[2] = (n)[2]*cos(q)-(n)[0]*sin(q);						\
}

#define VEC_ROT_Z(v,n,q)									\
{															\
	v[0] = (n)[0]*cos(q)-(n)[1]*sin(q);						\
	v[1] = (n)[0]*sin(q)+(n)[1]*cos(q);						\
	v[2] = (n)[2];											\
}


#define VEC_ROT_LEFT(v,n)				\
{										\
   v[0] = -(n)[1];						\
   v[1] = (n)[0];						\
   v[2] = (n)[2];						\
}

#define VEC_ROT_RIGHT(v,n)				\
{										\
   v[0] = (n)[1];						\
   v[1] = -(n)[0];						\
   v[2] = (n)[2];						\
}

#define VEC_ROT_LEFT_ABOUT_Y(v,n)		\
{										\
	v[0] = -(n)[2];						\
	v[1] = (n)[1];						\
	v[2] = (n)[0];						\
}

#define VEC_ROT_RIGHT_ABOUT_Y(v,n)				\
{										\
	v[0] = (n)[2];						\
	v[1] = (n)[1];						\
	v[2] = -(n)[0];						\
}

#define VEC_ROUND_PRECISION_1(v,a,b)                \
{                                                   \
    (v) = floorf((a) * (10*(b))) / (10*(b));        \
}

#define VEC_ROUND_PRECISION_2(v,a,b)                    \
        {                                               \
	(v)[0] = floorf((a)[0] * (10*(b))) / (10*(b));      \
	(v)[1] = floorf((a)[1] * (10*(b))) / (10*(b));      \
}


#define VEC_SET_2(v,a,b)			\
{						\
	(v)[0] = a;					\
	(v)[1] = b;					\
}

#define VEC_SET(v,a,b, c)			\
{						\
   (v)[0] = a;					\
   (v)[1] = b;					\
   (v)[2] = c;					\
}

#define VEC_SET_4(v,a,b,c,e)			\
{						\
   (v)[0] = a;					\
   (v)[1] = b;					\
   (v)[2] = c;					\
   (v)[3] = e;					\
}

#define VEC_MULT(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] * (v1)[0];		\
   (v21)[1] = (v2)[1] * (v1)[1];		\
   (v21)[2] = (v2)[2] * (v1)[2];		\
}
	
#define VEC_MULT_4(v21,v2,v1)			\
{						\
	(v21)[0] = (v2)[0] * (v1)[0];		\
	(v21)[1] = (v2)[1] * (v1)[1];		\
	(v21)[2] = (v2)[2] * (v1)[2];		\
	(v21)[3] = (v2)[3] * (v1)[3];		\
}

/* ========================================================== */
/* Zero out a 2D vector */

#define VEC_ZERO_2(a)				\
{						\
   (a)[0] = (a)[1] = 0.0;			\
}

/* ========================================================== */
/* Zero out a 3D vector */

#define VEC_ZERO(a)				\
{						\
   (a)[0] = (a)[1] = (a)[2] = 0.0;		\
}

/* ========================================================== */
/* Zero out a 4D vector */

#define VEC_ZERO_4(a)				\
{						\
   (a)[0] = (a)[1] = (a)[2] = (a)[3] = 0.0;	\
}

/* ========================================================== */
/* Vector copy */

#define VEC_COPY_2(b,a)				\
{						\
   (b)[0] = (a)[0];				\
   (b)[1] = (a)[1];				\
}

/* ========================================================== */
/* Copy 3D vector */

#define VEC_COPY(b,a)				\
{						\
   (b)[0] = (a)[0];				\
   (b)[1] = (a)[1];				\
   (b)[2] = (a)[2];				\
}

/* ========================================================== */
/* Copy 4D vector */

#define VEC_COPY_4(b,a)				\
{						\
   (b)[0] = (a)[0];				\
   (b)[1] = (a)[1];				\
   (b)[2] = (a)[2];				\
   (b)[3] = (a)[3];				\
}

/* ========================================================== */
/* Vector difference */

#define VEC_DIFF_2(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] - (v1)[0];		\
   (v21)[1] = (v2)[1] - (v1)[1];		\
}

/* ========================================================== */
/* Vector difference */

#define VEC_DIFF(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] - (v1)[0];		\
   (v21)[1] = (v2)[1] - (v1)[1];		\
   (v21)[2] = (v2)[2] - (v1)[2];		\
}

/* ========================================================== */
/* Vector difference */

#define VEC_DIFF_4(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] - (v1)[0];		\
   (v21)[1] = (v2)[1] - (v1)[1];		\
   (v21)[2] = (v2)[2] - (v1)[2];		\
   (v21)[3] = (v2)[3] - (v1)[3];		\
}

/* ========================================================== */
/* Vector sum */

#define VEC_SUM_2(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] + (v1)[0];		\
   (v21)[1] = (v2)[1] + (v1)[1];		\
}

/* ========================================================== */
/* Vector sum */

#define VEC_SUM(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] + (v1)[0];		\
   (v21)[1] = (v2)[1] + (v1)[1];		\
   (v21)[2] = (v2)[2] + (v1)[2];		\
}

/* ========================================================== */
/* Vector sum */

#define VEC_SUM_4(v21,v2,v1)			\
{						\
   (v21)[0] = (v2)[0] + (v1)[0];		\
   (v21)[1] = (v2)[1] + (v1)[1];		\
   (v21)[2] = (v2)[2] + (v1)[2];		\
   (v21)[3] = (v2)[3] + (v1)[3];		\
}

/* ========================================================== */
/* scalar times vector */

#define VEC_SCALE_2(c,a,b)			\
{						\
   (c)[0] = (a)*(b)[0];				\
   (c)[1] = (a)*(b)[1];				\
}

/* ========================================================== */
/* scalar times vector */

#define VEC_SCALE(c,a,b)			\
{						\
   (c)[0] = (a)*(b)[0];				\
   (c)[1] = (a)*(b)[1];				\
   (c)[2] = (a)*(b)[2];				\
}

/* ========================================================== */
/* scalar times vector */

#define VEC_SCALE_4(c,a,b)			\
{						\
   (c)[0] = (a)*(b)[0];				\
   (c)[1] = (a)*(b)[1];				\
   (c)[2] = (a)*(b)[2];				\
   (c)[3] = (a)*(b)[3];				\
}

/* ========================================================== */
/* accumulate scaled vector */

#define VEC_ACCUM_2(c,a,b)			\
{						\
   (c)[0] += (a)*(b)[0];			\
   (c)[1] += (a)*(b)[1];			\
}

/* ========================================================== */
/* accumulate scaled vector */

#define VEC_ACCUM(c,a,b)			\
{						\
   (c)[0] += (a)*(b)[0];			\
   (c)[1] += (a)*(b)[1];			\
   (c)[2] += (a)*(b)[2];			\
}

/* ========================================================== */
/* accumulate scaled vector */

#define VEC_ACCUM_4(c,a,b)			\
{						\
   (c)[0] += (a)*(b)[0];			\
   (c)[1] += (a)*(b)[1];			\
   (c)[2] += (a)*(b)[2];			\
   (c)[3] += (a)*(b)[3];			\
}

/* ========================================================== */
/* Vector dot product */

#define VEC_DOT_PRODUCT_2(c,a,b)			\
{							\
   c = (a)[0]*(b)[0] + (a)[1]*(b)[1];			\
}

/* ========================================================== */
/* Vector dot product */

#define VEC_DOT_PRODUCT(c,a,b)				\
{							\
   c = (a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2];	\
}

/* ========================================================== */
/* Vector dot product */

#define VEC_DOT_PRODUCT_4(c,a,b)			\
{							\
   c = (a)[0]*(b)[0] + (a)[1]*(b)[1] + (a)[2]*(b)[2] + (a)[3]*(b)[3] ;	\
}

/* ========================================================== */
/* vector impact parameter (squared) */

#define VEC_IMPACT_SQ(bsq,direction,position)		\
{							\
   gleDouble len, llel;					\
   VEC_DOT_PRODUCT (len, position, position);		\
   VEC_DOT_PRODUCT (llel, direction, position);		\
   bsq = len - llel*llel;				\
}

/* ========================================================== */
/* vector impact parameter */

#define VEC_IMPACT(bsq,direction,position)		\
{							\
   VEC_IMPACT_SQ(bsq,direction,position);		\
   bsq = sqrt (bsq);					\
}

/* ========================================================== */
/* Vector length */

#define VEC_LENGTH_2(len,a)			\
{						\
   len = a[0]*a[0] + a[1]*a[1];			\
   len = sqrt (len);				\
}

/* ========================================================== */
/* Vector length */

#define VEC_FAST_LENGTH(len,a)			\
{						\
   len = (a)[0]*(a)[0] + (a)[1]*(a)[1];		\
   len += (a)[2]*(a)[2];			\
}

#define VEC_FAST_LENGTH_2(len,a)			\
{						\
len = (a)[0]*(a)[0] + (a)[1]*(a)[1];		\
}

#define VEC_LENGTH(len,a)			\
{						\
   len = (a)[0]*(a)[0] + (a)[1]*(a)[1];		\
   len += (a)[2]*(a)[2];			\
   len = sqrt (len);				\
}

/* ========================================================== */
/* Vector length */

#define VEC_LENGTH_4(len,a)			\
{						\
   len = (a)[0]*(a)[0] + (a)[1]*(a)[1];		\
   len += (a)[2]*(a)[2];			\
   len += (a)[3] * (a)[3];			\
   len = sqrt (len);				\
}

/* ========================================================== */
/* distance between two points */

#define VEC_FAST_DISTANCE(len,va,vb)			\
{						\
    double tmp[4];				\
    VEC_DIFF (tmp, vb, va);			\
    VEC_FAST_LENGTH (len, tmp);			\
}

#define VEC_FAST_DISTANCE_2(len,va,vb)			\
{						\
	double tmp[2];				\
	VEC_DIFF_2 (tmp, vb, va);			\
	VEC_FAST_LENGTH_2 (len, tmp);			\
}

#define VEC_DISTANCE(len,va,vb)			\
{						\
    double tmp[4];				\
    VEC_DIFF (tmp, vb, va);			\
    VEC_LENGTH (len, tmp);			\
}

#define VEC_DISTANCE_2(len,va,vb)			\
{						\
	double tmp[4];				\
	VEC_DIFF_2 (tmp, vb, va);			\
	VEC_LENGTH_2 (len, tmp);			\
}

/* ========================================================== */
/* Vector length */

#define VEC_CONJUGATE_LENGTH(len,a)		\
{						\
   len = 1.0 - a[0]*a[0] - a[1]*a[1] - a[2]*a[2];\
   len = sqrt (len);				\
}

/* ========================================================== */
/* Vector length */

#define VEC_NORMALIZE_2(a)			\
{						\
	double len;					\
	VEC_LENGTH_2(len,a);				\
	if (len != 0.0) {				\
		len = 1.0 / len;				\
		a[0] *= len;				\
		a[1] *= len;				\
	}						\
}


#define VEC_NORMALIZE(a)			\
{						\
   double len;					\
   VEC_LENGTH (len,a);				\
   if (len != 0.0) {				\
      len = 1.0 / len;				\
      a[0] *= len;				\
      a[1] *= len;				\
      a[2] *= len;				\
   }						\
}

#define VEC_FAST_NORMALIZE(a)			\
{						\
   double len;					\
   VEC_FAST_LENGTH (len,a);				\
   if (len != 0.0) {				\
      len = 1.0 / len;				\
      a[0] *= len;				\
      a[1] *= len;				\
      a[2] *= len;				\
   }						\
}

/* ========================================================== */
/* Vector length */

#define VEC_RENORMALIZE(a,newlen)		\
{						\
   double len;					\
   VEC_LENGTH (len,a);				\
   if (len != 0.0) {				\
      len = newlen / len;				\
      a[0] *= len;				\
      a[1] *= len;				\
      a[2] *= len;				\
   }						\
}

/* ========================================================== */
/* 3D Vector cross product yeilding vector */

#define VEC_CROSS_PRODUCT(c,a,b)		\
{						\
   c[0] = (a)[1] * (b)[2] - (a)[2] * (b)[1];	\
   c[1] = (a)[2] * (b)[0] - (a)[0] * (b)[2];	\
   c[2] = (a)[0] * (b)[1] - (a)[1] * (b)[0];	\
}

/* ========================================================== */
/* Vector perp -- assumes that n is of unit length 
 * accepts vector v, subtracts out any component parallel to n */

#define VEC_PERP(vp,v,n)			\
{						\
   double dot;					\
						\
   VEC_DOT_PRODUCT (dot, v, n);			\
   vp[0] = (v)[0] - (dot) * (n)[0];		\
   vp[1] = (v)[1] - (dot) * (n)[1];		\
   vp[2] = (v)[2] - (dot) * (n)[2];		\
}

/* ========================================================== */
/* Vector parallel -- assumes that n is of unit length 
 * accepts vector v, subtracts out any component perpendicular to n */

#define VEC_PARALLEL(vp,v,n)			\
{						\
   double dot;					\
						\
   VEC_DOT_PRODUCT (dot, v, n);			\
   vp[0] = (dot) * (n)[0];			\
   vp[1] = (dot) * (n)[1];			\
   vp[2] = (dot) * (n)[2];			\
}

/* ========================================================== */
/* Vector reflection -- assumes n is of unit length */
/* Takes vector v, reflects it against reflector n, and returns vr */

#define VEC_REFLECT(vr,v,n)			\
{						\
   double dot;					\
						\
   VEC_DOT_PRODUCT (dot, v, n);			\
   vr[0] = (v)[0] - 2.0 * (dot) * (n)[0];	\
   vr[1] = (v)[1] - 2.0 * (dot) * (n)[1];	\
   vr[2] = (v)[2] - 2.0 * (dot) * (n)[2];	\
}

/* ========================================================== */
/* Vector blending */
/* Takes two vectors a, b, blends them together */ 

#define VEC_BLEND(vr,sa,a,sb,b)			\
{						\
						\
   vr[0] = (sa) * (a)[0] + (sb) * (b)[0];	\
   vr[1] = (sa) * (a)[1] + (sb) * (b)[1];	\
   vr[2] = (sa) * (a)[2] + (sb) * (b)[2];	\
}

/* ========================================================== */
/* Vector print */

#define VEC_PRINT_2(a)					\
{							\
   double len;						\
   VEC_LENGTH_2 (len, a);					\
   printf (" a is %f %f length of a is %f \n", a[0], a[1], len); \
}

/* ========================================================== */
/* Vector print */

#define VEC_PRINT(file, a)					\
{							\
   double len;						\
   VEC_LENGTH (len, (a));				\
   fprintf (file, " a is %f %f %f length of a is %f \n", (a)[0], (a)[1], (a)[2], len); \
}

/* ========================================================== */
/* Vector print */

#define VEC_PRINT_4(a)					\
{							\
   double len;						\
   VEC_LENGTH_4 (len, (a));				\
   printf (" a is %f %f %f %f length of a is %f \n",	\
       (a)[0], (a)[1], (a)[2], (a)[3], len);		\
}

/* ========================================================== */
/* print matrix */

#define MAT_PRINT_4X4(mmm) {				\
   int i,j;						\
   printf ("matrix mmm is \n");				\
   if (mmm == NULL) {					\
      printf (" Null \n");				\
   } else {						\
      for (i=0; i<4; i++) {				\
         for (j=0; j<4; j++) {				\
            printf ("%f ", mmm[i+j*4]);			\
         }						\
         printf (" \n");				\
      }							\
   }							\
}

/* ========================================================== */
/* print matrix */

#define MAT_PRINT_3X3(mmm) {				\
   int i,j;						\
   printf ("matrix mmm is \n");				\
   if (mmm == NULL) {					\
      printf (" Null \n");				\
   } else {						\
      for (i=0; i<3; i++) {				\
         for (j=0; j<3; j++) {				\
            printf ("%f ", mmm[i][j]);			\
         }						\
         printf (" \n");				\
      }							\
   }							\
}

/* ========================================================== */
/* print matrix */

#define MAT_PRINT_2X3(mmm) {				\
   int i,j;						\
   printf ("matrix mmm is \n");				\
   if (mmm == NULL) {					\
      printf (" Null \n");				\
   } else {						\
      for (i=0; i<2; i++) {				\
         for (j=0; j<3; j++) {				\
            printf ("%f ", mmm[i][j]);			\
         }						\
         printf (" \n");				\
      }							\
   }							\
}

/* ========================================================== */
/* initialize matrix */

#define IDENTIFY_MATRIX_3X3(m)			\
{						\
   m[0] = 1.0;				\
   m[1] = 0.0;				\
   m[2] = 0.0;				\
						\
   m[3] = 0.0;				\
   m[4] = 1.0;				\
   m[5] = 0.0;				\
						\
   m[6] = 0.0;				\
   m[7] = 0.0;				\
   m[8] = 1.0;				\
}

/* ========================================================== */
/* initialize matrix */

#define IDENTIFY_MATRIX_4X4(m)			\
{						\
   m[0] = 1.0;				\
   m[1] = 0.0;				\
   m[2] = 0.0;				\
   m[3] = 0.0;				\
						\
   m[4] = 0.0;				\
   m[5] = 1.0;				\
   m[6] = 0.0;				\
   m[7] = 0.0;				\
						\
   m[8] = 0.0;				\
   m[9] = 0.0;				\
   m[10] = 1.0;				\
   m[11] = 0.0;				\
						\
   m[12] = 0.0;				\
   m[13] = 0.0;				\
   m[14] = 0.0;				\
   m[15] = 1.0;				\
}
	
/* ========================================================== */
/* axis angle rotation matrix */
	
#define CREATE_ROTATE_AXIS_MATRIX_4X4(m, ang, x, y, z)										\
{																					\
	float rcos, rsin, invrcos, angle;												\
	angle = ang * 0.017453292519943295769236907684886f;								\
	rcos = cosf(angle);																\
	rsin = sinf(angle);																\
	invrcos = 1.0f - rcos;															\
																					\
	m[0] =  rcos+ x*x*invrcos;													\
	m[4] =  z*rsin+ y*x*invrcos;												\
	m[8] = -y*rsin+ z*x*invrcos;												\
	m[12]= 0;																		\
	m[1] = -z*rsin+ x*y*invrcos;												\
	m[5] =  rcos+ y*y*invrcos;													\
	m[9] =  x*rsin+ z*y*invrcos;												\
	m[13]= 0;																		\
	m[2] =  y*rsin+ x*z*invrcos;												\
	m[6] = -x*rsin+ y*z*invrcos;												\
	m[10]=  rcos+ z*z*invrcos;													\
	m[14]= 0;																		\
	m[3] = 0;																		\
	m[7] = 0;																		\
	m[11]= 0;																		\
	m[15]= 1;																		\
}
	
#define CREATE_ROTATE_AXIS_MATRIX_4X4_FAST(m, ang, x, y, z)										\
{																					\
float rcos, rsin, invrcos, angle;												\
angle = ang * 0.017453292519943295769236907684886f;								\
rcos = cosf(angle);																\
rsin = sinf(angle);																\
invrcos = 1.0f - rcos;															\
\
m[0] =  rcos+ x*x*invrcos;													\
m[4] =  z*rsin+ y*x*invrcos;												\
m[8] = -y*rsin+ z*x*invrcos;												\
m[1] = -z*rsin+ x*y*invrcos;												\
m[5] =  rcos+ y*y*invrcos;													\
m[9] =  x*rsin+ z*y*invrcos;												\
m[2] =  y*rsin+ x*z*invrcos;												\
m[6] = -x*rsin+ y*z*invrcos;												\
m[10]=  rcos+ z*z*invrcos;													\
m[15]= 1;																		\
}
	
/* ========================================================== */
/* scale matrix */
	
#define CREATE_SCALE_MATRIX_4X4(m, x, y, z)											\
{																					\
	m[0]=x;    m[4]=0;    m[8]=0;     m[12]=0;										\
	m[1]=0;    m[5]=y;    m[9]=0;     m[13]=0;										\
	m[2]=0;    m[6]=0;    m[10]=z;    m[14]=0;										\
	m[3]=0;    m[7]=0;    m[11]=0;    m[15]=1;										\
}
	
#define CREATE_SCALE_MATRIX_4X4_FAST(m, x, y, z)											\
{																					\
m[0]=x;										\
m[5]=y;										\
m[10]=z;										\
m[15]=1;										\
}
	
/* ========================================================== */
/* translation matrix */
	
#define CREATE_TRANSLATE_MATRIX_4X4(m, x, y, z)										\
{																					\
	m[0]=1; m[4]=0; m[8]=0;  m[12]=x;												\
	m[1]=0; m[5]=1; m[9]=0;  m[13]=y;												\
	m[2]=0; m[6]=0; m[10]=1; m[14]=z;												\
	m[3]=0; m[7]=0; m[11]=0; m[15]=1;												\
}
	
#define CREATE_TRANSLATE_MATRIX_4X4_FAST(m, x, y, z)							\
{																				\
m[0]=1; m[12]=x;												\
m[5]=1; m[13]=y;												\
m[10]=1; m[14]=z;												\
m[15]=1;												\
}

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_2X2(b,a)	memcpy(b, a, sizeof(float) * 4)

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_2X3(b,a)	memcpy(b, a, sizeof(float) * 6)

/* ========================================================== */
/* matrix copy */

#define COPY_MATRIX_3X3(b,a)	memcpy(b, a, sizeof(float) * 9)

/* ========================================================== */
/* matrix copy */
	
#define COPY_MATRIX_4X4(b,a)	memcpy(b, a, sizeof(float) * 16)

/* ========================================================== */
/* matrix transpose */

#define TRANSPOSE_MATRIX_2X2(b,a)	\
{				\
   b[0] = a[0];		\
   b[1] = a[4];		\
				\
   b[4] = a[1];		\
   b[5] = a[5];		\
}

/* ========================================================== */
/* matrix transpose */

#define TRANSPOSE_MATRIX_3X3(b,a)	\
{				\
   b[0] = a[0];		\
   b[1] = a[4];		\
   b[2] = a[8];		\
				\
   b[4] = a[1];		\
   b[5] = a[5];		\
   b[6] = a[9];		\
				\
   b[8] = a[2];		\
   b[9] = a[6];		\
   b[10] = a[10];		\
}

/* ========================================================== */
/* matrix transpose */

#define TRANSPOSE_MATRIX_4X4(b,a)	\
{				\
   b[0] = a[0];		\
   b[1] = a[4];		\
   b[2] = a[8];		\
   b[3] = a[12];		\
				\
   b[4] = a[1];		\
   b[5] = a[5];		\
   b[6] = a[9];		\
   b[7] = a[13];		\
				\
   b[8] = a[2];		\
   b[9] = a[6];		\
   b[10] = a[10];		\
   b[11] = a[14];		\
				\
   b[12] = a[3];		\
   b[13] = a[7];		\
   b[14] = a[11];		\
   b[15] = a[15];		\
}

/* ========================================================== */
/* multiply matrix by scalar */

#define SCALE_MATRIX_2X2(b,s,a)		\
{					\
   b[0] = (s) * a[0];		\
   b[1] = (s) * a[1];		\
					\
   b[4] = (s) * a[4];		\
   b[5] = (s) * a[5];		\
}

/* ========================================================== */
/* multiply matrix by scalar */

#define SCALE_MATRIX_3X3(b,s,a)		\
{					\
   b[0] = (s) * a[0];		\
   b[1] = (s) * a[1];		\
   b[2] = (s) * a[2];		\
					\
   b[4] = (s) * a[4];		\
   b[5] = (s) * a[5];		\
   b[6] = (s) * a[6];		\
					\
   b[8] = (s) * a[8];		\
   b[9] = (s) * a[9];		\
   b[10] = (s) * a[10];		\
}

/* ========================================================== */
/* multiply matrix by scalar */

#define SCALE_MATRIX_4X4(b,s,a)		\
{					\
   b[0] = (s) * a[0];		\
   b[1] = (s) * a[1];		\
   b[2] = (s) * a[2];		\
   b[3] = (s) * a[3];		\
					\
   b[4] = (s) * a[4];		\
   b[5] = (s) * a[5];		\
   b[6] = (s) * a[6];		\
   b[7] = (s) * a[7];		\
					\
   b[8] = (s) * a[8];		\
   b[9] = (s) * a[9];		\
   b[10] = (s) * a[10];		\
   b[11] = (s) * a[11];		\
					\
   b[12] = s * a[12];		\
   b[13] = s * a[13];		\
   b[14] = s * a[14];		\
   b[15] = s * a[15];		\
}

/* ========================================================== */
/* multiply matrix by scalar */

#define ACCUM_SCALE_MATRIX_2X2(b,s,a)		\
{					\
   b[0] += (s) * a[0];		\
   b[1] += (s) * a[1];		\
					\
   b[4] += (s) * a[4];		\
   b[5] += (s) * a[5];		\
}

/* +========================================================== */
/* multiply matrix by scalar */

#define ACCUM_SCALE_MATRIX_3X3(b,s,a)		\
{					\
   b[0] += (s) * a[0];		\
   b[1] += (s) * a[1];		\
   b[2] += (s) * a[2];		\
					\
   b[4] += (s) * a[4];		\
   b[5] += (s) * a[5];		\
   b[6] += (s) * a[6];		\
					\
   b[8] += (s) * a[8];		\
   b[9] += (s) * a[9];		\
   b[10] += (s) * a[10];		\
}

/* +========================================================== */
/* multiply matrix by scalar */

#define ACCUM_SCALE_MATRIX_4X4(b,s,a)		\
{					\
   b[0] += (s) * a[0];		\
   b[1] += (s) * a[1];		\
   b[2] += (s) * a[2];		\
   b[3] += (s) * a[3];		\
					\
   b[4] += (s) * a[4];		\
   b[5] += (s) * a[5];		\
   b[6] += (s) * a[6];		\
   b[7] += (s) * a[7];		\
					\
   b[8] += (s) * a[8];		\
   b[9] += (s) * a[9];		\
   b[10] += (s) * a[10];		\
   b[11] += (s) * a[11];		\
					\
   b[12] += (s) * a[12];		\
   b[13] += (s) * a[13];		\
   b[14] += (s) * a[14];		\
   b[15] += (s) * a[15];		\
}

/* +========================================================== */
/* matrix product */
/* c[x][y] = a[x][0]*b[0][y]+a[x][1]*b[1][y]+a[x][2]*b[2][y]+a[x][3]*b[3][y];*/

#define MATRIX_PRODUCT_2X2(c,a,b)		\
{						\
   c[0] = a[0]*b[0]+a[1]*b[4];	\
   c[1] = a[0]*b[1]+a[1]*b[5];	\
						\
   c[4] = a[4]*b[0]+a[5]*b[4];	\
   c[5] = a[4]*b[1]+a[5]*b[5];	\
						\
}

/* ========================================================== */
/* matrix product */
/* c[x][y] = a[x][0]*b[0][y]+a[x][1]*b[1][y]+a[x][2]*b[2][y]+a[x][3]*b[3][y];*/

#define MATRIX_PRODUCT_3X3(c,a,b)				\
{								\
   c[0] = a[0]*b[0]+a[1]*b[4]+a[2]*b[8];	\
   c[1] = a[0]*b[1]+a[1]*b[5]+a[2]*b[9];	\
   c[2] = a[0]*b[2]+a[1]*b[6]+a[2]*b[10];	\
								\
   c[4] = a[4]*b[0]+a[5]*b[4]+a[6]*b[8];	\
   c[5] = a[4]*b[1]+a[5]*b[5]+a[6]*b[9];	\
   c[6] = a[4]*b[2]+a[5]*b[6]+a[6]*b[10];	\
								\
   c[8] = a[8]*b[0]+a[9]*b[4]+a[10]*b[8];	\
   c[9] = a[8]*b[1]+a[9]*b[5]+a[10]*b[9];	\
   c[10] = a[8]*b[2]+a[9]*b[6]+a[10]*b[10];	\
}

/* ========================================================== */
/* matrix product */
/* c[x][y] = a[x][0]*b[0][y]+a[x][1]*b[1][y]+a[x][2]*b[2][y]+a[x][3]*b[3][y];*/

#define MATRIX_PRODUCT_4X4(c,a,b)		\
{														\
	c[0] = a[0]*b[0]+a[1]*b[4]+a[2]*b[8]+a[3]*b[12];	\
	c[1] = a[0]*b[1]+a[1]*b[5]+a[2]*b[9]+a[3]*b[13];	\
	c[2] = a[0]*b[2]+a[1]*b[6]+a[2]*b[10]+a[3]*b[14];	\
	c[3] = a[0]*b[3]+a[1]*b[7]+a[2]*b[11]+a[3]*b[15];	\
														\
	c[4] = a[4]*b[0]+a[5]*b[4]+a[6]*b[8]+a[7]*b[12];	\
	c[5] = a[4]*b[1]+a[5]*b[5]+a[6]*b[9]+a[7]*b[13];	\
	c[6] = a[4]*b[2]+a[5]*b[6]+a[6]*b[10]+a[7]*b[14];	\
	c[7] = a[4]*b[3]+a[5]*b[7]+a[6]*b[11]+a[7]*b[15];	\
														\
	c[8] = a[8]*b[0]+a[9]*b[4]+a[10]*b[8]+a[11]*b[12];	\
	c[9] = a[8]*b[1]+a[9]*b[5]+a[10]*b[9]+a[11]*b[13];	\
	c[10] = a[8]*b[2]+a[9]*b[6]+a[10]*b[10]+a[11]*b[14];	\
	c[11] = a[8]*b[3]+a[9]*b[7]+a[10]*b[11]+a[11]*b[15];	\
															\
	c[12] = a[12]*b[0]+a[13]*b[4]+a[14]*b[8]+a[15]*b[12];	\
	c[13] = a[12]*b[1]+a[13]*b[5]+a[14]*b[9]+a[15]*b[13];	\
	c[14] = a[12]*b[2]+a[13]*b[6]+a[14]*b[10]+a[15]*b[14];	\
	c[15] = a[12]*b[3]+a[13]*b[7]+a[14]*b[11]+a[15]*b[15];	\
}
	
#define MATRIX_PRODUCT_AB_TRANSLATE_4X4(b,x,y,z)				\
{															\
	b[12] = x*b[0]+y*b[4]+z*b[8]+b[12];						\
	b[13] = x*b[1]+y*b[5]+z*b[9]+b[13];						\
	b[14] = x*b[2]+y*b[6]+z*b[10]+b[14];					\
	b[15] = x*b[3]+y*b[7]+z*b[11]+b[15];					\
}
	

#define MATRIX_PRODUCT_BA_TRANSLATE_4X4(a,x,y,z)		\
{									\
	a[0] = a[0]+a[3]*x;				\
	a[1] = a[1]+a[3]*y;				\
	a[2] = a[2]+a[3]*z;				\
	a[3] = a[3];					\
									\
	a[4] = a[4]+a[7]*x;				\
	a[5] = a[5]+a[7]*y;				\
	a[6] = a[6]+a[7]*z;				\
	a[7] = a[7];					\
									\
	a[8] = a[8]+a[11]*x;			\
	a[9] = a[9]+a[11]*y;			\
	a[10] = a[10]+a[11]*z;			\
	a[11] = a[11];					\
									\
	a[12] = a[12]+a[15]*x;			\
	a[13] = a[13]+a[15]*y;			\
	a[14] = a[14]+a[15]*z;			\
	a[15] = a[15];					\
}	
	
/* ========================================================== */
/* matrix times vector */

#define MAT_DOT_VEC_2X2(p,m,v)					\
{								\
   p[0] = m[0]*v[0] + m[2]*v[1];				\
   p[1] = m[1]*v[0] + m[3]*v[1];				\
}

/* ========================================================== */
/* matrix times vector */

#define MAT_DOT_VEC_3X3(p,m,v)					\
{								\
   p[0] = m[0]*v[0] + m[3]*v[1] + m[6]*v[2];		\
   p[1] = m[1]*v[0] + m[4]*v[1] + m[7]*v[2];		\
   p[2] = m[2]*v[0] + m[5]*v[1] + m[8]*v[2];		\
}
	
#define MAT_DOT_VEC_3X4(p,m,v)					\
{								\
	p[0] = m[0]*v[0] + m[4]*v[1] + m[8]*v[2] + m[12];	\
	p[1] = m[1]*v[0] + m[5]*v[1] + m[9]*v[2] + m[13];	\
	p[2] = m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14];	\
}

/* ========================================================== */
/* matrix times vector */

#define MAT_DOT_VEC_4X4(p,m,v)					\
{								\
   p[0] = m[0]*v[0] + m[4]*v[1] + m[8]*v[2] + m[12]*v[3];	\
   p[1] = m[1]*v[0] + m[5]*v[1] + m[9]*v[2] + m[13]*v[3];	\
   p[2] = m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14]*v[3];	\
   p[3] = m[3]*v[0] + m[7]*v[1] + m[11]*v[2] + m[15]*v[3];	\
}
	
	/*
	 *x=(fx*mat->data[0])+(fy*mat->data[1])+(fz*mat->data[2])+mat->data[3];
	 *y=(fx*mat->data[4])+(fy*mat->data[5])+(fz*mat->data[6])+mat->data[7];
	 *z=(fx*mat->data[8])+(fy*mat->data[9])+(fz*mat->data[10])+mat->data[11];*/

/* ========================================================== */
/* vector transpose times matrix */
/* p[j] = v[0]*m[0][j] + v[1]*m[1][j] + v[2]*m[2][j]; */

#define VEC_DOT_MAT_3X3(p,v,m)					\
{								\
   p[0] = v[0]*m[0] + v[1]*m[3] + v[2]*m[6];		\
   p[1] = v[0]*m[1] + v[1]*m[4] + v[2]*m[7];		\
   p[2] = v[0]*m[2] + v[1]*m[5] + v[2]*m[8];		\
}

/* ========================================================== */
/* affine matrix times vector */
/* The matrix is assumed to be an affine matrix, with last two 
 * entries representing a translation */

#define MAT_DOT_VEC_2X3(p,m,v)					\
{								\
   p[0] = m[0]*v[0] + m[1]*v[1] + m[2];		\
   p[1] = m[4]*v[0] + m[5]*v[1] + m[6];		\
}

/* ========================================================== */
/* inverse transpose of matrix times vector
 *
 * This macro computes inverse transpose of matrix m, 
 * and multiplies vector v into it, to yeild vector p
 *
 * DANGER !!! Do Not use this on normal vectors!!!
 * It will leave normals the wrong length !!!
 * See macro below for use on normals.
 */

#define INV_TRANSP_MAT_DOT_VEC_2X2(p,m,v)			\
{								\
   gleDouble det;						\
								\
   det = m[0]*m[5] - m[1]*m[4];			\
   p[0] = m[5]*v[0] - m[4]*v[1];				\
   p[1] = - m[1]*v[0] + m[0]*v[1];			\
								\
   /* if matrix not singular, and not orthonormal, then renormalize */ \
   if ((det!=1.0) && (det != 0.0)) {				\
      det = 1.0 / det;						\
      p[0] *= det;						\
      p[1] *= det;						\
   }								\
}

/* ========================================================== */
/* transform normal vector by inverse transpose of matrix 
 * and then renormalize the vector 
 *
 * This macro computes inverse transpose of matrix m, 
 * and multiplies vector v into it, to yeild vector p
 * Vector p is then normalized.
 */


#define NORM_XFORM_2X2(p,m,v)					\
{								\
   double len;							\
								\
   /* do nothing if off-diagonals are zero and diagonals are 	\
    * equal */							\
   if ((m[1] != 0.0) || (m[4] != 0.0) || (m[0] != m[5])) { \
      p[0] = m[5]*v[0] - m[4]*v[1];			\
      p[1] = - m[1]*v[0] + m[0]*v[1];			\
								\
      len = p[0]*p[0] + p[1]*p[1];				\
      len = 1.0 / sqrt (len);					\
      p[0] *= len;						\
      p[1] *= len;						\
   } else {							\
      VEC_COPY_2 (p, v);					\
   }								\
}

/* ========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define OUTER_PRODUCT_2X2(m,v,t)				\
{								\
   m[0] = v[0] * t[0];					\
   m[1] = v[0] * t[1];					\
								\
   m[4] = v[1] * t[0];					\
   m[5] = v[1] * t[1];					\
}

/* ========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define OUTER_PRODUCT_3X3(m,v,t)				\
{								\
   m[0] = v[0] * t[0];					\
   m[1] = v[0] * t[1];					\
   m[2] = v[0] * t[2];					\
								\
   m[4] = v[1] * t[0];					\
   m[5] = v[1] * t[1];					\
   m[6] = v[1] * t[2];					\
								\
   m[8] = v[2] * t[0];					\
   m[9] = v[2] * t[1];					\
   m[10] = v[2] * t[2];					\
}

/* ========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define OUTER_PRODUCT_4X4(m,v,t)				\
{								\
   m[0] = v[0] * t[0];					\
   m[1] = v[0] * t[1];					\
   m[2] = v[0] * t[2];					\
   m[3] = v[0] * t[3];					\
								\
   m[4] = v[1] * t[0];					\
   m[5] = v[1] * t[1];					\
   m[6] = v[1] * t[2];					\
   m[7] = v[1] * t[3];					\
								\
   m[8] = v[2] * t[0];					\
   m[9] = v[2] * t[1];					\
   m[10] = v[2] * t[2];					\
   m[11] = v[2] * t[3];					\
								\
   m[12] = v[3] * t[0];					\
   m[13] = v[3] * t[1];					\
   m[14] = v[3] * t[2];					\
   m[15] = v[3] * t[3];					\
}

/* +========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define ACCUM_OUTER_PRODUCT_2X2(m,v,t)				\
{								\
   m[0] += v[0] * t[0];					\
   m[1] += v[0] * t[1];					\
								\
   m[4] += v[1] * t[0];					\
   m[5] += v[1] * t[1];					\
}

/* +========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define ACCUM_OUTER_PRODUCT_3X3(m,v,t)				\
{								\
   m[0] += v[0] * t[0];					\
   m[1] += v[0] * t[1];					\
   m[2] += v[0] * t[2];					\
								\
   m[4] += v[1] * t[0];					\
   m[5] += v[1] * t[1];					\
   m[6] += v[1] * t[2];					\
								\
   m[8] += v[2] * t[0];					\
   m[9] += v[2] * t[1];					\
   m[10] += v[2] * t[2];					\
}

/* +========================================================== */
/* outer product of vector times vector transpose 
 *
 * The outer product of vector v and vector transpose t yeilds 
 * dyadic matrix m.
 */

#define ACCUM_OUTER_PRODUCT_4X4(m,v,t)				\
{								\
   m[0] += v[0] * t[0];					\
   m[1] += v[0] * t[1];					\
   m[2] += v[0] * t[2];					\
   m[3] += v[0] * t[3];					\
								\
   m[4] += v[1] * t[0];					\
   m[5] += v[1] * t[1];					\
   m[6] += v[1] * t[2];					\
   m[7] += v[1] * t[3];					\
								\
   m[8] += v[2] * t[0];					\
   m[9] += v[2] * t[1];					\
   m[10] += v[2] * t[2];					\
   m[11] += v[2] * t[3];					\
								\
   m[12] += v[3] * t[0];					\
   m[13] += v[3] * t[1];					\
   m[14] += v[3] * t[2];					\
   m[15] += v[3] * t[3];					\
}

/* +========================================================== */
/* determinant of matrix
 *
 * Computes determinant of matrix m, returning d
 */

#define DETERMINANT_2X2(d,m)					\
{								\
   d = m[0] * m[5] - m[1] * m[4];			\
}

/* ========================================================== */
/* determinant of matrix
 *
 * Computes determinant of matrix m, returning d
 */

#define DETERMINANT_3X3(d,m)					\
{								\
   d = m[0] * (m[5]*m[10] - m[6] * m[9]);		\
   d -= m[1] * (m[4]*m[10] - m[6] * m[8]);	\
   d += m[2] * (m[4]*m[9] - m[5] * m[8]);	\
}

/* ========================================================== */
/* i,j,th cofactor of a 4x4 matrix
 *
 */

#define COFACTOR_4X4_IJ(fac,m,i,j) 				\
{								\
   int ii[4], jj[4], k;						\
								\
   /* compute which row, columnt to skip */			\
   for (k=0; k<i; k++) ii[k] = k;				\
   for (k=i; k<3; k++) ii[k] = k+1;				\
   for (k=0; k<j; k++) jj[k] = k;				\
   for (k=j; k<3; k++) jj[k] = k+1;				\
								\
   (fac) = m[ii[0]][jj[0]] * (m[ii[1]][jj[1]]*m[ii[2]][jj[2]] 	\
                            - m[ii[1]][jj[2]]*m[ii[2]][jj[1]]); \
   (fac) -= m[ii[0]][jj[1]] * (m[ii[1]][jj[0]]*m[ii[2]][jj[2]]	\
                             - m[ii[1]][jj[2]]*m[ii[2]][jj[0]]);\
   (fac) += m[ii[0]][jj[2]] * (m[ii[1]][jj[0]]*m[ii[2]][jj[1]]	\
                             - m[ii[1]][jj[1]]*m[ii[2]][jj[0]]);\
								\
   /* compute sign */						\
   k = i+j;							\
   if ( k != (k/2)*2) {						\
      (fac) = -(fac);						\
   }								\
}

/* ========================================================== */
/* determinant of matrix
 *
 * Computes determinant of matrix m, returning d
 */

#define DETERMINANT_4X4(d,m)					\
{								\
   double cofac;						\
   COFACTOR_4X4_IJ (cofac, m, 0, 0);				\
   d = ((float*)m)[0] * cofac;						\
   COFACTOR_4X4_IJ (cofac, m, 0, 1);				\
   d += ((float*)m)[1] * cofac;					\
   COFACTOR_4X4_IJ (cofac, m, 0, 2);				\
   d += ((float*)m)[2] * cofac;					\
   COFACTOR_4X4_IJ (cofac, m, 0, 3);				\
   d += ((float*)m)[3] * cofac;					\
}

/* ========================================================== */
/* cofactor of matrix
 *
 * Computes cofactor of matrix m, returning a
 */

#define COFACTOR_2X2(a,m)					\
{								\
   a[0] = (m)[5];						\
   a[1] = - (m)[4];						\
   a[4] = - (m)[1];						\
   a[5] = (m)[0];						\
}

/* ========================================================== */
/* cofactor of matrix
 *
 * Computes cofactor of matrix m, returning a
 */

#define COFACTOR_3X3(a,m)					\
{								\
   a[0] = m[5]*m[10] - m[6]*m[9];			\
   a[1] = - (m[4]*m[10] - m[8]*m[6]);		\
   a[2] = m[4]*m[9] - m[5]*m[8];			\
   a[4] = - (m[1]*m[10] - m[2]*m[9]);		\
   a[5] = m[0]*m[10] - m[2]*m[8];			\
   a[6] = - (m[0]*m[9] - m[1]*m[8]);		\
   a[8] = m[1]*m[6] - m[2]*m[5];			\
   a[9] = - (m[0]*m[6] - m[2]*m[4]);		\
   a[10] = m[0]*m[5] - m[1]*m[4]);		\
}

/* ========================================================== */
/* cofactor of matrix
 *
 * Computes cofactor of matrix m, returning a
 */

#define COFACTOR_4X4(a,m)					\
{								\
   int i,j;							\
								\
   for (i=0; i<4; i++) {					\
      for (j=0; j<4; j++) {					\
         COFACTOR_4X4_IJ (a[i][j], m, i, j);			\
      }								\
   }								\
}

/* ========================================================== */
/* adjoint of matrix
 *
 * Computes adjoint of matrix m, returning a
 * (Note that adjoint is just the transpose of the cofactor matrix)
 */

#define ADJOINT_2X2(a,m)					\
{								\
   a[0] = (m)[5];						\
   a[4] = - (m)[4];						\
   a[1] = - (m)[1];						\
   a[5] = (m)[0];						\
}

/* ========================================================== */
/* adjoint of matrix
 *
 * Computes adjoint of matrix m, returning a
 * (Note that adjoint is just the transpose of the cofactor matrix)
 */


#define ADJOINT_3X3(a,m)					\
{								\
   a[0] = m[5]*m[10] - m[6]*m[9];			\
   a[4] = - (m[4]*m[10] - m[8]*m[6]);		\
   a[8] = m[4]*m[9] - m[5]*m[8];			\
   a[1] = - (m[1]*m[10] - m[2]*m[9]);		\
   a[5] = m[0]*m[10] - m[2]*m[8];			\
   a[9] = - (m[0]*m[9] - m[1]*m[8]);		\
   a[2] = m[1]*m[6] - m[2]*m[5];			\
   a[6] = - (m[0]*m[6] - m[2]*m[4]);		\
   a[10] = m[0]*m[5] - m[1]*m[4]);		\
}

/* ========================================================== */
/* adjoint of matrix
 *
 * Computes adjoint of matrix m, returning a
 * (Note that adjoint is just the transpose of the cofactor matrix)
 */

#define ADJOINT_4X4(a,m)					\
{								\
   int i,j;							\
								\
   for (i=0; i<4; i++) {					\
      for (j=0; j<4; j++) {					\
         COFACTOR_4X4_IJ (a[j][i], m, i, j);			\
      }								\
   }								\
}

/* ========================================================== */
/* compute adjoint of matrix and scale
 *
 * Computes adjoint of matrix m, scales it by s, returning a
 */

#define SCALE_ADJOINT_2X2(a,s,m)				\
{								\
   a[0] = (s) * m[5];					\
   a[4] = - (s) * m[4];					\
   a[1] = - (s) * m[1];					\
   a[5] = (s) * m[0];					\
}

/* ========================================================== */
/* compute adjoint of matrix and scale
 *
 * Computes adjoint of matrix m, scales it by s, returning a
 */

#define SCALE_ADJOINT_3X3(a,s,m)				\
{								\
   a[0] = (s) * (m[5] * m[10] - m[6] * m[9]);	\
   a[4] = (s) * (m[6] * m[8] - m[4] * m[10]);	\
   a[8] = (s) * (m[4] * m[9] - m[5] * m[8]);	\
								\
   a[1] = (s) * (m[2] * m[9] - m[1] * m[10]);	\
   a[5] = (s) * (m[0] * m[10] - m[2] * m[8]);	\
   a[9] = (s) * (m[1] * m[8] - m[0] * m[9]);	\
								\
   a[2] = (s) * (m[1] * m[6] - m[2] * m[5]);	\
   a[6] = (s) * (m[2] * m[4] - m[0] * m[6]);	\
   a[10] = (s) * (m[0] * m[5] - m[1] * m[4]);	\
}

/* ========================================================== */
/* compute adjoint of matrix and scale
 *
 * Computes adjoint of matrix m, scales it by s, returning a
 */

#define SCALE_ADJOINT_4X4(a,s,m)				\
{								\
   int i,j;							\
								\
   for (i=0; i<4; i++) {					\
      for (j=0; j<4; j++) {					\
         COFACTOR_4X4_IJ (a[j][i], m, i, j);			\
         a[j][i] *= s;						\
      }								\
   }								\
}

/* ========================================================== */
/* inverse of matrix 
 *
 * Compute inverse of matrix a, returning determinant m and 
 * inverse b
 */

#define INVERT_2X2(b,det,a)			\
{						\
   double tmp;					\
   DETERMINANT_2X2 (det, a);			\
   tmp = 1.0 / (det);				\
   SCALE_ADJOINT_2X2 (b, tmp, a);		\
}

/* ========================================================== */
/* inverse of matrix 
 *
 * Compute inverse of matrix a, returning determinant m and 
 * inverse b
 */

#define INVERT_3X3(b,det,a)			\
{						\
   double tmp;					\
   DETERMINANT_3X3 (det, a);			\
   tmp = 1.0 / (det);				\
   SCALE_ADJOINT_3X3 (b, tmp, a);		\
}

/* ========================================================== */
/* inverse of matrix 
 *
 * Compute inverse of matrix a, returning determinant m and 
 * inverse b
 */

#define INVERT_4X4(b,det,a)			\
{						\
   double tmp;					\
   DETERMINANT_4X4 (det, a);			\
   tmp = 1.0 / (det);				\
   SCALE_ADJOINT_4X4 (b, tmp, a);		\
}

#endif
