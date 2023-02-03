#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <vector>
#include <string>

void msg(const char *msg);

void die(const char *msg);

const size_t k_max_msg = 4096;

#endif
