#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
  int *arr;
  int left;
  int right;
} thread_args;

void merge(int arr[], int left, int mid, int right) {
  
  // create new left and right subarrays and copy data into them
  int num_left = mid - left +1;
  int num_right = right - mid;

  int left_arr[num_left], right_arr[num_right];
 
  for (int i = 0; i<num_left; i++) {
    left_arr[i] = arr[left + i];
  }
  for (int j = 0; j<num_right; j++) {
    right_arr[j] = arr[mid+1 + j];
  }

  // merge left and right subarrays
  int i = 0, j = 0, k = left;
  while (i<num_left && j<num_right) {
    if(left_arr[i] <= right_arr[j]) {
      arr[k] = left_arr[i];
      i++;
    } 
    else{
      arr[k] = right_arr[j];
      j++;
    }
    k++;
  }

  // remaining elements of left_arr[]
  while(i<num_left) {
    arr[k] = left_arr[i];
    i++;
    k++;
  }

  // remaining elements of right_arr[]
  while(j<num_right) {
    arr[k] = right_arr[j];
    j++;
    k++;
  }
}

void *merge_sort(void *args) {
  thread_args *t_args = (thread_args *)args;
  int arr_size = t_args->right - t_args->left + 1;

  if (arr_size <= 1) {
    return NULL;
  }

  int mid = t_args->left + (arr_size - 1) / 2;

  pthread_t left_thread, right_thread;

  thread_args left_args, right_args;
  left_args.arr = t_args->arr;
  left_args.left = t_args->left;
  left_args.right = mid;

  right_args.arr = t_args->arr;
  right_args.left = mid+1;
  right_args.right = t_args->right;

  // Sort each half recursively
  pthread_create(&left_thread, NULL, merge_sort, &left_args);
  pthread_create(&right_thread, NULL, merge_sort, &right_args);

  pthread_join(left_thread, NULL);
  pthread_join(right_thread, NULL);

  // merging sorted halves
  merge(t_args->arr, t_args->left, mid, t_args->right);

  return NULL;
}

int main() {
  int arr_size;
  char filename[] = "merge.txt";
  
  FILE *file = fopen(filename, "r");
  if (!file) {
      printf("Error opening file!\n");
      return 1;
  }

  fscanf(file,"%d",&arr_size);
  
  //printf("Enter number of elements:\n");
  //scanf("%d", &arr_size);

  int arr[arr_size];

  //printf("Enter elements:\n");
  for (int i = 0; i < arr_size; i++) {
    fscanf(file,"%d", &arr[i]);
    //scanf("%d", &arr[i]);
  }

  // initialize threads and call merge function
  pthread_t thread;
  thread_args t_args = {arr,0,arr_size-1};
  pthread_create(&thread, NULL, merge_sort, &t_args);

  pthread_join(thread, NULL);

  printf("Sorted array:\n");
  for (int i = 0; i < arr_size; i++) {
    printf("%d ", arr[i]);
  }
  printf("\n");

  return 0;
}
