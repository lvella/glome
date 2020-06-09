#pragma once

#include <memory>
#include <cassert>

#include "object.hpp"

/** Either a SubObject, SuperObject or both. */
class TreeNode: virtual public Object
{
public:
	virtual const QRot& get_world_t() const;
};

/** Object contained in another object.
 *
 * For simplicity, must not be Updatable.
 * Updatables are objects managed by World.
 * TODO: rename Updatable to WorldManaged or something.
 */
class SubObject: virtual public TreeNode
{
public:
	using ParentRef = std::weak_ptr<TreeNode>;

	SubObject(ParentRef&& parent):
		parent(std::move(parent))
	{
	}

	virtual ~SubObject() = default;

	virtual const QRot& get_world_t() const final override;

private:
	ParentRef parent;
	mutable QRot world_t;
};

/** Object that contains and manages other objects.
 *
 * The SuperObject must own the shared_ptr to its SubObjects, and
 * SubObject's transformation is relative to its SuperObject.
 */
class SuperObject:
	virtual public TreeNode,
	public std::enable_shared_from_this<SuperObject>
{
public:
	virtual ~SuperObject() = default;
	virtual std::vector<std::weak_ptr<SubObject>> create_sub_objects() = 0;
};
