static int read(int fd, char *buf, int count) {
    int ret;
    asm volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(3), "b"(fd), "c"(buf), "d"(count)
        : "memory"
    );
    return ret;
}

static int write(int fd, const char *buf, int count) {
    int ret;
    asm volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(4), "b"(fd), "c"(buf), "d"(count)
        : "memory"
    );
    return ret;
}

static void exit(int code) {
    asm volatile (
        "int $0x80"
        :
        : "a"(1), "b"(code)
        :
    );
}

static int strlen(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

static void hex(unsigned num, char *buf) {
    static const char hex_poss[] = "0123456789abcdef";
    int i = 0;

    if (num == 0) {
        buf[i++] = '0';
        buf[i++] = 'x';
        buf[i++] = '0';
        buf[i++] = '\n';
        buf[i] = '\0';
        return;
    }

    char temp[8];
    while (num > 0) {
        temp[i++] = hex_poss[num % 16];
        num /= 16;
    }

    int j = 2;
    while (--i >= 0) {
        buf[j++] = temp[i];
    }

    buf[0] = '0';
    buf[1] = 'x';
    buf[j++] = '\n';
    buf[j] = '\0';
}



int isnum(char ch) {
    return ch >= '0' && ch <= '9';
}

int isspc(char ch) {
    return ch == ' ' || ch == '\n';
}

static void print(unsigned num) {
    char buf[20];
    hex(num, buf);
    int ret = write(1, buf, strlen(buf));
    if (ret == -1) {
        exit(1);
    }
}

void _start() {
    char buf[20];
    unsigned num = 0;
    int i = 0;
    int num_digits = 0;
    unsigned chars_to_process = 0;

    for (; ; i++, chars_to_process--) {
        if (chars_to_process == 0) {
            int ret = read(0, buf, sizeof(buf));
            if (ret < 0) {
                exit(1);
            }
            i = 0;
            chars_to_process = ret;
        }
        if (num_digits > 0 && (chars_to_process == 0 || !isnum(buf[i]))) {
            print(num);
            num_digits = 0;
            num = 0;
        }
        if (chars_to_process == 0 || (!isspc(buf[i]) && !isnum(buf[i]))) {
            exit(0);
        }
        if (isnum(buf[i])) {
            num = num * 10 + buf[i] - '0';
            num_digits++;
        }
    }
}
