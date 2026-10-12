#include <stdio.h>
#include <stdint.h>
#include <string.h>

int8_t is_ascii(char string[]) {
    for (int i = 0; string[i] != '\0'; i++) {
        if ((unsigned char)string[i] > 127) {
            return 0;
        }
    }
    return 1;
}

int32_t capitalize_ascii(char str[]) {
    int32_t count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 32;
            count++;
        }
    }
    return count;
}

int8_t codepoint_size(char string[]) {
    unsigned char b = (unsigned char)string[0];
    if (b == '\0') {
        return 0;
    }
    if ((b & 0x80) == 0x00) {
        return 1;
    }
    if ((b & 0xE0) == 0xC0) {
        return 2;
    }
    if ((b & 0xF0) == 0xE0) {
        return 3;
    }
    if ((b & 0xF8) == 0xF0) {
        return 4;
    }
    return -1;
}

int32_t utf8_strlen(char str[]) {
    int32_t count = 0;
    int i = 0;
    while (str[i] != '\0') {
        int8_t size = codepoint_size(&str[i]);
        if (size == -1) {
            return -1;
        }
        for (int k = 1; k < size; k++) {
            if (((unsigned char)str[i + k] & 0xC0) != 0x80) {
                return -1;
            }
        }
        i += size;
        count++;
    }
    return count;
}

void utf8_substring(const char str[], int start, int end, char result[]) {
    int r = 0;
    if (start >= 0 && start < end) {
        int cp = -1;
        for (int i = 0; str[i] != '\0'; i++) {
            if (((unsigned char)str[i] & 0xC0) != 0x80) {
                cp++;
            }
            if (cp >= end) {
                break;
            }
            if (cp >= start) {
                result[r] = str[i];
                r++;
            }
        }
    }
    result[r] = '\0';
}

int32_t codepoint_at(char str[], int32_t byte_index) {
    if (byte_index < 0 || byte_index >= (int32_t)strlen(str)) {
        return -1;
    }
    unsigned char b0 = (unsigned char)str[byte_index];
    if ((b0 & 0x80) == 0x00) {
        return b0;
    }
    if ((b0 & 0xE0) == 0xC0) {
        unsigned char b1 = (unsigned char)str[byte_index + 1];
        return ((b0 & 0x1F) << 6) | (b1 & 0x3F);
    }
    if ((b0 & 0xF0) == 0xE0) {
        unsigned char b1 = (unsigned char)str[byte_index + 1];
        unsigned char b2 = (unsigned char)str[byte_index + 2];
        return ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
    }
    if ((b0 & 0xF8) == 0xF0) {
        unsigned char b1 = (unsigned char)str[byte_index + 1];
        unsigned char b2 = (unsigned char)str[byte_index + 2];
        unsigned char b3 = (unsigned char)str[byte_index + 3];
        return ((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
    }
    return -1;
}

int is_animal_emoji_at(const char str[], int index) {
    if (index < 0) {
        return 0;
    }
    int cp = -1;
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char b = (unsigned char)str[i];
        if ((b & 0xC0) != 0x80) {
            cp++;
            if (cp == index) {
                if ((b & 0xF8) != 0xF0) {
                    return 0;
                }
                unsigned char b1 = (unsigned char)str[i + 1];
                unsigned char b2 = (unsigned char)str[i + 2];
                unsigned char b3 = (unsigned char)str[i + 3];
                int c = ((b & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
                return (c >= 0x1F400 && c <= 0x1F43F) || (c >= 0x1F980 && c <= 0x1F9AE);
            }
        }
    }
    return 0;
}

int main() {
    char input[1000];
    printf("Enter a UTF-8 encoded string: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        input[0] = '\0';
    }
    input[strcspn(input, "\r\n")] = '\0';
    printf("\n");

    // Valid ASCII
    if (is_ascii(input)) {
        printf("Valid ASCII: true\n");
    } else {
        printf("Valid ASCII: false\n");
    }

    // Uppercased ASCII (work on a copy so the original stays unchanged)
    char upper[1000];
    strcpy(upper, input);
    capitalize_ascii(upper);
    printf("Uppercased ASCII: \"%s\"\n", upper);

    // Length in bytes
    printf("Length in bytes: %d\n", (int)strlen(input));

    // Number of code points
    int32_t num_cp = utf8_strlen(input);
    printf("Number of code points: %d\n", num_cp);

    // Bytes per code point
    printf("Bytes per code point:");
    int i = 0;
    while (input[i] != '\0') {
        int8_t size = codepoint_size(&input[i]);
        if (size <= 0) {
            break;
        }
        printf(" %d", size);
        i += size;
    }
    printf("\n");

    // Substring of the first 6 code points
    char sub[1000];
    utf8_substring(input, 0, 6, sub);
    printf("Substring of the first 6 code points: \"%s\"\n", sub);

    // Code points as decimal numbers
    printf("Code points as decimal numbers:");
    i = 0;
    while (input[i] != '\0') {
        int8_t size = codepoint_size(&input[i]);
        if (size <= 0) {
            break;
        }
        printf(" %d", codepoint_at(input, i));
        i += size;
    }
    printf("\n");

    // Animal emojis
    printf("Animal emojis:");
    int cp_index = 0;
    i = 0;
    while (input[i] != '\0') {
        int8_t size = codepoint_size(&input[i]);
        if (size <= 0) {
            break;
        }
        if (is_animal_emoji_at(input, cp_index)) {
            printf(" %.*s", size, &input[i]);
        }
        i += size;
        cp_index++;
    }
    printf("\n");

    return 0;
}
