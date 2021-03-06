#include "stdafx.h"
#include "SpotLight.h"

CSpotLight::CSpotLight(const CVector3& pos, const CVector3& direction, float angle, float decay)
	:	CLight(LightType::Spotlight)
	,	mAttenConstant(0.0f)
	,	mAttenLinear(0.0f)
	,	mAttenQuadratic(0.0f)
	,	mDirection(direction)
	,	mAngle(angle)
	,	mDecay(decay)
{
	mPosition = pos;
}
// ------------------------------------------------------------------------------------------

float CSpotLight::CalculateIntensity(const CVector3& sp)
{
	float dist = Distance(mPosition, sp);

	// iL = 1 / (Kc + Kl * dist + Kq * dist^2)
	return (1.0f / (mAttenConstant + (mAttenLinear * dist) + (mAttenQuadratic * (dist * dist))));
}
// ------------------------------------------------------------------------------------------

void CSpotLight::GetSurfaceColor(const CVertex3& sp, const CVector3& viewerPos, CMaterial& outmat)
{
	CColor color; // init to 0; result if point not inside cone
	const CVector3& pos = sp.Get3DPoint();
	CVector3 pointToLight = Normalize(mPosition - pos); // L

	// Check if point is inside the cone
	float spot = Dot(pointToLight.Inverse(), mDirection);
	if (spot >= cosf(mAngle))
	{
		// point is inside the cone; add decay factor
		spot = powf(spot, mDecay);
		float intensity = spot / CalculateIntensity(pos);

		outmat.ambient += ComputeAmbient(intensity, sp.material.ambient);
		outmat.diffuse += ComputeDiffuse(intensity, pointToLight, sp);
		outmat.specular += ComputeSpecular(intensity, pointToLight,  viewerPos, sp);
	}
}