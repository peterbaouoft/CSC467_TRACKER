{
    vec4 eyeNorm;
    vec4 coeff;
    eyeNorm = gl_TexCoord;
    eyeNorm[3] = dp3(eyeNorm,eyeNorm);
    eyeNorm[3] = rsq(eyeNorm[3]);
    coeff = lit(coeff);
}