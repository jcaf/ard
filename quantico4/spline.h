#ifndef SPLINE_H_
#define SPLINE_H_

    #define SPLINE_NODES_MIN 3
    #define SPLINE_NODES_MAX 20

    //float get_spline(float xi, float *X, float *A, uint8_t A_Length);
    void spline_poly_get_coeffs(void);
    float spline_poly_eval(float xi);

#endif // SPLINE_H_
