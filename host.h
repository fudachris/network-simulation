#ifndef HOST_H
#define HOST_H

#include <string>
#include <mutex>
#include "frame_generators.h"
#include "frames.h"

class Host{
	public:
		// void operator () (std::string, std::string, std::mutex*, Frame*);
		Host();
		Host(std::string, std::string, std::string);
		Host(std::string, std::string, std::string, std::mutex*, Frame*);
		~Host();
		// void initialize();
		void run(std::string);
		std::string get_ip();
		std::string get_mac();
		int get_frame_count();
		void set_ip(std::string);
		void set_mac(std::string);
		void set_interface(Frame*);
		void set_mutex(std::mutex*);
	private:
		void send_frame(int, std::string);
		void process_frame();
		void increment_frame_count();
		void mutex_sleep();
		Poisson* frame_generator;
		Frame* interface;
		std::string ip;
		std::string mac;
		std::mutex* interface_mutex;
		std::chrono::time_point<std::chrono::high_resolution_clock> host_start_time;
		int rx_frame_count;
		std::string name;
};

#endif