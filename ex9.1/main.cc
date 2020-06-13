#include <iostream>
#include <chrono>
#include <thread>

//compile with: g++ -o main main.cc -std=c++11 -pthread

void printSleepPrint(int& n,int ID){
	std::cout<<"ID: "<<ID<<". Printing before sleeping."<<std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(n));
	std::cout<<"Printing AFTER sleeping for "<<n<<" seconds."<<std::endl;
	n=n+1;
	std::cout<<"Changed n"<<ID<<" to "<<n<<"."<<std::endl;
}

void sleepPrint(){
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout<<"Thread slept for 1 second."<<std::endl;
}

int main(){
	int n1=4; //seconds
	int n2=1; //seconds
	std::thread th1{printSleepPrint,std::ref(n1),1};
	std::thread th2{printSleepPrint,std::ref(n2),2};
	std::cout<<"n1 = "<<n1<<std::endl;
	std::cout<<"n2 = "<<n2<<std::endl;			//A lot of these prints probably come out in a whacky order
	th1.join();
	th2.join();
	std::cout<<"AFTER calling join()"<<std::endl;
	std::cout<<"n1 = "<<n1<<std::endl;			//These should be ordered properly again and be modified from before
	std::cout<<"n2 = "<<n2<<std::endl;
}
