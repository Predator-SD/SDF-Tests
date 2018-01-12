#ifndef DS_H
#define DS_H

#include<cmath>

class Vector{
public:
	double x,y;
	
	Vector():x(0),y(0){}
	Vector(const double x0,const double y0):x(x0),y(y0){}
	
	inline Vector operator + (const Vector rhs){
		return (Vector){x+rhs.x,y+rhs.y};
	}
	
	inline Vector operator - (const Vector rhs){
		return (Vector){x-rhs.x,y-rhs.y};
	}
	
	inline Vector operator - (void){
		return (Vector){-x,-y};
	}
	
	inline double operator * (const Vector rhs){
		return x*rhs.x+y*rhs.y;
	}
	
	inline Vector operator * (const double k){
		return (Vector){x*k,y*k};
	}
	
	inline Vector operator / (const double k){
		return (Vector){x/k,y/k};
	}
	
	inline double len(void){
		return sqrt(x*x+y*y);
	}
	
	inline Vector norm(void){
		double l=len();
		return l>0.0?(Vector){x/l,y/l}:(Vector){0.0,0.0};
	}
	
	inline Vector reflect(const Vector n){
		double idn2=(n.x*x+n.y*y)*2.0;
		return (Vector){x-n.x*idn2,y-n.y*idn2};
	}
};

class Point{
public:
	double x,y;
	
	Point():x(0),y(0){}
	Point(const double x0,const double y0):x(x0),y(y0){}
	
	inline Point operator + (const Vector v){
		return (Point){x+v.x,y+v.y};
	}
	
	inline Point operator - (const Vector v){
		return (Point){x-v.x,y-v.y};
	}
	
	inline Vector operator - (const Point p){
		return (Vector){x-p.x,y-p.y};
	}
	
	inline bool isvaild(void){
		return x>=0.0&&x<=1.0&&y>0.0&&y<1.0;
	}
};

class Color{
public:
	double r,g,b;
	
	Color():r(0),g(0),b(0){}
	Color(const double r0,const double g0,const double b0):r(r0),g(g0),b(b0){}
	
	inline Color operator + (const Color rhs){
		return (Color){r+rhs.r,g+rhs.g,b+rhs.b};
	}
	inline Color operator * (const double k){
		return (Color){r*k,g*k,b*k};
	}
	
	inline Color operator / (const double k){
		return (Color){r/k,g/k,b/k};
	}
	inline bool operator > (const Color rhs) const {
		return (r+g+b>rhs.r+rhs.g+rhs.b);
	}
	inline bool operator < (const Color rhs) const {
		return (r+g+b<rhs.r+rhs.g+rhs.b);
	}
};

#endif
