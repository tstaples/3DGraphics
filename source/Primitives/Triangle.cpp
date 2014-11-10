#include "stdafx.h"
#include "Triangle.h"
#include "Graphics/Rasterizer.h"
#include "Graphics/StateManager.h"
#include "Primitives.h"
#include "Containers/Matrix33.h"
#include "Containers/Vector3.h"
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
	if (mVertIndex != kVerts)
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
	AIASSERT(mVertIndex < kVerts, "All verticies already defined");
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
	return kVerts;
}
// ------------------------------------------------------------------------------------------

void CTriangle::GetVert(int index, CVertex2& out)
{
	AIASSERT(index >= 0 && index < kVerts, "Index out of range");
	if (index == 0)
		out = mV1;
	if (index == 1)
		out = mV2;
	if (index == 2)
		out = mV3;
}
// ------------------------------------------------------------------------------------------

void CTriangle::SetVert(int index, const CVertex2& v)
{
	AIASSERT(index >= 0 && index < kVerts, "Index out of range");
	if (index == 0)
		mV1 = v;
	if (index == 1)
		mV2 = v;
	if (index == 2)
		mV3 = v;
}
// ------------------------------------------------------------------------------------------

CVector2 CTriangle::GetPivot()
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

void CTriangle::Draw()
{
	FillMode::Mode fMode = StateManager::Instance()->GetFillMode();
	switch (fMode)
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
	// Cache the points
	CLine l1(mV1, mV2);
	CLine l2(mV2, mV3);
	CLine l3(mV1, mV3);

	l1.Draw();
	l2.Draw();
	l3.Draw();
}
// ------------------------------------------------------------------------------------------

void CTriangle::DrawPoints()
{
	DrawVertex(mV1);
	DrawVertex(mV2);
	DrawVertex(mV3);
}
// ------------------------------------------------------------------------------------------

void CTriangle::Fill()
{
	// Sort the verts from top to bottom
	CVertex2 v1, v2, v3;
	SortVertsY(v1, v2, v3);

	Point p1(v1.point.x + 0.5f, v1.point.y + 0.5f);
	Point p2(v2.point.x + 0.5f, v2.point.y + 0.5f);
	Point p3(v3.point.x + 0.5f, v3.point.y + 0.5f);

	// Initialize assuming there will be a breakpoint.
	// Determine if p2 is on the left or right of p1
	CLine left = (p2.x < p1.x) ? CLine(v1, v2) : CLine(v1, v3);
	CLine right = (p2.x > p1.x) ? CLine(v1, v2) : CLine(v1, v3);

	// Determine if there is a breakpoint
	bool breakpoint = true;
	if (p2.y == p1.y)
	{
		// Top is flat. Only need to re-assign for this case
		left = CLine(v1, v3);
		right = CLine(v2, v3);
		breakpoint = false;
	}
	else if (p2.y == p3.y)
	{
		// Bottom is flat
		breakpoint = false;
	}

	// Check if either lines are straight vertical
	bool lvert = left.IsVertical();
	bool rvert = right.IsVertical();

	// x1 and 2's values will be changed if the corresponding line isn't vertical.
	// If it is, then x would stay the same.
	int x1 = static_cast<int>(left.MinX());
	int x2 = static_cast<int>(right.MinX());

	int y = p1.y;
	while (y != p3.y)
	{
		// Check if we are going to run into a breakpoint at some time
		if (breakpoint)
		{
			// Check if we have reached the breakpoint
			if (y == p2.y)
			{
				// Re-assign the corresponding line and
				// re-evaluate if it is straight vertical.
				if (p2.x < p1.x)
				{
					left = CLine(v2, v3);
					lvert = CompareFloat(left.MinX(), left.MaxX());
				}
				else
				{
					right = CLine(v2, v3);
					rvert = CompareFloat(right.MinX(), right.MaxX());
				}
				// Set to false since we have dealt with it now
				breakpoint = false;
			}
		}

		// Get the furthest point along the run for this y if the line isn't vertical
		if (!lvert)
			x1 = left.GetMaxLeftX(y);
		if (!rvert)
			x2 = right.GetMaxRightX(y);

		// Create verts from the left and right points
		CVertex2 v1(x1, y, left.GetColorAtY(y)), 
				 v2(x2, y, right.GetColorAtY(y)); 

		// Draw a line between them
		DrawLine(v1, v2);

		y++;
	}
}
// ------------------------------------------------------------------------------------------

void CTriangle::SortVertsY(CVertex2& p1, CVertex2& p2, CVertex2& p3)
{
	AIASSERT(mVertIndex == kVerts, "Not all verticies defined");

	// Sets p1 to the vert with the lowest y value (highest on screen)
	// p2 = middle y value
	// p3 = highest y value (lowest on screen)

	if (mV1.point.y <= mV2.point.y &&
		mV1.point.y <= mV3.point.y)
	{
		p1 = mV1;
		p2 = (mV2.point.y <= mV3.point.y) ? mV2 : mV3;
		p3 = (p2 == mV2) ? mV3 : mV2;
	}
	else if (mV2.point.y <= mV1.point.y &&
			mV2.point.y <= mV3.point.y)
	{
		p1 = mV2;
		p2 = (mV1.point.y <= mV3.point.y) ? mV1 : mV3;
		p3 = (p2 == mV1) ? mV3 : mV1;
	}
	else
	{
		p1 = mV3;
		p2 = (mV1.point.y <= mV2.point.y) ? mV1 : mV2;
		p3 = (p2 == mV1) ? mV2 : mV1;
	}
}
// ------------------------------------------------------------------------------------------

void CTriangle::SortVertsX(CVertex2& p1, CVertex2& p2, CVertex2& p3)
{
	if (mV1.point.x <= mV2.point.x &&
		mV1.point.x <= mV3.point.x)
	{
		p1 = mV1;
		p2 = (mV2.point.x <= mV3.point.x) ? mV2 : mV3;
		p3 = (p2 == mV2) ? mV3 : mV2;
	}
	else if (mV2.point.x <= mV1.point.x &&
			mV2.point.x <= mV3.point.x)
	{
		p1 = mV2;
		p2 = (mV1.point.x <= mV3.point.x) ? mV1 : mV3;
		p3 = (p2 == mV1) ? mV3 : mV1;
	}
	else
	{
		p1 = mV3;
		p2 = (mV1.point.x <= mV2.point.x) ? mV1 : mV2;
		p3 = (p2 == mV1) ? mV2 : mV1;
	}
}
// ------------------------------------------------------------------------------------------

void CTriangle::SortVertsY(int* p1, int* p2, int* p3)
{
	int vals[3] = { mV1.point.y, mV2.point.y, mV3.point.y };
	SortArray(vals, 3);

	*p1 = vals[0];
	*p2 = vals[1];
	*p3 = vals[2];
}
// ------------------------------------------------------------------------------------------

void CTriangle::SortVertsX(int* p1, int* p2, int* p3)
{
	int vals[3] = { mV1.point.x, mV2.point.x, mV3.point.x };
	SortArray(vals, 3);

	*p1 = vals[0];
	*p2 = vals[1];
	*p3 = vals[2];
}