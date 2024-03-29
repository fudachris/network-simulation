#include <stdlib.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <unistd.h>
#include <string>
#include <chrono>
#include <typeinfo>

#include "host.h"
#include "frame_generators.h"
#include "frames.h"

using namespace std;

#define POISSON 10

Host::Host() {
	rx_frame_count = 0;
	frame_generator = new Poisson(POISSON);
}

Host::Host(std::string ip_addr, std::string mac_addr, std::string hostname) {
	ip = ip_addr;
	mac = mac_addr;
	rx_frame_count = 0;
	name = hostname;
	frame_generator = new Poisson(POISSON);
}

Host::Host(std::string ip_addr, std::string mac_addr, std::string hostname, std::mutex* if_mutex, Frame* iface) {
	ip = ip_addr;
	mac = mac_addr;
	interface_mutex = if_mutex;
	interface = iface;
	rx_frame_count = 0;
	name = hostname;
	frame_generator = new Poisson(POISSON);
}

Host::~Host() {
	// delete interface;
	delete frame_generator;
}

// void Host::initialize() {}

void Host::run(std::string dst_mac) {
	// get frame delay
	// loop until then
	// in loop, check to see if mutex is grabbed
	// when grabbed, retrieve frame from interface and
	// process it
	// after loop finishes, grab mutex and put on interface
	// go back to beginning

	std::chrono::duration<double> delay_us; // the time that the code should wait for
	auto start_time = std::chrono::high_resolution_clock::now(); // the time to test if code has waited
																// long enough
	host_start_time = start_time;
	std::chrono::duration<double> diff;

	// cout << typeid(start_time).name() << endl;

	while (1) {
		// cout << name << "(" << mac << ")" <<" looping" << endl;
		//update start time
		start_time = std::chrono::high_resolution_clock::now();
		// get the interarrival time for the next frame
		delay_us = std::chrono::duration<double>(frame_generator->interarrival_time());
		// loop until its time to send the frame
		diff = std::chrono::high_resolution_clock::now() - start_time;
		while (diff.count() < delay_us.count()) {
			mutex_sleep();
			// cout << name << "(" << mac << ")" <<": waiting " << delay_us.count()<< " s (at " << diff.count() << " s)"<< endl;
			// check if the host on the other end of the link has locked the mutex
			if (interface_mutex->try_lock()) {
				// release the mutex if the other host has not locked it
				// since it isn't time to send the frame yet and there's nothing to
				// read
				// cout << "got lock when should not have" << endl;
				interface_mutex->unlock();
				if (interface->get_frame_size() != 0) {
					process_frame();
				}
			}
			else {
				// process the frame
				process_frame();			
			}

			diff = std::chrono::high_resolution_clock::now() - start_time;
		}

		// now that the code is done waiting, it can attempt to send the frame
		send_frame(rand() % 1500, dst_mac);



	}
}

std::string Host::get_ip() {return ip;}

std::string Host::get_mac() {return mac;}

int Host::get_frame_count() {return rx_frame_count;}

void Host::set_ip(std::string ip_addr) {
	ip = ip_addr;
}

void Host::set_mac(std::string mac_addr) {
	mac = mac_addr;
}

void Host::set_interface(Frame* frame_interface) {
	interface = frame_interface;
}

void Host::set_mutex(std::mutex* if_mutex) {
	interface_mutex = if_mutex;
}

void Host::send_frame(int frame_size, std::string dst_mac) {
	// cout << name << "(" << mac << ")" <<": sending frame" << endl;	
	// recursively attempt to put the frame on the link
	// just in case there's a conflict
	if (interface_mutex->try_lock()) {
		interface->set_src_mac(mac);
		interface->set_dst_mac(dst_mac);
		interface->set_frame_size(frame_size);
		std::cout << name << "(" << mac << ")" <<" sending " << frame_size << " bytes to " 
			<< dst_mac << std::endl;
		// std::cout << mac << " sent frame of " << interface-> get_frame_size() << " bytes to " 
		// 	<< dst_mac << " at " << runtime << " s" << std::endl;
		interface_mutex->unlock();		
	}
	else {
		send_frame(frame_size, dst_mac);
	}
}

void Host::process_frame() {
	std::chrono::duration<double> diff;
	// increase the frame count
	// hang here to lock the mutex as soon as the other host is done with it
	interface_mutex->lock();
	// check on the link to see if the frame is for this host

	// cout << name << "(" << mac << ")" <<": received from " << interface->get_src_mac();
	if (interface->get_dst_mac() == mac) {
		// cout << name << "(" << mac << ")" <<": processing frame" << endl;
		diff = std::chrono::high_resolution_clock::now() - host_start_time;
		// print some details about the frame
		increment_frame_count();
		cout << get_frame_count() << " " << mac << ": " << diff.count() << " frame of "
			<< interface->get_frame_size() << " bytes received from " << interface->get_src_mac() << endl;
		// get rid of the frame to signify that it was received
		interface->erase();
	}	
	// give the mutex back
	interface_mutex->unlock();	
}

void Host::increment_frame_count() {
	rx_frame_count += 1;
	// cout << rx_frame_count << endl;
}

void Host::mutex_sleep() {
	usleep(rand() % 100);
}

// int main(void) {

// 	Host alice("192.168.0.188", "aa:bb:cc:dd:ee:ff");
// 	std::mutex test_mutex;
// 	Frame interface;
// 	alice.set_mutex(&test_mutex);
// 	alice.set_interface(&interface);
// 	alice.run("00:11:22:33:44:55");
// }