#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma warning(disable : 4996)

#define STRINGS_COUNT 5
#define MAX_STRING_SIZE 10

void generate_random_string(char* str, int size) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int charset_size = strlen(charset);
    for (int i = 0; i < size; i++) {
        str[i] = charset[rand() % charset_size];
    }
    str[size] = '\0';
}

void generate_test_file(const char* filename) {
    srand(time(NULL));

    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < STRINGS_COUNT; i++) {
        int len = rand() % (3 * MAX_STRING_SIZE - 1) + 1;
        char str[3 * MAX_STRING_SIZE];
        generate_random_string(str, len);
        fprintf(file, "%s\n", str);
    }

    fclose(file);
}

void read_strings_from_file(const char* filename, char static_array[][MAX_STRING_SIZE], char* dynamic_array[]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < STRINGS_COUNT; i++) {
        char buffer[1000];
        if (fgets(buffer, sizeof(buffer), file)) {
            size_t len = strlen(buffer);
            if (buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            len--;

            if (len <= MAX_STRING_SIZE) {
                strncpy(static_array[i], buffer, len);
                memset(static_array[i] + len, ' ', MAX_STRING_SIZE - len);
                dynamic_array[i] = NULL;
            }
            else {
                strncpy(static_array[i], buffer, MAX_STRING_SIZE);
                dynamic_array[i] = strdup(buffer + MAX_STRING_SIZE);
            }
        }
    }

    fclose(file);
}

void print_strings(char static_array[][MAX_STRING_SIZE], char* dynamic_array[]) {
    for (int i = 0; i < STRINGS_COUNT; i++) {
        for (int j = 0; j < MAX_STRING_SIZE; j++) {
            printf("%c", static_array[i][j]);
        }
        if (dynamic_array[i]) {
            printf("%s", dynamic_array[i]);
        }
        printf("\n");
    }
    printf("\n");
}

int my_strcasecmp(const char* str1, const char* str2) {
    while (*str1 && *str2) {
        unsigned char c1 = tolower(*str1);
        unsigned char c2 = tolower(*str2);
        if (c1 != c2) {
            return c1 - c2;
        }
        str1++;
        str2++;
    }
    return tolower(*str1) - tolower(*str2);
}

int compare_lexicographic(char static_str1[][MAX_STRING_SIZE], char* dynamic_str1[],
    char static_str2[][MAX_STRING_SIZE], char* dynamic_str2[]) {
    for (int i = 0; i < STRINGS_COUNT; i++) {
        int result = strncmp(static_str1[i], static_str2[i], MAX_STRING_SIZE);
        if (result != 0) {
            return result;
        }

        if (dynamic_str1[i] && dynamic_str2[i]) {
            result = strcmp(dynamic_str1[i], dynamic_str2[i]);
            if (result != 0) {
                return result;
            }
        }
    }

    return 0;
}

int compare_lexicographic_case_insensitive(char static_str1[][MAX_STRING_SIZE], char* dynamic_str1[],
    char static_str2[][MAX_STRING_SIZE], char* dynamic_str2[]) {
    for (int i = 0; i < STRINGS_COUNT; i++) {
        int result = my_strcasecmp(static_str1[i], static_str2[i]);
        if (result != 0) {
            return result;
        }

        if (dynamic_str1[i] && dynamic_str2[i]) {
            result = my_strcasecmp(dynamic_str1[i], dynamic_str2[i]);
            if (result != 0) {
                return result;
            }
        }
    }

    return 0;
}

int compare_length(char static_str1[][MAX_STRING_SIZE], char* dynamic_str1[],
    char static_str2[][MAX_STRING_SIZE], char* dynamic_str2[]) {
    for (int i = 0; i < STRINGS_COUNT; i++) {
        int len1 = 0, len2 = 0;

        while (len1 < MAX_STRING_SIZE && static_str1[i][len1] != ' ') {
            len1++;
        }
        while (len2 < MAX_STRING_SIZE && static_str2[i][len2] != ' ') {
            len2++;
        }

        if (len1 != len2) {
            return len2 - len1;
        }
        if (dynamic_str1[i] && dynamic_str2[i]) {
            int dyn_len1 = strlen(dynamic_str1[i]);
            int dyn_len2 = strlen(dynamic_str2[i]);
            if (dyn_len1 != dyn_len2) {
                return dyn_len2 - dyn_len1;
            }
        }
        else if (dynamic_str1[i] && !dynamic_str2[i]) {
            return -1;
        }
        else if (!dynamic_str1[i] && dynamic_str2[i]) {
            return 1;
        }
    }
    return 0;
}

void count_char_frequency(const char str[][MAX_STRING_SIZE], char* dynamic_array[], int index, char target_char, int* freq) {
    int count = 0;
    int max_count = MAX_STRING_SIZE;

    const char* dyn_str = dynamic_array[index];
    for (int j = 0; j < MAX_STRING_SIZE; j++) {
        char ch;
        if (j < max_count) {
            ch = str[index][j];
            if (ch == ' ') {
                max_count = j;
                break;
            }
            if (ch == target_char) {
                count++;
            }
        }
        else if (dyn_str && *dyn_str != '\0') {
            ch = *dyn_str++;
            if (ch == target_char) {
                count++;
            }
        }
        
    }
    *freq = count;
}

int compare_char_frequency(char static_str1[][MAX_STRING_SIZE], char* dynamic_str1[],
    char static_str2[][MAX_STRING_SIZE], char* dynamic_str2[], char target_char) {
    int freq1[STRINGS_COUNT] = { 0 };
    int freq2[STRINGS_COUNT] = { 0 };

    for (int i = 0; i < STRINGS_COUNT; i++) {
        count_char_frequency(static_str1, dynamic_str1, i, target_char, &freq1[i]);
        count_char_frequency(static_str2, dynamic_str2, i, target_char, &freq2[i]);
    }

    for (int i = 0; i < STRINGS_COUNT; i++) {
        if (freq1[i] != freq2[i]) {
            return freq2[i] - freq1[i];
        }
    }
    return 0;
}

void sort_strings(char static_array[][MAX_STRING_SIZE], char* dynamic_array[],
    char target_char, int (*compare)(char[][MAX_STRING_SIZE], char* [], char[][MAX_STRING_SIZE], char* [], char)) {

    for (int i = 0; i < STRINGS_COUNT - 1; i++) {
        for (int j = 0; j < STRINGS_COUNT - 1 - i; j++) {
            int result = compare(static_array + j, dynamic_array + j, static_array + j + 1, dynamic_array + j + 1, target_char);
            if (result > 0) {
                char temp_static[MAX_STRING_SIZE];
                strncpy(temp_static, static_array[j], MAX_STRING_SIZE);
                strncpy(static_array[j], static_array[j + 1], MAX_STRING_SIZE);
                strncpy(static_array[j + 1], temp_static, MAX_STRING_SIZE);

                char* temp_dynamic = dynamic_array[j];
                dynamic_array[j] = dynamic_array[j + 1];
                dynamic_array[j + 1] = temp_dynamic;
            }
        }
    }
}


int main() {
    const char* filename = "test_strings.txt";
    char target_char = 'a';

    char static_array[STRINGS_COUNT][MAX_STRING_SIZE];
    char* dynamic_array[STRINGS_COUNT];

    generate_test_file(filename);
    read_strings_from_file(filename, static_array, dynamic_array);

    printf("Before sorting:\n");
    print_strings(static_array, dynamic_array);

    sort_strings(static_array, dynamic_array, target_char, compare_lexicographic);
    printf("Sorted by lexicographic:\n");
    print_strings(static_array, dynamic_array);

    sort_strings(static_array, dynamic_array, target_char, compare_lexicographic_case_insensitive);
    printf("Sorted by lexicographic case insensitive:\n");
    print_strings(static_array, dynamic_array);

    sort_strings(static_array, dynamic_array, target_char, compare_length);
    printf("Sorted by length:\n");
    print_strings(static_array, dynamic_array);

    sort_strings(static_array, dynamic_array, target_char, compare_char_frequency);
    printf("Sorted by character frequency (target_char = '%c'):\n", target_char);
    print_strings(static_array, dynamic_array);

    for (int i = 0; i < STRINGS_COUNT; i++) {
        free(dynamic_array[i]);
    }

    return 0;
}
