{
    float a;
    vec4 b;

    a = rsq(4.32);
    a = rsq(4);
    a = dp3(vec4(1.2,3.4,4.5,2.1), vec4(1.4,3.3, 1.2, 1.2));
    a = dp3(vec3(1.2,3.1,2.1), vec3(1.2,3.1,4.2));
    a = dp3(ivec4(1,2,3,4), ivec4(1,2,3,4));
    a = dp3(ivec3(1,2,3), ivec3(1,2,3));
    b = lit(vec4(1.2,2.3,3.4,4.5));
}
