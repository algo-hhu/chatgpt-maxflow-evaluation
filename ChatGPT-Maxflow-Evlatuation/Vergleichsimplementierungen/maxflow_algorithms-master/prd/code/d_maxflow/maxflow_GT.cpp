#include "maxflow_GT.h"
#include "hi_pr/types.h"

#include <cmath>

maxflow_GT::maxflow_GT(){
	info.name = " GT";
};

void maxflow_GT::construct(const dynamic::num_array<int,2>  & E, dynamic::num_array<int,2> & cap, dynamic::num_array<int,1> & excess){
	using namespace exttype;
	info.construct_t.start();
	g.construct(excess.size(),E.size()[1],E.begin(),cap.begin(),excess.begin());
	info.construct_t.stop();
};

maxflow_GT::tflow maxflow_GT::maxflow(){
	info.solve_t.start();
	g.stageOne();
	info.flow = g.flow;
	g.globalUpdate(); // required to determine the minimum cut
	info.solve_t.stop();
	//info.name = txt::String::Format("GT%2.1f",g.globUpdtFreq);
	if(g.globUpdtFreq<0.001){
		info.name = "GT0";
	}else if(fabs(g.globUpdtFreq-0.5)<0.01){
		info.name = "GT05";
	}else{
		info.name = txt::String::Format("GT%2.1f",g.globUpdtFreq);
	};
	info.nV = g.n;
	info.nE = g.m;
	return info.flow;
};

void maxflow_GT::construct(const char * filename){
	info.construct_t.start();
	//g.construct(filename);
	dimacs_parser(filename,*this,2);
	info.construct_t.stop();
};

void maxflow_GT::allocate1(int n ,int m, int S, int T,int d,int * sz){
	g.n = n;
	g.nodes = (node*)calloc( (n+2),sizeof(node) );
	g.source = g.nodes+S;
	g.sink = g.nodes+T;
	g.m = 0;
	g.flow0 = 0;
	g.flow = 0;
	g.nMin = 0;
};

void maxflow_GT::allocate2(int loop){
	if(loop==0){
		g.arcs = (arc*)calloc( (2*g.m+1),sizeof(arc));
		g.cap = (cType*)calloc( (2*g.m+1),sizeof(cType));
		size_t accum_size = 0;
		//compute desired starting positions for where to allocate arcs
		for(node * v = g.nodes;v!=g.nodes+g.n+2;++v){
			size_t s = (size_t&)v->first;
			v->first = g.arcs+accum_size;
			v->current = v->first;
			accum_size+=s;
		};
		assert(accum_size<=2*g.m+1);
		g.m = 0;
	}else{//loop ==1
		g.allocDS();
		g.init();
	};
};

void maxflow_GT::read_arc(int loop,int u,int v,int cap1, int cap2){
	if(loop==0){
		//regular edge
		++g.m;
		++(size_t&)g.nodes[u].first;//for now will count how many outcoming arcs
		++(size_t&)g.nodes[v].first;
	}else{//loop==1
		//fill in arcs
		arc *& uv = g.nodes[u].current;
		arc *& vu = g.nodes[v].current;
		uv->head = g.nodes+v;
		vu->head = g.nodes+u;
		uv->resCap = cap1;
		g.cap[uv-g.arcs] = cap1;
		vu->resCap = cap2;
		g.cap[vu-g.arcs] = cap2;
		uv->rev = vu;
		vu->rev = uv;
		++uv;
		++vu;
		++g.m;
	};		
};

void maxflow_GT::save_cut(const std::string & filename){
	FILE * f = fopen(filename.c_str(),"wt+");
	setvbuf(f,NULL,_IOFBF,1024*1024*2);
	fprintf(f,"p max %lli %lli\n",(long long)g.n, (long long)g.m);
	fprintf(f,"c minimum cut, generated by %s\n",info.name.c_str());
	fprintf(f,"f %lli\n",info.flow);
	//fprintf(f,"n 1 1\n");//source has label 1
	//fprintf(f,"n 2 0\n");//sink has label 0
	for(int v=0;v<g.n;++v){
		fprintf(f,"n %i %i\n",v+1,g.is_weak_source(&g.nodes[v]));
	};
};

void maxflow_GT::get_cut(int * C){
	for(int v=0;v<g.n;++v){
		C[v] = g.is_weak_source(&g.nodes[v]);
	};
};

long long maxflow_GT::cut_cost(){
	return g.cut_cost();
};
