#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"
#include "Lighting.hlsl"
#include "Fog.hlsl"

uniform bool cGridEnable;
uniform bool cSelectionEnable;
uniform float4 cSelectionRect;
uniform float4 cRangeData0;
uniform float4 cRangeData1;
uniform float4 cRangeData2;
uniform float4 cRangeData3;
uniform float4 cRangeData4;

#ifndef D3D11

// D3D9 uniforms and samplers
#ifdef COMPILEVS
uniform float2 cDetailTiling;
#else
sampler2D sWeightMap0 : register(s0);
sampler2D sDetailMap1 : register(s1);
sampler2D sDetailMap2 : register(s2);
sampler2D sDetailMap3 : register(s3);

#endif

#else

// D3D11 constant buffers and samplers
#ifdef COMPILEVS
cbuffer CustomVS : register(b6)
{
	float2 cDetailTiling;
}
#else
Texture2D tWeightMap0 : register(t0);
Texture2D tDetailMap1 : register(t1);
Texture2D tDetailMap2 : register(t2);
Texture2D tDetailMap3 : register(t3);
SamplerState sWeightMap0 : register(s0);
SamplerState sDetailMap1 : register(s1);
SamplerState sDetailMap2 : register(s2);
SamplerState sDetailMap3 : register(s3);
#endif

#endif

float4 circleColor(float dist, float4 diffColor, float2 rangeData){
	dist = dist * rangeData.x;
	if (dist < rangeData.x) {
		float a = dist / rangeData.x;
		const int x =(int) rangeData.y;
		int b = (x>>16)%256;
		int g = (x>>8)%256;
		int r = x%256;
		
		diffColor = lerp(diffColor, float4(r/255.f, g/255.f, b/255.f, 1), pow(a*.9,5));
	}

	return diffColor;
}

void VS(float4 iPos : POSITION,
	float3 iNormal : NORMAL,
	float2 iTexCoord : TEXCOORD0,
	#ifdef SKINNED
		float4 iBlendWeights : BLENDWEIGHT,
		int4 iBlendIndices : BLENDINDICES,
	#endif
	#ifdef INSTANCED
		float4x3 iModelInstance : TEXCOORD4,
	#endif
	#if defined(BILLBOARD) || defined(DIRBILLBOARD)
		float2 iSize : TEXCOORD1,
	#endif
	#if defined(TRAILFACECAM) || defined(TRAILBONE)
		float4 iTangent : TANGENT,
	#endif
	out float2 oTexCoord : TEXCOORD0,
	out float3 oNormal : TEXCOORD1,
	out float4 oWorldPos : TEXCOORD2,
	out float2 oDetailTexCoord : TEXCOORD3,
	#ifdef PERPIXEL
		#ifdef SHADOW
			out float4 oShadowPos[NUMCASCADES] : TEXCOORD4,
		#endif
		#ifdef SPOTLIGHT
			out float4 oSpotPos : TEXCOORD5,
		#endif
		#ifdef POINTLIGHT
			out float3 oCubeMaskVec : TEXCOORD5,
		#endif
	#else
		out float3 oVertexLight : TEXCOORD4,
		out float4 oScreenPos : TEXCOORD5,
	#endif
	#if defined(D3D11) && defined(CLIPPLANE)
		out float oClip : SV_CLIPDISTANCE0,
	#endif
	out float4 oPos : OUTPOSITION)
{
	float4x3 modelMatrix = iModelMatrix;
	float3 worldPos = GetWorldPos(modelMatrix);
	oPos = GetClipPos(worldPos);
	oNormal = GetWorldNormal(modelMatrix);
	oWorldPos = float4(worldPos, GetDepth(oPos));
	oTexCoord = GetTexCoord(iTexCoord);
	oDetailTexCoord = cDetailTiling * oTexCoord;

	#if defined(D3D11) && defined(CLIPPLANE)
		oClip = dot(oPos, cClipPlane);
	#endif

	#ifdef PERPIXEL
		// Per-pixel forward lighting
		float4 projWorldPos = float4(worldPos.xyz, 1.0);

		#ifdef SHADOW
			// Shadow projection: transform from world space to shadow space
			GetShadowPos(projWorldPos, oNormal, oShadowPos);
		#endif

		#ifdef SPOTLIGHT
			// Spotlight projection: transform from world space to projector texture coordinates
			oSpotPos = mul(projWorldPos, cLightMatrices[0]);
		#endif

		#ifdef POINTLIGHT
			oCubeMaskVec = mul(worldPos - cLightPos.xyz, (float3x3)cLightMatrices[0]);
		#endif
	#else
		// Ambient & per-vertex lighting
		oVertexLight = GetAmbient(GetZonePos(worldPos));

		#ifdef NUMVERTEXLIGHTS
			for (int i = 0; i < NUMVERTEXLIGHTS; ++i)
				oVertexLight += GetVertexLight(i, worldPos, oNormal) * cVertexLights[i * 3].rgb;
		#endif
		
		oScreenPos = GetScreenPos(oPos);
	#endif
}

void PS(float2 iTexCoord : TEXCOORD0,
	float3 iNormal : TEXCOORD1,
	float4 iWorldPos : TEXCOORD2,
	float2 iDetailTexCoord : TEXCOORD3,
	float4 iPos : OUTPOSITION,
	#ifdef PERPIXEL
		#ifdef SHADOW
			float4 iShadowPos[NUMCASCADES] : TEXCOORD4,
		#endif
		#ifdef SPOTLIGHT
			float4 iSpotPos : TEXCOORD5,
		#endif
		#ifdef POINTLIGHT
			float3 iCubeMaskVec : TEXCOORD5,
		#endif
	#else
		float3 iVertexLight : TEXCOORD4,
		float4 iScreenPos : TEXCOORD5,
	#endif
	#if defined(D3D11) && defined(CLIPPLANE)
		float iClip : SV_CLIPDISTANCE0,
	#endif
	#ifdef PREPASS
		out float4 oDepth : OUTCOLOR1,
	#endif
	#ifdef DEFERRED
		out float4 oAlbedo : OUTCOLOR1,
		out float4 oNormal : OUTCOLOR2,
		out float4 oDepth : OUTCOLOR3,
	#endif
	out float4 oColor : OUTCOLOR0)
{

	float2 coord = iWorldPos.xz*0.5;

	// Compute anti-aliased world-space grid lines
	float2 grid = abs(frac(coord - 0.5) - 0.5) / fwidth(coord);
	float line1 = min(grid.x, grid.y);

	// Just visualize the grid lines directly
	float a = 1.0 - min(line1, 1.0);

	float4 weights = Sample2D(WeightMap0, iTexCoord).rgba;

	float4 diffColor;
	
	if (cGridEnable && a > 0.0 && (weights.a == 0.0 || weights.r>0.0 || weights.g>0.0|| weights.b>0.0)){
		if(weights.a != 0.0){
			diffColor = float4(0.5, 0.5, 0.5, 0.5);
		}else{
			diffColor = float4(1.0, 1.0, 1.0, 1.0);
		}
	} else {	
		float sumWeights = weights.r + weights.g + weights.b + weights.a;
		weights /= sumWeights;
	
		diffColor = cMatDiffColor * (
			weights.r * Sample2D(DetailMap1, iDetailTexCoord) +
			weights.g * Sample2D(DetailMap2, iDetailTexCoord) +
			weights.b * Sample2D(DetailMap3, iDetailTexCoord) +
			weights.a * float4(0,0,0,1)
		);
		
		
		if(cSelectionEnable 
			&& iWorldPos.x>cSelectionRect.x && iWorldPos.x<cSelectionRect.z 
			&& iWorldPos.z>cSelectionRect.y && iWorldPos.z<cSelectionRect.w){
				diffColor = lerp (diffColor, float4(0,1,0,1),0.5);
		} 
	}
	
	float ax[5];

	ax[0] = distance(cRangeData0.xy, iWorldPos.xz) / cRangeData0.z;
	ax[1] = distance(cRangeData1.xy, iWorldPos.xz) / cRangeData1.z;
	ax[2] = distance(cRangeData2.xy, iWorldPos.xz) / cRangeData2.z;
	ax[3] = distance(cRangeData3.xy, iWorldPos.xz) / cRangeData3.z;
	ax[4] = distance(cRangeData4.xy, iWorldPos.xz) / cRangeData4.z;
	
	float min = ax[0]; 
	for(int i=1;i<5;i++) { 
		if(min>ax[i]) {
			min=ax[i]; 
		}
	} 
	if(min==ax[0]){
		diffColor = circleColor(ax[0], diffColor, cRangeData0.zw);
	} else if(min==ax[1]){
		diffColor = circleColor(ax[1], diffColor, cRangeData1.zw);
	} else if(min==ax[2]){
		diffColor = circleColor(ax[2], diffColor, cRangeData2.zw);
	} else if(min==ax[3]){
		diffColor = circleColor(ax[3], diffColor, cRangeData3.zw);
	} else {
		diffColor = circleColor(ax[4], diffColor, cRangeData4.zw);
	}  
	
	//diffColor = circleColor(iWorldPos.xz, diffColor, cRangeData0);
	//diffColor = circleColor(iWorldPos.xz, diffColor, cRangeData1);
	//diffColor = circleColor(iWorldPos.xz, diffColor, cRangeData2);
	//diffColor = circleColor(iWorldPos.xz, diffColor, cRangeData3);
	//diffColor = circleColor(iWorldPos.xz, diffColor, cRangeData4);

	// Get material specular albedo
	float3 specColor = cMatSpecColor.rgb;

	// Get normal
	float3 normal = normalize(iNormal);

	// Get fog factor
	#ifdef HEIGHTFOG
		float fogFactor = GetHeightFogFactor(iWorldPos.w, iWorldPos.y);
	#else
		float fogFactor = GetFogFactor(iWorldPos.w);
	#endif

	#if defined(PERPIXEL)
		// Per-pixel forward lighting
		float3 lightDir;
		float3 lightColor;
		float3 finalColor;
		
		float diff = GetDiffuse(normal, iWorldPos.xyz, lightDir);

		#ifdef SHADOW
			diff *= GetShadow(iShadowPos, iWorldPos.w);
		#endif
	
		#if defined(SPOTLIGHT)
			lightColor = iSpotPos.w > 0.0 ? Sample2DProj(LightSpotMap, iSpotPos).rgb * cLightColor.rgb : 0.0;
		#elif defined(CUBEMASK)
			lightColor = SampleCube(LightCubeMap, iCubeMaskVec).rgb * cLightColor.rgb;
		#else
			lightColor = cLightColor.rgb;
		#endif
	
		#ifdef SPECULAR
			float spec = GetSpecular(normal, cCameraPosPS - iWorldPos.xyz, lightDir, cMatSpecColor.a);
			finalColor = diff * lightColor * (diffColor.rgb + spec * specColor * cLightColor.a);
		#else
			finalColor = diff * lightColor * diffColor.rgb;
		#endif

		#ifdef AMBIENT
			finalColor += cAmbientColor.rgb * diffColor.rgb;
			finalColor += cMatEmissiveColor;
			oColor = float4(GetFog(finalColor, fogFactor), diffColor.a);
		#else
			oColor = float4(GetLitFog(finalColor, fogFactor), diffColor.a);
		#endif
	#elif defined(PREPASS)
		// Fill light pre-pass G-Buffer
		float specPower = cMatSpecColor.a / 255.0;

		oColor = float4(normal * 0.5 + 0.5, specPower);
		oDepth = iWorldPos.w;
	#elif defined(DEFERRED)
		// Fill deferred G-buffer
		float specIntensity = specColor.g;
		float specPower = cMatSpecColor.a / 255.0;

		float3 finalColor = iVertexLight * diffColor.rgb;

		oColor = float4(GetFog(finalColor, fogFactor), 1.0);
		oAlbedo = fogFactor * float4(diffColor.rgb, specIntensity);
		oNormal = float4(normal * 0.5 + 0.5, specPower);
		oDepth = iWorldPos.w;
	#else
		// Ambient & per-vertex lighting
		float3 finalColor = iVertexLight * diffColor.rgb;

		#ifdef MATERIAL
			// Add light pre-pass accumulation result
			// Lights are accumulated at half intensity. Bring back to full intensity now
			float4 lightInput = 2.0 * Sample2DProj(LightBuffer, iScreenPos);
			float3 lightSpecColor = lightInput.a * (lightInput.rgb / GetIntensity(lightInput.rgb));

			finalColor += lightInput.rgb * diffColor.rgb + lightSpecColor * specColor;
		#endif

		oColor = float4(GetFog(finalColor, fogFactor), diffColor.a);
	#endif
}
