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


void distribute_to_buckets(int book_id, char** words, int count) {
    for(int w = 0; w < count; w++) {
        char first = words[w][0];
        int bucket;
        
        if(first >= 'a' && first <= 'z') {
            bucket = first - 'a';
        } else {
            bucket = 25;  
        }
        
        all_buckets[book_id][bucket][bucket_sizes[book_id][bucket]++] = words[w];
    }
}

void sort_bucket(int book_id, int bucket) {
    for(int i = 1; i < bucket_sizes[book_id][bucket]; i++) {
        char* key = all_buckets[book_id][bucket][i];
        int j = i - 1;
        
        while(j >= 0 && strcmp(all_buckets[book_id][bucket][j], key) > 0) {
            all_buckets[book_id][bucket][j + 1] = all_buckets[book_id][bucket][j];
            j--;
        }
        
        all_buckets[book_id][bucket][j + 1] = key;
    }
}

void sort_all_buckets(int book_id) {
    printf(" Sorting words alphabetically in buckets...\n");
    
    for(int b = 0; b < 26; b++) {
        if(bucket_sizes[book_id][b] > 0) {
            sort_bucket(book_id, b);
        }
    }
    
    printf(" All buckets sorted successfully!\n");
}

void write_output_file(char* filename, int book_id, int count) {
    char outname[50];
    sprintf(outname, "%s_perfect.txt", filename);
    
    FILE* out = fopen(outname, "w");
    if(!out) {
        printf(" Failed to create output file: %s\n", outname);
        return;
    }
    
    fprintf(out, "=== %s - PERFECT A-Z SORT ===\n", filename);
    fprintf(out, "Total words: %d\n\n", count);
    
    for(int b = 0; b < 26; b++) {
        if(bucket_sizes[book_id][b] > 0) {
            fprintf(out, "%c[%d]: ", 'a' + b, bucket_sizes[book_id][b]);
            
            for(int i = 0; i < bucket_sizes[book_id][b] && i < 25; i++) {
                fprintf(out, "%s ", all_buckets[book_id][b][i]);
            }
            
            fprintf(out, "\n");
        }
    }
    
    fclose(out);
    printf(" Output file created: %s\n", outname);
}

void perfect_process(char* filename, int book_id) {
    print_processing_header(filename);
    
    FILE* fp = fopen(filename, "r");
    if(!fp) {
        printf(" ERROR: Could not open file %s\n", filename);
        return;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* text = malloc(size + 1);
    fread(text, 1, size, fp);
    text[size] = '\0';
    fclose(fp);
    
    printf(" Cleaning text data...\n");
    long clean_len = 0;
    char* clean = clean_text_data(text, size, &clean_len);
    printf(" Text cleaned: %ld characters\n", clean_len);
    
    printf(" Extracting words...\n");
    int count = 0;
    char** words = extract_words(clean, clean_len, &count);
    
    total_words[book_id] = count;
    print_file_stats(size, count);
    
    printf(" Initializing storage buckets...\n");
    initialize_buckets(book_id, count);
    printf(" 26 buckets initialized!\n");
    
    printf(" Distributing words to buckets...\n");
    distribute_to_buckets(book_id, words, count);
    printf(" Words distributed across buckets!\n");
    
    sort_all_buckets(book_id);
    
    printf(" Writing output file...\n");
    write_output_file(filename, book_id, count);
    
    free(words);
    free(text);
    free(clean);
    
    printf(" %s PERFECTLY PROCESSED!\n", filename);
    print_separator('=', 60);
}

void display_search_header() {
    printf("\n");
    print_separator('=', 50);
    printf("       EXACT WORD SEARCH        \n");
    print_separator('=', 50);
}

void display_search_results_header(char* keyword) {
    printf("\n SEARCH RESULTS FOR: '%s'\n", keyword);
    print_separator('=', 40);
}

int count_word_in_book(int book, char* keyword) {
    int count = 0;
    
    for(int b = 0; b < 26; b++) {
        for(int i = 0; i < bucket_sizes[book][b]; i++) {
            if(strcmp(all_buckets[book][b][i], keyword) == 0) {
                count++;
            }
        }
    }
    
    return count;
}

void case1_exact_search() {
    char keyword[50];
    
    display_search_header();
    printf(" Enter word to search: ");
    scanf("%s", keyword);

    for(int i = 0; keyword[i]; i++) {
        keyword[i] = tolower(keyword[i]);
    }
    
    int file_count[4] = {0};
    printf("\n Searching across all 4 books...\n");
    
    for(int book = 0; book < 4; book++) {
        file_count[book] = count_word_in_book(book, keyword);
    }
    
    display_search_results_header(keyword);
    
    int found_any = 0;
    for(int book = 0; book < 4; book++) {
        if(file_count[book] > 0) {
            printf(" %-12s    %3d times\n", book_names[book], file_count[book]);
            found_any = 1;
        }
    }
    
    int total = 0;
    for(int book = 0; book < 4; book++) {
        total += file_count[book];
    }
    
    print_separator('=', 40);
    if(!found_any) {
        printf(" No matches found!\n");
    } else {
        printf(" SUCCESS: %d total matches across all books\n", total);
    }
    print_separator('=', 50);
}

int collect_prefix_suggestions(char* prefix, char suggestions[][50], int* file_counts) {
    int suggestion_count = 0;
    
    printf("\n Searching for words starting with '%s'...\n", prefix);
    
    int bucket = tolower(prefix[0]) - 'a';
    if(bucket < 0 || bucket >= 26) {
        bucket = 25;
    }
    
    for(int book = 0; book < 4; book++) {
        for(int i = 0; i < bucket_sizes[book][bucket] && suggestion_count < 1000; i++) {
            char* word = all_buckets[book][bucket][i];
            
            if(strncmp(word, prefix, strlen(prefix)) == 0) {
                int is_duplicate = 0;
                for(int j = 0; j < suggestion_count; j++) {
                    if(strcmp(suggestions[j], word) == 0) {
                        is_duplicate = 1;
                        break;
                    }
                }
                
                if(!is_duplicate) {
                    strcpy(suggestions[suggestion_count], word);
                    file_counts[book]++;
                    suggestion_count++;
                }
            }
        }
    }
    
    return suggestion_count;
}



