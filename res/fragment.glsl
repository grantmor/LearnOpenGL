#version 330 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 TexCoord;

//uniform vec4 uniColor;
uniform sampler2D container; 
uniform sampler2D smile; 

void main()
{
    //FragColor = vec4(1.0,0.0,0.0,1.0);
	//FragColor = vertexColor;
	//FragColor = uniColor;
	//FragColor = texture(rectTexture, TexCoord + vec2(0.0, 0.0));
	
	//FragColor = texture(rectTexture, TexCoord);

	//FragColor = mix(texture(container, TexCoord), texture(smile, TexCoord), 0.2);

	//Ex 1
	//FragColor = mix(texture(container, TexCoord), texture(smile, vec2(-TexCoord.s, TexCoord.t)), 0.2);

	// Ex 2
	//FragColor = mix(texture(container, TexCoord), texture(smile, TexCoord * 2.0), 0.2);
	
	// Ex 3
	//FragColor = mix(texture(container, TexCoord), texture(smile, ((TexCoord / 4.0)- vec2(-.25, -.25))), 0.2);

	FragColor = mix(texture(container, TexCoord), texture(smile, TexCoord), 0.5);
}
