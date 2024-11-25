#include <stdio.h>
#include <immintrin.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

extern void distance_kernel_asm(int n, float *X1, float *X2, float *Y1, float *Y2, float *Z);

void distance_kernel_c(int n, float *X1, float *X2, float *Y1, float *Y2, float *Z) {
    for (int i = 0; i < n; i++) {
        float dx = X2[i] - X1[i];
        float dy = Y2[i] - Y1[i];
        Z[i] = sqrtf(dx * dx + dy * dy);
    }
}

// Function to check if two floats are approximately equal
int is_approximately_equal(float a, float b) {
    const float epsilon = 1e-5;  
    return fabs(a - b) < epsilon;
}

int main() {
    const int sizes[] = {1 << 20, 1 << 24, 1 << 29};
    const int num_runs = 30;
    
    // Initialize random seed
    srand(time(NULL));
    
    for (int size_idx = 0; size_idx < 3; size_idx++) {
        int n = sizes[size_idx];
        printf("\nTesting with vector size n = 2^%d\n", (int)log2(n));
        
        // Allocate memory
        float *X1 = _mm_malloc(n * sizeof(float), 16);
        float *X2 = _mm_malloc(n * sizeof(float), 16);
        float *Y1 = _mm_malloc(n * sizeof(float), 16);
        float *Y2 = _mm_malloc(n * sizeof(float), 16);
        float *Z_c = _mm_malloc(n * sizeof(float), 16);
        float *Z_asm = _mm_malloc(n * sizeof(float), 16);
        
        if (!X1 || !X2 || !Y1 || !Y2 || !Z_c || !Z_asm) {
            printf("Memory allocation failed!\n");
            return 1;
        }

        // Initialize arrays with random values between 0 and 10
        for (int i = 0; i < n; i++) {
            X1[i] = (float)(rand() % 11);
            X2[i] = (float)(rand() % 11);
            Y1[i] = (float)(rand() % 11);
            Y2[i] = (float)(rand() % 11);
        }

        // Time C version
        clock_t total_time = 0;
        for (int run = 0; run < num_runs; run++) {
            clock_t start = clock();
            distance_kernel_c(n, X1, X2, Y1, Y2, Z_c);
            total_time += clock() - start;
        }
        double c_time = ((double)total_time / CLOCKS_PER_SEC) / num_runs;
        printf("C version average time: %f seconds\n", c_time);
        
        // Time Assembly version
        total_time = 0;
        for (int run = 0; run < num_runs; run++) {
            clock_t start = clock();
            distance_kernel_asm(n, X1, X2, Y1, Y2, Z_asm);
            total_time += clock() - start;
        }
        double asm_time = ((double)total_time / CLOCKS_PER_SEC) / num_runs;
        printf("Assembly version average time: %f seconds\n", asm_time);

        // Correctness check
        printf("\nCorrectness Check:\n");
        int mismatch_count = 0;
        printf("First 10 results comparison:\n");
        for (int i = 0; i < 10; i++) {
            printf("Index %d: C = %f, ASM = %f, %s\n", 
                   i, Z_c[i], Z_asm[i], 
                   is_approximately_equal(Z_c[i], Z_asm[i]) ? "MATCH" : "MISMATCH");
        }

        // Check all results
        for (int i = 0; i < n; i++) {
            if (!is_approximately_equal(Z_c[i], Z_asm[i])) {
                mismatch_count++;
            }
        }

        printf("\nFull verification result:\n");
        if (mismatch_count == 0) {
            printf("? Performance speedup: %.2fx\n", c_time / asm_time);
        } else {
            printf("? Found %d mismatches out of %d elements\n", mismatch_count, n);
        }

        // Free memory
        _mm_free(X1);
        _mm_free(X2);
        _mm_free(Y1);
        _mm_free(Y2);
        _mm_free(Z_c);
        _mm_free(Z_asm);
    }
    
    return 0;
}
