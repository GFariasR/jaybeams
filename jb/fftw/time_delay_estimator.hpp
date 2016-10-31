#ifndef jb_fftw_time_delay_estimator_hpp
#define jb_fftw_time_delay_estimator_hpp

#include <jb/complex_traits.hpp>
#include <jb/fftw/aligned_vector.hpp>
#include <jb/fftw/plan.hpp>

#include <type_traits>

namespace jb {
namespace fftw {

/**
 * Determine if a timeseries tye guarantees alignment.
 */
template <typename T>
struct always_aligned : public std::false_type {};

template <typename T>
struct always_aligned<jb::fftw::aligned_vector<T>> : public std::true_type {};

/**
 * A simple time delay estimator based on cross-correlation
 */
template <typename timeseries_t,
          template <typename T> class vector = jb::fftw::aligned_vector>
class time_delay_estimator {
public:
  typedef timeseries_t timeseries_type;
  typedef typename timeseries_type::value_type value_type;
  typedef typename jb::extract_value_type<value_type>::precision precision_type;
  typedef jb::fftw::plan<precision_type> plan;

  time_delay_estimator(timeseries_type const& a, timeseries_type const& b)
      : tmpa_(a.size())
      , tmpb_(b.size())
      , a2tmpa_(plan::create_forward(a, tmpa_, planning_flags()))
      , b2tmpb_(plan::create_forward(b, tmpb_, planning_flags()))
      , out_(a.size())
      , tmpa2out_(plan::create_backward(tmpa_, out_, planning_flags())) {
    if (a.size() != b.size()) {
      throw std::invalid_argument("size mismatch in time_delay_estimator ctor");
    }
  }

  std::pair<bool, precision_type>
  estimate_delay(timeseries_type const& a, timeseries_type const& b) {
    // Validate the input sizes.  For some types of timeseries the
    // alignment may be different too, but we only use the alignment
    // when the type of timeseries guarantees to always be aligned.
    if (a.size() != tmpa_.size() or b.size() != tmpa_.size()) {
      throw std::invalid_argument(
          "size mismatch in time_delay_estimator<>::estimate_delay()");
    }
    // First we apply the Fourier transform to both inputs ...
    a2tmpa_.execute(a, tmpa_);
    b2tmpb_.execute(b, tmpb_);
    // ... then we compute Conj(A) * B for the transformed inputs ...
    for (std::size_t i = 0; i != tmpa_.size(); ++i) {
      tmpa_[i] = std::conj(tmpa_[i]) * tmpb_[i];
    }
    // ... then we compute the inverse Fourier transform to the result ...
    tmpa2out_.execute(tmpa_, out_);
    // ... finally we compute (argmax,max) for the result ...
    precision_type max = std::numeric_limits<precision_type>::min();
    std::size_t argmax = 0;
    for (std::size_t i = 0; i != out_.size(); ++i) {
      if (max < out_[i]) {
        max = out_[i];
        argmax = i;
      }
    }
    // TODO(#13) the threshold for acceptance should be configurable,
    // maybe we want the value to be close to the expected area of 'a'
    // for example ...
    if (max <= std::numeric_limits<precision_type>::epsilon()) {
      return std::make_pair(false, precision_type(0));
    }
    return std::make_pair(true, precision_type(argmax));
  }

private:
  /**
   * Determine the correct FFTW planning flags given the inputs.
   */
  static int planning_flags() {
    if (always_aligned<timeseries_t>::value) {
      return FFTW_MEASURE;
    }
    return FFTW_MEASURE | FFTW_UNALIGNED;
  }

private:
  vector<std::complex<precision_type>> tmpa_;
  vector<std::complex<precision_type>> tmpb_;
  plan a2tmpa_;
  plan b2tmpb_;
  vector<precision_type> out_;
  plan tmpa2out_;
};

} // namespace fftw
} // namespace jb

#endif // jb_fftw_time_delay_estimator_hpp
