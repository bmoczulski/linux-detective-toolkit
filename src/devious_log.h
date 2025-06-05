#pragma once

#define YELLOW  "\033[33m"      /* Yellow */
#define RED     "\033[31m"      /* Red */
#define RESET   "\033[0m"
#define devious_log(f, ...) fprintf(stderr, "\n" RED "]:-> " YELLOW f RESET "\n", ##__VA_ARGS__ )
