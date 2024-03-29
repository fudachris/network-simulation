#ifndef FRAME_GENERATORS_H
#define FRAME_GENERATORS_H

#include <vector>

class Poisson {
	public:
		Poisson(); // default constructor
		Poisson(double); // constructor that takes a lambda
		Poisson(double,int); // constructor that takes a lambda and a time length
		~Poisson();
		void set_lambda(double); // modify the lambda value
		void set_seconds(int); // modify the length of the process
		void run(); // a primary function in case we want to see outputs
		double interarrival_time(); // a single interarrival time which was produced by 
									// a poisson random variable
	private:						
		double lambda; // average number of frames per second
		int seconds;	// length that the process runs if we use "run"
		std::vector< std::vector<double> > cdf; // the cdf of the interarrival times for the listed lambda
		std::vector<double> process_points; // the actual times of arrival if we use "run"
		void build_cdf(); // rebuild the cdf (maybe we changed lambda)
		void run_capture(); // print out all the frames whose interarrival times were generated for "run"
		void print_capture_pmf(); // show the pmf of the frames that arrived in "run"
};


#endif