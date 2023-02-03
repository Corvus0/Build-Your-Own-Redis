#include "utils.h"

int32_t write_all(int fd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t send_req(int fd, const std::vector<std::string> &cmd) {
    uint32_t len = 4;
    for (const std::string &s : cmd) {
        len += 4 + s.size();
    }
    if (len > k_max_msg) {
        return -1;
    }

    char wbuf[4 + k_max_msg];
    memcpy(&wbuf[0], &len, 4);  // assume little endian
    uint32_t n = cmd.size();
    memcpy(&wbuf[4], &n, 4);
    size_t cur = 8;
    for (const std::string &s : cmd) {
        uint32_t p = (uint32_t)s.size();
        memcpy(&wbuf[cur], &p, 4);
        memcpy(&wbuf[cur + 4], s.data(), s.size());
        cur += 4 + s.size();
    }
    return write_all(fd, wbuf, 4 + len);
}

const size_t buf_size = k_max_msg + 4;
static char rbuf[buf_size];
static size_t remaining = buf_size;

static inline void set_buf_offset(size_t offset = 0) {
    remaining = buf_size - offset;
}

static int32_t read_res(int fd) {
    // 4 bytes header
    int32_t rd = read(fd, &rbuf[buf_size - remaining], remaining);
    if (rd == 0) {
        msg("EOF");
        return -1;
    } else if (rd < 0) {
        msg("read() error");
        return rd;
    }

    // try to parse multiple requests
    // if early return, read will continue writing after data
    size_t offset = 0;
    size_t data_in_buf = buf_size - remaining + rd; 
    while (offset < data_in_buf) {
        uint32_t len = 0;
        if (offset + 4 > data_in_buf) {
            uint32_t diff = data_in_buf - offset;
            memcpy(rbuf, &rbuf[offset], diff);
            set_buf_offset(diff);
            return 0;
        }
        memcpy(&len, &rbuf[offset], 4);  // assume little endian
        if (len > k_max_msg) {
            msg("too long");
            set_buf_offset();
            return -1;
        } else if (offset + 4 + len > data_in_buf) {
            uint32_t diff = data_in_buf - offset;
            memcpy(rbuf, &rbuf[offset], diff);
            set_buf_offset(diff);
            return 0;
        }
        // print the result
        uint32_t rescode = 0;
        if (len < 4) {
            msg("bad response");
            return -1;
        }
        memcpy(&rescode, &rbuf[offset + 4], 4);
        printf("server says: [%u] %.*s\n", rescode, len - 4, &rbuf[offset + 8]);
        offset += (len + 4);
    }
    
    set_buf_offset();

    return 0;
}

int main(int argc, char **argv) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    if (rv) {
        die("connect");
    }

    std::vector<std::string> cmd;
    for (int i = 1; i < argc; ++i) {
        cmd.push_back(argv[i]);
    }
    int32_t err = send_req(fd, cmd);
    if (err) {
        goto L_DONE;
    }
    err = read_res(fd);
    if (err) {
        goto L_DONE;
    }

L_DONE:
    close(fd);
    return 0;
}