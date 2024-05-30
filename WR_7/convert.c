#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 1000
#define MAX_MATRIX_SIZE 1000
#define CACHE_SIZE 1000

struct cache_entry {
    char* key;
    char* value;
};

struct cache_entry cache[CACHE_SIZE];
int cache_size = 0;

void cache_insert(char* key, char* value) {
    // Check if cache is initialized
    if (cache_size >= CACHE_SIZE) {
        // Free memory for the oldest entry
        free(cache[0].key);
        free(cache[0].value);

        // Shift cache entries left
        for (int i = 1; i < CACHE_SIZE; i++) {
            cache[i-1].key = cache[i].key;
            cache[i-1].value = cache[i].value;
        }

        // Decrement cache size
        cache_size--;
    }

    // Allocate memory for new key and value
    cache[cache_size].key = strdup(key);
    cache[cache_size].value = strdup(value);

    // Increment cache size
    cache_size++;
}

void cache_cleanup() {
    // Free memory for all cache entries
    for (int i = 0; i < cache_size; i++) {
        free(cache[i].key);
        free(cache[i].value);
    }
}

void convert_to_mat_in_x(char* filename) {
    FILE* fp_in;
    FILE* fp_out;
    char line[MAX_LINE_LENGTH];
    char out_filename[MAX_LINE_LENGTH];
    char matrix[MAX_MATRIX_SIZE];
    int rows, cols;

    fp_in = fopen(filename, "r");
    if (fp_in == NULL) {
        perror("Error opening mat.in file");
        exit(EXIT_FAILURE);
    }

    sprintf(out_filename, "%s.x", filename);
    fp_out = fopen(out_filename, "w");
    if (fp_out == NULL) {
        perror("Error creating mat.in.x file");
        fclose(fp_in);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), fp_in)) {
        char *ptr = strchr(line, ':');
        if (ptr == NULL) {
            fprintf(stderr, "Invalid input format: %s", line);
            continue;
        }

        *ptr = '\0'; // Null-terminate before ':'
        sscanf(line, "%dx%d", &rows, &cols);
        strcpy(matrix, ptr + 1);

        // Convert binary string to hexadecimal equivalent
        for (int i = 0; i < rows * cols; i += 4) {
            char four_bits[5] = { matrix[i], matrix[i+1], matrix[i+2], matrix[i+3], '\0' };
            int decimal = strtol(four_bits, NULL, 2);
            fprintf(fp_out, "%X", decimal);
        }
        fprintf(fp_out, "\n");
    }

    fclose(fp_in);
    fclose(fp_out);
}

void convert_to_mat_in(char* filename) {
    FILE* fp_in;
    FILE* fp_out;
    char line[MAX_LINE_LENGTH];
    char out_filename[MAX_LINE_LENGTH];
    char matrix[MAX_MATRIX_SIZE];
    int rows, cols;

    fp_in = fopen(filename, "r");
    if (fp_in == NULL) {
        perror("Error opening mat.in.x file");
        exit(EXIT_FAILURE);
    }

    sprintf(out_filename, "%s.in", filename);
    fp_out = fopen(out_filename, "w");
    if (fp_out == NULL) {
        perror("Error creating mat.in file");
        fclose(fp_in);
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), fp_in)) {
        char *ptr = strchr(line, ':');
        if (ptr == NULL) {
            fprintf(stderr, "Invalid input format: %s", line);
            continue;
        }

        *ptr = '\0'; // Null-terminate before ':'
        sscanf(line, "%dx%d", &rows, &cols);
        strcpy(matrix, ptr + 1);

        // Convert hexadecimal string to binary equivalent
        for (int i = 0; i < strlen(matrix); i++) {
            int decimal = strtol(&matrix[i], NULL, 16);
            for (int j = 3; j >= 0; j--) {
                fprintf(fp_out, "%d", (decimal >> j) & 1);
            }
        }
        fprintf(fp_out, "\n");
    }

    fclose(fp_in);
    fclose(fp_out);
}

void test_conversion_performance(char* filename) {
    clock_t start, end;
    double cpu_time_used;

    // Test conversion from mat.in to mat.in.x
    start = clock();
    convert_to_mat_in_x(filename);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to convert %s to %s.x: %f seconds\n", filename, filename, cpu_time_used);

    // Test conversion from mat.in.x back to mat.in
    start = clock();
    convert_to_mat_in(filename);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time taken to convert %s.x back to %s: %f seconds\n", filename, filename, cpu_time_used);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char* filename = argv[1];

    // Perform conversion and testing
    test_conversion_performance(filename);

    return 0;
}
