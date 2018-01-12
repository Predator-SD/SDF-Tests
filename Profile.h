#ifndef PROFILE_H
#define PROFILE_H

#include "DS.h"

#define maxx(a,b) ((a)>(b)?(a):(b))
#define minn(a,b) ((a)<(b)?(a):(b))
#define abss(x) ((x)<0?(-(x)):(x))

#define EPS 1e-8

class Profile{
public:
	Profile(){}
	~Profile(){}
	
	virtual bool Inter(Point p,Vector d){
		return false;
	}

	virtual bool Inter(Point p,Vector d,Point &inter){
		return false;
	}

	virtual bool Inside(Point p){
		return false;
	}

	virtual bool OnBound(Point p){
		return false;
	}

	virtual Vector GetN(Point p){
		return (Vector){0,1};
	}

	virtual bool Contained(double up,double down,double left,double right){
		return false;
	}
};

//Half Plane ---> Ax+By+C>0
class HalfPlane :
public Profile{
protected:
	double A,B,C;
	Vector Norm;
	
	inline void CalInter(Point p,Vector d,Point &inter){
		inter.x =(B*d.y*p.x-B*d.x*p.y-C*d.x)/(A*d.x+B*d.y);
		if(d.x) inter.y=(inter.x-p.x)*d.y/d.x+p.y;
		else inter.y=(-C-A*inter.x)/B;
	}

public:
	HalfPlane(const double a,const double b,const double c):A(a),B(b),C(c){
		Norm=(Vector){-a,-b}.norm();
	}
	HalfPlane(const Point p1,const Point p2,const Point inner){
		A=p2.y-p1.y;
		B=p1.x-p2.x;
		C=p2.x*p1.y-p1.x*p2.y;

		if(!Inside(inner))
			A=-A,
			B=-B,
			C=-C;

		Norm=(Vector){-A,-B}.norm();
	}

	Vector GetN(Point p){
		return Norm;
	}

	bool Inside(Point p){
		return p.x*A+p.y*B+C>=0.0;
	}

	bool OnBound(Point p){
		return abss(p.x*A+p.y*B+C)<=EPS;
	}

	bool Inter(Point p,Vector d){
		if(Inside(p)) return true;
		if(d*Norm<0.0){
            Point inter;
            CalInter(p,d,inter);
            //if(inter.isvaild())
			return true;
		}
		return false;
	}

	bool Inter(Point p,Vector d,Point &inter){
		if(Inside(p)){
			CalInter(p,d,inter);
			if((inter-p)*d<0.0) inter=(Point){-1.0,-1.0};
			//inter=p;
			return true;
		}
		if(d*Norm<0.0){
			CalInter(p,d,inter);
			//if(inter.isvaild())
			return true;
		}
		return false;
	}
};

class Circle :
public Profile{
protected:
	Point O;
	double R;

public:
	Circle(const Point o,const double r):O(o),R(r){}

	inline Point GetO(void){
		return O;
	}
	inline double GetR(void){
		return R;
	}

	bool Iniside(Point p){
		return (O-p).len()<=R+EPS;
	}

	Vector GetN(Point p){
		return (p-O).norm();
	}

	bool OnBound(Point p){
		return abss((O-p).len()-R)<=EPS;
	}

	bool Inter(Point p,Vector d){
		if(Inside(p)) return true;
		double proj=(O-p)*d;
		if(proj<=0) return false;
		Point foot=p+d*proj;
		double dis=(O-foot).len();
		return dis<R;
	}

	bool Inter(Point p,Vector d,Point &inter){
		if(Inside(p)){
			puts("0");
			double proj=(O-p)*d;
			Point foot=p+d*proj;
			double dis1=(O-foot).len();
			double dis2=sqrt(R*R-dis1*dis1);
			inter=foot+d*dis2;
			//inter=p;
			return true;
		}
		double proj=(O-p)*d;
		if(proj<=0) return false;
		Point foot=p+d*proj;
		double dis1=(O-foot).len();
		if(dis1>R) return false;
		double dis2=sqrt(R*R-dis1*dis1);
		inter=foot-d*dis2;
		return true;
	}
};

class PUnion :
public Profile{
private:
	Profile *pr1;
	Profile *pr2;

public:
	PUnion(Profile *p1,Profile *p2):pr1(p1),pr2(p2){}
	/*
	~PUnion(){
		delete pr1;
		delete pr2;
	}
	*/

	bool Inside(Point p){
		return pr1->Inside(p)||pr2->Inside(p);
	}

	bool OnBound(Point p){
		return (pr1->OnBound(p))||(pr2->OnBound(p));
	}

	Vector GetN(Point p){
		if(pr1->OnBound(p)&&pr2->OnBound(p))
			return (pr1->GetN(p)+pr2->GetN(p))/2.0;

		if(pr1->OnBound(p))
			return pr1->GetN(p);
		if(pr2->OnBound(p))
			return pr2->GetN(p);

		return (Vector){0.0,1.0};
	}

	bool Inter(Point p,Vector d){
		Point inter1,inter2;
		return (!(pr1->Inter(p,d,inter1)||pr2->Inter(p,d,inter2)))?false:true;
	}

	bool Inter(Point p,Vector d,Point &inter){
		Point inter1,inter2;
		bool r1=pr1->Inter(p,d,inter1),r2=pr2->Inter(p,d,inter2);
		if(!r1&&!r2) return false;

		if(!r1)
			inter=inter2;
		else if(!r2)
			inter=inter1;
		else
			inter=(inter1-p).len()>(inter2-p).len()?inter2:inter1;

		return true;
	}
};

class PIntersect :
public Profile{
private:
	Profile *pr1;
	Profile *pr2;

public:
	PIntersect(Profile *p1,Profile *p2):pr1(p1),pr2(p2){}
	
	/*
	~PIntersect(){
		delete pr1;
		delete pr2;
	}
	*/

	bool Inside(Point p){
		return pr1->Inside(p)&&pr2->Inside(p);
	}

	bool OnBound(Point p){
		return pr1->OnBound(p)||pr2->OnBound(p);
	}

	Vector GetN(Point p){
		if(pr1->OnBound(p)&&pr2->OnBound(p))
			return (pr1->GetN(p)+pr2->GetN(p))/2.0;

		if(pr1->OnBound(p))
			return pr1->GetN(p);
		if(pr2->OnBound(p))
			return pr2->GetN(p);

		return (Vector){0.0,1.0};
	}

	bool Inter(Point p,Vector d){
		Point inter1,inter2;
		return !(pr1->Inter(p,d,inter1)&&pr2->Inter(p,d,inter2))?false:(pr2->Inside(inter1)||pr1->Inside(inter2));
	}

	bool Inter(Point p,Vector d,Point &inter){
		Point inter1,inter2;
		if(!(pr1->Inter(p,d,inter1)&&pr2->Inter(p,d,inter2))) return false;

		if(pr2->Inside(inter1)&&pr1->Inside(inter2))
			inter=(inter1-p).len()>(inter2-p).len()?inter2:inter1;
		else if(pr2->Inside(inter1))
			inter=inter1;
		else if(pr1->Inside(inter2))
			inter=inter2;
		else return false;

		return true;
	}
};

#endif
