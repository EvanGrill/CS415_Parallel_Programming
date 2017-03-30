#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_SIZE	1000

int main(){
	uint64_t i;
	uint64_t number_of_points;
	long random_num;

	scanf("%lld",&number_of_points);


	printf("%llu\n",number_of_points);
	for(i=0;i<number_of_points;i++){
		random_num = random();
		printf("%ld\n", (random_num % MAX_SIZE));
	}

	return 0;
}