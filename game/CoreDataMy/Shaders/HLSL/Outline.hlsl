#include "Uniforms.hlsl"
#include "Samplers.hlsl"
#include "Transform.hlsl"
#include "ScreenPos.hlsl"

#ifdef COMPILEPS
	uniform float4 cOutlineColor;
	uniform float2 coutlineMaskInvSize;
	uniform float2 coutlineBlurredMaskHInvSize;
	uniform bool cOutlineEnable;	
#endif

void VS(float4 iPos : POSITION,
	out float2 oTexCoord : TEXCOORD0,
	out float2 oScreenPos : TEXCOORD1,
	out float4 oPos : OUTPOSITION)
{
	float4x3 modelMatrix = iModelMatrix;
	float3 worldPos = GetWorldPos(modelMatrix);
	oPos = GetClipPos(worldPos);
	oTexCoord = GetQuadTexCoord(oPos);
	oScreenPos = GetScreenPosPreDiv(oPos);
}

void PS(
	float2 iTexCoord : TEXCOORD0,
	float2 iScreenPos : TEXCOORD1,
	out float4 oColor : OUTCOLOR0)
{
	#ifdef MASK
		if (!cOutlineEnable)
			discard;
		oColor = float4(cOutlineColor.rgb, 1);
	#endif

	#ifdef BLURH
		float4 rgba = Sample2D(DiffMap, iTexCoord + float2(0.0, 0.0) * coutlineMaskInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(-1.0, 0.0) * coutlineMaskInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(1.0, 0.0) * coutlineMaskInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(-2.0, 0.0) * coutlineMaskInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(2.0, 0.0) * coutlineMaskInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(-3.0, 0.0) * coutlineMaskInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(3.0, 0.0) * coutlineMaskInvSize) 
				+ Sample2D(DiffMap, iTexCoord + float2(-4.0, 0.0) * coutlineMaskInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(4.0, 0.0) * coutlineMaskInvSize) 
				+ Sample2D(DiffMap, iTexCoord + float2(-5.0, 0.0) * coutlineMaskInvSize) 
				+ Sample2D(DiffMap, iTexCoord + float2(5.0, 0.0) * coutlineMaskInvSize);
		oColor = rgba / 11.0;
	#endif

	#ifdef BLURV
		float4 rgba = Sample2D(DiffMap, iTexCoord + float2(0.0, 0.0) * coutlineBlurredMaskHInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(0.0, -1.0) * coutlineBlurredMaskHInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(0.0, 1.0) * coutlineBlurredMaskHInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(0.0, -2.0) * coutlineBlurredMaskHInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(0.0, 2.0) * coutlineBlurredMaskHInvSize)
				+ Sample2D(DiffMap, iTexCoord + float2(0.0, -3.0) * coutlineBlurredMaskHInvSize) 
				+ Sample2D(DiffMap, iTexCoord + float2(0.0, 3.0) * coutlineBlurredMaskHInvSize) 
				+ Sample2D(DiffMap, iTexCoord + float2(0.0, -4.0) * coutlineBlurredMaskHInvSize) 
				+ Sample2D(DiffMap, iTexCoord + float2(0.0, 4.0) * coutlineBlurredMaskHInvSize) 
				+ Sample2D(DiffMap, iTexCoord + float2(0.0, -5.0) * coutlineBlurredMaskHInvSize) 
				+ Sample2D(DiffMap, iTexCoord + float2(0.0, 5.0) * coutlineBlurredMaskHInvSize);
		oColor = rgba / 11.0;
	#endif


	#ifdef OUTPUT
		float4 blurredMask = Sample2D(DiffMap, iScreenPos);
		float4 mask = Sample2D(NormalMap, iScreenPos);
		float4 viewport = Sample2D(SpecMap, iScreenPos);
		blurredMask = clamp(blurredMask - mask.a, 0.0, 1.0);
		blurredMask *= 3.0;
		oColor = viewport * (1.0 - blurredMask.a) + blurredMask;
		
		//float4 blurredMask = Sample2D(DiffMap, iTexCoord);
		//float4 mask = Sample2D(NormalMap, iTexCoord);
		//float4 viewport = Sample2D(SpecMap, iTexCoord);
		//blurredMask.a = clamp(blurredMask.a - mask.a, 0.0, 1.0);
		//blurredMask = clamp(blurredMask * 200.0, 0.0, 1.0);
		//oColor = float4(viewport.rgb * (1.0 - blurredMask.a) + blurredMask.rgb * blurredMask.a, viewport.a);
	#endif


}