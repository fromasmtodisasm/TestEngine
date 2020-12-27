GLSLShader
{
	#version 460 core
}

GLSLShader vert
{

	out layout(location = 0) vec4 v_color;

	uniform layout(location = 0) mat4 projection;
	uniform layout(location = 1) mat4 view;
	uniform layout(location = 2) mat4 model;

	void main()
	{
		gl_Position = projection * view * vec4(aPos, 1.0);
		v_color = vec4(aColor);
	} 
}

GLSLShader frag
{
	out layout(location = 0) vec4 FragColor;
	in layout(location = 0) vec4 v_color;
	void main()
	{ 
		FragColor = vec4(v_color);
	}
}

// Default technique for auxiliary geometry rendering
technique AuxGeometry
{

  pass p0
  {
	InputLayout
	{
		vec3 aPos : POSITION
		vec4 aColor : COLOR
		vec2 aTC : TEXCOORD
	}
    VertexShader = vert
    PixelShader = frag
  }
  pass p1
  {
    VertexShader = vert
    PixelShader = frag
  }
}

technique Test
{
  pass p0
  {
    VertexShader = vert
    PixelShader = frag
  }
}