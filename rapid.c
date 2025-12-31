#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global buffer
char* all_text = NULL;
int text_size = 0;
int file_count = 0;

void load_all_text_files() {
    printf("=== Loading All .txt Files ===\n");
    
    // Hardcode 4 files
    char* filenames[] = {"book1.txt", "book2.txt", "book3.txt", "book4.txt"};
    
    for(int i = 0; i < 4; i++) {
        printf("Loading [%d]: %s\n", i+1, filenames[i]);
        
        FILE* fp = fopen(filenames[i], "r");
        if(fp) {
            // Get file size
            fseek(fp, 0, SEEK_END);
            long size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            
            // Allocate + read
            char* temp = realloc(all_text, text_size + size + 2);
            if(temp) {
                all_text = temp;
                fread(all_text + text_size, 1, size, fp);
                text_size += size;
                
                // Add separator
                all_text[text_size++] = '\n';
                all_text[text_size++] = '\n';
                all_text[text_size] = '\0';
                file_count++;  // FIXED: file_count increment
            }
            fclose(fp);
            printf("✅ Loaded %ld bytes\n", size);
        } else {
            printf("❌ %s not found\n", filenames[i]);
        }
        printf("\n");
    }
    
    printf("✅ Total %d files! %d chars loaded\n", file_count, text_size);
}

// Usage in main():
int main() {
    load_all_text_files();
    return 0;
}
