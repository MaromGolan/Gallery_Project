#include <iostream>
#include <ctime>

#define NAME "Marom_Golan"


int main() 
{
	time_t now = time(0);
	char* dateTime = ctime(&now);
	std::cout << "The name of the Developer: "<< NAME << "Current date and time: " << dateTime;
	return 0;
}