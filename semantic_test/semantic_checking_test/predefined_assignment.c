{
    vec4 test;
    
    gl_FragCoord = test; /* Assign Error */
    gl_Light_Half = test; /* Assign Error */
    test = gl_FragColor; /* Read Error */

}