#ifndef ENTITY_H
#define ENTITY_H

#include<vector>
#include<cstdlib>
#include<cstdio>
#include "Profile.h"

#define SAFE_DELETE(p) do{delete (p);(p)=NULL;}while(false)

#define PI 3.141592653589

#define W 512
#define H 512
#define N 1024
#define BIAS 1e-6
#define MAX_DEPTH 7

using std::vector;

int cnt=0;

class Entity{
protected:
	Profile *pr;
	Color EM;
	double RE;
	double RA;
	double RI[3];

public:
	Entity(Profile *p,Color e,double re=0.0,double ra=0.0,double *ri=NULL):pr(p),EM(e),RE(re),RA(ra){
		if(ri)
			RI[0]=ri[0],
			RI[1]=ri[1],
			RI[2]=ri[2];
	}

	inline Profile *GetPr(void){
		return pr;
	}
	inline Color GetEM(void){
		return EM;
	}
	inline double GetRE(void){
		return RE;
	}
	inline double GetRA(void){
		return RA;
	}
	inline double GetRI(int index){
		return RI[index];
	}
	
	virtual bool Inter(Point p,Vector d,Point &inter){
		return pr->Inter(p,d,inter);
	}
};

class Spotlight :
public Entity{
protected:
	Vector dir;
	double cosa;
	
public:
	Spotlight(Profile *s,Color e,double re=0.0,double ra=0.0,double *ri=NULL,Vector dir={0.0,1.0},double a=0.03):
		Entity(s,e,re,ra,ri),dir(dir){
		cosa=cos(a);
	}
	
	bool Inter(Point p,Vector d,Point &inter){
		if(d*(-dir)<cosa)
			return false;
		else return pr->Inter(p,d,inter);
	}
};

class Space{
protected:
	vector<Entity*> ie;

public:
	Space(vector<Entity*> e):ie(e){}

	inline vector<Entity*> GetEn(void){
		return ie;
	}

	Color Reflect(Entity *e,Point inter,Vector d,Vector normal,int index,int depth){
		if(depth>MAX_DEPTH||e->GetRE()==0.0) return (Color){0.0,0.0,0.0};
		//Vector normal=e->GetPr()->GetN(inter);
		//if(normal*d>0.0) return (Color){0.0,0.0,0.0};
		Vector red=d.reflect(normal);
		return GetColor(inter+red*BIAS,red,index,depth)*e->GetRE();
	}

	Color Refract(Entity *e,Point inter,Vector d,Vector normal,int index,int depth){
		if(depth>MAX_DEPTH||e->GetRA()==0.0) return (Color){0.0,0.0,0.0};
		//Vector normal=e->GetPr()->GetN(inter);
		
		double idotn=d*normal;
		double ri=e->GetRI(index);
		double k,a;
		
		if(idotn>0.0){
			++cnt;
			k=1.0-ri*ri*(1.0-idotn*idotn);
			if(k<0.0) return (Color){0.0,0.0,0.0};
			a=ri*idotn-sqrt(k);
		}else{
			ri=1.0/ri;
			k=1.0-ri*ri*(1.0-idotn*idotn);
			a=ri*idotn+sqrt(k);
		}
		
		Vector refract=d*ri-normal*a;
		return GetColor(inter+refract*BIAS,refract,index,depth)*e->GetRA();
	}
	
	Color GetColor(Point p,Vector d,int ci,int depth=0){
		double dis=10.0;
		int ei=-1;
		Point inter;

		for(int j=0;j<ie.size();++j){
			Point temp;
			if(ie[j]->Inter(p,d,temp)){
				double ndis=(temp-p).len();
				if(dis>ndis)
					ei=j,
					dis=ndis,
					inter=temp;
			}
		}
		if(ei>=0){
			Entity *ent=ie[ei];
			Vector normal=ent->GetPr()->GetN(inter);
			Color reflect=Reflect(ie[ei],inter,d,normal,ci,depth+1);
			Color refract=(Color){0.0,0.0,0.0};
			if(ci>=3){
				refract.r=Refract(ent,inter,d,normal,0,depth+1).r;
				refract.g=Refract(ent,inter,d,normal,1,depth+1).g;
				refract.b=Refract(ent,inter,d,normal,2,depth+1).b;
			}else
				refract=Refract(ent,inter,d,normal,ci,depth+1);
			return ent->GetEM()+reflect+refract;
		}else
			return (Color){0.0,0.0,0.0};
	}

	Color Integrate(Point p){
		Color sum=(Color){0.0,0.0,0.0};
		double theta;
		for(int i=0;i<N;++i)
			theta=2.0*PI*(i+(double)rand()/RAND_MAX)/N,
			sum=sum+GetColor(p,(Vector){cos(theta),sin(theta)},3);
		return sum/N;
	}
};

Profile *Polygon(vector<Point> points){
	Point sig=(Point){0.0,0.0};
	for(vector<Point>::iterator it=points.begin();it!=points.end();++it)
		sig.x+=(*it).x,sig.y+=(*it).y;
	Point center=(Point){sig.x/points.size(),sig.y/points.size()};
	Profile *s=NULL;
	for(vector<Point>::iterator it=points.begin();(it+1)!=points.end();++it){
		Point p1=*it,p2=*(it+1);
		HalfPlane *l=new HalfPlane(p1,p2,center);
		if(s) s=new PIntersect(s,l);
		else s=l;
	}
	s=new PIntersect(s,new HalfPlane(*(points.end()-1),*(points.begin()),center));
	return s;
}

#endif
