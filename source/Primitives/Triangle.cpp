#include "stdafx.h"
#include "Triangle.h"
#include "Line.h"
#include "Graphics/Rasterizer.h"
#include "Graphics/StateManager.h"
#include "Graphics/Viewport.h"
#include "Graphics/PrimManager.h"
#include "Containers/Matrix33.h"
#include "Containers/Vector3.h"
#include "Containers/Vertex3.h"
#include "Containers/Point.h"
#include "Utility/Transforms.h"
#include "Utility/MiniMath.h"

CTriangle::CTriangle() 
	:	CPrimitive(PrimType::Triangle)
	,	mVertIndex(0)
{
}
// ------------------------------------------------------------------------------------------

CTriangle::CTriangle(const CTriangle& rhs)
	:	CPrimitive(rhs.mType)
{
	*this = rhs;
}
// ------------------------------------------------------------------------------------------

CTriangle& CTriangle::operator=(const CTriangle& rhs)
{
	if (this != &rhs)
	{
		// Copy verticies over
		mV1 = rhs.mV1;
		mV2 = rhs.mV2;
		mV3 = rhs.mV3;
	}
	return *this;
}
// ------------------------------------------------------------------------------------------

bool CTriangle::IsValid() const
{
	// Check that we have the correct number of verticies
	if (mVertIndex != MaxVerts)
	{
		return false;
	}
	else
	{
		// Ensure no more than 2 verticies are parallel
		if (CompareFloat(mV1.point.x, mV2.point.x) &&
			CompareFloat(mV1.point.x, mV3.point.x) ||
			CompareFloat(mV1.point.y, mV2.point.y) &&
			CompareFloat(mV1.point.y, mV3.point.y))
		{
			return false;
		}
	}
	return (CPrimitive::IsValid());
}
// ------------------------------------------------------------------------------------------

void CTriangle::AddVertex(const CVertex2& vert)
{
	AIASSERT(mVertIndex < MaxVerts, "All verticies already defined");
	switch (mVertIndex++)
	{
	case 0:
	mV1 = vert;
		break;
	case 1:
		mV2 = vert;
		break;
	case 2:
		mV3 = vert;
		break;
	}
}
// ------------------------------------------------------------------------------------------

const int CTriangle::VertexCount() const
{
	return mVertIndex;
}
// ------------------------------------------------------------------------------------------

const int CTriangle::MaxVerticies() const
{
	return MaxVerts;
}
// ------------------------------------------------------------------------------------------

CVector2 CTriangle::GetPivot() const
{
	// todo: call get center from bounding box class
	CVector2 pmax(max(max(mV1.point.x, mV2.point.x), mV3.point.x),
				  max(max(mV1.point.y, mV2.point.y), mV3.point.y));
	CVector2 pmin(min(min(mV1.point.x, mV2.point.x), mV3.point.x),
				  min(min(mV1.point.y, mV2.point.y), mV3.point.y));

	float midx = (abs(pmax.x) - abs(pmin.x)) * 0.5f;
	float midy = (abs(pmax.y) - abs(pmin.y)) * 0.5f;

	return CVector2(pmin.x + midx, pmin.y + midy);
}
// ------------------------------------------------------------------------------------------

float CTriangle::GetZDepth()
{
	return ((mV1.z + mV2.z + mV3.z) / 3.0f);
}
// ------------------------------------------------------------------------------------------

void CTriangle::Transform(const CMatrix33& tm)
{
	CVector2 pivot = GetPivot();
	CMatrix33 t = CreateTransformAroundCenter(pivot, tm);

	// Apply the transformation matrix to our verticies
	mV1.point = Transform3HC(mV1.point, t);
	mV2.point = Transform3HC(mV2.point, t);
	mV3.point = Transform3HC(mV3.point, t);
}
// ------------------------------------------------------------------------------------------

CVector3 CTriangle::ComputeNormal() const
{
	CVector3 v1(mV1.point.x, mV1.point.y, mV1.z);
	CVector3 v2(mV2.point.x, mV2.point.y, mV2.z);
	CVector3 v3(mV3.point.x, mV3.point.y, mV3.z);

	return Cross(Normalize(v2 - v1), Normalize(v3 - v2));
}
// ------------------------------------------------------------------------------------------

void CTriangle::SetVertexNormals(const CVector3& normal)
{
	mV1.normal = normal;
	mV2.normal = normal;
	mV3.normal = normal;
}
// ------------------------------------------------------------------------------------------

void CTriangle::SetVertexColors(const CColor& color)
{
	mV1.color = color;
	mV2.color = color;
	mV3.color = color;
}
// ------------------------------------------------------------------------------------------

void CTriangle::Draw(FillMode::Mode mode)
{
	switch (mode)
	{
	case FillMode::Point:
		DrawPoints();
		break;
	case FillMode::Line:
		DrawSolid();
		break;
	case FillMode::Fill:
		Fill();
		break;
	}
}
// ------------------------------------------------------------------------------------------

void CTriangle::DrawSolid()
{
	DrawLine(mV1, mV2);
	DrawLine(mV2, mV3);
	DrawLine(mV1, mV3);
}
// ------------------------------------------------------------------------------------------

void CTriangle::DrawPoints()
{
	DrawVertex(mV1);
	DrawVertex(mV2);
	DrawVertex(mV3);
}
// ------------------------------------------------------------------------------------------

void CTriangle::DrawSection(const CLine& left, const CLine& right)
{
	bool zEnabled = Viewport::Instance()->ZbufferEnabled();

	// Precompute inverse slope of the lines
	const float leftM = left.CalcInvSlope();
	const float rightM = right.CalcInvSlope();

	const int y1 = RoundPixel(left.mFrom.point.y);
	const int y2 = RoundPixel(left.mTo.point.y);

	float leftX = left.mFrom.point.x;
	float rightX = right.mFrom.point.x;

	float divisor = CalcTimeDivisor(left.mFrom.point.y, left.mTo.point.y);
	for (int y=y1; y < y2; ++y)
	{
		float t = (y - y1) * divisor;
		float leftZ = Lerp(left.mFrom.z, left.mTo.z, t);
		float rightZ = Lerp(right.mFrom.z, right.mTo.z, t);

		CColor lc = left.GetColorAtY(y);
		CColor rc = right.GetColorAtY(y);

		// Draw the horizontal span between the two points
		if (zEnabled)
		{
			DrawHorizontalLine_Z(leftX, rightX, (float)y, leftZ, rightZ, lc, rc);
		}
		else
		{
			// No need for depth checks
			DrawHorizontalLine(leftX, rightX, (float)y, lc, rc);
		}

		// Increment by the inverse of the slope
		leftX += leftM;
		rightX += rightM;
	}
}
// ------------------------------------------------------------------------------------------

void CTriangle::DrawSection_Phong(const CLine& left, const CLine& right)
{
	bool zEnabled = Viewport::Instance()->ZbufferEnabled();

	// Precompute inverse slope of the lines
	const float leftM = left.CalcInvSlope();
	const float rightM = right.CalcInvSlope();
	const int y1 = RoundPixel(left.mFrom.point.y);
	const int y2 = RoundPixel(left.mTo.point.y);
	float leftX = left.mFrom.point.x;
	float rightX = right.mFrom.point.x;

	CVertex2 lp, rp; 
	float divisor = CalcTimeDivisor(left.mFrom.point.y, left.mTo.point.y);
	for (int y=y1; y < y2; ++y)
	{
		float t = (y - y1) * divisor;
		lp = Vertex2Lerp(left.mFrom, left.mTo, t);
		rp = Vertex2Lerp(right.mFrom, right.mTo, t);

		lp.material = left.mFrom.material;
		rp.material = right.mFrom.material;

		lp.z = Lerp(left.mFrom.z, left.mTo.z, t);
		rp.z = Lerp(right.mFrom.z, right.mTo.z, t);

		//lp.color = left.GetColorAtY(y);
		//rp.color = right.GetColorAtY(y);

		lp.normal = LerpVector3(left.mFrom.normal, left.mTo.normal, t);
		rp.normal = LerpVector3(right.mFrom.normal, right.mTo.normal, t);

		// Lerp the world coordinates -> used for lighting calculations
		lp.worldPoint = LerpVector3(left.mFrom.worldPoint, left.mTo.worldPoint, t);
		rp.worldPoint = LerpVector3(right.mFrom.worldPoint, right.mTo.worldPoint, t);

		DrawHLine_Z_Phong(lp, rp);

		// Increment by the inverse of the slope
		leftX += leftM;
		rightX += rightM;
	}
}
// ------------------------------------------------------------------------------------------

void CTriangle::Fill()
{
	// Sort the verts from top to bottom
	CVertex2 v1(mV1), v2(mV2), v3(mV3);
	v1.Ceil(); v2.Ceil(); v3.Ceil();	// Prevent edge fighting
	SortVertsY(v1, v2, v3);
	bool phong = (StateManager::Instance()->GetShadingMode() == ShadingMode::Phong) ? true : false;

	if (CompareFloat(v1.point.y, v2.point.y))
	{
		// Flat top
		CLine left = (IsLeft(v1, v2)) ? CLine(v1, v3) : CLine(v2, v3);
		CLine right = (!IsLeft(v1, v2)) ? CLine(v1, v3) : CLine(v2, v3);
		if (!phong)
			DrawSection(left, right);
		else
			DrawSection_Phong(left, right);
	}
	else if (CompareFloat(v2.point.y, v3.point.y))
	{
		// Flat bottom
		CLine left = (IsLeft(v2, v3)) ? CLine(v1, v2) : CLine(v1, v3);
		CLine right = (!IsLeft(v2, v3)) ? CLine(v1, v2) : CLine(v1, v3);
		if (!phong)
			DrawSection(left, right);
		else
			DrawSection_Phong(left, right);
	}
	else
	{
		// Split
		CVertex2 v4 = GetSplitPoint(v1, v2, v3);

		if (IsLeft(v2, v4))
		{
			// v2 is on the left
			if (!phong)
			{
				DrawSection(CLine(v1, v2), CLine(v1, v4));	// Top half
				DrawSection(CLine(v2, v3), CLine(v4, v3));	// Bottom half
			}
			else
			{
				DrawSection_Phong(CLine(v1, v2), CLine(v1, v4));	// Top half
				DrawSection_Phong(CLine(v2, v3), CLine(v4, v3));	// Bottom half
			}
		}
		else
		{
			// v2 is on the right
			if (!phong)
			{
				DrawSection(CLine(v1, v4), CLine(v1, v2));
				DrawSection(CLine(v4, v3), CLine(v2, v3));
			}
			else
			{
				DrawSection_Phong(CLine(v1, v4), CLine(v1, v2));
				DrawSection_Phong(CLine(v4, v3), CLine(v2, v3));
			}
		}
	}
}
// ------------------------------------------------------------------------------------------

CVertex2 CTriangle::GetSplitPoint(const CVertex2& v1, const CVertex2& v2, const CVertex2& v3)
{
	CVertex2 v4;

	// Interpolate along V1V3 to find where v2.x intersects
	float t = (v2.point.y - v1.point.y) / (v3.point.y - v1.point.y);
	v4.point.x = Lerp(v1.point.x, v3.point.x, t);
	v4.z = Lerp(v1.z, v3.z, t);
	v4.point.y = v2.point.y;
	v4.worldPoint = LerpVector3(v1.worldPoint, v3.worldPoint, t);
	v4.normal = LerpVector3(v1.normal, v3.normal, t);
	v4.material = v1.material;

	// Create a temp line to find the color at that intersection
	CLine l(v1, v3);
	v4.color = l.GetColorAtY((int)v2.point.y);

	return v4;
}
// ------------------------------------------------------------------------------------------

void CTriangle::SortVertsY(CVertex2& p1, CVertex2& p2, CVertex2& p3)
{
	AIASSERT(mVertIndex == MaxVerts, "Not all verticies defined");

	if (p1.point.y > p2.point.y) std::swap(p1, p2);
	if (p1.point.y > p3.point.y) std::swap(p1, p3);
	if (p2.point.y > p3.point.y) std::swap(p2, p3);
}
// ------------------------------------------------------------------------------------------

bool IsLeft(const CVertex2& v1, const CVertex2& v2)
{
	return ((v2.point.x - v1.point.x) > 0.0f);
}