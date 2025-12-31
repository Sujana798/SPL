#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Global buffers
char* all_text = NULL;
int text_size = 0;
int file_count = 0;

void load_and_clean_files() {
    printf("=== Load + Clean All .txt Files ===\n");
    
    // Hardcode 4 files
    char* filenames[] = {"book1.txt", "book2.txt", "book3.txt", "book4.txt"};
    
    for(int i = 0; i < 4; i++) {
        printf("\n--- Processing [%d]: %s ---\n", i+1, filenames[i]);
        
        // Step 1: Load raw file
        FILE* in_fp = fopen(filenames[i], "r");
        if(!in_fp) {
            printf("%s not found\n", filenames[i]);
            continue;
        }
        
        fseek(in_fp, 0, SEEK_END);
        long raw_size = ftell(in_fp);
        fseek(in_fp, 0, SEEK_SET);
        
        char* raw_text = malloc(raw_size + 1);
        fread(raw_text, 1, raw_size, in_fp);
        raw_text[raw_size] = '\0';
        fclose(in_fp);
        
        // Step 2: Clean (remove punctuation + lowercase)
        printf("Cleaning %ld raw chars...\n", raw_size);
        char* clean_text = malloc(raw_size + 1);
        int clean_count = 0;
        
        for(int j = 0; j < raw_size; j++) {
            char c = raw_text[j];
            if(isalpha(c)) {
                clean_text[clean_count++] = tolower(c);
            } else if(c == ' ' || c == '\n' || c == '\t') {
                clean_text[clean_count++] = ' ';
            }
        }
        clean_text[clean_count] = '\0';
        
        // Step 3: Add to global buffer
        char* temp = realloc(all_text, text_size + clean_count + 4);
        if(temp) {
            all_text = temp;
            strcpy(all_text + text_size, clean_text);
            text_size += clean_count;
            
            // Add file separator
            all_text[text_size++] = '\n';
            all_text[text_size++] = '-';
            all_text[text_size++] = '-';
            all_text[text_size++] = '-';
            all_text[text_size] = '\0';
            
            file_count++;
            printf(" %s  %d clean chars added\n", filenames[i], clean_count);
        }
        
        free(raw_text);
        free(clean_text);
    }
    
    printf("\n=== SUMMARY ===\n");
    printf(" Total %d files processed!\n", file_count);
    printf(" Total clean chars: %d\n", text_size);
}

int main() {
    load_and_clean_files();
    
    printf("\n=== READY FOR ANALYSIS ===\n");
    printf("all_text buffer contains all clean data\n");
    
    return 0;
}
