#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <casadi/casadi.hpp>

using namespace std;
using namespace casadi;

class MPC
{
public:
    MPC();
    void LoadParams(const std::map<std::string, double> &params);
    Function setDynamicEquation();
    Function test_PV_integration();
    Function test_q_integration();
    bool setSolver();
    // bool test_setSolver();
    bool solve(const Eigen::Matrix<double, 10, 1> &current_states, const Eigen::Matrix<double, 10, 1> &desired_states, const Eigen::Vector4d &q_G, const Eigen::Vector3d &omega_G, const Eigen::Vector3d &alpha_G, const Eigen::Vector3d &a_G);
    // bool test_solve(const Eigen::Matrix<double, 10, 1> &current_states, const Eigen::Matrix<double, 10, 1> &desired_states);
    vector<double> getFirstU();
    // vector<double> test_getFirstU();

private:
    int N, n_state, n_input, n_param;
    double dt;
    double mass;
    double T_min, T_max, omega_max;
    double last_T, last_omega_x, last_omega_y, last_omega_z;
    double last_U_x, last_U_y, last_U_z;

    vector<double> lbg;
    vector<double> ubg;
    vector<double> lbx;
    vector<double> ubx;

    DM Q, Q_n, R;
    // SX X, U, P;

    map<string, DM> res;
    Function solver;
    Function dynamic_equation;
};
