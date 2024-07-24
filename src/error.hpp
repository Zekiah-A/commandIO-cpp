#pragma once

namespace commandIO {
	enum Error {
		SUCCESS,
		EXCESS_PARAM,
		MISSING_VALUE,
		INVALID_PARAM_TYPE,
		UNKNOWN_PARAM
	};

	extern const char *errorMessages[];
}
