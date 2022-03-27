#version 430 core
out vec4 f_color;

in V_OUT
{
   vec2 uv;
} f_in;

uniform sampler2D screen;
uniform float offset;
uniform float param;
uniform float time;
uniform int isOffset;
uniform vec2 size;
uniform vec3 lightDir;
uniform int pixel_w = 10;
uniform int pixel_h = 10;
uniform vec2 resolution;

void main()
{
    if (f_in.uv.x < offset)
    {
        
        vec2 coord;
        if (isOffset == 1)
        {
            float dx = (param+0.5)*20 * (1. / size.x);
        	float dy = (param+0.5)*20 * (1. / size.y);
            if(dx == 0)
            {
            	coord = vec2(f_in.uv.x, f_in.uv.y);
            }
            else
            {
            	coord = vec2(dx * floor(f_in.uv.x / dx), dy * floor(f_in.uv.y/ dy));
            }
            f_color = texture(screen, coord);
        }
        else if (isOffset == 2)
        {
            f_color = texture(screen, f_in.uv + 0.01 * (param + 0.5) * vec2(sin(time + size.x * f_in.uv.x), cos(time + size.y * f_in.uv.y)));
        }
        else if(isOffset == 3)
        {		 
		    
		    coord = vec2(f_in.uv.x, f_in.uv.y);
		    f_color = texture(screen, coord);
		    f_color.r = abs((param + 0.5) - f_color.r);
		    f_color.g = abs((param + 0.5)- f_color.g);
		    f_color.b = abs((param + 0.5) - f_color.b);
		    //f_color.a += 0.2;
		}
		else if(isOffset == 4)
        {		 
		    
		    coord = vec2(f_in.uv.x, f_in.uv.y);
		    f_color = texture(screen, coord);
		    f_color.r = param + f_color.r;
		    f_color.g = param + f_color.g;
		    f_color.b = param + f_color.b;
		    //f_color.a += 0.2;
		}
		else if(isOffset == 5)
        {		 
		    
		    coord = vec2(f_in.uv.x, f_in.uv.y);
		    f_color = texture(screen, coord);
		    f_color.r = (param) + f_color.r;
		    f_color.g = (- param ) + f_color.g;
		    f_color.b = f_color.b;
		    //f_color.a += 0.2;
		}
		else if(isOffset == 6)
        {		 
		    
		    coord = vec2(f_in.uv.x, f_in.uv.y);
		    f_color = texture(screen, coord);
		    vec4 color = f_color;
		    if(color.r < 0.2)
		    {
		    	if(color.g < 0.2)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.1;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.1;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.1;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.1;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.1;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.4)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.3;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.3;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.3;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.3;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.3;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.6)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.5;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.5;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.5;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.5;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.5;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.8)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.7;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.7;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.7;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.7;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.7;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 1)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.9;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.9;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.9;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.9;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.1;
					    f_color.g = 0.9;
					    f_color.b = 0.9;
		    		}
		    	}
		    }
		    else if(color.r < 0.4)
		    {
		    	if(color.g < 0.2)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.1;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.1;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.1;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.1;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.1;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.4)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.3;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.3;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.3;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.3;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.3;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.6)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.5;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.5;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.5;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.5;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.5;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.8)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.7;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.7;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.7;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.7;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.7;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 1)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.9;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.9;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.9;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.9;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.3;
					    f_color.g = 0.9;
					    f_color.b = 0.9;
		    		}
		    	}
		    }
		    else if(color.r < 0.6)
		    {
		    	if(color.g < 0.2)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.1;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.1;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.1;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.1;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.1;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.4)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.3;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.3;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.3;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.3;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.3;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.6)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.5;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.5;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.5;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.5;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.5;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.8)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.7;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.7;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.7;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.7;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.7;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 1)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.9;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.9;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.9;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.9;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.5;
					    f_color.g = 0.9;
					    f_color.b = 0.9;
		    		}
		    	}
		    }
		    else if(color.r < 0.8)
		    {
		    	if(color.g < 0.2)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.1;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.1;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.1;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.1;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.1;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.4)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.3;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.3;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.3;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.3;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.3;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.6)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.5;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.5;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.5;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.5;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.5;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.8)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.7;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.7;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.7;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.7;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.7;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 1)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.9;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.9;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.9;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.9;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.7;
					    f_color.g = 0.9;
					    f_color.b = 0.9;
		    		}
		    	}
		    }
		    else if(color.r < 1)
		    {
		    	if(color.g < 0.2)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.1;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.1;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.1;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.1;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.1;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.4)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.3;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.3;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.3;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.3;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.3;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.6)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.5;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.5;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.5;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.5;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.5;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 0.8)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.7;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.7;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.7;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.7;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.7;
					    f_color.b = 0.9;
		    		}
		    	}
		    	else if(color.g < 1)
		    	{
		    		if(color.b < 0.2)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.9;
					    f_color.b = 0.1;
		    		}
		    		else if(color.b < 0.4)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.9;
					    f_color.b = 0.3;
		    		}
		    		else if(color.b < 0.6)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.9;
					    f_color.b = 0.5;
		    		}
		    		else if(color.b < 0.8)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.9;
					    f_color.b = 0.7;
		    		}
		    		else if(color.b < 1)
		    		{
		    			f_color.r = 0.9;
					    f_color.g = 0.9;
					    f_color.b = 0.9;
		    		}
		    	}
		    }
		}

		else if(isOffset == 7)
        {		 
		    
		    coord = vec2(f_in.uv.x, f_in.uv.y);

		    f_color = texture(screen, coord);
		    float c = (f_color.r * (param+0.5)+f_color.g *(param+0.5)+f_color.b *(param+0.5))/3;
		    f_color.r = c;
		    f_color.g = c;
		    f_color.b = c;
		}
		else if(isOffset == 8)
		{
			vec2 iResolution;
            iResolution.x = size.x;
            iResolution.y = size.y;
            float x = 1.0 / iResolution.x;
            float y = 1.0 / iResolution.y;
            vec4 horizEdge = vec4( 0.0 );
                horizEdge -= texture2D( screen, vec2( f_in.uv.x - x, f_in.uv.y - y ) ) * 1.0;
                horizEdge -= texture2D( screen, vec2( f_in.uv.x - x, f_in.uv.y     ) ) * 2.0;
                horizEdge -= texture2D( screen, vec2( f_in.uv.x - x, f_in.uv.y + y ) ) * 1.0;
                horizEdge += texture2D( screen, vec2( f_in.uv.x + x, f_in.uv.y - y ) ) * 1.0;
                horizEdge += texture2D( screen, vec2( f_in.uv.x + x, f_in.uv.y     ) ) * 2.0;
                horizEdge += texture2D( screen, vec2( f_in.uv.x + x, f_in.uv.y + y ) ) * 1.0;
           vec4 vertEdge = vec4( 0.0 );
                vertEdge -= texture2D( screen, vec2( f_in.uv.x - x, f_in.uv.y - y ) ) * 1.0;
                vertEdge -= texture2D( screen, vec2( f_in.uv.x    , f_in.uv.y - y ) ) * 2.0;
                vertEdge -= texture2D( screen, vec2( f_in.uv.x + x, f_in.uv.y - y ) ) * 1.0;
                vertEdge += texture2D( screen, vec2( f_in.uv.x - x, f_in.uv.y + y ) ) * 1.0;
                vertEdge += texture2D( screen, vec2( f_in.uv.x    , f_in.uv.y + y ) ) * 2.0;
            vertEdge += texture2D( screen, vec2( f_in.uv.x + x, f_in.uv.y + y ) ) * 1.0;
           vec3 edge = sqrt((horizEdge.rgb * horizEdge.rgb) + (vertEdge.rgb * vertEdge.rgb));
           f_color = vec4(edge, texture2D(screen, f_in.uv.xy).a);
		}
    }
    else
    {
        f_color = texture(screen, f_in.uv);
    }
}