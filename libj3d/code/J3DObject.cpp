/******************************************************************************
 J3DObject.cpp

	This is the base class for all objects in 3D space.  These objects form
	a tree, with a single instance of J3DUniverse as the root node.  When an
	object is moved, rotated, or scaled, so are all its children.  This allows
	a composite object (formalized by J3DComposite) to act as if it were
	a single unit.

	Animation is best handled by creating a separate object that draws nothing
	but whose position, rotation, and scaling are functions of time.  The
	actual objects that are to be animated are then attached as children of
	this invisible object.  Time itself requires a loop (e.g. an event loop).
	In the case of JX, an idle task is most appropriate.

	BASE CLASS = NONE

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <J3DObject.h>
#include <JColormap.h>
#include <GL/gl.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

J3DObject::J3DObject
	(
	J3DObject*		parent,
	const JFloat	x,
	const JFloat	y,
	const JFloat	z
	)
	:
	itsPosition(3, x,y,z),
	itsZRotator(kJ3DUnitZ),
	itsScale(kJ3DUnitScale),

	itsParent(parent),
	itsChildList(NULL),
	itsIgnoreRemoveChildFlag(kJFalse)
{
	J3DObjectX();
}

J3DObject::J3DObject
	(
	J3DObject*		parent,
	const JVector&	position
	)
	:
	itsPosition(position),
	itsZRotator(kJ3DUnitZ),
	itsScale(kJ3DUnitScale),

	itsParent(parent),
	itsChildList(NULL),
	itsIgnoreRemoveChildFlag(kJFalse)
{
	J3DObjectX();
}

// private

void
J3DObject::J3DObjectX()
{
	itsZAngle  = 0.0;
	itsXYAngle = 0.0;

	if (itsParent != NULL)
		{
		itsParent->AddChild(this);
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

J3DObject::~J3DObject()
{
	if (itsParent != NULL)
		{
		itsParent->RemoveChild(this);
		}

	itsIgnoreRemoveChildFlag = kJTrue;
	delete itsChildList;
}

/******************************************************************************
 SetParent

 ******************************************************************************/

void
J3DObject::SetParent
	(
	J3DObject* parent
	)
{
	assert( itsParent != NULL && parent != NULL );
	itsParent->RemoveChild(this);
	itsParent = parent;
	itsParent->AddChild(this);
	Refresh();
}

/******************************************************************************
 GetChildIterator

 ******************************************************************************/

JBoolean
J3DObject::GetChildIterator
	(
	JOrderedSetIterator<J3DObject*>** iter
	)
	const
{
	if (itsChildList != NULL)
		{
		*iter = itsChildList->NewIterator();
		return kJTrue;
		}
	else
		{
		*iter = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 Rotation

	The axes are first rotated to align the +z-axis with the given vector
	and then rotated counter-clockwise around this new +z-axis.

 ******************************************************************************/

void
J3DObject::SetRotation
	(
	const JVector&	zAxis,
	const JFloat	longitude
	)
{
	itsZAngle   = acos( JDotProduct(zAxis, kJ3DUnitZ) / zAxis.GetLength() ) * kJRadToDeg;
	itsZRotator = JCrossProduct(kJ3DUnitZ, zAxis);
	itsXYAngle  = longitude;
	Refresh();
}

/******************************************************************************
 RenderAll

	Draw ourselves and then all our children relative to ourselves.

 ******************************************************************************/

void
J3DObject::RenderAll
	(
	J3DPainter& p
	)
{
	glPushMatrix();

	Place();
	Rotate();
	Scale();
	Render(p);

	if (itsChildList != NULL)
		{
		const JSize count = itsChildList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			(itsChildList->NthElement(i))->RenderAll(p);
			}
		}

	glPopMatrix();
}

/******************************************************************************
 Place (virtual protected)

	Position ourselves relative to the current origin.

	The default is to use the information from SetPosition().
	Derived classes can override this either completely or to call
	SetPosition() before calling inherited::Place().

 ******************************************************************************/

void
J3DObject::Place()
{
	glTranslated(itsPosition.GetElement(1),
				 itsPosition.GetElement(2),
				 itsPosition.GetElement(3));
}

/******************************************************************************
 Rotate (virtual protected)

	Rotate ourselves relative to the current origin.

	The default is to use the information from SetRotation().
	Derived classes can override this either completely or to call
	SetRotation() before calling inherited::Rotate().

 ******************************************************************************/

void
J3DObject::Rotate()
{
	if (itsZAngle != 0.0)
		{
		glRotated(itsZAngle,
				  itsZRotator.GetElement(1),
				  itsZRotator.GetElement(2),
				  itsZRotator.GetElement(3));
		}

	if (itsXYAngle != 0.0)
		{
		glRotated(itsXYAngle, 0.0, 0.0, 1.0);
		}
}

/******************************************************************************
 Scale (virtual protected)

	Scale ourselves and all our children.

	The default is to use the information from SetScale().
	Derived classes can override this either completely or to call
	SetScale() before calling inherited::Scale().

 ******************************************************************************/

void
J3DObject::Scale()
{
	if (itsScale != kJ3DUnitScale)
		{
		glScaled(itsScale.GetElement(1), itsScale.GetElement(2), itsScale.GetElement(3));
		}
}

/******************************************************************************
 Refresh (virtual protected)

	Tell our parent to redraw when convenient.  This propagates up to
	J3DUniverse, which tells its cameras to refresh.

 ******************************************************************************/

void
J3DObject::Refresh()
{
	assert( itsParent != NULL );
	itsParent->Refresh();
}

/******************************************************************************
 Redraw (virtual protected)

	Tell our parent to redraw immediately.  This propagates up to
	J3DUniverse, which tells its cameras to redraw.

	Since this forces an immediate call to RenderAll(), don't do it unless
	you have to.  It is much better to call Refresh().

 ******************************************************************************/

void
J3DObject::Redraw()
{
	assert( itsParent != NULL );
	itsParent->Redraw();
}

/******************************************************************************
 GetDefaultColor (virtual protected)

	Ask our parent for the default color.  This propagates up to
	J3DUniverse, which returns a value.

 ******************************************************************************/

JColorIndex
J3DObject::GetDefaultColor()
	const
{
	assert( itsParent != NULL );
	return itsParent->GetDefaultColor();
}

/******************************************************************************
 AddChild (private)

 ******************************************************************************/

void
J3DObject::AddChild
	(
	J3DObject* child
	)
{
	if (itsChildList == NULL)
		{
		itsChildList = new JPtrArray<J3DObject>(JPtrArrayT::kDeleteAll);
		assert( itsChildList != NULL );
		}

	itsChildList->Append(child);
	Refresh();
}

/******************************************************************************
 RemoveChild (private)

 ******************************************************************************/

void
J3DObject::RemoveChild
	(
	J3DObject* child
	)
{
	if (itsChildList != NULL && !itsIgnoreRemoveChildFlag)
		{
		itsChildList->Remove(child);

		if (itsChildList->IsEmpty())
			{
			delete itsChildList;
			itsChildList = NULL;
			}

		Refresh();
		}
}
