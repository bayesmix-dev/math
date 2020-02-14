#ifdef STAN_OPENCL

#include <stan/math/prim/fun/Eigen.hpp>
#include <stan/math/opencl/matrix_cl.hpp>
#include <stan/math/opencl/copy.hpp>
#include <stan/math/opencl/kernel_generator.hpp>
#include <stan/math.hpp>
#include <gtest/gtest.h>
#include <vector>

using Eigen::Dynamic;
using Eigen::MatrixXd;
using Eigen::MatrixXi;
using stan::math::matrix_cl;

#define EXPECT_MATRIX_NEAR(A, B, DELTA) \
  for (int i = 0; i < A.size(); i++)    \
    EXPECT_NEAR(A(i), B(i), DELTA);

TEST(MathMatrixCL, colwise_sum_test) {
  MatrixXd m(3, 2);
  m << 1.1, 1.2, 1.3, 1.4, 1.5, 1.6;

  matrix_cl<double> m_cl(m);

  matrix_cl<double> res_cl = stan::math::colwise_sum(m_cl);
  MatrixXd raw_res = stan::math::from_matrix_cl(res_cl);
  EXPECT_GE(m.rows(), raw_res.rows());
  MatrixXd res = raw_res.colwise().sum();
  MatrixXd correct = m.colwise().sum();
  EXPECT_EQ(correct.rows(), res.rows());
  EXPECT_EQ(correct.cols(), res.cols());
  EXPECT_MATRIX_NEAR(correct, res, 1e-9);
}

TEST(MathMatrixCL, colwise_min_test) {
  MatrixXd m(3, 2);
  m << 1.1, 1.2, 1.3, 1.4, 1.5, 1.6;

  matrix_cl<double> m_cl(m);

  matrix_cl<double> res_cl = stan::math::colwise_min(m_cl);
  MatrixXd raw_res = stan::math::from_matrix_cl(res_cl);
  EXPECT_GE(m.rows(), raw_res.rows());
  MatrixXd res = raw_res.colwise().minCoeff();
  MatrixXd correct = m.colwise().minCoeff();
  EXPECT_EQ(correct.rows(), res.rows());
  EXPECT_EQ(correct.cols(), res.cols());
  EXPECT_MATRIX_NEAR(correct, res, 1e-9);
}

TEST(MathMatrixCL, colwise_max_test) {
  MatrixXd m(2, 3);
  m << 1.1, 1.2, 1.3, 1.4, 1.5, 1.6;

  matrix_cl<double> m_cl(m);

  matrix_cl<double> res_cl = stan::math::colwise_max(m_cl);
  MatrixXd raw_res = stan::math::from_matrix_cl(res_cl);
  EXPECT_GE(m.rows(), raw_res.rows());
  MatrixXd res = raw_res.colwise().maxCoeff();
  MatrixXd correct = m.colwise().maxCoeff();
  EXPECT_EQ(correct.rows(), res.rows());
  EXPECT_EQ(correct.cols(), res.cols());
  EXPECT_MATRIX_NEAR(correct, res, 1e-9);
}

TEST(MathMatrixCL, colwise_sum_triangular) {
  MatrixXd m(3, 2);
  m << 1.1, 1.2, 1.3, 1.4, 1.5, 1.6;

  matrix_cl<double> m_cl(m, stan::math::matrix_cl_view::Lower);
  MatrixXd m1 = m;
  m1.triangularView<Eigen::StrictlyUpper>() = MatrixXd::Constant(3, 2, 0);

  matrix_cl<double> res1_cl = stan::math::colwise_sum(m_cl);
  MatrixXd raw_res1 = stan::math::from_matrix_cl(res1_cl);
  EXPECT_GE(m1.rows(), raw_res1.rows());
  MatrixXd res1 = raw_res1.colwise().sum();
  MatrixXd correct1 = m1.colwise().sum();
  EXPECT_EQ(correct1.rows(), res1.rows());
  EXPECT_EQ(correct1.cols(), res1.cols());
  EXPECT_MATRIX_NEAR(correct1, res1, 1e-9);

  m_cl.view(stan::math::matrix_cl_view::Upper);
  MatrixXd m2 = m;
  m2.triangularView<Eigen::StrictlyLower>() = MatrixXd::Constant(3, 2, 0);

  matrix_cl<double> res2_cl = stan::math::colwise_sum(m_cl);
  MatrixXd raw_res2 = stan::math::from_matrix_cl(res2_cl);
  EXPECT_GE(m2.rows(), raw_res2.rows());
  MatrixXd res2 = raw_res2.colwise().sum();
  MatrixXd correct2 = m2.colwise().sum();
  EXPECT_EQ(correct2.rows(), res2.rows());
  EXPECT_EQ(correct2.cols(), res2.cols());
  EXPECT_MATRIX_NEAR(correct2, res2, 1e-9);
}

TEST(MathMatrixCL, nested_rowwise_colwise_sum) {
  MatrixXd m(3, 2);
  m << 1.1, 1.2, 1.3, 1.4, 1.5, 1.6;

  matrix_cl<double> m_cl(m);

  matrix_cl<double> res_cl
      = stan::math::colwise_sum(stan::math::rowwise_sum(m_cl));
  MatrixXd raw_res = stan::math::from_matrix_cl(res_cl);
  EXPECT_GE(m.rows(), raw_res.rows());
  MatrixXd res = raw_res.colwise().sum();
  double correct = m.sum();
  EXPECT_EQ(1, res.rows());
  EXPECT_EQ(1, res.cols());
  EXPECT_NEAR(correct, res(0), 1e-9);
}

TEST(MathMatrixCL, colwise_sum_test_large) {
  std::vector<int> Ns = {123, 25, 63, 64, 65, 147};
  std::vector<int> Ms = {147, 4, 63, 64, 65, 123};
  for (int i = 0; i < Ns.size(); i++) {
    MatrixXd m = MatrixXd::Random(Ns[i], Ms[i]);

    matrix_cl<double> m_cl(m);
    matrix_cl<double> res_cl = stan::math::colwise_sum(m_cl);
    MatrixXd raw_res = stan::math::from_matrix_cl(res_cl);
    EXPECT_GT(m.rows(), raw_res.rows());
    MatrixXd res = raw_res.colwise().sum();
    MatrixXd correct = m.colwise().sum();
    EXPECT_EQ(correct.rows(), res.rows());
    EXPECT_EQ(correct.cols(), res.cols());
    EXPECT_MATRIX_NEAR(correct, res, 1e-9);
  }
}

#endif
