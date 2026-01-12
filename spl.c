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

void sort_suggestions_alphabetically(char suggestions[][50], int count) {
    for(int i = 0; i < count - 1; i++) {
        for(int j = 0; j < count - i - 1; j++) {
            if(strcmp(suggestions[j], suggestions[j+1]) > 0) {
                char temp[50];
                strcpy(temp, suggestions[j]);
                strcpy(suggestions[j], suggestions[j+1]);
                strcpy(suggestions[j+1], temp);
            }
        }
    }
}

void display_page_header(int current_page, int total_pages, int start, int end, char* prefix) {
    printf("\n PAGE %d/%d | Words %d-%d | Prefix: '%s'\n", 
           current_page + 1, total_pages, start + 1, end, prefix);
    print_separator('=', 50);
}

void display_page_words(char suggestions[][50], int start, int end) {
    for(int i = start; i < end; i++) {
        printf("%4d. %s\n", i + 1, suggestions[i]);
    }
}

void display_page_commands() {
    printf("\n AVAILABLE COMMANDS:\n");
    printf("N = Next Page\n");
    printf("P = Previous Page\n");
    printf("L = Last Page\n");
    printf("E = Check Exact Match\n");
    printf("A = Add to Prefix\n");
    printf("D = Delete from Prefix\n");
    printf("0 = Back to Menu\n");
    printf(" Enter command: ");
}

void check_exact_match(char* prefix, char suggestions[][50], int count, int* file_counts) {
    int found_exact = 0;
    
    for(int i = 0; i < count; i++) {
        if(strcmp(suggestions[i], prefix) == 0) {
            found_exact = 1;
            break;
        }
    }
    
    if(found_exact) {
        printf("\n EXACT MATCH FOUND: '%s'\n", prefix);
        printf(" OCCURRENCE BY BOOK:\n");
        print_separator('=', 30);
        
        int total_count = 0;
        for(int book = 0; book < 4; book++) {
            if(file_counts[book] > 0) {
                printf(" %-12s   %d times\n", book_names[book], file_counts[book]);
                total_count += file_counts[book];
            }
        }
        
        print_separator('=', 30);
        printf("Total occurrences: %d\n", total_count);
        printf("\nPress Enter to continue...");
        while(getchar() != '\n');
        getchar();
    } else {
        printf("\n No exact match found for '%s'\n", prefix);
        printf("Press Enter to continue...");
        while(getchar() != '\n');
        getchar();
    }
}

void case2_prefix_search() {
    char prefix[50] = {0};
    
    printf("\n");
    print_separator('=', 50);
    printf("       PREFIX SEARCH (Paginated)      \n");
    print_separator('=', 50);
    printf(" Enter prefix (e.g., 'r'): ");
    scanf("%s", prefix);
    
    while(1) {
        char suggestions[1000][50];
        int file_counts[4] = {0};
        int suggestion_count = collect_prefix_suggestions(prefix, suggestions, file_counts);
        
        sort_suggestions_alphabetically(suggestions, suggestion_count);
        
        const int PAGE_SIZE = 20;
        int total_pages = (suggestion_count + PAGE_SIZE - 1) / PAGE_SIZE;
        int current_page = 0;
        
        printf("\n Found %d unique words | %d pages total\n", 
               suggestion_count, total_pages);

        while(1) {
            int start = current_page * PAGE_SIZE;
            int end = (current_page + 1) * PAGE_SIZE;
            if(end > suggestion_count) {
                end = suggestion_count;
            }
            
            display_page_header(current_page, total_pages, start, end, prefix);
            display_page_words(suggestions, start, end);
            display_page_commands();
            
            char command;
            scanf(" %c", &command);
            
            if(command == 'N' || command == 'n') {
                if(current_page < total_pages - 1) {
                    current_page++;
                } else {
                    printf(" Already on last page!\n");
                }
            }
            else if(command == 'P' || command == 'p') {
                if(current_page > 0) {
                    current_page--;
                } else {
                    printf(" Already on first page!\n");
                }
            }
            else if(command == 'L' || command == 'l') {
                current_page = total_pages - 1;
            }
            else if(command == 'E' || command == 'e') {
                check_exact_match(prefix, suggestions, suggestion_count, file_counts);
            }
            else if(command == 'A' || command == 'a') {
                printf(" Current prefix: '%s' + ", prefix);
                char add[10];
                scanf("%s", add);
                strcat(prefix, add);
                printf(" New prefix: '%s'\n", prefix);
                printf("Press Enter to search...");
                while(getchar() != '\n');
                getchar();
                break; 
            }
            else if(command == 'D' || command == 'd') {
                int len = strlen(prefix);
                if(len > 0) {
                    prefix[len-1] = '\0';
                    printf(" New prefix: '%s'\n", prefix);
                } else {
                    printf(" Cannot delete more characters!\n");
                }
                printf("Press Enter to search...");
                while(getchar() != '\n');
                getchar();
                break; 
            }
            else if(command == '0') {
                return; 
            }
            else {
                printf(" Invalid command!\n");
            }
        }
    }
}

void display_menu() {
    printf("\n");
    print_separator('=', 50);
    printf("       TEXT ANALYZER PRO       \n");
    print_separator('=', 50);
    printf("1. Exact Keyword Search\n");
    printf("2. Prefix Search (Interactive)\n");
    printf("3. A-Z Column Statistics\n");
    printf("4. Word Frequency Count\n");
    printf("5. Longest/Shortest Words\n");
    printf("0. Exit Program\n");
    print_separator('=', 50);
    printf(" Enter your choice: ");
}

int main() {
    printf(" TEXT ANALYZER PRO - PERFECT PROCESSOR v2.0\n");
    print_separator('=', 60);
    printf("Starting book processing...\n");
    
    perfect_process("book1.txt", 0);
    perfect_process("book2.txt", 1);
    perfect_process("book3.txt", 2);
    perfect_process("book4.txt", 3);
    
    printf("\n ALL BOOKS SUCCESSFULLY PROCESSED!\n");
    printf(" Starting interactive menu system...\n\n");
    
    int choice;
    while(1) {
        display_menu();
        
        if(scanf("%d", &choice) != 1) {
            printf(" Invalid input! Please enter a number.\n");
            while(getchar() != '\n');
            continue;
        }
        
        switch(choice) {
            case 1:
                case1_exact_search();
                break;
            
            case 2:
                case2_prefix_search();
                break;
            
            case 3:
                printf("\n A-Z Column Statistics\n");
                printf(" Feature coming soon!\n");
                break;
            
            case 4:
                printf("\n Word Frequency Count\n");
                printf(" Feature coming soon!\n");
                break;
            
            case 5:
                printf("\n Longest/Shortest Words Analysis\n");
                printf(" Feature coming soon!\n");
                break;
            
            case 0:
                printf("\n");
                print_separator('=', 50);
                printf(" Thank you for using Text Analyzer Pro!\n");
                printf(" Have a great day!\n");
                print_separator('=', 50);
                return 0;
            
            default:
                printf("\n Invalid option! Please choose 0-5\n");
        }
        
        printf("\nPress Enter to continue...");
        while(getchar() != '\n');
        getchar();
    }
    
    return 0;
}


