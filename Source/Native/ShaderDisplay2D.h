#pragma once

static const char* display2D_vs = R"(
	in vec3 AttrPosition;
	in vec4 AttrColor;
	in vec2 AttrUV;

	out vec4 Color;
	out vec2 UV;

	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * vec4(AttrPosition, 1.0);
		Color = AttrColor;
		UV = AttrUV;
	}
)";

static const char* display2D_ps_fsaa = R"(
	in vec4 Color;
	in vec2 UV;

	out vec4 FragColor;

	// Render settings
	// x = texel width
	// y = texel height
	// z = FSAA blend factor
	// w = transparency
	uniform vec4 rendersettings;
	uniform float desaturation;

	uniform sampler2D texture1;
	uniform vec4 texturefactor;

	// This blends the max of 2 pixels
	vec4 addcolor(vec4 c1, vec4 c2)
	{
		return vec4(
			max(c1.r, c2.r),
			max(c1.g, c2.g),
			max(c1.b, c2.b),
			clamp(c1.a + c2.a * 0.5, 0.0, 1.0));
	}

	vec3 desaturate(vec3 texel)
	{
		float gray = (texel.r * 0.3 + texel.g * 0.56 + texel.b * 0.14);	
		return mix(texel, vec3(gray), desaturation);
	}

	void main()
	{
		vec4 c = texture(texture1, UV);
	
		// If this pixel is not drawn on...
		if(c.a < 0.1)
		{
			// Mix the colors of nearby pixels
			vec4 n = vec4(0.0);
			n = addcolor(n, texture(texture1, vec2(UV.x + rendersettings.x, UV.y)));
			n = addcolor(n, texture(texture1, vec2(UV.x - rendersettings.x, UV.y)));
			n = addcolor(n, texture(texture1, vec2(UV.x, UV.y + rendersettings.y)));
			n = addcolor(n, texture(texture1, vec2(UV.x, UV.y - rendersettings.y)));
		
			FragColor = vec4(desaturate(n.rgb), n.a * rendersettings.z * rendersettings.w);
		}
		else
		{
			FragColor = vec4(desaturate(c.rgb), c.a * rendersettings.w) * Color;
		}

		FragColor *= texturefactor;

		#if defined(ALPHA_TEST)
		if (FragColor.a < 0.5) discard;
		#endif
	}
)";

const char* display2D_ps_normal = R"(
	in vec4 Color;
	in vec2 UV;

	out vec4 FragColor;

	// Render settings
	// x = texel width
	// y = texel height
	// z = FSAA blend factor
	// w = transparency
	uniform vec4 rendersettings;
	uniform float desaturation;

	uniform sampler2D texture1;
	uniform vec4 texturefactor;

	vec3 desaturate(vec3 texel)
	{
		float gray = (texel.r * 0.3 + texel.g * 0.56 + texel.b * 0.14);	
		return mix(texel, vec3(gray), desaturation);
	}

	void main()
	{
		vec4 c = texture(texture1, UV);
		FragColor = vec4(desaturate(c.rgb), c.a * rendersettings.w) * Color;
		FragColor *= texturefactor;

		#if defined(ALPHA_TEST)
		if (FragColor.a < 0.5) discard;
		#endif
	}
)";

const char* display2D_ps_fullbright = R"(
	in vec4 Color;
	in vec2 UV;

	out vec4 FragColor;

	// Render settings
	// x = texel width
	// y = texel height
	// z = FSAA blend factor
	// w = transparency
	uniform vec4 rendersettings;

	uniform sampler2D texture1;
	uniform vec4 texturefactor;

	void main()
	{
		vec4 c = texture(texture1, UV);
		FragColor = vec4(c.rgb, c.a * rendersettings.w);
		FragColor *= texturefactor;

		#if defined(ALPHA_TEST)
		if (FragColor.a < 0.5) discard;
		#endif
	}
)";
