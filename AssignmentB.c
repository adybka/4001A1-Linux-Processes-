#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

//magic number for shared memory 
#define MAGIC_NUM 1111

//shared matrices 
typedef struct Matrix{
	int M[4][4];
	int N[4][4];
	int Q[4][4];
}Matr;


//the four process function
void fourProcess();
void twoProcess();
void oneProcess();

int main(void){
	


	int n,sec,usec;

	printf("Input the number of processesof the folloing size: 1,2 or 4 : ");
        scanf("%d", &n);

	if(n == 1){
		struct timeval start,end;

		gettimeofday(&start,0);

		oneProcess();
		gettimeofday(&end,0);
		sec = end.tv_sec - start.tv_sec;
    		usec = end.tv_usec - start.tv_usec;
    		printf("\nMultiplying matrices by 4 process: %f sec\n", sec + (usec/1000000.0));
	}
	else if(n==2){
		struct timeval start,end;

		gettimeofday(&start,0);

		twoProcess();
		gettimeofday(&end,0);
		sec = end.tv_sec - start.tv_sec;
    		usec = end.tv_usec - start.tv_usec;
    		printf("\nMultiplying matrices by 4 process: %f sec\n", sec + (usec/1000000.0));
	}
	else{
		struct timeval start,end;

		gettimeofday(&start,0);

		fourProcess();
		gettimeofday(&end,0);
		sec = end.tv_sec - start.tv_sec;
    		usec = end.tv_usec - start.tv_usec;
    		printf("\nMultiplying matrices by 4 process: %f sec\n", sec + (usec/1000000.0));
	}
	
	
	
					
	return 0;
}




void oneProcess(){

	int i,j,k;
	key_t key = MAGIC_NUM;
	int shmid = shmget(key,sizeof(Matr)*4,0666 | IPC_CREAT);
	if(shmid < 0){
             printf("Error creating shared memory\n");
             exit(1);
     	}
	
	Matr matr;
	Matr *matrix = &matr;
	Matr *gg;
      	gg = shmat(shmid, NULL, 0);
	
	matrix = (Matr *)gg;

	if(matrix == NULL){
          printf("Error attaching shared memory\n");
          exit(1);
     	} 

	int I, J;
	int temp[4][4] = {{1,2,3,4}, {5,6,7,8}, {4,3,2,1}, {8,7,6,5}};
	int temp2[4][4] = {{1,3,5,7},{2,4,6,8},{7,3,5,7},{8,6,4,2}};
	for(I=0; I<4; I++){
		for(J=0; J<4; J++){
			matrix->M[I][J]=temp[I][J];
			matrix->N[I][J]=temp2[I][J];
			matrix->Q[I][J]=0;
		}
	}

	for( j=0 ; j<4 ; j++ ){
                    	for( k=0 ; k<4 ; k++ ){
                        	matrix->Q[0][j] += matrix->M[0][k] * matrix->N[k][j];
                    	}
				
                }
	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[1][j] += matrix->M[1][k] * matrix->N[k][j];
                    		}
                	}
	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[2][j] += matrix->M[2][k] * matrix->N[k][j];
                    		}
                	}
	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[3][j] += matrix->M[3][k] * matrix->N[k][j];
                    		}
                	}
		

		for(I=0; I<4; I++){
			printf("\n");
			for(J=0; J<4; J++){
				printf("%d  ", matrix->Q[I][J]);
			}
		}


		
		if (shmdt(matrix) == -1) {
        		fprintf(stderr, "shmdt failed\n");
        		exit(EXIT_FAILURE);
    		}
		if (shmctl(shmid, IPC_RMID, 0) == -1) {
        		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        		exit(1);
    		}		

        	


}



void twoProcess(){

	int i,j,k;
	key_t key = MAGIC_NUM;
	int shmid = shmget(key,sizeof(Matr)*4,0666 | IPC_CREAT);
	if(shmid < 0){
             printf("Error creating shared memory\n");
             exit(1);
     	}
	
	Matr matr;
	Matr *matrix = &matr;
	Matr *gg;
      	gg = shmat(shmid, NULL, 0);
	
	matrix = (Matr *)gg;

	if(matrix == NULL){
          printf("Error attaching shared memory\n");
          exit(1);
     	} 

	int I, J;
	int temp[4][4] = {{1,2,3,4}, {5,6,7,8}, {4,3,2,1}, {8,7,6,5}};
	int temp2[4][4] = {{1,3,5,7},{2,4,6,8},{7,3,5,7},{8,6,4,2}};
	for(I=0; I<4; I++){
		for(J=0; J<4; J++){
			matrix->M[I][J]=temp[I][J];
			matrix->N[I][J]=temp2[I][J];
			matrix->Q[I][J]=0;
		}
	}

	
	pid_t pid;
	pid = fork();

	if(pid<0){
		// if error, fork failed
        	fprintf(stderr, "Fork Failed");
        	exit(-1);
	}
	else if(pid == 0){
		
            	
                	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[0][j] += matrix->M[0][k] * matrix->N[k][j];
                    		}
				
                	}
            		
            	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[1][j] += matrix->M[1][k] * matrix->N[k][j];
                    		}
                	}

            		wait(NULL);
        	
        	if (shmdt(matrix) == -1) {
        		fprintf(stderr, "shmdt failed\n");
        		exit(EXIT_FAILURE);
    		}
        	exit(0);
    	}
    	else {
            		
            	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[2][j] += matrix->M[2][k] * matrix->N[k][j];
                    		}
                	}
            		
            	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[3][j] += matrix->M[3][k] * matrix->N[k][j];
                    		}
                	}
		

            	// if parent
            	wait(NULL);
            	wait(NULL);


		for(I=0; I<4; I++){
			printf("\n");
			for(J=0; J<4; J++){
				printf("%d  ", matrix->Q[I][J]);
			}
		}


		
		if (shmdt(matrix) == -1) {
        		fprintf(stderr, "shmdt failed\n");
        		exit(EXIT_FAILURE);
    		}
		if (shmctl(shmid, IPC_RMID, 0) == -1) {
        		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        		exit(1);
    		}		

        	
    	
	}

}




void fourProcess(){

	int i,j,k;
	key_t key = MAGIC_NUM;
	int shmid = shmget(key,sizeof(Matr)*4,0666 | IPC_CREAT);
	if(shmid < 0){
             printf("Error creating shared memory\n");
             exit(1);
     	}
	
	Matr matr;
	Matr *matrix = &matr;
	Matr *gg;
      	gg = shmat(shmid, NULL, 0);
	
	matrix = (Matr *)gg;

	if(matrix == NULL){
          printf("Error attaching shared memory\n");
          exit(1);
     	} 

	int I, J;
	int temp[4][4] = {{1,2,3,4}, {5,6,7,8}, {4,3,2,1}, {8,7,6,5}};
	int temp2[4][4] = {{1,3,5,7},{2,4,6,8},{7,3,5,7},{8,6,4,2}};
	for(I=0; I<4; I++){
		for(J=0; J<4; J++){
			matrix->M[I][J]=temp[I][J];
			matrix->N[I][J]=temp2[I][J];
			matrix->Q[I][J]=0;
		}
	}


	pid_t pid;
	pid = fork();

	if(pid<0){
		// if error, fork failed
        	fprintf(stderr, "Fork Failed");
        	exit(-1);
	}
	else if(pid == 0){
        	// fork here
        	pid_t pid2;
        	pid2 = fork();

        	if(pid2 <0) {
            		
            		fprintf(stderr, "Fork Failed");
            		exit(-1);
        	}
        	else if(pid2 == 0){
		
            	
                	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[0][j] += matrix->M[0][k] * matrix->N[k][j];
                    		}
				
                	}
            	

           	if (shmdt(matrix) == -1) {
        		fprintf(stderr, "shmdt failed\n");
        		exit(EXIT_FAILURE);
    		}
            	exit(0);
        	}
        	else {
            		
            	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[1][j] += matrix->M[1][k] * matrix->N[k][j];
                    		}
                	}

            		wait(NULL);
        	}
        	if (shmdt(matrix) == -1) {
        		fprintf(stderr, "shmdt failed\n");
        		exit(EXIT_FAILURE);
    		}
        	exit(0);
    	}
    	else {
        	// fork here
        	pid_t pid3;
        	pid3 = fork();

        	if(pid3 <0) {
            		// if error, fork failed
            		fprintf(stderr, "Fork Failed");
            		exit(-1);
        	}
        	else if(pid3 == 0){
            		
            	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[2][j] += matrix->M[2][k] * matrix->N[k][j];
                    		}
                	}
            	if (shmdt(matrix) == -1) {
        		fprintf(stderr, "shmdt failed\n");
        		exit(EXIT_FAILURE);
    		}
            	exit(0);
        	}
        	else {
            		
            	for( j=0 ; j<4 ; j++ ){
                    		for( k=0 ; k<4 ; k++ ){
                        		matrix->Q[3][j] += matrix->M[3][k] * matrix->N[k][j];
                    		}
                	}
		

            	// if parent
            	wait(NULL);
            	wait(NULL);


		for(I=0; I<4; I++){
			printf("\n");
			for(J=0; J<4; J++){
				printf("%d  ", matrix->Q[I][J]);
			}
		}


		
		if (shmdt(matrix) == -1) {
        		fprintf(stderr, "shmdt failed\n");
        		exit(EXIT_FAILURE);
    		}
		if (shmctl(shmid, IPC_RMID, 0) == -1) {
        		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        		exit(1);
    		}		

        	}
    	}

    

}


