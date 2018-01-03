#include <math.h>

#define M_PI 3.14f

typedef struct{
	float x,y,z;
}Point;

typedef struct{
	Point p[2];
}Vector;

typedef struct{
	Point p[10][3];
}Voxel;

void rotX(Point * p, float alpha){
	p->y = p->y * cos(alpha) - p->z * sin(alpha);
	p->z = p->y * sin(alpha) + p->z * cos(alpha);
}
void rotY(Point * p, float alpha){
	p->z = p->z * cos(alpha) - p->x * sin(alpha);
	p->x = p->z * sin(alpha) + p->x * cos(alpha);
}
void rotZ(Point * p, float alpha){
	p->x = p->x * cos(alpha) - p->y * sin(alpha);
	p->y = p->x * sin(alpha) + p->y * cos(alpha);
}

void translate(Point *p,Vector v){
	p->x += v.p[1].x - v.p[0].x;
	p->y += v.p[1].y - v.p[0].y;
	p->z += v.p[1].z - v.p[0].z;
}

void print_point(Point p){
	printf("Point:\nXYZ (%.2f,%.2f,%.2f)\n\n",p.x,p.y,p.z);
}

void print_vector(Vector v){
	printf("Vector:\nP0 = XYZ (%.2f,%.2f,%.2f)\n",v.p[0].x,v.p[0].y,v.p[0].z);
	printf("P1 = XYZ (%.2f,%.2f,%.2f)\n\n",v.p[1].x,v.p[1].y,v.p[1].z);
}

Point init_point(){
	Point p;
	p.x = p.y = p.z = 0;
	return p;
}

Vector init_vector(){
	Vector v;
	Point p = init_point();
	v.p[0] = v.p[1] = p;
	return v;
}

void setV(Vector * v,float x1,float x2,float y1,float y2,float z1,float z2){
	v->p[0].x = x1;
	v->p[0].y = y1;
	v->p[0].z = z1;
	v->p[1].x = x2;
	v->p[1].y = y2;
	v->p[1].z = z2;
}

void setP(Point * p,float x,float y,float z){
	p->x = x;
	p->y = y;
	p->z = z;
}

void main(){
	Vector v;
	Point p;
	v=init_vector();
	p=init_point();

	setP(&p,1.f,2.f,3.f);
	setV(&v,1,2,3,4,5,6);
	printf("> Before translation:\n"); 
	print_point(p);	print_vector(v);

	printf("> After translation:\n");
	translate(&p,v);
	print_point(p);	print_vector(v);

	printf("> After rotation:\n");
	printf("  >> Rot X:\n");
	rotX(&p,M_PI); print_point(p);
	printf(" >> Rot Y:\n");
	rotY(&p,M_PI); print_point(p);
	printf(" >> Rot Z:\n");
	rotZ(&p,M_PI); print_point(p);
	getch();
}
