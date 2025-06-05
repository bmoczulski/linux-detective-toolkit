/*
 *  Example from "Linux Detective Toolkit: 360° debugging and black-box analysis without printf() or GDB" by Bartosz Moczulski
 *  Copyright © 2025
 *
 *  Provided for educational purposes only.
 *  Not for reuse, redistribution, or public presentation without permission.
 */

#pragma once

#define YELLOW  "\033[33m"      /* Yellow */
#define RED     "\033[31m"      /* Red */
#define RESET   "\033[0m"
#define devious_log(f, ...) fprintf(stderr, "\n" RED "]:-> " YELLOW f RESET "\n", ##__VA_ARGS__ )
