#ifndef STAN_MATH_PRIM_PROB_UNIFORM_RNG_HPP
#define STAN_MATH_PRIM_PROB_UNIFORM_RNG_HPP

#include <stan/math/prim/meta.hpp>
#include <stan/math/prim/err.hpp>
#include <stan/math/prim/fun/constants.hpp>
#include <stan/math/prim/fun/max_size.hpp>
#include <stan/math/prim/fun/value_of.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/variate_generator.hpp>

namespace stan {
namespace math {

/** \ingroup prob_dists
 * Return a uniform random variate for the given upper and lower bounds using
 * the specified random number generator.
 *
 * alpha and beta can each be a scalar or a one-dimensional container. Any
 * non-scalar inputs must be the same size.
 *
 * @tparam T_alpha Type of shape parameter
 * @tparam T_beta Type of inverse scale parameter
 * @tparam RNG type of random number generator
 * @param alpha (Sequence of) lower bound parameter(s)
 * @param beta (Sequence of) upper bound parameter(s)
 * @param rng random number generator
 * @return (Sequence of) uniform random variate(s)
 * @throw std::domain_error if alpha or beta are non-finite
 * @throw std::invalid_argument if non-scalar arguments are of different
 * sizes
 */
template <typename T_alpha, typename T_beta, class RNG>
inline typename VectorBuilder<true, double, T_alpha, T_beta>::type uniform_rng(
    const T_alpha& alpha, const T_beta& beta, RNG& rng) {
  using boost::random::uniform_real_distribution;
  using boost::variate_generator;

  static const char* function = "uniform_rng";

  check_finite(function, "Lower bound parameter", alpha);
  check_finite(function, "Upper bound parameter", beta);
  check_consistent_sizes(function, "Lower bound parameter", alpha,
                         "Upper bound parameter", beta);
  check_greater(function, "Upper bound parameter", beta, alpha);

  scalar_seq_view<T_alpha> alpha_vec(alpha);
  scalar_seq_view<T_beta> beta_vec(beta);
  size_t N = max_size(alpha, beta);
  VectorBuilder<true, double, T_alpha, T_beta> output(N);

  variate_generator<RNG&, uniform_real_distribution<> > uniform_rng(
      rng, uniform_real_distribution<>(0.0, 1.0));
  for (size_t n = 0; n < N; ++n) {
    output[n] = (beta_vec[n] - alpha_vec[n]) * uniform_rng() + alpha_vec[n];
  }

  return output.data();
}

}  // namespace math
}  // namespace stan
#endif
