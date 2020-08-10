#pragma once

#include <memory>
#include <cassert>

#include "object.hpp"

/** Object contained in another object.
 *
 * For simplicity, must not be Updatable.
 * Updatables are objects managed by World.
 * TODO: rename Updatable to WorldManaged or something.
 */
class SubObject: virtual public Object
{
public:
	using ParentRef = std::weak_ptr<Object>;

	SubObject(ParentRef&& parent):
		parent(std::move(parent))
	{
	}

	virtual ~SubObject() = default;

	virtual const QRot& get_world_t() const final override;

private:
	void invalidate_sub_cache()
	{
		dirty = true;
	}
	friend class SuperObject;

	ParentRef parent;
	mutable QRot world_t;
	mutable bool dirty;
};

/** Object that contains and manages other objects.
 *
 * The SuperObject must own the shared_ptr to its SubObjects, and
 * SubObject's transformation is relative to its SuperObject.
 */
class SuperObject:
	virtual public Object,
	public std::enable_shared_from_this<SuperObject>
{
public:
	virtual ~SuperObject() = default;
	const std::vector<std::weak_ptr<SubObject>>& get_sub_objects();

	virtual void invalidate_cache() final override;

protected:
	virtual void create_sub_objects(std::vector<std::weak_ptr<SubObject>>&) = 0;

private:
	std::vector<std::weak_ptr<SubObject>> objs;
	bool initialized = false;
};
