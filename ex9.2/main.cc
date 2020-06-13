#include <iostream>
#include <thread>
#include <vector>
#include <random>
#include <mutex>
#include <condition_variable>

//compile with: g++ -o main main.cc -std=c++11 -pthread

std::vector<int> theQ;
std::mutex mtx;
std::condition_variable cv_produce,cv_consume;
bool condition = false;

void produce(){
	std::unique_lock<std::mutex> lck(mtx);
	std::mt19937 m(std::random_device{}());		//Mersenne twister
	int t_max=1000;
	std::uniform_int_distribution<int> distr(0., t_max);
	int n;
	while(true){
		n = distr(m);
		std::cout<<"Pushed numbers: "<<n<<std::endl;
		theQ.push_back(n);
		condition=true;
		cv_consume.notify_one();
		std::this_thread::sleep_for(std::chrono::milliseconds(n));
		cv_produce.wait(lck);
		std::cout<<"Size of the queue: "<<theQ.size()<<std::endl;
	}
}

void consume(int ID){
	std::unique_lock<std::mutex> l(mtx);		//Unique_lock for consume()
	while(true){
		std::mt19937 m(std::random_device{}());		//Mersenne twister
		int s_max=theQ.size();
		if(s_max==0){
			cv_produce.notify_one();						//First notify and THEN wait. 
			condition=false;
			cv_consume.wait(l);
		}
		std::uniform_int_distribution<int> dist(0.,s_max);
		int i = dist(m);
		std::cout<<"i = "<<i<<" Size = "<<theQ.size()<<std::endl;
		std::cout<<"ID: "<<ID<<": Popped: "<<theQ[i-1]<<std::endl;
		theQ.erase(theQ.begin()+i-1);
		cv_produce.notify_one();						//First notify and THEN wait. 
		condition=false;
		cv_consume.wait(l);
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

int main(){
	fillQ();
	std::thread sender{produce};
	std::cout<<"\033[1;32mNOTE\033[0m Maximum number of threads should not exceed "<<std::thread::hardware_concurrency()<<" on this machine."<<std::endl;
	std::thread consumers[std::thread::hardware_concurrency()-1];
	int i;
	for(i=0;i<std::thread::hardware_concurrency()-1;i++){
		consumers[i]=std::thread{consume,i+1};
	}
	sender.join();
	for(i=0;i<std::thread::hardware_concurrency()-1;i++){
		consumers[i].join();
	}
}
