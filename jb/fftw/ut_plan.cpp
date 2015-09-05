#include <jb/fftw/plan.hpp>
#include <jb/testing/check_array_close_enough.hpp>
#include <jb/testing/check_vector_close_enough.hpp>

#include <boost/test/unit_test.hpp>
#include <algorithm>

namespace {

template<typename precision_t>
void test_plan_complex2complex() {
  int nsamples = 1<<15;
  int tol = nsamples;

  typedef jb::fftw::plan<precision_t> tested;
  typedef typename tested::precision_type precision_type;
  typedef std::complex<precision_type> complex;

  std::vector<complex> in(nsamples);
  std::vector<complex> tmp(nsamples);
  std::vector<complex> out(nsamples);
  
  std::size_t h = nsamples / 2;
  for (std::size_t i = 0; i != h; ++i) {
    in[i] = complex(i - h/4.0, 0);
    in[i + h] = complex(h/4.0 - i, 0);
  }
  
  tested dir = tested::create_forward(in, tmp);
  tested inv = tested::create_backward(tmp, out);

  dir.execute(in, tmp);
  inv.execute(tmp, out);
  for (std::size_t i = 0; i != nsamples; ++i) {
    out[i] /= nsamples;
  }
  jb::testing::check_vector_close_enough(out, in, tol);
}

} // namespace

/**
 * @test Verify that we can create and operate a jb::fftw::plan<double>
 */
BOOST_AUTO_TEST_CASE(fftw_plan_complex_double) {
  test_plan_complex2complex<double>();
}

/**
 * @test Verify that we can create and operate a jb::fftw::plan<float>
 */
BOOST_AUTO_TEST_CASE(fftw_plan_complex_float) {
  test_plan_complex2complex<float>();
}

/**
 * @test Verify that we can create and operate a jb::fftw::plan<long double>
 */
BOOST_AUTO_TEST_CASE(fftw_plan_complex_long_double) {
  test_plan_complex2complex<long double>();
}