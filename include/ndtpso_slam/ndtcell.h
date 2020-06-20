#ifndef NDTCELL_H
#define NDTCELL_H

#include "config.h"
#include "ndtpso_slam/window_adder.h"
#include <eigen3/Eigen/Core>
#include <vector>

// 1. The global sum
//        (plus) the currnet partial sum
//        (minus) the partial sum of the item we will replace with the current partial sum
// 2. We replace the item
#define WINDOW_ADD(global, partial, partials, idx) \
    (global = (global + partial - partials[idx])); \
    (partials[idx] = partial)

#define WINDOW_INC_ID(idx) (idx = ((idx + 1) % NDT_WINDOW_SIZE))

using namespace Eigen;
using std::vector;

class NDTCell {
private:
    Vector2d s_current_partial_sum;
    Matrix2d s_inv_covar;
    int s_current_count{ 0 };
    //size_t s_current_window_id{ 0 };
    WindowAdder<int> s_count{ WindowAdder<int>(NDT_WINDOW_SIZE, 0) };
    WindowAdder<Vector2d> s_points_sum{ WindowAdder<Vector2d>(NDT_WINDOW_SIZE, Vector2d::Zero()) };
    WindowAdder<Matrix2d> s_covars_sum{ WindowAdder<Matrix2d>(NDT_WINDOW_SIZE, Matrix2d::Zero()) };

    inline void s_calc_covar_inverse();

public:
    std::vector<Vector2d> points[NDT_WINDOW_SIZE];
    Vector2d mean;
    NDTCell(bool calculate_params = true);
    void addPoint(const Vector2d& point);
    bool built{ false };
    bool created{ false };
    bool build();
    double normalDistribution(const Vector2d& point);
    void reset();
};

#endif // NDTCELL_H
