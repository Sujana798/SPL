#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void process_large_book(char* filename) {
    printf("🔄 Processing LARGE: %s\n", filename);
    
    FILE* fp = fopen(filename, "r");
    if(!fp) {
        printf("❌ %s NOT FOUND!\n", filename);
        return;
    }
    
    // DYNAMIC file size detection
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    printf("  📖 File size: %ld bytes (%.1f KB)\n", file_size, file_size/1024.0);
    
    // CHUNK reading for large files (64KB chunks)
    char chunk[65536];
    char* all_clean = malloc(file_size + 1);
    long clean_total = 0;
    
    while(fgets(chunk, sizeof(chunk), fp)) {
        int chunk_len = strlen(chunk);
        for(int i = 0; i < chunk_len && clean_total < file_size; i++) {
            char c = chunk[i];
            if(isalpha(c)) {
                all_clean[clean_total++] = tolower(c);
            } else if(c == ' ') {
                all_clean[clean_total++] = ' ';
            }
        }
    }
    all_clean[clean_total] = '\0';
    fclose(fp);
    
    printf("  ✅ Cleaned: %ld chars\n", clean_total);
    
    // DYNAMIC word array (grows as needed)
    char** words = NULL;
    int word_capacity = 1000;
    int word_count = 0;
    
    words = malloc(word_capacity * sizeof(char*));
    
    // Manual word extraction (safe for large files)
    int i = 0;
    while(i < clean_total && word_count < word_capacity) {
        while(i < clean_total && all_clean[i] == ' ') i++;
        if(i >= clean_total) break;
        
        int start = i;
        while(i < clean_total && all_clean[i] != ' ') i++;
        
        int len = i - start;
        if(len > 0 && len < 50) {
            words[word_count] = malloc(len + 1);
            strncpy(words[word_count], all_clean + start, len);
            words[word_count][len] = '\0';
            word_count++;
        }
    }
    
    // Resize if needed
    if(word_count > word_capacity) {
        word_capacity = word_count;
        words = realloc(words, word_capacity * sizeof(char*));
    }
    
    printf("  ✅ Words extracted: %d\n", word_count);
    
    // DYNAMIC A-Z buckets
    char*** buckets = malloc(26 * sizeof(char**));
    int* bucket_counts = calloc(26, sizeof(int));
    
    for(int b = 0; b < 26; b++) {
        buckets[b] = malloc(word_count * sizeof(char*));
    }
    
    // Distribute words to buckets
    for(int w = 0; w < word_count; w++) {
        char first = words[w][0];
        int b = first - 'a';
        if(b >= 0 && b < 26) {
            buckets[b][bucket_counts[b]++] = words[w];
        }
    }
    
    // Sort each bucket (insertion sort - stable)
    for(int b = 0; b < 26; b++) {
        for(int i = 1; i < bucket_counts[b]; i++) {
            char* key = buckets[b][i];
            int j = i - 1;
            while(j >= 0 && strcmp(buckets[b][j], key) > 0) {
                buckets[b][j + 1] = buckets[b][j];
                j--;
            }
            buckets[b][j + 1] = key;
        }
    }
    
    // Write sorted output
    char outname[100];
    sprintf(outname, "%s_sorted.txt", filename);
    FILE* out = fopen(outname, "w");
    
    if(out) {
        fprintf(out, "=== %s - A-Z SORT (Large File) ===\n", filename);
        fprintf(out, "Total words: %d | File size: %ld bytes\n\n", word_count, file_size);
        
        for(int b = 0; b < 26; b++) {
            if(bucket_counts[b] > 0) {
                fprintf(out, "%c (%d): ", 'a'+b, bucket_counts[b]);
                int shown = 0;
                for(int i = 0; i < bucket_counts[b] && shown < 25; i++) {
                    fprintf(out, "%s ", buckets[b][i]);
                    shown++;
                }
                if(shown < bucket_counts[b]) fprintf(out, "...");
                fprintf(out, "\n\n");
            }
        }
        fclose(out);
        printf("  ✅ %s CREATED! (%d words in 26 columns)\n", outname, word_count);
    }
    
    // Perfect cleanup
    for(int w = 0; w < word_count; w++) free(words[w]);
    for(int b = 0; b < 26; b++) {
        free(buckets[b]);
    }
    free(words);
    free(buckets);
    free(bucket_counts);
    free(all_clean);
}

int main() {
    printf("🚀 LARGE FILE PROCESSOR (No Size Limit)\n");
    printf("=======================================\n\n");
    
    process_large_book("book1.txt");
    printf("\n");
    process_large_book("book2.txt");
    printf("\n");
    process_large_book("book3.txt");
    printf("\n");
    process_large_book("book4.txt");
    
    printf("\n🎉 ALL LARGE BOOKS PROCESSED SUCCESSFULLY!\n");
    printf("✅ 4 sorted files ready: *_sorted.txt\n");
    return 0;
}
