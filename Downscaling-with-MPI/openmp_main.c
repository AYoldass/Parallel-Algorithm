/**
 * Parallel Image Downscaling with OpenMP
 * 
 * Usage: ./openmp_main <aybu.jpg> <aybu_openmp.jpg> [num_threads]
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#define CHANNEL_NUM 1


void openmp_downscaling(uint8_t* input_image, uint8_t* output_image, 
                        int width, int height, int num_threads) {
    int new_width = width / 2;
    int new_height = height / 2;
    
    
    omp_set_num_threads(num_threads);
    
    // Parallel OpenMP
    #pragma omp parallel for schedule(dynamic, 8) collapse(2)
    for(int i = 0; i < new_height; i++) {
        for(int j = 0; j < new_width; j++) {
            int sum = 0;
            sum += input_image[(2*i) * width + (2*j)];
            sum += input_image[(2*i) * width + (2*j + 1)];
            sum += input_image[(2*i + 1) * width + (2*j)];
            sum += input_image[(2*i + 1) * width + (2*j + 1)];
            output_image[i * new_width + j] = (uint8_t)(sum / 4);
        }
    }
}

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

int main(int argc, char* argv[]) {
    if(argc < 3 || argc > 4) {
        printf("Usage: %s <aybu.jpg> <aybu_openmp.jpg> [num_threads]\n", argv[0]);
        return 1;
    }
    
  
    int num_threads = omp_get_max_threads();
    if(argc == 4) {
        num_threads = atoi(argv[3]);
        if(num_threads <= 0) {
            printf("Error: Invalid number of threads\n");
            return 1;
        }
    }
    
    int width, height, bpp;
    
    // Load 
    uint8_t* input_image = stbi_load(argv[1], &width, &height, &bpp, CHANNEL_NUM);
    
    if(input_image == NULL) {
        printf("Error: Could not load image %s\n", argv[1]);
        return 1;
    }
    
    printf("Width: %d  Height: %d\n", width, height);
    printf("Input: %s, Output: %s\n", argv[1], argv[2]);
    printf("Number of threads: %d\n", num_threads);
    printf("Max available threads: %d\n", omp_get_max_threads());
    
    
    if(width % 2 != 0 || height % 2 != 0) {
        printf("Warning: Image dimensions should be even for 2x downscaling\n");
    }
    
    int new_width = width / 2;
    int new_height = height / 2;
    
    // Allocate output buffer
    uint8_t* output_image = (uint8_t*)malloc(new_width * new_height * sizeof(uint8_t));
    if(output_image == NULL) {
        printf("Error: Could not allocate output buffer\n");
        stbi_image_free(input_image);
        return 1;
    }
    
    
    double time1 = get_time();
    
    openmp_downscaling(input_image, output_image, width, height, num_threads);
    
    double time2 = get_time();
    printf("Elapsed time: %lf seconds\n", time2 - time1);
    
    // Save 
    int result = stbi_write_jpg(argv[2], new_width, new_height, CHANNEL_NUM, 
                                output_image, 100);
    
    if(!result) {
        printf("Error: Could not save output image\n");
    }
    
   
    stbi_image_free(input_image);
    free(output_image);
    
    return 0;
}