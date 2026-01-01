/**
 * Parallel Image Downscaling with MPI
 * 
 * Usage: mpirun -np <num_processes> ./mpi_main <aybu.jpg> <aybu_mpi.jpg>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mpi.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#define CHANNEL_NUM 1


void parallel_downscaling(uint8_t* local_input, uint8_t* local_output,
                         int width, int local_rows) {
    int new_width = width / 2;
    int local_new_rows = local_rows / 2;
    
    for(int i = 0; i < local_new_rows; i++) {
        for(int j = 0; j < new_width; j++) {
            int sum = 0;
            sum += local_input[(2*i) * width + (2*j)];
            sum += local_input[(2*i) * width + (2*j + 1)];
            sum += local_input[(2*i + 1) * width + (2*j)];
            sum += local_input[(2*i + 1) * width + (2*j + 1)];
            local_output[i * new_width + j] = (uint8_t)(sum / 4);
        }
    }
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if(argc != 3) {
        if(rank == 0) {
            printf("Usage: mpirun -np <num_processes> %s <aybu.jpg> <aybu_mpi.jpg>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }
    
    int width, height;
    uint8_t* input_image = NULL;
    uint8_t* output_image = NULL;
    
    
    if(rank == 0) {
        int bpp;
        input_image = stbi_load(argv[1], &width, &height, &bpp, CHANNEL_NUM);
        
        if(input_image == NULL) {
            printf("Error: Could not load image %s\n", argv[1]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        printf("Width: %d  Height: %d\n", width, height);
        printf("Input: %s, Output: %s\n", argv[1], argv[2]);
        printf("Number of processes: %d\n", size);
        
        
        if(height % (2 * size) != 0) {
            printf("Warning: Height should be divisible by %d for optimal load balancing\n", 2 * size);
        }
    }
    
    // Broadcast dimensions to all processes
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Calculate rows per process 
    int rows_per_process = (height / size);
    if(rows_per_process % 2 != 0) {
        rows_per_process = (rows_per_process / 2) * 2;
    }
    
    // Calculate send counts and displacements for scatterv
    int* sendcounts = NULL;
    int* displs = NULL;
    int* recvcounts = NULL;
    int* recvdispls = NULL;
    
    if(rank == 0) {
        sendcounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
        recvcounts = (int*)malloc(size * sizeof(int));
        recvdispls = (int*)malloc(size * sizeof(int));
        
        int current_row = 0;
        for(int i = 0; i < size; i++) {
            int local_rows = rows_per_process;
            if(i == size - 1) {
                local_rows = height - current_row;
                if(local_rows % 2 != 0) local_rows--;
            }
            
            sendcounts[i] = local_rows * width;
            displs[i] = current_row * width;
            recvcounts[i] = (local_rows / 2) * (width / 2);
            recvdispls[i] = (current_row / 2) * (width / 2);
            current_row += local_rows;
        }
    }
    
    // Broadcast local_rows to all processes
    int local_rows = rows_per_process;
    if(rank == size - 1 && rank == 0) {
        local_rows = height;
        if(local_rows % 2 != 0) local_rows--;
    }
    
    
    uint8_t* local_input = (uint8_t*)malloc(local_rows * width * sizeof(uint8_t));
    int local_new_rows = local_rows / 2;
    int new_width = width / 2;
    uint8_t* local_output = (uint8_t*)malloc(local_new_rows * new_width * sizeof(uint8_t));
    
    if(local_input == NULL || local_output == NULL) {
        printf("Process %d: Error allocating memory\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    // Start timing
    MPI_Barrier(MPI_COMM_WORLD);
    double start_time = MPI_Wtime();
    
    // Scatter input image to all processes
    MPI_Scatterv(input_image, sendcounts, displs, MPI_UNSIGNED_CHAR,
                 local_input, local_rows * width, MPI_UNSIGNED_CHAR,
                 0, MPI_COMM_WORLD);
    
    // Perform local downscaling
    parallel_downscaling(local_input, local_output, width, local_rows);
    
    // Gather output image at root
    if(rank == 0) {
        int new_height = height / 2;
        output_image = (uint8_t*)malloc(new_width * new_height * sizeof(uint8_t));
        if(output_image == NULL) {
            printf("Error: Could not allocate output buffer\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }
    
    MPI_Gatherv(local_output, local_new_rows * new_width, MPI_UNSIGNED_CHAR,
                output_image, recvcounts, recvdispls, MPI_UNSIGNED_CHAR,
                0, MPI_COMM_WORLD);
    
    // End timing
    MPI_Barrier(MPI_COMM_WORLD);
    double end_time = MPI_Wtime();
    
    if(rank == 0) {
        printf("Elapsed time: %lf seconds\n", end_time - start_time);
        
        // Save output image
        int new_height = height / 2;
        int result = stbi_write_jpg(argv[2], new_width, new_height, CHANNEL_NUM, 
                                    output_image, 100);
        
        if(!result) {
            printf("Error: Could not save output image\n");
        }
        
        // Cleanup
        stbi_image_free(input_image);
        free(output_image);
        free(sendcounts);
        free(displs);
        free(recvcounts);
        free(recvdispls);
    }
    
    free(local_input);
    free(local_output);
    
    MPI_Finalize();
    return 0;
}