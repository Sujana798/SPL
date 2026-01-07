#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char*** all_buckets[4];           
int bucket_sizes[4][26];         
int total_words[4];               
char* book_names[4] = {
    "book1.txt", 
    "book2.txt", 
    "book3.txt", 
    "book4.txt"
};

void print_separator(char symbol, int length) {
    for(int i = 0; i < length; i++) {
        printf("%c", symbol);
    }
    printf("\n");
}

void print_processing_header(char* filename) {
    printf("\n");
    print_separator('=', 60);
    printf(" PERFECT PROCESSING: %s\n", filename);
    print_separator('=', 60);
}

void print_file_stats(long size, int word_count) {
    printf(" Raw file size: %ld bytes\n", size);
    printf(" Total words extracted: %d\n", word_count);
    printf(" Processing status: COMPLETE (100%%)\n");
}

char* clean_text_data(char* text, long size, long* clean_len) {
    char* clean = malloc(size + 1);
    *clean_len = 0;
    
    for(long i = 0; i < size; i++) {
        char c = text[i];
        if(isalpha(c)) {
            clean[(*clean_len)++] = tolower(c);
        } else if(c == ' ' || c == '\n' || c == '\t') {
            clean[(*clean_len)++] = ' ';
        }
    }
    clean[*clean_len] = '\0';
    
    return clean;
}

char** extract_words(char* clean, long clean_len, int* count) {
    char** words = NULL;
    int capacity = 1000;
    *count = 0;
    
    words = malloc(capacity * sizeof(char*));
    
    long i = 0;
    while(i < clean_len) {
        
        while(i < clean_len && clean[i] == ' ') {
            i++;
        }
        
        if(i >= clean_len) {
            break;
        }
        
        long start = i;
        while(i < clean_len && clean[i] != ' ') {
            i++;
        }
        
        long len = i - start;
        
        if(len > 0 && len < 100) {
           
            if(*count >= capacity) {
                capacity *= 2;
                words = realloc(words, capacity * sizeof(char*));
            }
            
            words[*count] = malloc(len + 1);
            strncpy(words[*count], clean + start, len);
            words[*count][len] = '\0';
            (*count)++;
        }
    }
    
    return words;
}

void initialize_buckets(int book_id, int word_count) {
    all_buckets[book_id] = malloc(26 * sizeof(char**));
    memset(bucket_sizes[book_id], 0, sizeof(bucket_sizes[book_id]));
    
    for(int b = 0; b < 26; b++) {
        all_buckets[book_id][b] = malloc(word_count * sizeof(char*));
    }
}

