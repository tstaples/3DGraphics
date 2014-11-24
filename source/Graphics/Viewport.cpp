#include "Stdafx.h"
#include "Viewport.h"
#include "Containers/Rect2.h"
#include "Containers/Vector3.h"
#include "Containers/Vector4.h"
#include "Camera.h"
#include "Primitives/Triangle.h"

// static singleton member initialization
Viewport* Viewport::spInstance = nullptr;

// ------------------------------------------------------------------------------------------

// Singleton accessor
Viewport* Viewport::Instance()
{
	if (!spInstance)
	{
		spInstance = new Viewport();
	}
	return spInstance;
}
// ------------------------------------------------------------------------------------------

void Viewport::DestroyInstance()
{
	if (spInstance)
	{
		delete spInstance;
		spInstance = nullptr;
	}
}
// ------------------------------------------------------------------------------------------

// Default constructor
Viewport::Viewport()
	:	mOrigin(0.0f, 0.0f)
	,	mWidth(0)
	,	mHeight(0)
	,	mAspectRatio(0.0f)
	,	mDraw(false)
	,	mBackfaceCull(false)
	,	mZBufferOn(false)
	,	mZBufferIsset(false)
{
}
// ------------------------------------------------------------------------------------------

Viewport::~Viewport()
{
	if (mZBufferOn)
	{
		mZBuffer.Clear();
	}
}
// ------------------------------------------------------------------------------------------

void Viewport::Reset()
{
	mOrigin.Zero();
	mWidth = 0;
	mHeight = 0;
	mAspectRatio = 0.0f;
	mDraw = false;
	mBackfaceCull = false;
	mZBufferOn = false;
	mZBufferIsset = false;

	mNDCToScreen.Identity();
	mZBuffer.Clear();
}
// ------------------------------------------------------------------------------------------

void Viewport::Set(const CVector2& topleft, const CVector2& btmright)
{
	mOrigin = topleft;
	mWidth = RoundPixel(btmright.x);
	mHeight = RoundPixel(btmright.y);
	mAspectRatio = btmright.x / btmright.y;

	// Init the zbuffer if it hasn't been already (depends on what order zbuffer cmd called)
	if (mZBufferOn && !mZBufferIsset)
	{
		mZBuffer.Clear();
		mZBuffer.Resize(mWidth, mHeight, ZBUFF_DEFAULT);
		mZBufferIsset = true;
	}

	CreateNDCToScreenMatrix();
}
// ------------------------------------------------------------------------------------------

void Viewport::Set(float l, float t, float r, float b)
{
	mOrigin = CVector2(l, t);
	mWidth = RoundPixel(r);
	mHeight = RoundPixel(b);
	mAspectRatio = r / b;

	// Init the zbuffer if it hasn't been already (depends on what order zbuffer cmd called)
	if (mZBufferOn && !mZBufferIsset)
	{
		mZBuffer.Clear();
		mZBuffer.Resize(mWidth, mHeight, ZBUFF_DEFAULT);
		mZBufferIsset = true;
	}

	CreateNDCToScreenMatrix();
}
// ------------------------------------------------------------------------------------------

void Viewport::EnableZBuffer()
{
	mZBufferOn = true;
	if (!mZBufferIsset && mWidth > 0 && mHeight > 0)
	{
		mZBuffer.Clear();
		mZBuffer.Resize(mWidth, mHeight, ZBUFF_DEFAULT);
		mZBufferIsset = true;
	}
	else
	{
		WipeZBuffer();
	}
}
// ------------------------------------------------------------------------------------------

void Viewport::DisableZBuffer()
{
	mZBufferOn = false;
}
// ------------------------------------------------------------------------------------------

bool Viewport::CheckZDepth(const int x, const int y, const FLOAT z)
{
	//const float zfar = 1000.0f, znear = 0.1f, d = 1.0f;
	//float a = zfar / (zfar - znear);
	//float b = zfar * znear / (znear-zfar);
	//UINT32 zbuffval = (1<<((sizeof(int) * 8)-1)) * (a + b / 1.0f);
	UINT32 zi = (1 << ((sizeof(int) * 8)-1)) * z;
	//FLOAT zi = z;
	if (zi < mZBuffer.Get(x, y))
	{
		if (mZBuffer.Get(x, y) != ZBUFF_DEFAULT)
		{
			int i = 0;
		}
		// Update that index with the new value
		mZBuffer.Set(x, y, zi);
		return true;
	}
	return false;
}

void Viewport::WipeZBuffer()
{
	mZBuffer.SetAll(ZBUFF_DEFAULT);
}

void Viewport::BackfaceCull(PrimList& primitives)
{
	// Backface culling is disabled
	if (!mBackfaceCull)
		return;

	CVector3 cameraLook = Camera::Instance()->GetLookDirection();
	const int sz = primitives.size();
	for (int i = sz-1; i >= 0; --i)
	{
		CVector3 norm = primitives[i]->ComputeNormal();
		if (Dot(norm, cameraLook) > 0.0f)
		{
			// Not visible to us; don't draw it
			primitives.erase(primitives.begin() + i);
		}
	}
	// Resize
	primitives.shrink_to_fit();
}
// ------------------------------------------------------------------------------------------

void Viewport::Draw()
{
	CRect2 r(mOrigin.x, mOrigin.y, (float)mWidth, (float)mHeight);
	if (r.IsValid())
	{
		r.GetLeftSegment().DrawVertical();
		r.GetBottomSegment().DrawHorizontal();
		r.GetRightSegment().DrawVertical();
		r.GetTopSegment().DrawHorizontal();
	}
}
// ------------------------------------------------------------------------------------------

CRect2 Viewport::GetViewport()
{
	return CRect2(mOrigin.x, mOrigin.y, (float)mWidth, (float)mHeight);
}
// ------------------------------------------------------------------------------------------

void Viewport::CreateNDCToScreenMatrix()
{
	float n = Camera::Instance()->GetNear();
	float f = Camera::Instance()->GetFar();
	float d = 1.0f; // Z depth scale

	float width = static_cast<float>(mWidth) * 0.5f;
	float height = static_cast<float>(mHeight) * 0.5f;

	mNDCToScreen.Identity();
	CVector4 c0, c1, c2, c3;
	mNDCToScreen.GetColumns(c0, c1, c2, c3);

	c0.x = width;
	c1.y = -height;
	c2.z = d / 2;
	c3.x = width + mOrigin.x;
	c3.y = height + mOrigin.y;
	c3.z = d / 2;

	mNDCToScreen.SetColumns(c0, c1, c2, c3);
}