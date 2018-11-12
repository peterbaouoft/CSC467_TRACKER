{

    int a = 5 - 3;
    int b = 5.0 - 3; /* Base type mismatch */
    vec3 c;
    float h;
    int d = c - a; /* Minus dimension mismatch */
    int l = 3 * c; /* Base mismatch for times */
    /* int a = c / 5;
    int a = a / a;*/

    int n = true && false && a; /* Logical comparison */
    int m = h > c; /* Line for >, <=, >=, < */

    bool last = h == c; /* Line for == comparisons */
}
