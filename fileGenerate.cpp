/*
Compile this file using the C++11 standard ---- "g++ -std=c++11 fileGenerate.cpp -o <output_file_name>"
This is because of the wrappers used along with Meressen Twister engine to generate random numbers
of uniform distribution in the range (0,1) inclusive

argv[1] is the a name of the transaction record file that is to be generated

argv[2] is an integer can be used to generate the same sequence of pesudo-random transaction records
for multiple runs
*/


#include<iostream>
#include<random>
#include<functional>
#include<fstream>
#include<cstdlib>
using namespace std;

int main(int argc,char *argv[])
{
	int attributes,transactions;
	
	ofstream datafile;
	
	if(argc < 3)
	{
		cout<<"\nPlease execute the program with the following parameters:";
		cout<<"\n<binary_file> <filename> <32-bit (integer) seed_value>";
		return 0;
	}

	datafile.open(argv[1],ios::out);

	mt19937::result_type seed = atoi(argv[2]);
	auto rand_number = bind(uniform_int_distribution<int>(0,1),mt19937(seed));

	cout<<"\nEnter the number of attributes in a transaction: ";
	cin>>attributes;
	cout<<"\nEnter the number of transactions to be entered: ";
	cin>>transactions;
	
	for(int i=0;i<transactions;i++)
	{
		for(int j=0;j<attributes;j++)
			datafile<<rand_number()<<char(9);

		datafile<<"\n";
	}

	datafile.close();
	
	return 0;
}
