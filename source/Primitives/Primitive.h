//====================================================================================================
//	File: Primitive.h
//	Created by: Tyler Staples
//	Date created: 6/10/2014
//	Description: 
//		Base class for all primitive types.
//====================================================================================================

#ifndef INCLUDED_PRIMITIVE_H
#define INCLUDED_PRIMITIVE_H

#include "Containers/Vertex3.h"
#include "Containers/Vector3.h"
#include "3DGFXTypes.h"
#include <vector>

class CMatrix33;

class CPrimitive
{
public:
	CPrimitive(const PrimType::Type type);
	CPrimitive(const int type);

	// Return the primitives type
	const PrimType::Type Type() const;
	
	// Returns true if the primitive is valid
	virtual bool IsValid() const;

	// Adds a vertex to the primitive
	virtual void AddVertex(const CVertex3& vert) = 0;

	// Returns number of verticies the primitive has been assigned
	virtual const int VertexCount() const = 0;

	// Returns the number of verticies required for the primitive
	virtual const int MaxVerticies() const = 0;

	// Returns the pivot/center of the primitive
	virtual CVector2 GetPivot() const = 0;

	virtual float GetZDepth() = 0;

	// Returns the normal of the primitive
	virtual CVector3 ComputeNormal() const = 0;

	// Sets the normal of all the primitive's verticies to the normal passed in
	virtual void SetVertexNormals(const CVector3& normal) = 0;

	virtual void SetVertexColors(const CColor& color) = 0;

	// The primitive's draw logic
	virtual void Draw(FillMode::Mode mode) = 0;

	// Transforms all the verticies by the transformation matrix
	virtual void Transform(const CMatrix33& tm) = 0;

protected:
	// Draw the primitive with solid edges
	virtual void DrawSolid() = 0;

	// Draw only the verticies
	virtual void DrawPoints() = 0;

	// Fill the area with color (prim needs 3+ verts)
	virtual void Fill() {AIASSERT(false, "Invoked base class definition");};

protected:
	const PrimType::Type mType;
};

#endif