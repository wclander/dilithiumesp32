#pragma once

#include "poly.h"

/*
    Calculates a * b using the RSA accelerator and stores result in c
*/
void poly_mult(poly *c, const poly *a, const poly *b);