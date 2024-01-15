#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


typedef struct {
  int row;
  int col;
  int ncols1;
  int ncols2; 
  int **matrix1;
  int **matrix2;
  int **result;
} thread_args;

void *multiply_elementwise(void* args){
    thread_args* data = args;
    int n,sum=0;
    for(n = 0; n < data->ncols1; n++)  //(column1 )for each element in the matrix1 row
    {
        sum += data->matrix1[data->row][n] * data->matrix2[n][data->col];
    }
    data->result[data->row][data->col] = sum;

    return NULL;
}

void *multiply_rowwise(void* args){
    thread_args* data = args;
    int i,n,sum=0;
    for(i=0; i< data->ncols2; i++){ // for every column in second matrix
        for(n = 0; n < data->ncols1; n++){
                sum += data->matrix1[data->row][n] * data->matrix2[n][i];
        }

        data->result[data->row][i] = sum;
        sum=0;
    }

    return NULL;
}


int main() {
    // Write C code here
    
    // open file
    char filename[] = "matrices.txt";
  
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file!\n");
        return 1;
    }

    // read first matrix from file
    int rows1,columns1;
    fscanf(file,"%d %d",&rows1, &columns1);
  
    int **matrix1 = malloc(rows1 * sizeof(int *));
    for (int i = 0; i < rows1; i++) {
        matrix1[i] = malloc(columns1 * sizeof(int));
    }
       
    for (int i = 0; i<rows1; i++){
        for (int j = 0; j<columns1; j++){
            fscanf(file,"%d",&matrix1[i][j]);
        }
    }
    printf("First matrix:\n");
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < columns1; j++)
            printf("%d ", matrix1[i][j]);
        printf("\n");
    }

    // read second matrix from file
    int rows2,columns2;
    fscanf(file,"%d %d",&rows2, &columns2);

    // check entered dimensions
    if (columns1 != rows2) {
    printf("Error: Matrix dimensions don't match!\n");
    return 1;
    }
  
    int **matrix2 = malloc(rows2 * sizeof(int *));
    for (int i = 0; i < rows2; i++) {
        matrix2[i] = malloc(columns2 * sizeof(int));
    }
       
    for (int i = 0; i<rows2; i++){
        for (int j = 0; j<columns2; j++){
            fscanf(file,"%d",&matrix2[i][j]);
        }
    }
    printf("Second matrix:\n");
    for (int i = 0; i < rows2; i++) {
        for (int j = 0; j < columns2; j++)
            printf("%d ", matrix2[i][j]);
        printf("\n");
    }
    fclose(file);

    // initialize result matrix
    int **result = malloc(rows1 * sizeof(int *));
    for (int i = 0; i < rows1; i++) {
        result[i] = malloc(columns2 * sizeof(int));
    }


    // row-wise
    clock_t start = clock();
    pthread_t threads[rows1];
    // allocate memory for array or args for each thread
    thread_args args1[rows1];
    for (int i = 0; i<rows1; i++){
        args1[i].row = i;
        args1[i].col = 0;
        args1[i].ncols1 = columns1;
        args1[i].ncols2 = columns2;
        args1[i].matrix1 = matrix1;
        args1[i].matrix2 = matrix2;
        args1[i].result = result;
        pthread_create(&threads[i],NULL, multiply_rowwise,&args1[i]);
    }
    for(int i = 0; i<rows1; i++){
        pthread_join(threads[i],NULL);
    }
    clock_t end = clock();
    // print results
    printf("Row-wise result:\n");
    double time_spent = (double) (end - start) / CLOCKS_PER_SEC;
     for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < columns2; j++)
            printf("%d ", result[i][j]);
        printf("\n");
    }
    printf("Row-wise Time : %f s\n", time_spent);


    // element-wise
    start = clock();
    pthread_t threads2[rows1*columns2]; //second set of threads for element wise
    thread_args args2[rows1 * columns2];

    for (int i = 0; i<rows1; i++){
        for(int j = 0; j<columns2; j++){
            args2[i * columns2 + j].row = i;
            args2[i * columns2 + j].col = j;
            args2[i * columns2 + j].ncols1 = columns1;
            args2[i * columns2 + j].ncols2 = columns2;
            args2[i * columns2 + j].matrix1 = matrix1;
            args2[i * columns2 + j].matrix2 = matrix2;
            args2[i * columns2 + j].result = result;
            pthread_create(&threads2[i*columns2 + j],NULL, multiply_elementwise,&args2[i * columns2 + j]);   
        }
        
    }
    for(int i = 0; i<rows1*columns2; i++){
          pthread_join(threads2[i],NULL); 
    }
    end = clock();
    time_spent = (double) (end - start) / CLOCKS_PER_SEC;
    printf("Element-wise result:\n");
    for (int i = 0; i < rows1; i++) {
        for (int j = 0; j < columns2; j++)
            printf("%d ", result[i][j]);
        printf("\n");
    }
    printf("Element-wise Time : %f s\n", time_spent);

    free(matrix1);
    free(matrix2);
    free(result);

    return 0;
}