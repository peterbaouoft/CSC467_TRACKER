{
    vec4 shade;
    vec4 coeff;
    vec4 fCol = gl_Color;
    shade = shade + coeff[1] * fCol;
    shade = shade + coeff[2] * fCol;
}
