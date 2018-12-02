{
    vec4 test;
    
    gl_FragCoord = test; /* Assign Error */
    gl_Light_Half = test; /* Assign Error */
    test = gl_FragColor; /* Read Error */

    if (true)
        gl_FragColor = test;
    else 
        gl_FragDepth = false;
    gl_FragColor = test;
    gl_FragColor = 1;
}
