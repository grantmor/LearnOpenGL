#version 330 core
in vec4 vertexColor;
out vec4 FragColor;

uniform vec4 uniColor;

void main()
{
    //FragColor = vec4(1.0,0.0,0.0,1.0);
	FragColor = vertexColor;
	//FragColor = uniColor;
}
