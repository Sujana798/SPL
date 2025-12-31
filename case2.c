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

// CASE 1: DETAILED EXACT SEARCH
void case1_exact_search() {
    char keyword[50];
    printf("\n🔍================================\n");
    printf("🔍       EXACT WORD SEARCH        \n");
    printf("🔍================================\n");
    printf("🔍 Enter word to search: ");
    scanf("%s", keyword);
    
    int file_count[4] = {0};
    printf("\n🔍 Searching in 4 books...\n");
    
    for(int book = 0; book < 4; book++) {
        for(int b = 0; b < 26; b++) {
            for(int i = 0; i < bucket_sizes[book][b]; i++) {
                if(strcmp(all_buckets[book][b][i], keyword) == 0) {
                    file_count[book]++;
                }
            }
        }
    }
    
    printf("\n📖 '%s' SEARCH RESULTS:\n", keyword);
    printf("════════════════════════\n");
    
    for(int book = 0; book < 4; book++) {
        if(file_count[book] > 0) {
            printf("📄 %-12s → %3d times\n", book_names[book], file_count[book]);
        }
    }
    
    int total = 0;
    for(int book = 0; book < 4; book++) total += file_count[book];
    
    printf("════════════════════════\n");
    if(total == 0) printf("❌ No matches found!\n");
    else printf("✅ SUCCESS: %d total matches\n", total);
    printf("🔍================================\n");
}

// CASE 2: INTERACTIVE PREFIX SEARCH ✅ FIXED
// CASE 2: FULL PAGINATION + ADD/DELETE PREFIX (150+ lines)
void case2_prefix_search() {
    char prefix[50] = {0};
    
    printf("\n📝═══════════════════════════════════════\n");
    printf("📝       PREFIX SEARCH (Paginated)      \n");
    printf("📝═══════════════════════════════════════\n");
    printf("📝 Enter prefix (e.g., 'r'): ");
    scanf("%s", prefix);
    
    while(1) {
        // Collect suggestions (NO DUPLICATES)
        char suggestions[1000][50];
        int suggestion_count = 0;
        int file_counts[4] = {0};
        
        printf("\n🔍 Searching '%s*' in all books...\n", prefix);
        
        int bucket = tolower(prefix[0]) - 'a';
        if(bucket < 0 || bucket >= 26) bucket = 25;
        
        for(int book = 0; book < 4; book++) {
            for(int i = 0; i < bucket_sizes[book][bucket] && suggestion_count < 1000; i++) {
                char* word = all_buckets[book][bucket][i];
                if(strncmp(word, prefix, strlen(prefix)) == 0) {
                    // REMOVE DUPLICATES
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
        
        // SORT ALPHABETICALLY
        for(int i = 0; i < suggestion_count - 1; i++) {
            for(int j = 0; j < suggestion_count - i - 1; j++) {
                if(strcmp(suggestions[j], suggestions[j+1]) > 0) {
                    char temp[50];
                    strcpy(temp, suggestions[j]);
                    strcpy(suggestions[j], suggestions[j+1]);
                    strcpy(suggestions[j+1], temp);
                }
            }
        }
        
        // PAGINATION SETUP
        const int PAGE_SIZE = 20;
        int total_pages = (suggestion_count + PAGE_SIZE - 1) / PAGE_SIZE;
        int current_page = 0;
        
        printf("\n📊 %d UNIQUE WORDS | %d PAGES\n", suggestion_count, total_pages);
        
        // PAGINATION LOOP
        while(1) {
            int start = current_page * PAGE_SIZE;
            int end = (current_page + 1) * PAGE_SIZE;
            if(end > suggestion_count) end = suggestion_count;
            
            printf("\n📄 PAGE %d/%d | Words %d-%d | Prefix: '%s'\n", 
                   current_page + 1, total_pages, start + 1, end, prefix);
            printf("═══════════════════════════════════════\n");
            
            // DISPLAY CURRENT PAGE
            for(int i = start; i < end; i++) {
                printf("%4d. %s\n", i + 1, suggestions[i]);
            }
            
            printf("\n📱 COMMANDS:\n");
            printf("N=Next | P=Prev | L=Last | E=Exact | A=Add | D=Del | 0=Back\n");
            printf("➤ Command: ");
            
            char command;
            scanf(" %c", &command);
            
            if(command == 'N' || command == 'n') {
                if(current_page < total_pages - 1) current_page++;
            }
            else if(command == 'P' || command == 'p') {
                if(current_page > 0) current_page--;
            }
            else if(command == 'L' || command == 'l') {
                current_page = total_pages - 1;
            }
            else if(command == 'E' || command == 'e') {
                // CHECK EXACT MATCH
                int found_exact = 0;
                for(int i = 0; i < suggestion_count; i++) {
                    if(strcmp(suggestions[i], prefix) == 0) {
                        found_exact = 1;
                        break;
                    }
                }
                if(found_exact) {
                    printf("\n🎯 EXACT MATCH '%s' FOUND!\n", prefix);
                    printf("📊 LOCATION:\n");
                    printf("═══════════════\n");
                    int total_count = 0;
                    for(int book = 0; book < 4; book++) {
                        if(file_counts[book] > 0) {
                            printf("📄 %s (%d times)\n", book_names[book], file_counts[book]);
                            total_count += file_counts[book];
                        }
                    }
                    printf("═══════════════\n");
                    printf("✅ Total: %d times\n", total_count);
                    printf("\nPress Enter...");
                    while(getchar() != '\n');
                    return;
                } else {
                    printf("❌ '%s' not found exactly!\n", prefix);
                    printf("Press Enter...");
                    getchar();
                }
            }
            else if(command == 'A' || command == 'a') {
                // ADD LETTERS TO PREFIX
                printf("➕ Current: '%s' + ", prefix);
                char add[10];
                scanf("%s", add);
                strcat(prefix, add);
                printf("✅ New prefix: '%s'\n", prefix);
                printf("Press Enter to search...");
                while(getchar() != '\n');
                break;  // Restart search with new prefix
            }
            else if(command == 'D' || command == 'd') {
                // DELETE LAST LETTER
                int len = strlen(prefix);
                if(len > 0) {
                    prefix[len-1] = '\0';
                    printf("🔙 New prefix: '%s'\n", prefix);
                } else {
                    printf("❌ Cannot delete more!\n");
                }
                printf("Press Enter to search...");
                while(getchar() != '\n');
                break;  // Restart search
            }
            else if(command == '0') {
                return;  // Back to menu
            }
        }
    }
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
    printf("🎯 PERFECT PROCESSOR + MENU v2.0\n");
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
            case 2:  case2_prefix_search(); break;
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
