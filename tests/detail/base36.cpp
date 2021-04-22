#include <ozo/detail/base36.h>
#include <boost/hana/comparing.hpp>

#include <gtest/gtest.h>

namespace {

TEST(ltob36, should_verify_runtime_version_of_ltob36) {
    EXPECT_EQ(ozo::detail::ltob36(29999809), "HV001");
    EXPECT_EQ(ozo::detail::ltob36(833328), "HV00");
    EXPECT_EQ(ozo::detail::ltob36(0), "0");
}

static_assert(ozo::detail::ltob36<29999809>() == BOOST_HANA_STRING("HV001"));
static_assert(ozo::detail::ltob36<833328>() == BOOST_HANA_STRING("HV00"));
static_assert(ozo::detail::ltob36<0>() == BOOST_HANA_STRING("0"));

} // namespace
