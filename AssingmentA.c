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

int main(void){
	


	int sec,usec;

	
	struct timeval start,end;

	gettimeofday(&start,0);

	fourProcess();
	gettimeofday(&end,0);
	sec = end.tv_sec - start.tv_sec;
    	usec = end.tv_usec - start.tv_usec;
    	printf("Multiplying matrices by 4 process: %f sec\n", sec + (usec/1000000.0));
					
	return 0;
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
		
            	for( i=0 ; i<2 ; i++ ){
                	for( j=0 ; j<2 ; j++ ){
                    		for( k=0 ; k<2 ; k++ ){
                        		matrix->Q[i][k] += matrix->M[i][j] * matrix->N[j][k];
                    		}
                	}
            	}

           	if (shmdt(matrix) == -1) {
        		fprintf(stderr, "shmdt failed\n");
        		exit(EXIT_FAILURE);
    		}
            	exit(0);
        	}
        	else {
            		
            	for( i=0 ; i<4 ; i++ ){
                	for( j=0 ; j<2 ; j++ ){
                    			
                    		for( k=0 ; k<2 ; k++ ){
                        		matrix->Q[i][j] += matrix->M[i][k] * matrix->N[k][j];
                    		}
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
            		
            	for( i=0 ; i<2 ; i++ ){
                	for( j=2 ; j<4 ; j++ ){
                    			
                    		for( k=0 ; k<2 ; k++ ){
                        		matrix->Q[i][j] += matrix->M[i][k] * matrix->N[k][j];
                    		}
                	}
            	}
            	if (shmdt(matrix) == -1) {
        		fprintf(stderr, "shmdt failed\n");
        		exit(EXIT_FAILURE);
    		}
            	exit(0);
        	}
        	else {
            		
            	for( i=2 ; i<4 ; i++ ){
                	for( j=2 ; j<4 ; j++ ){
                    			
                    		for( k=0 ; k<2 ; k++ ){
                        		matrix->Q[i][j] += matrix->M[i][k] * matrix->N[k][j];
                    		}
                	}
            	}
		for(I=0; I<4; I++){
			printf("\n");
			for(J=0; J<4; J++){
				printf("%d  ", matrix->Q[I][J]);
			}
		}

            	// if parent
            	wait(NULL);
            	wait(NULL);


		


		
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
