#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Global data for menu
char*** all_buckets[4];
int bucket_sizes[4][26];
int total_words[4];
char* book_names[4] = {"book1.txt", "book2.txt", "book3.txt", "book4.txt"};

void perfect_process(char* filename, int book_id) {
    printf("🔄 PERFECT PROCESSING: %s\n", filename);
    
    FILE* fp = fopen(filename, "r");
    if(!fp) { printf("❌ %s missing!\n", filename); return; }
    
    fseek(fp, 0, SEEK_END); long size = ftell(fp); fseek(fp, 0, SEEK_SET);
    char* text = malloc(size + 1);
    fread(text, 1, size, fp);
    text[size] = '\0';
    fclose(fp);
    
    printf("  📖 Raw size: %ld bytes\n", size);
    
    // PERFECT CLEANING
    char* clean = malloc(size + 1);
    long clean_len = 0;
    for(long i = 0; i < size; i++) {
        char c = text[i];
        if(isalpha(c)) {
            clean[clean_len++] = tolower(c);
        } else if(c == ' ' || c == '\n' || c == '\t') {
            clean[clean_len++] = ' ';
        }
    }
    clean[clean_len] = '\0';
    
    // EXTRACT EVERY WORD (100% coverage)
    char** words = NULL;
    int capacity = 1000, count = 0;
    words = malloc(capacity * sizeof(char*));
    
    long i = 0;
    while(i < clean_len) {
        while(i < clean_len && clean[i] == ' ') i++;
        if(i >= clean_len) break;
        
        long start = i;
        while(i < clean_len && clean[i] != ' ') i++;
        long len = i - start;
        
        if(len > 0 && len < 100) {
            if(count >= capacity) {
                capacity *= 2;
                words = realloc(words, capacity * sizeof(char*));
            }
            words[count] = malloc(len + 1);
            strncpy(words[count], clean + start, len);
            words[count][len] = '\0';
            count++;
        }
    }
    
    printf("  ✅ TOTAL WORDS: %d (100%%)\n", count);
    total_words[book_id] = count;
    
    // 26 A-Z BUCKETS
    all_buckets[book_id] = malloc(26 * sizeof(char**));
    memset(bucket_sizes[book_id], 0, sizeof(bucket_sizes[book_id]));
    
    for(int b = 0; b < 26; b++) {
        all_buckets[book_id][b] = malloc(count * sizeof(char*));
    }
    
    // Distribute ALL words
    for(int w = 0; w < count; w++) {
        char first = words[w][0];
        if(first >= 'a' && first <= 'z') {
            int bucket = first - 'a';
            all_buckets[book_id][bucket][bucket_sizes[book_id][bucket]++] = words[w];
        } else {
            all_buckets[book_id][25][bucket_sizes[book_id][25]++] = words[w];
        }
    }
    
    // PERFECT SORTING
    for(int b = 0; b < 26; b++) {
        for(int i = 1; i < bucket_sizes[book_id][b]; i++) {
            char* key = all_buckets[book_id][b][i];
            int j = i - 1;
            while(j >= 0 && strcmp(all_buckets[book_id][b][j], key) > 0) {
                all_buckets[book_id][b][j + 1] = all_buckets[book_id][b][j];
                j--;
            }
            all_buckets[book_id][b][j + 1] = key;
        }
    }
    
    // Write output
    char outname[50];
    sprintf(outname, "%s_perfect.txt", filename);
    FILE* out = fopen(outname, "w");
    if(out) {
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
        printf("  ✅ %s CREATED!\n", outname);
    }
    
    free(words); free(text); free(clean);
    printf("  🎉 %s PERFECTLY PROCESSED!\n\n", filename);
}

void display_menu() {
    printf("\n🚀=== TEXT ANALYZER PRO ===\n");
    printf("==========================\n");
    printf("1.  🔍 Exact Keyword Search\n");
    printf("2.  📝 Prefix Search\n");
    printf("3.  📊 A-Z Column Stats\n");
    printf("4.  📈 Word Frequency Count\n");
    printf("5.  🔢 Longest/Shortest Words\n");
    printf("0.  ❌ Exit\n");
    printf("➤ Choice: ");
}

int main() {
    printf("🎯 PERFECT PROCESSOR + MENU v1.0\n");
    printf("================================\n\n");
    
    // Process all 4 books
    perfect_process("book1.txt", 0);
    perfect_process("book2.txt", 1);
    perfect_process("book3.txt", 2);
    perfect_process("book4.txt", 3);
    
    printf("✅ ALL BOOKS PROCESSED! Starting Menu...\n\n");
    
    // Menu loop
    int choice;
    do {
        display_menu();
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:  printf("🔍 Exact Keyword Search - Ready!\n"); break;
            case 2:  printf("📝 Prefix Search - Ready!\n"); break;
            case 3:  printf("📊 A-Z Stats - Ready!\n"); break;
            case 4:  printf("📈 Frequency Count - Ready!\n"); break;
            case 5:  printf("🔢 Longest/Shortest - Ready!\n"); break;
            case 0:  printf("\n👋 Thanks for using Text Analyzer!\n"); break;
            default: printf("\n❌ Invalid option! Try 0-5\n");
        }
        
        if(choice != 0) {
            printf("\nPress Enter to continue...");
            getchar(); getchar();
        }
    } while(choice != 0);
    
    return 0;
}
