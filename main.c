#include "windows.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"

#define IDEA_LENGTH 512
#define DEFAULT_PATH_LENGTH 256
#define KEY_VALUE_LENGTH 256

#define CONFIG_FILE "config.txt"

#define PROMO_CHANCES 25
#define GITHUB_PROFILE "https://github.com/SuperDelphi"

typedef enum PARSE_STATE {
    PARSE_KEY,
    PARSE_VALUE
} ParseState;

void get_absolute_folder_path(char* dest_path) {
    // Get the folder path
    HMODULE module_handle = GetModuleHandle(NULL);
    GetModuleFileName(module_handle, dest_path, DEFAULT_PATH_LENGTH);

    // Strip the executable name
    char* lastSlash = strrchr(dest_path, '\\');

    if (lastSlash) {
        *lastSlash = '\0';
    }

    if (strlen(dest_path) < DEFAULT_PATH_LENGTH - 1) {
        strcat(dest_path, "\\");
    } else {
        perror("Couldn't finish processing the absolute folder path (destination string too short).");
        exit(-1);
    }
}

FILE* open_config() {
    char* config_path = malloc(sizeof(char) * DEFAULT_PATH_LENGTH);
    get_absolute_folder_path(config_path);
    strcat(config_path, CONFIG_FILE);

    FILE* config = fopen(config_path, "a+");

    if (config == NULL) {
        free(config_path);
        perror("Couldn't create the configuration file.");
        exit(-1);
    }

    free(config_path);

    return config;
}

void close_config(FILE* config) {
    int status = fclose(config);

    if (status != 0) {
        perror("Couldn't close the configuration file properly.");
        exit(-1);
    }
}

void create_config_if_not_exists() {
    FILE* config = open_config();
    close_config(config);
}

void config_set(char* key, char* value) {
    // Original file (for removal, later)
    char* config_path = malloc(sizeof(char) * DEFAULT_PATH_LENGTH);
    get_absolute_folder_path(config_path);
    strcat(config_path, CONFIG_FILE);

    FILE* config = open_config();

    // Temp file
    char* temp_path = malloc(sizeof(char) * DEFAULT_PATH_LENGTH);
    get_absolute_folder_path(temp_path);
    strcat(temp_path, "config_temp.txt");

    FILE* temp = fopen(temp_path, "a+");

    if (temp == NULL) {
        free(config_path);
        free(temp_path);
        perror("Couldn't create the temporary file.");
        exit(-1);
    }

    rewind(config);

    // Writing

    const int LINE_SIZE = KEY_VALUE_LENGTH * 2 + 2;
    char line[LINE_SIZE]; // Key (256) + "=" (1) + value (256) + "\n" (1)
    size_t key_len = strlen(key);
    short int found = 0;

    while (fgets(line, LINE_SIZE, config)) {
        if (strncmp(line, key, key_len) == 0 && line[key_len] == '=') {
            fprintf(temp, "%s=%s\n", key, value);
            found = 1;
        } else {
            fputs(line, temp);
        }
    }

    // Adds a new line at the end of the file (to safely add the eventual unset key/value pair)
    fseek(temp, -1, SEEK_END);
    if (fgetc(temp) != '\n') {
        fseek(temp, -1, SEEK_END);
        fputc('\n', temp);
    }

    if (!found) {
        fprintf(temp, "%s=%s\n", key, value);
    }

    close_config(config);
    int temp_status = fclose(temp);

    // Temp close error handling
    if (temp_status != 0) {
        free(config_path);
        free(temp_path);
        perror("Couldn't close the temporary file properly.");
        exit(-1);
    }

    // Replace the original config file with the temporary file

    if (remove(config_path) != 0 || rename(temp_path, config_path) != 0) {
        free(config_path);
        free(temp_path);
        perror("Couldn't update the configuration file.");
        exit(-1);
    }

    free(config_path);
    free(temp_path);
}

bool config_get(char* dest, char* key) {
    FILE* config = open_config();

    rewind(config);

    char cur_key[KEY_VALUE_LENGTH] = "";
    int key_cpt = 0;

    char* cur_value = dest;
    int value_cpt = 0;

    int read; // Character
    ParseState state = PARSE_KEY;

    do {
        read = fgetc(config);
        char c = (char) read;

        if (c == '\n' || read == EOF) { // End of line/file
            cur_value[value_cpt] = '\0';
            value_cpt = 0;

            // Compare with the key in parameter
            if (strcmp(cur_key, key) == 0) {
                return true;
            }

            state = PARSE_KEY;
        } else if (c == '=') { // Begin to parse the value
            cur_key[key_cpt] = '\0';
            key_cpt = 0;

            state = PARSE_VALUE;
        } else if (state == PARSE_KEY && key_cpt < KEY_VALUE_LENGTH - 1) { // Parsing the key
            cur_key[key_cpt++] = c;
        } else if (value_cpt < KEY_VALUE_LENGTH - 1) { // if (state == PARSE_VALUE) {
            cur_value[value_cpt++] = c;
        }
    } while (read != EOF);

    close_config(config);

    return false;
}

void save_into_file(char* path, char* idea) {
    FILE* file = fopen(path, "a+");

    if (file == NULL) {
        perror("Couldn't create/open the file.");
        exit(-1);
    }

    // Save data
    fputs("- ", file);
    fputs(idea, file);
    fputs("\n", file);

    int status = fclose(file);

    if (status != 0) {
        perror("Couldn't close the file properly.");
        exit(-1);
    }
}

void prompt_output_path() {
    char output_path[DEFAULT_PATH_LENGTH] = "";

    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    printf("\nThanks for using Bingo!\nWhere do you want to store your list of ideas? Please paste the ");

    SetConsoleTextAttribute(console_handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
    printf("absolute path (file name included), without quotes");
    SetConsoleTextAttribute(console_handle, 7);

    printf(".\nDon't worry, the file will be created automatically."
           "\n\n\tExample: If you want a text file called ");

    SetConsoleTextAttribute(console_handle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("\"ideas.txt\"");
    SetConsoleTextAttribute(console_handle, 7);

    printf(" on your desktop,"
           "\n\tyour path will be similar to something like: ");

    SetConsoleTextAttribute(console_handle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    printf("\"C:\\Users\\your_name\\Desktop\\ideas.txt\"");
    SetConsoleTextAttribute(console_handle, 7);

    printf("."
           "\n\n> ");

    fflush(stdin);
    fgets(output_path, DEFAULT_PATH_LENGTH - 1, stdin);

    config_set("OUTPUT_PATH", output_path);
}

void show_ideas(char* path) {
    printf("\n");

    FILE* file = fopen(path, "r");

    if (file == NULL) {
        printf("There are no ideas saved yet (file not found).\nYou can add your first idea with \"bingo <idea>\".\n");
        return;
    }

    // Show the list of ideas

    printf("_______________\n\nYour ideas:\n\n");

    char line[IDEA_LENGTH] = "";
    while (fgets(line, IDEA_LENGTH - 1, file)) {
        printf("%s", line);
    }

    printf("\n_______________\n");

    int status = fclose(file);

    if (status != 0) {
        perror("Couldn't close the idea file properly.");
        exit(-1);
    }
}

int main(int argc, char** argv) {
    create_config_if_not_exists();

    char idea[IDEA_LENGTH] = "";

    char file_path[KEY_VALUE_LENGTH] = "";
    bool found = config_get(file_path, "OUTPUT_PATH");

    // Prompt the user to set a definitive file path
    if (!found) {
        prompt_output_path();
        config_get(file_path, "OUTPUT_PATH");
    }

    if (argc < 2) { // Prompt the user
        show_ideas(file_path);
    } else { // Get the sentence from the arguments
        int cpt = 0;

        for (int i = 1; i < argc; i++) {
            if (cpt + strlen(argv[i] + 1) >= IDEA_LENGTH) break;

            strcat(idea, argv[i]);

            if (i != argc - 1) {
                strcat(idea, " ");
            }

            cpt += (int) strlen(argv[i]) + 1;
        }

        save_into_file(file_path, idea);

        HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(console_handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        // Confirmation message
        printf("✓ Saved!");
        SetConsoleTextAttribute(console_handle, 8);
        printf(" %s\n", file_path);

        char disable_promo[KEY_VALUE_LENGTH] = "";
        bool status = config_get(disable_promo, "DISABLE_PROMO");

        if (!status || strcmp(disable_promo, "true") != 0) {
            srand(time(NULL));
            if (rand() % PROMO_CHANCES == 0) {
                SetConsoleTextAttribute(console_handle, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                printf("★ Oh, by the way: don't hesitate to check out my \e]8;;%s\a%s\e]8;;\a (%s)\n", GITHUB_PROFILE, "GitHub profile", GITHUB_PROFILE);
            }
        }

        SetConsoleTextAttribute(console_handle, 7);
    }

    return 0;
}