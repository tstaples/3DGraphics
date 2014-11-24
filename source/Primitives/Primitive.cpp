#include "stdafx.h"
#include "Primitive.h"
#include "Graphics/Rasterizer.h"

CPrimitive::CPrimitive(const PrimType::Type type) 
	:	mType(type) 
{}

CPrimitive::CPrimitive(const int type) 
	:	mType(PrimType::Type(type)) 
{}

bool CPrimitive::IsValid() const
{ 
	return (PrimType::IsValid(mType));
}

const PrimType::Type CPrimitive::Type() const
{ 
	return mType;
}