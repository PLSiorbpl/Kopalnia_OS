namespace string {
    int strlen(const char* text) {
        int len = 0;
        while (text[len] != '\0') {
            len++;
        }
        return len;
    }

    void int_to_str(char* buffer, int value) {
        int i = 0;
        bool is_negative = false;

        if (value == 0) {
            buffer[0] = '0';
            buffer[1] = '\0';
            return;
        }
        if (value < 0) {
            is_negative = true;
            value = -value;
        }

        while (value > 0) {
            const int digit = value % 10;
            buffer[i] = '0' + digit;
            i += 1;
            value /= 10;
        }

        if (is_negative) {
            buffer[i] = '-';
            i += 1;
        }
        buffer[i] = '\0';

        for (int j = 0; j < i/2; j++) {
            const char tmp = buffer[j];
            buffer[j] = buffer[i-1-j];
            buffer[i-1-j] = tmp;
        }
    }
}

namespace term {
    const auto video = reinterpret_cast<volatile char *>(0xB8000);
    int cursor_x = 0;
    int cursor_y = 0;
    constexpr int VGA_WIDTH = 80;
    constexpr int VGA_HEIGHT = 25;
    constexpr int TAB_SIZE = 4;

    void scroll(const int amount) {
        for (int i = amount; i > 0; i--) {
            for (int j = 0; j < VGA_HEIGHT - 1; j++) {
                for (int k = 0; k < VGA_WIDTH * 2; k++) {
                    video[(j * VGA_WIDTH * 2) + k] = video[((j + 1) * VGA_WIDTH * 2) + k];
                }
            }
            for (int k = 0; k < VGA_WIDTH; k++) {
                const int index = (VGA_HEIGHT - 1) * VGA_WIDTH * 2 + k * 2;
                video[index] = ' ';
                video[index + 1] = 0x07;

            }
            cursor_y--;
        }
    }

    void print(const char* text) {
        for (int i = 0; text[i] != '\0'; i++) {
            const char c = text[i];
            if (cursor_y >= VGA_HEIGHT) {
                scroll(1);
            }

            if (c == '\t') {
                cursor_x += TAB_SIZE;
                if (cursor_x >= 80) {
                    cursor_x = 0;
                    cursor_y+=1;
                }
                if (cursor_y >= VGA_HEIGHT) {
                    scroll(1);
                }
                continue;
            }
            if (c == '\n') {
                cursor_x = 0;
                cursor_y+=1;
                if (cursor_y >= VGA_HEIGHT) {
                    scroll(1);
                }
                continue;
            }

            const int idx = (cursor_y*160)+cursor_x*2;
            video[idx] = c;
            video[idx+1] = 0x07;
            cursor_x+=1;

            if (cursor_x >= 80) {
                cursor_x = 0;
                cursor_y+=1;
                if (cursor_y >= VGA_HEIGHT) {
                    scroll(1);
                }
            }
        }
    }

    void print_int(const int value) {
        char buf[12];
        string::int_to_str(buf, value);
        print(buf);
    }

    void clear() {
        for (int y = 0; y < VGA_HEIGHT; y++) {
            for (int x = 0; x < VGA_WIDTH; x++) {
                const int index = (y * VGA_WIDTH + x) * 2;
                video[index] = ' ';
                video[index + 1] = 0x07;
            }
        }
        cursor_x = 0;
        cursor_y = 0;
    }
}
