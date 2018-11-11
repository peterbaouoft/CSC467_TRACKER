{
	vec4 b;
	const vec4 a = gl_Light_Half + b; /* Error: we only support singular uniform or literals */
	const int ab = 5 + 3; /* Error we only support "evaluated" expressions */
	const vec4 c = gl_FragCoord; /* Error: assigning non-uniform type */

	const int ad = 5;
	const vec4 d = gl_Light_Half; 
}
