#pragma once

#include "poly.h"
#include "polyvec.h"

/*
    Calculates a * b using the RSA accelerator and stores result in c
*/
void poly_mult(poly *c, const poly *a, const poly *b);

/*
    Multiplies polynomials in b by the polynomial a and store in c
*/
void mult_polyvecl(polyvecl *c, const poly *a, const polyvecl *b);

/*
    Multiplies polynomials in b by the polynomial a and store in c
*/
void mult_polyveck(polyveck *c, const poly *a, const polyveck *b);