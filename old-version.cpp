#include<cmath>
#include<cstdlib>
#include<windows.h>
#include "svpng.inc"

using namespace std;

namespace heads{
	#define maxx(a,b) ((a)>(b)?(a):(b))
	#define minn(a,b) ((a)<(b)?(a):(b))
	#define abss(x) ((x)<0?-(x):(x))

	#define PI 3.141592653589793238
	#define EPS 1e-8

	#define W 2048
	#define H 2048

	#define N 1024
	#define MAX_STEP 256
	#define MAX_DISTANCE 7.0

	#define BIAS 1e-6
	#define MAX_DEPTH 4
}

class ZSDF{
public:
	unsigned char img[W*H*3];
	unsigned int pixels;
	double R,G,B;
	
	ZSDF(int r,int g,int b):R(r),G(g),B(b),pixels(0){}

	struct Data{
		double SD,EM,RE,ETA;
	
		inline Data unio(Data rhs){
			return (SD<rhs.SD)?*this:rhs;
		}
	
		inline Data subs(Data rhs){
			Data r=*this;
			r.SD=SD>-rhs.SD?SD:-rhs.SD;
			return r;
		}
	};

	inline double CSDF(double x,double y,double cx,double cy,double r){
		return sqrt((x-cx)*(x-cx)+(y-cy)*(y-cy))-r;
	}

	inline double PSDF(double x,double y,double px,double py,double nx,double ny){
		return (x-px)*nx+(y-py)*ny;
	}

	double RSDF(double x,double y,double rx,double ry,double ex,double ey,double theta){
		double c=cos(theta),s=sin(theta);
		double dx=abss((x-rx)*c+(y-ry)*s)-ex;
		double dy=abss((x-rx)*(-s)+(y-ry)*c)-ey;
		double ax=maxx(dx,0.0),ay=maxx(dy,0.0);
		return minn(maxx(dx,dy),0.0)+sqrt(ax*ax+ay*ay);
	}

	void Reflect(double ix,double iy,double nx,double ny,double *rx,double *ry){
		double vec=(ix*nx+iy*ny)*2.0;
		*rx=ix-nx*vec;
		*ry=iy-ny*vec;
	}

	bool Refract(double ix,double iy,double nx,double ny,double eta,double *rx,double *ry){
		double vec=ix*nx+iy*ny;
		double k=1.0-eta*eta*(1.0-vec*vec);
		if(k<0.0) return false;
		double len=eta*vec+sqrt(k);
		*rx=ix*eta-len*nx;
		*ry=iy*eta-len*ny;
		return true;
	}

	Data Phi(double x,double y){
		Data r1=(Data){CSDF(x,y,-0.2,-0.2,0.1),15.0,0.0,0.0};
		Data r2=(Data){CSDF(x,y,0.3,0.3,0.1),0.03,0.3,1.5};
		Data r3=(Data){RSDF(x,y,0.3,0.75,0.2,0.1,PI/12.0),0.01,0.3,1.5};
		Data r4=(Data){CSDF(x,y,0.75,0.2,0.072),0.03,0.3,1.5};
		return r1.unio(r2).unio(r3).unio(r4);
	}

	void Grad(double x,double y,double *nx,double *ny){
		*nx=(Phi(x+EPS,y).SD-Phi(x-EPS,y).SD)/(2.0*EPS);
		*ny=(Phi(x,y+EPS).SD-Phi(x,y-EPS).SD)/(2.0*EPS);
	}

	double Trace(double ox,double oy,double dx,double dy,int depth){
		double t(1e-4);
		Data res=Phi(ox,oy);
		double sign=res.SD>0.0?1.0:-1.0;
		for(int i(0);i<MAX_STEP&&t<MAX_DISTANCE;++i,t+=res.SD*sign){
			double x(ox+dx*t),y(oy+dy*t);
			res=Phi(x,y);
			if(res.SD*sign<EPS){
				double ans=res.EM;
				if(depth<MAX_DEPTH&&(res.RE>0.0||res.ETA>0.0)){
					double nx,ny,rx,ry;
					Grad(x,y,&nx,&ny);
					nx*=sign,ny*=sign;
					if(res.ETA>0.0)
						if(Refract(dx,dy,nx,ny,sign<0.0?res.ETA:(1.0/res.ETA),&rx,&ry))
							ans+=(1.0-res.RE)*Trace(x-nx*BIAS,y-ny*BIAS,rx,ry,depth+1);
					if(res.RE>0.0){
						Reflect(dx,dy,nx,ny,&rx,&ry);
						ans+=res.RE*Trace(x+nx*BIAS,y+ny*BIAS,rx,ry,depth+1);
					}
				}
				return ans;
			}
		}
		return 0.0;
	}

	double Integrate(double x,double y){
		double ans(0.0),theta(0.0);
		for(int i(0);i<N;++i){
			theta=(PI*2)*(i+(double)rand()/RAND_MAX)/N;
			ans+=Trace(x,y,cos(theta),sin(theta),0);
		}
		return ans/N;
	}
	
	void cal(int lb,int le){
		unsigned char *p=img+3*W*lb;
		double ans;
		for(int y(lb);y<le;++y,pixels+=W)
        	for(int x(0);x<W;++x,p+=3)
            	ans=Integrate((double)x/W,(double)y/H),
            	p[0]=(int)(minn(ans*R,R)),
            	p[1]=(int)(minn(ans*G,G)),
            	p[2]=(int)(minn(ans*B,B));
	}
};

namespace Cal{
	ZSDF test(102,204,255);

	int n,id;
	int begin,end;

	DWORD WINAPI solve1(LPVOID pm){
		int y1=begin,y2=begin+(end-begin)/4;
		test.cal(y1,y2);
    	return 0;
	}

	DWORD WINAPI solve2(LPVOID pm){
		int y1=begin+(end-begin)/4,y2=begin+(end-begin)/2;
		test.cal(y1,y2);
    	return 0;
	}

	DWORD WINAPI solve3(LPVOID pm){
		int y1=begin+(end-begin)/2,y2=begin+(end-begin)*3/4;
		test.cal(y1,y2);
    	return 0;
	}

	DWORD WINAPI solve4(LPVOID pm){
		int y1=begin+(end-begin)*3/4,y2=end;
		test.cal(y1,y2);
    	return 0;
	}
	
	inline void prelude(){
		freopen("config","r",stdin);
		scanf("%d%d",&n,&id);
		fclose(stdin);
	
		end=H*id/n;
		begin=end-H/n;
	}

	void exec(){
		HANDLE handle1=CreateThread(NULL,0,solve1,NULL,0,NULL);
    	HANDLE handle2=CreateThread(NULL,0,solve2,NULL,0,NULL);
    	HANDLE handle3=CreateThread(NULL,0,solve3,NULL,0,NULL);
    	HANDLE handle4=CreateThread(NULL,0,solve4,NULL,0,NULL);
    	while(test.pixels<W*H/n) _sleep(5000),printf("Now: %lf\%\n",100.0*test.pixels/(1.0*W*H/n));
	}
}

int main(){
	using namespace Cal;
	
	prelude();
    exec();
    svpng(fopen("result.png","wb"),W,H,test.img,0);
}
