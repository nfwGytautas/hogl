#include "transform.hpp"

namespace hogl_ui {
	tinfo::tinfo() {
	}

	tinfo::tinfo(float xoff, float yoff, float zoff, float xscale, float yscale, float zscale)
		: x_offset(xoff), y_offset(yoff), z_offset(zoff), x_scale(xscale), y_scale(yscale), z_scale(zscale)
	{
	}

	tinfo tinfo::combine(const tinfo& t) const {
		return tinfo(
			this->x_offset + t.x_offset,
			this->y_offset + t.y_offset,
			this->z_offset + t.z_offset,
			this->x_scale * t.x_scale,
			this->y_scale * t.y_scale,
			this->z_scale * t.z_scale
		);
	}

}
