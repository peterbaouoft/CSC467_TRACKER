{

    int a = 5 - 3;
    /* int b = 5.0 - 3; *//* Base type mismatch */
    vec3 c;
    float h;
    int d = c - a; /* Minus dimension mismatch */
    /* int a = 3 * c;
    int a = c / 5;
    int a = a / a;*/

    int a = true && false; /* Logical comparison */
    int b = h > c; /* Line for >, <=, >=, < */

    bool last = h == c; /* Line for == comparisons */
}
