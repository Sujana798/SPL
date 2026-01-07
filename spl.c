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
