#include "static.h"

int a;
int main(int argc, char* argv[]){
	while(a<3){
		static int b=1;
		++b;
		++a;
	}
	printf("a= %d, b= %d\n", a, b);
	return a;
} 


