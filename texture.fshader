#version 410

in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D Tex1;

struct LightInfo{
    vec4 Position;
    vec3 La; //Intensidad Luz ambiental
    vec3 Ld; //Intensidad Luz difusa
    vec3 Ls; //Intensidad Luz especular
};

uniform LightInfo Lights[2];

struct MaterialInfo{
    vec3 Ka; // Reflectividad Ambiental
    vec3 Kd; // Difusa
    vec3 Ks; // Especular
    float Shininess; // Factor de brillo
};

uniform MaterialInfo Material;

struct FogInfo {
    float maxDist;
    float minDist;
    float density;
    vec4 color;
};

uniform FogInfo Fog;

uniform int Fog_Type;

layout( location = 0 ) out vec4 FragColor;

vec3 ads(int lightIndex, vec4 position, vec3 norm,vec4 tex){

	LightInfo l = Lights[lightIndex];

	vec3 n = normalize( norm );
	vec3 s = normalize( vec3(l.Position - position) );
	vec3 v = normalize(vec3(-position));
	vec3 r = reflect( -s, n );
	return	 vec3(tex) *(l.La * Material.Ka +
				l.Ld * Material.Kd * max( dot(s, n), 0.0 )) +
				l.Ls * Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess );
}


void main(){

	vec4 texColor = texture( Tex1, TexCoord );

	vec3 shadeColor = vec3(0.0);
    for(int i = 0; i < 2; i++){
        shadeColor += ads(i,Position,Normal,texColor);
    }

    vec3 Color;
    if(Fog_Type != 0){
    	float dist = abs(Position.z);
	    float fogFactor;

	    switch(Fog_Type){
	    	case 1:
	    		fogFactor = (Fog.maxDist - dist) / (Fog.maxDist - Fog.minDist);
	    		break;
	    	case 2:
	    		fogFactor = exp(-Fog.density * dist);
	    	case 3:
	    		fogFactor = exp(- pow(Fog.density * dist,2.0));
	    }

	    fogFactor = clamp(fogFactor,0.0,1.0);

    	Color = mix(vec3(Fog.color), shadeColor, fogFactor);
    }
    else
    	Color = shadeColor;

	FragColor = vec4(Color, 1.0);

}
