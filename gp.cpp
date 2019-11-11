#include <iostream>
#include <fstream>

using namespace std;

int main () {
	ofstream f;
	f.open("myfile");
	f << "set xrange [0:70]\n";
	f << "set yrange [0:60]\n";
	f << "set zeroaxis \n";
	f << "plot 'data.dat' with lines\n";
	f << "pause 3\n";
	f << "reread";
	f.close();
	system("gnuplot myfile <data.dat" );
	//system("gnuplot -p <myfile" );
}
