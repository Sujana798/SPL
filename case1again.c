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
    
    char* clean = malloc(size + 1);
    long clean_len = 0;
    for(long i = 0; i < size; i++) {
        char c = text[i];
        if(isalpha(c)) clean[clean_len++] = tolower(c);
        else if(c == ' ' || c == '\n' || c == '\t') clean[clean_len++] = ' ';
    }
    clean[clean_len] = '\0';
    
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
    
    all_buckets[book_id] = malloc(26 * sizeof(char**));
    memset(bucket_sizes[book_id], 0, sizeof(bucket_sizes[book_id]));
    
    for(int b = 0; b < 26; b++) {
        all_buckets[book_id][b] = malloc(count * sizeof(char*));
    }
    
    for(int w = 0; w < count; w++) {
        char first = words[w][0];
        if(first >= 'a' && first <= 'z') {
            int bucket = first - 'a';
            all_buckets[book_id][bucket][bucket_sizes[book_id][bucket]++] = words[w];
        } else {
            all_buckets[book_id][25][bucket_sizes[book_id][25]++] = words[w];
        }
    }
    
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

// PERFECT CASE 1 ✅ UNIQUE FILES + COUNT
void case1_exact_search() {
    char keyword[50];
    printf("\n🔍 Enter word to search: ");
    scanf("%s", keyword);
    
    int file_count[4] = {0};
    printf("\n📖 '%s' found in:\n", keyword);
    printf("================\n");
    
    for(int book = 0; book < 4; book++) {
        for(int b = 0; b < 26; b++) {
            for(int i = 0; i < bucket_sizes[book][b]; i++) {
                if(strcmp(all_buckets[book][b][i], keyword) == 0) {
                    file_count[book]++;
                }
            }
        }
    }
    
    for(int book = 0; book < 4; book++) {
        if(file_count[book] > 0) {
            printf("%s (%d times)\n", book_names[book], file_count[book]);
        }
    }
    
    int total = 0;
    for(int book = 0; book < 4; book++) total += file_count[book];
    
    printf("================\n");
    if(total == 0) printf("❌ Not found!\n");
    else printf("✅ Total: %d times\n", total);
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
    
    perfect_process("book1.txt", 0);
    perfect_process("book2.txt", 1);
    perfect_process("book3.txt", 2);
    perfect_process("book4.txt", 3);
    
    printf("✅ ALL BOOKS PROCESSED! Starting Menu...\n\n");
    
    int choice;
    while(1) {
        display_menu();
        if(scanf("%d", &choice) != 1) {
            printf("❌ Invalid input! Try again.\n");
            while(getchar() != '\n');
            continue;
        }
        
        switch(choice) {
            case 1:  case1_exact_search(); break;
            case 2:  printf("📝 Prefix Search - Coming Soon!\n"); break;
            case 3:  printf("📊 A-Z Stats - Coming Soon!\n"); break;
            case 4:  printf("📈 Frequency Count - Coming Soon!\n"); break;
            case 5:  printf("🔢 Longest/Shortest - Coming Soon!\n"); break;
            case 0:  
                printf("\n👋 Thanks for using Text Analyzer!\n");
                return 0;
            default: 
                printf("\n❌ Invalid option! Try 0-5\n");
        }
        
        printf("\nPress Enter to continue...");
        while(getchar() != '\n');
    }
    
    return 0;
}
