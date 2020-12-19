#pragma once

#define IOLIVE_ASSERT_OR_RETURN(some) \
	if (some); else return

#define IOLIVE_ASSERT_OR_RETURN_VAL(some, val) \
	if (some); else return (val)