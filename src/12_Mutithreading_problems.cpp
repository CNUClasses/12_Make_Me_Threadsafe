//============================================================================
// Name        : 12_Make_Me_Threadsafe.cpp
// Author      : 
// Version     :
// Copyright   : Steal this code!
// Description : 2 deques, 
// FRAME_CREATERS thread(s) create cars and add them to qInProgress_cars deque
// WHEEL_ADDERS thread(s) add wheels to cars in qInProgress_cars dequeue and put the cars back
// in qInProgress_cars until there are 4, at which point they go into qFinished_cars
// uncomment lock_guards and atomics for flawless operation 
//============================================================================

#include <iostream>
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <chrono>
#include <time.h>
#include <mutex>
//#include <atomic>
using namespace std;

//a car 
struct car {
	int numb_wheels = 0;
};

bool bDoWork = true;
const int FRAME_CREATERS = 10;
const int WHEEL_ADDERS = 40;
const int NUMB_CARS = 10000;

std::deque<car> qInProgress_cars;	//holds cars that do not have wheels
std::deque<car> qFinished_cars;		//once cars have 4 wheels they go here

mutex mInProgress;					//used to protect qInProgress_cars 
mutex mFinished;					//used to protect qFinished_cars

//creates cars
void createcars(int i) {	
	car aCar;
	for (int j = 0; j < i; j++) {
		lock_guard<mutex> lck(mInProgress);
		qInProgress_cars.push_back(aCar);
	}
	bDoWork = false;
}

//adds wheels to cars
void addonewheel() {
	while (true) {
		//hmmm the following locks mInProgress for a lot of code
		//can it be more granular?
		lock_guard<mutex> lck(mInProgress);
		if (!qInProgress_cars.empty()) {
			
			car aCar = qInProgress_cars.front();	//get the car					
			qInProgress_cars.pop_front();			//remove from deque
					
			//need to add a wheel?
			if (aCar.numb_wheels < 4)
				aCar.numb_wheels++;
			
			//if has 4 wheels then move to back of qFinished_cars otherwise add to back of qInProgress_cars
			if (aCar.numb_wheels == 4){
				//protect access to qFinished_cars
				lock_guard<mutex> lck(mFinished);
				qFinished_cars.push_back(aCar);
			}
			else
				qInProgress_cars.push_back(aCar);
		}
		else{
			//leave if we are done working and finished putting wheels on
			if (!bDoWork) 
				break;
		}
	}
}

//goes through qFinished_cars and verifies each car has 4 wheels 
void cntWheels(){
	bool all_correct=true;
	for (auto &car : qFinished_cars) {
		if (car.numb_wheels != 4){
			all_correct = false;
			cout << "BAD CAR has " << car.numb_wheels << " wheels!" << endl;
		}
	}
	if (all_correct)
		cout << "All cars have correct number of wheels " << endl;
}

int main() {
	//holds threads creating and working on cars
	std::vector<thread> myThreads;
	cout << "Creating " << FRAME_CREATERS << " createcars threads." << "  And "
			<< WHEEL_ADDERS << " addonewheel threads" << endl;

	//create creaters and wheel people
	for (int i = 0; i < FRAME_CREATERS; ++i) {
		myThreads.push_back(std::thread(createcars, NUMB_CARS));
	}
	for (int i = 0; i < WHEEL_ADDERS; ++i) {
		myThreads.push_back(std::thread(addonewheel));
	}

	//wait for em all to finish
	for (auto& thread : myThreads) {
		thread.join();
	}

	//lets see what we have
	cout << "number cars created:" << NUMB_CARS * FRAME_CREATERS
			<< " number cars finished:" << qFinished_cars.size() << endl;
	cntWheels();
}
