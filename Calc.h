#ifndef CALC_H
#define CALC_H

#include<cstdio>
#include<windows.h>

#include "Entity.h"
#include "svpng.h"

Space* Scene(void){
	Circle* c1=new Circle({0.5,0.6},0.1);
	Circle* c2=new Circle({0.5,0.7},0.12);
	Profile* su1=new PUnion(c1,c2);
	Profile* triangle=Polygon({{0.9,0.5},{0.7,0.5},{0.8,0.4}});
	Profile* quad1=Polygon({{0.3,0.4},{0.4,0.4},{0.4,0.3},{0.3,0.3}});
	Profile* quad2=Polygon({{0.2,0.5},{0.3,0.5},{0.3,0.6},{0.2,0.6}});
	Entity* e1=new Entity(su1,{1.8,0.9,0.7},0.0);
	Entity* e2=new Entity(triangle,(Color){0.2,0.9,1.1},0.1);
	Entity* e3=new Entity(quad1,{0.05,0.05,0.2},0.8);
	Entity* e4=new Entity(quad2,{0.05,0.05,0.2},0.8);
	return new Space({e1,e2,e3,e4});
}

Space* zheshe(){
	//Circle* c1 = new Circle({ 1.0f, -0.5f }, 0.05f);
	Circle* c3 = new Circle({ 0.0f, -0.5f }, 0.05f);
	//HalfPlane* l1 = new HalfPlane(0.f, 1.f, -0.3f);
	Circle* c2 = new Circle({ 0.5f, 0.3f }, 0.075f);
	//Profile* si1 = new PIntersect(l1, c2);
	//Entity* e1 = new Entity(c1, { 2.f, 9.f, 11.f });
	Entity* e2 = new Entity(c3, { 11.f, 2.f, 9.f });
	double refract[3] = { 1.5f, 1.5f, 1.5f };
	Entity* e4 = new Entity(c2, { 0.0, 0.0, 0.0 }, 0.1, 0.9, refract);
	return new Space({ e2, e4});
}

Space* s2(){
	Circle *c=new Circle({-0.1,-0.1},0.05);
	Profile *q=Polygon({{0.2,0.4},{0.8,0.4},{0.8,0.6},{0.2,0.6}});
	Entity *e1=new Entity(c,{10,10,10},0.0);
	double r[3]={1.2,1.4,1.6};
	Entity *e2=new Entity(q,{0.01,0.01,0.01},0.2,0.8,r);
	return new Space({e1,e2});
}

Space* ttt() //ÈýÀâ¾µºÍ¼¤¹âµÆ
{
	Circle* c1 = new Circle({ 0.5f, -0.5f }, 0.05f);
	//Shape* triangle = GeneratePolygon({ { 0.3f,0.3f },{ 0.7f, 0.3f },{ 0.5f, 0.5f } });
	Profile* triangle = Polygon({ { 0.3f,0.3f },{ 0.7f, 0.4f },{ 0.7f, 0.2f } });
	Entity* e1 = new Spotlight(c1, { 75.f, 75.f, 75.f });
	double refract[3] = { 1.2f, 1.4f, 1.6f };
	Entity* e2 = new Entity(triangle, { 0.01f, 0.12f, 0.17f }, 0.2f, 1.f, refract);
	return new Space({ e1, e2});
}

int id,n;
int begin,end;
unsigned int pixels=0;

unsigned char img[W*H*3];
Space *s=ttt();

void Generate(const int &l1,const int &l2){
	unsigned char *p=img+3*W*l1;
	for(register int y=l1;y<l2;++y,pixels+=W)
		for(register int x=0;x<W;++x,p+=3){
			Color color=s->Integrate((Point){(double)x/W,(double)y/H});
			p[0]=(int)minn(color.r*255.0,255.0);
			p[1]=(int)minn(color.g*255.0,255.0);
			p[2]=(int)minn(color.b*255.0,255.0);
		}
}

inline void prelude(void){
	freopen("config","r",stdin);
	scanf("%d%d",&n,&id);
	fclose(stdin);
	
	end=H*id/n;
	begin=end-H/n;
}

DWORD WINAPI solve1(LPVOID pm){
	int y1=begin,y2=begin+(end-begin)/4;
	Generate(y1,y2);
    return 0;
}

DWORD WINAPI solve2(LPVOID pm){
	int y1=begin+(end-begin)/4,y2=begin+(end-begin)/2;
	Generate(y1,y2);
    return 0;
}

DWORD WINAPI solve3(LPVOID pm){
	int y1=begin+(end-begin)/2,y2=begin+(end-begin)*3/4;
	Generate(y1,y2);
    return 0;
}

DWORD WINAPI solve4(LPVOID pm){
	int y1=begin+(end-begin)*3/4,y2=end;
	Generate(y1,y2);
    return 0;
}

void EXEC(void){
	HANDLE handle1=CreateThread(NULL,0,solve1,NULL,0,NULL);
    HANDLE handle2=CreateThread(NULL,0,solve2,NULL,0,NULL);
    HANDLE handle3=CreateThread(NULL,0,solve3,NULL,0,NULL);
    HANDLE handle4=CreateThread(NULL,0,solve4,NULL,0,NULL);
    while(pixels<W*H/n) _sleep(3000),printf("Now: %lf\%\n",100.0*pixels/(1.0*W*H/n));
    svpng(fopen("reflect.png","wb"),W,H,img,0);
}

#endif
