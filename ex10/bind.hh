#include <thread>
#include <vector>
#include <random>
#include <mutex>
#include <condition_variable>
#include <iostream>

#ifndef BIND_HH
#define BIND_HH
class bind{
	public:
		bind(){
			pDone = false; 
			condition = false;
		}
		
		void produce(int& ID,int& set_T_MAX, int& maxiter){
			pDone=false;
			std::unique_lock<std::mutex> lck(mtx);
			std::mt19937 m(std::random_device{}());		//Mersenne twister
			int t_max=set_T_MAX;
			std::uniform_int_distribution<int> distr(0., t_max);
			int n;
			int count=0;
			while(count<maxiter){
				n = distr(m);
				std::cout<<"Pushed numbers: "<<n<<std::endl;
				theQ.push_back(n);
				condition=true;
				cv_consume.notify_one();
				std::this_thread::sleep_for(std::chrono::milliseconds(n));
				cv_produce.wait(lck);
				std::cout<<"Size of the queue: "<<theQ.size()<<std::endl;
				count++;
			}
			pDone=true;
			cv_consume.notify_all();
		}

		void consume(int& ID){
			std::unique_lock<std::mutex> l(mtx);		//Unique_lock for consume()
			while(theQ.size()>=1){
				std::mt19937 m(std::random_device{}());		//Mersenne twister
				int s_max = theQ.size();
				std::uniform_int_distribution<int> dist(0.,s_max);
				int i = dist(m);
				std::cout<<"i = "<<i<<" Size = "<<theQ.size()<<std::endl;
				std::cout<<"ID: "<<ID<<": Popped: "<<theQ[i-1]<<std::endl;
				theQ.erase(theQ.begin()+i-1);
				if(!pDone){
					cv_produce.notify_one();						//First notify and THEN wait. 
					condition=false;
					cv_consume.wait(l);
				}else{
					break;
				}
			}
		}
		
		void fillQ(){
			std::mt19937 m(std::random_device{}());		//Mersenne twister
			int t_max=1000;
			std::uniform_int_distribution<int> distr(0., t_max);
			int n;
			for(int i=0;i<50;i++){
				n = distr(m);
				theQ.push_back(n);
			}
		}
		
		void createThreads(int set_tmax,int maxiter,int numConsumerThreads){
			if((unsigned int) numConsumerThreads>std::thread::hardware_concurrency()-1){
				std::cout<<"\033[1;31mWarning!\033[0m Using too many threads might be inefficient!"<<std::endl;
				std::cout<<"Recommended maximum number of consumer threads on this machine is "<<std::thread::hardware_concurrency()-1<<"!"<<std::endl;
			}
			fillQ();
			int IDsend=0;int IDconsume=1;
			std::thread sender = std::thread(&bind::produce,this,std::ref(IDsend),std::ref(set_tmax),std::ref(maxiter));
			std::thread consumer[numConsumerThreads];
			for(int i=0; i<numConsumerThreads; i++){
				int ID=IDconsume+i;
				consumer[i] = std::thread(&bind::consume,this,std::ref(ID));
			}
			sender.join();
			for(int i=0; i<numConsumerThreads; i++){
				consumer[i].join();
			}
		}
		
	private:
		std::vector<int> theQ;
		std::mutex mtx;
		std::condition_variable cv_produce;
		std::condition_variable cv_consume;
		bool condition;
		bool pDone;
};
#endif
