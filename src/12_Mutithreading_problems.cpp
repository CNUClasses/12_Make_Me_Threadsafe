//============================================================================
// Name        : 12_Make_Me_Threadsafe.cpp
// Author      : 
// Version     :
// Copyright   : Steal this code!
// Description : 2 deques, 
// thread(s) create cars and add them to qInProgress_cars deque
// threads add wheels to cars in qInProgress_cars dequeue and put the cars back
// in qInProgress_cars until there are 4, at which pint they go onto qFinished_cars
//============================================================================

#include <iostream>
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <chrono>
#include <time.h>
#include <mutex>

using namespace std;

bool bDoWork = true;
const int CREATERS = 1;
const int WHEELERS = 2;
const int NUMB_CARS = 100;

mutex m1;
mutex m2;

//a car 
struct car{
	int numb_wheels 	= 0;
};

std::deque<car> qInProgress_cars;	//holds cars that do not have wheels
std::deque<car> qFinished_cars;		//once cars have 4 wheels they go here

int count1 =0;
int count2 =0;
bool bWork = true;

void output(string s){
	cout << s << endl;
}

//creates cars
void createcars(int i) {
	while(i>0){
		car aCar;
		qInProgress_cars.push_back(aCar);
		i--;
	}
	bWork = false;
}

//adds wheels to cars
void addonewheel() {
	while(true){ 
		if (!qInProgress_cars.empty()){
			//get the widget
			car aCar = qInProgress_cars.front();
			
			//remove from queue
			qInProgress_cars.pop_front();
			
			//lets see if we should add wheels
			if (aCar.numb_wheels < 4)	
				aCar.numb_wheels++;	
			
			//if has 4 wheels then done otherwise add back to inprogress
			//in any case it goes back to the end of the line 
			if (aCar.numb_wheels==4)	
				qFinished_cars.push_back(aCar);
			else
				qInProgress_cars.push_back(aCar);
		}
		
		//leave if we are done working and finished putting wheels on
		if (!bWork && qInProgress_cars.empty())
			break;
			
	}
}

int main() {
	//holds threads creating and working on cars
	std::vector<thread> myThreads;	

	cout << "Creating" <<CREATERS<<" createcars threads."<<"  And "<<WHEELERS<<" addonewheel threads"<< endl;

	//create creaters
	for (int i = 0; i < CREATERS; ++i) {
		myThreads.push_back(std::thread(createcars,NUMB_CARS));
	}
	
	//create wheel people
	for (int i = 0; i < WHEELERS; ++i) {
		myThreads.push_back(std::thread(addonewheel));
	}

	//wait for em all to finish
	for (auto& thread : myThreads) {
		thread.join();
	}
	
	//lets see what we have
	cout << "number cars created:" <<NUMB_CARS<<" number cars finished:"<<qFinished_cars.size()<< endl;
	
	for(auto &car:qFinished_cars){
		if (car.numb_wheels != 4)
			cout<<"BAD CAR has "<<car.numb_wheels<<" wheels!"<<endl;	
	}
		
}
