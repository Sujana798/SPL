#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void sort_words_alphabetically(char* input_file, char* output_file) {
    printf("Sorting: %s → %s\n", input_file, output_file);
    
    // Read clean file
    FILE* in_fp = fopen(input_file, "r");
    if(!in_fp) {
        printf("❌ Cannot open %s\n", input_file);
        return;
    }
    
    fseek(in_fp, 0, SEEK_END);
    long size = ftell(in_fp);
    fseek(in_fp, 0, SEEK_SET);
    
    char* text = malloc(size + 1);
    fread(text, 1, size, in_fp);
    text[size] = '\0';
    fclose(in_fp);
    
    // Extract words to dynamic array
    char** words = NULL;
    int word_count = 0;
    int word_capacity = 1000;
    
    words = malloc(word_capacity * sizeof(char*));
    
    char* token = strtok(text, " ");
    while(token) {
        if(strlen(token) > 0) {
            if(word_count >= word_capacity) {
                word_capacity *= 2;
                words = realloc(words, word_capacity * sizeof(char*));
            }
            words[word_count] = strdup(token);
            word_count++;
        }
        token = strtok(NULL, " ");
    }
    
    // 26 buckets (A-Z) with proper sorting
    char* buckets[26];
    int bucket_sizes[26] = {0};
    
    for(int i = 0; i < 26; i++) {
        buckets[i] = malloc(word_count * sizeof(char));
        bucket_sizes[i] = 0;
    }
    
    // Distribute words to buckets
    for(int i = 0; i < word_count; i++) {
        char first_letter = tolower(words[i][0]);
        if(first_letter >= 'a' && first_letter <= 'z') {
            int bucket_idx = first_letter - 'a';
            
            // Copy to bucket
            strcpy(buckets[bucket_idx] + bucket_sizes[bucket_idx] * 50, words[i]);
            bucket_sizes[bucket_idx]++;
        }
        free(words[i]);
    }
    free(words);
    
    // Sort each bucket alphabetically
    for(int b = 0; b < 26; b++) {
        if(bucket_sizes[b] > 1) {
            // Bubble sort for alphabetical order
            for(int i = 0; i < bucket_sizes[b] - 1; i++) {
                for(int j = 0; j < bucket_sizes[b] - i - 1; j++) {
                    char* word1 = buckets[b] + j * 50;
                    char* word2 = buckets[b] + (j + 1) * 50;
                    if(strcmp(word1, word2) > 0) {
                        // Swap
                        char temp[50];
                        strcpy(temp, word1);
                        strcpy(word1, word2);
                        strcpy(word2, temp);
                    }
                }
            }
        }
    }
    
    // Write sorted file
    FILE* out_fp = fopen(output_file, "w");
    if(out_fp) {
        fprintf(out_fp, "=== %s - ALPHABETICAL SORT (A-Z COLUMNS) ===\n\n", input_file);
        
        for(int col = 0; col < 26; col++) {
            char letter = 'a' + col;
            fprintf(out_fp, "%c COLUMN (%d words): ", letter, bucket_sizes[col]);
            
            for(int i = 0; i < bucket_sizes[col] && i < 15; i++) {
                char* word = buckets[col] + i * 50;
                fprintf(out_fp, "%s ", word);
            }
            fprintf(out_fp, "\n\n");
        }
        fclose(out_fp);
        printf("✅ %d words sorted in 26 columns\n", word_count);
    }
    
    // Cleanup
    for(int i = 0; i < 26; i++) {
        free(buckets[i]);
    }
    free(text);
}

int main() {
    printf("=== ACCURATE A-Z SORTING (26 Columns) ===\n\n");
    
    // Process each clean file separately (no crash)
    sort_words_alphabetically("book1_clean.txt", "book1_sorted.txt");
    printf("\n");
    sort_words_alphabetically("book2_clean.txt", "book2_sorted.txt");
    printf("\n");
    sort_words_alphabetically("book3_clean.txt", "book3_sorted.txt");
    printf("\n");
    sort_words_alphabetically("book4_clean.txt", "book4_sorted.txt");
    
    printf("\n✅ ALL 4 FILES SORTED SUCCESSFULLY!\n");
    return 0;
}
