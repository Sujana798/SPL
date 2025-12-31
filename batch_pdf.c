#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int main() {
    printf("=== Rapidoop Batch PDF to Text Converter ===\n");
    printf("Scanning folder for PDF files...\n\n");
    
    DIR* dir = opendir(".");
    struct dirent* entry;
    int count = 0;
    
    if(!dir) {
        printf("❌ Error: Cannot read folder!\n");
        printf("Make sure pdftotext.exe is in Rapidoop folder\n");
        return 1;
    }
    
    while((entry = readdir(dir)) != NULL) {
        if(strstr(entry->d_name, ".pdf")) {
            char cmd[300];
            char output_name[200];
            
            // PDF name → TXT name (book1.pdf → book1.txt)
            strcpy(output_name, entry->d_name);
            int len = strlen(output_name);
            if(len > 4 && strcmp(output_name + len - 4, ".pdf") == 0) {
                output_name[len-4] = '\0';  // Remove .pdf
            }
            strcat(output_name, ".txt");
            
            // pdftotext command
            sprintf(cmd, "pdftotext \"%s\" \"%s\"", entry->d_name, output_name);
            
            printf("Converting [%d/4]: %s → %s\n", ++count, entry->d_name, output_name);
            printf("Command: %s\n", cmd);
            
            int res = system(cmd);
            
            if(res == 0) {
                printf("✅ SUCCESS! %s created\n", output_name);
            } else {
                printf("❌ FAILED! Check pdftotext.exe & PDF file\n");
            }
            printf("----------------------------------------\n");
        }
    }
    
    closedir(dir);
    printf("\n=== BATCH COMPLETE! %d PDFs converted ===\n", count);
    printf("Generated: book1.txt, book2.txt, book3.txt, book4.txt\n");
    printf("\nNext steps:\n");
    printf("1. gcc rapido.c -o rapido.exe\n");
    printf("2. ./rapido.exe\n");
    printf("3. Search across all 4 books!\n");
    
    return 0;
}
