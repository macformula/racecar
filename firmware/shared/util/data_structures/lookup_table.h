/// @author Blake Freer
/// @date 2023-12-04

#ifndef SHARED_UTIL_DATA_STRUCTURES_LOOKUP_TABLE_H_
#define SHARED_UTIL_DATA_STRUCTURES_LOOKUP_TABLE_H_

namespace shared::util {

class LookupTable {
public:
	LookupTable(int length, float* keys, float* values) : length_(length), keys_(keys), values_(values) {

	}

	float Interpolate(float key) const {
		int least_greater_idx = 0;

		// Find next greatest element in keys_, assumes keys_ is sorted
		while (keys_[least_greater_idx] < key && least_greater_idx < length_) {
			least_greater_idx += 1;
		}

		// If key is outside of range, return edge value
		if (least_greater_idx == 0) {
			return values_[0];
		}
		if (least_greater_idx == length_) {
			return values_[length_-1];
		}
		
		float fraction = (key - keys_[least_greater_idx - 1])
			/ (keys_[least_greater_idx] - keys_[least_greater_idx - 1]);

		float interpolated_value = (1 - fraction) * values_[least_greater_idx - 1]
			+ fraction * values_[least_greater_idx];

		return interpolated_value;
	}

private:
	int length_;
	float* keys_;
	float* values_;
};


} // namespace shared::util

#endif