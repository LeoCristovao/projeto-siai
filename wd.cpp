#include <iostream>
#include <fstream>
#include <unistd.h>
#include <math.h>

using namespace std;

int main () {
	ofstream f;
	for(int i = 0; i <= 100000; i++) {
		f.open("data.dat", ios::app);
		f << i << " " << i << "\n";
		usleep(1000000);
		//f << "8 8 2\n";
		f.close();
	}
}
