#include "mpc_landing.h"

MPC::MPC(string type)
{
    this->type = type;
    if (this->type == "NMPC")
    {
        N = 20;
        dt = 0.1;
        n_state = 13;
        n_input = 4;
        n_param = 13;

        T_min = 4.0;
        T_max = 15.0;
        omega_max = 1.0;

        Q = DM::zeros(13, 13);
        Q_n = DM::zeros(13, 13);
        R = DM::zeros(4, 4);

        last_T = 9.8;
        last_omega_x = 0.0;
        last_omega_y = 0.0;
        last_omega_z = 0.0;
    }
    else if (this->type == "LMPC")
    {
        N = 20;
        dt = 0.1;
        n_state = 9;
        n_input = 3;
        // n_param = 12;

        last_U_x = 0.0;
        last_U_y = 0.0;
        last_U_z = 0.0;

        Q = DM::zeros(9, 9);
        Q_n = DM::zeros(9, 9);
        R = DM::zeros(3, 3);
    }
}

void MPC::LoadParams(const std::map<std::string, double> &params)
{
    if (this->type == "NMPC")
    {
        Q(0, 0) = params.find("w_P_xy") != params.end() ? params.at("w_P_xy") : 0;
        Q(1, 1) = params.find("w_P_xy") != params.end() ? params.at("w_P_xy") : 0;
        Q(2, 2) = params.find("w_P_z") != params.end() ? params.at("w_P_z") : 0;
        Q(3, 3) = params.find("w_P_xy_sum") != params.end() ? params.at("w_P_xy_sum") : 0;
        Q(4, 4) = params.find("w_P_xy_sum") != params.end() ? params.at("w_P_xy_sum") : 0;
        Q(5, 5) = params.find("w_P_z_sum") != params.end() ? params.at("w_P_z_sum") : 0;
        Q(6, 6) = params.find("w_V_xy") != params.end() ? params.at("w_V_xy") : 0;
        Q(7, 7) = params.find("w_V_xy") != params.end() ? params.at("w_V_xy") : 0;
        Q(8, 8) = params.find("w_V_z") != params.end() ? params.at("w_V_z") : 0;
        Q(9, 9) = params.find("w_q") != params.end() ? params.at("w_q") : 0;
        Q(10, 10) = params.find("w_q") != params.end() ? params.at("w_q") : 0;
        Q(11, 11) = params.find("w_q") != params.end() ? params.at("w_q") : 0;
        Q(12, 12) = params.find("w_q") != params.end() ? params.at("w_q") : 0;

        Q_n(0, 0) = params.find("w_P_xy_n") != params.end() ? params.at("w_P_xy_n") : 0;
        Q_n(1, 1) = params.find("w_P_xy_n") != params.end() ? params.at("w_P_xy_n") : 0;
        Q_n(2, 2) = params.find("w_P_z_n") != params.end() ? params.at("w_P_z_n") : 0;
        Q_n(3, 3) = params.find("w_P_xy_sum_n") != params.end() ? params.at("w_P_xy_sum_n") : 0;
        Q_n(4, 4) = params.find("w_P_xy_sum_n") != params.end() ? params.at("w_P_xy_sum_n") : 0;
        Q_n(5, 5) = params.find("w_P_z_sum_n") != params.end() ? params.at("w_P_z_sum_n") : 0;
        Q_n(6, 6) = params.find("w_V_xy_n") != params.end() ? params.at("w_V_xy_n") : 0;
        Q_n(7, 7) = params.find("w_V_xy_n") != params.end() ? params.at("w_V_xy_n") : 0;
        Q_n(8, 8) = params.find("w_V_z_n") != params.end() ? params.at("w_V_z_n") : 0;
        Q_n(9, 9) = params.find("w_q_n") != params.end() ? params.at("w_q_n") : 0;
        Q_n(10, 10) = params.find("w_q_n") != params.end() ? params.at("w_q_n") : 0;
        Q_n(11, 11) = params.find("w_q_n") != params.end() ? params.at("w_q_n") : 0;
        Q_n(12, 12) = params.find("w_q_n") != params.end() ? params.at("w_q_n") : 0;

        R(0, 0) = params.find("w_T") != params.end() ? params.at("w_T") : 0;
        R(1, 1) = params.find("w_omega_x") != params.end() ? params.at("w_omega_x") : 0;
        R(2, 2) = params.find("w_omega_y") != params.end() ? params.at("w_omega_y") : 0;
        R(3, 3) = params.find("w_omega_z") != params.end() ? params.at("w_omega_z") : 0;

        mass = params.find("mass") != params.end() ? params.at("mass") : 1.0;
    }
    else if (this->type == "LMPC")
    {
        Q(0, 0) = params.find("w_P_xy") != params.end() ? params.at("w_P_xy") : 0;
        Q(1, 1) = params.find("w_P_xy") != params.end() ? params.at("w_P_xy") : 0;
        Q(2, 2) = params.find("w_P_z") != params.end() ? params.at("w_P_z") : 0;
        Q(3, 3) = params.find("w_P_xy_sum") != params.end() ? params.at("w_P_xy_sum") : 0;
        Q(4, 4) = params.find("w_P_xy_sum") != params.end() ? params.at("w_P_xy_sum") : 0;
        Q(5, 5) = params.find("w_P_z_sum") != params.end() ? params.at("w_P_z_sum") : 0;
        Q(6, 6) = params.find("w_V_xy") != params.end() ? params.at("w_V_xy") : 0;
        Q(7, 7) = params.find("w_V_xy") != params.end() ? params.at("w_V_xy") : 0;
        Q(8, 8) = params.find("w_V_z") != params.end() ? params.at("w_V_z") : 0;

        Q_n(0, 0) = params.find("w_P_xy_n") != params.end() ? params.at("w_P_xy_n") : 0;
        Q_n(1, 1) = params.find("w_P_xy_n") != params.end() ? params.at("w_P_xy_n") : 0;
        Q_n(2, 2) = params.find("w_P_z_n") != params.end() ? params.at("w_P_z_n") : 0;
        Q_n(3, 3) = params.find("w_P_xy_sum_n") != params.end() ? params.at("w_P_xy_sum_n") : 0;
        Q_n(4, 4) = params.find("w_P_xy_sum_n") != params.end() ? params.at("w_P_xy_sum_n") : 0;
        Q_n(5, 5) = params.find("w_P_z_sum_n") != params.end() ? params.at("w_P_z_sum_n") : 0;
        Q_n(6, 6) = params.find("w_V_xy_n") != params.end() ? params.at("w_V_xy_n") : 0;
        Q_n(7, 7) = params.find("w_V_xy_n") != params.end() ? params.at("w_V_xy_n") : 0;
        Q_n(8, 8) = params.find("w_V_z_n") != params.end() ? params.at("w_V_z_n") : 0;

        R(0, 0) = params.find("w_U_x") != params.end() ? params.at("w_U_x") : 0;
        R(1, 1) = params.find("w_U_y") != params.end() ? params.at("w_U_y") : 0;
        R(2, 2) = params.find("w_U_z") != params.end() ? params.at("w_U_z") : 0;

        mass = params.find("mass") != params.end() ? params.at("mass") : 1.0;
    }
}

Function MPC::setNmpcDynamicEquation()
{

    SX P_x = SX::sym("P_x");
    SX P_y = SX::sym("P_y");
    SX P_z = SX::sym("P_z");
    SX P_x_sum = SX::sym("P_x_sum");
    SX P_y_sum = SX::sym("P_y_sum");
    SX P_z_sum = SX::sym("P_z_sum");
    SX V_x = SX::sym("V_x");
    SX V_y = SX::sym("V_y");
    SX V_z = SX::sym("V_z");
    SX q_w = SX::sym("q_w");
    SX q_x = SX::sym("q_x");
    SX q_y = SX::sym("q_y");
    SX q_z = SX::sym("q_z");
    SX state_vars = SX::vertcat({P_x, P_y, P_z, P_x_sum, P_y_sum, P_z_sum, V_x, V_y, V_z, q_w, q_x, q_y, q_z});

    SX T = SX::sym("T");
    SX omega_x = SX::sym("omega_x");
    SX omega_y = SX::sym("omega_y");
    SX omega_z = SX::sym("omega_z");
    SX control_vars = SX::vertcat({T, omega_x, omega_y, omega_z});

    SX q_G_w = SX::sym("q_G_w");
    SX q_G_x = SX::sym("q_G_x");
    SX q_G_y = SX::sym("q_G_y");
    SX q_G_z = SX::sym("q_G_z");
    SX omega_G_x = SX::sym("omega_G_x");
    SX omega_G_y = SX::sym("omega_G_y");
    SX omega_G_z = SX::sym("omega_G_z");
    SX alpha_G_x = SX::sym("alpha_G_x");
    SX alpha_G_y = SX::sym("alpha_G_y");
    SX alpha_G_z = SX::sym("alpha_G_z");
    SX a_G_x = SX::sym("a_G_x");
    SX a_G_y = SX::sym("a_G_y");
    SX a_G_z = SX::sym("a_G_z");

    SX params = SX::vertcat({q_G_w, q_G_x, q_G_y, q_G_z, omega_G_x, omega_G_y, omega_G_z, alpha_G_x, alpha_G_y, alpha_G_z, a_G_x, a_G_y, a_G_z});

    SX rhs = SX::vertcat({V_x,
                          V_y,
                          V_z,
                          P_x,
                          P_y,
                          P_z,
                          (omega_G_z * omega_G_z + omega_G_y * omega_G_y) * P_x + (-omega_G_x * omega_G_y + alpha_G_z) * P_y + (-omega_G_x * omega_G_z - alpha_G_y) * P_z + 2 * (omega_G_z * V_y - omega_G_y * V_z) + 2 * (q_x * q_z + q_w * q_y) * T - 2 * (q_G_x * q_G_z - q_G_w * q_G_y) * 9.8 - a_G_x,
                          (-omega_G_x * omega_G_y - alpha_G_z) * P_x + (omega_G_x * omega_G_x + omega_G_z * omega_G_z) * P_y + (-omega_G_y * omega_G_z + alpha_G_x) * P_z + 2 * (-omega_G_z * V_x + omega_G_x * V_z) + 2 * (q_y * q_z - q_w * q_x) * T - 2 * (q_G_y * q_G_z + q_G_w * q_G_x) * 9.8 - a_G_y,
                          (-omega_G_x * omega_G_z + alpha_G_y) * P_x + (-omega_G_y * omega_G_z - alpha_G_x) * P_y + (omega_G_x * omega_G_x + omega_G_y * omega_G_y) * P_z + 2 * (omega_G_y * V_x - omega_G_x * V_y) + (1 - 2 * q_x * q_x - 2 * q_y * q_y) * T - (1 - 2 * q_G_x * q_G_x - 2 * q_G_y * q_G_y) * 9.8 - a_G_z,
                          0.5 * (-omega_x * q_x - omega_y * q_y - omega_z * q_z + omega_G_x * q_x + omega_G_y * q_y + omega_G_z * q_z),
                          0.5 * (omega_x * q_w + omega_z * q_y - omega_y * q_z - omega_G_x * q_w + omega_G_z * q_y - omega_G_y * q_z),
                          0.5 * (omega_y * q_w - omega_z * q_x + omega_x * q_z - omega_G_y * q_w - omega_G_z * q_x + omega_G_x * q_z),
                          0.5 * (omega_z * q_w + omega_y * q_x - omega_x * q_y - omega_G_z * q_w + omega_G_y * q_x - omega_G_x * q_y)});

    return Function("dynamic_equation", {state_vars, control_vars, params}, {rhs});
}

bool MPC::setNmpcSolver()
{
    Slice all;

    SX X = SX::sym("X", n_state, N + 1);
    SX U = SX::sym("U", n_input, N);
    SX P = SX::sym("P", n_state + n_state + n_param);

    SX obj = 0;
    SX constraints;

    // 参数P  39 x 1
    //     P[0:13)          P[13:26)          P[26:30)            P[30:33)            P[33:36)          P[36:39)
    // current_states    desired_states    q_G(w, x, y, z)    omega_G(x, y, z)    alpha_G(x, y, z)    a_G(x, y, z)

    constraints = X(all, 0) - P(Slice(0, 13));
    constraints = SX::reshape(constraints, -1, 1);

    // Function q_integration = test_q_integration();
    // Function PV_integration = test_PV_integration();
    Function dynamic_equation = setNmpcDynamicEquation();

    // casadi::Dict integrator_opts;
    // integrator_opts["dt"] = dt;
    // Function rk4_integrator = integrator("rk4_integrator", "rk4", dynamic_equation, integrator_opts);

    for (int i = 0; i < N; i++)
    {
        obj += SX::mtimes(SX::mtimes((X(all, i) - P(Slice(13, 26))).T(), Q), X(all, i) - P(Slice(13, 26)));
        obj += SX::mtimes(SX::mtimes(U(all, i).T(), R), U(all, i));

        /*
        vector<SX> input_q(3);
        input_q[0] = X(Slice(6, 10), i);
        input_q[1] = U(Slice(1, 4), i);
        input_q[2] = P(Slice(24, 27));

        vector<SX> input_PV(3);
        input_PV[0] = X(all, i);
        input_PV[1] = U(0, i);
        input_PV[2] = P(Slice(20, 33));

        SX dq = q_integration(input_q).at(0);
        SX dx = PV_integration(input_PV).at(0);

        SX X_next = SX::sym("X_next", n_state);
        X_next(Slice(0, 6)) = dx * dt + X(Slice(0, 6), i);
        X_next(6) = X(6, i) * exp(dq(0) * dt) - X(7, i) * exp(dq(1) * dt) - X(8, i) * exp(dq(2) * dt) - X(9, i) * exp(dq(3) * dt);
        X_next(7) = X(6, i) * exp(dq(1) * dt) + X(7, i) * exp(dq(0) * dt) + X(8, i) * exp(dq(3) * dt) - X(9, i) * exp(dq(2) * dt);
        X_next(8) = X(6, i) * exp(dq(2) * dt) - X(7, i) * exp(dq(3) * dt) + X(8, i) * exp(dq(0) * dt) + X(9, i) * exp(dq(1) * dt);
        X_next(9) = X(6, i) * exp(dq(3) * dt) + X(7, i) * exp(dq(2) * dt) - X(8, i) * exp(dq(1) * dt) + X(9, i) * exp(dq(0) * dt);
        */

        vector<SX> input(3);
        input[0] = X(all, i);
        input[1] = U(all, i);
        input[2] = P(Slice(26, 39));

        SX X_next = dynamic_equation(input).at(0) * dt + X(all, i);

        constraints = SX::vertcat({constraints, X_next - X(all, i + 1)});
    }

    obj += SX::mtimes(SX::mtimes((X(all, N) - P(Slice(13, 26))).T(), Q_n), (X(all, N) - P(Slice(13, 26))));

    SXDict qp = {{"x", SX::vertcat({SX::reshape(X, -1, 1), SX::reshape(U, -1, 1)})}, {"f", obj}, {"p", P}, {"g", constraints}};

    // string solver_name = "osqp";
    // string solver_name = "qpoases";
    string solver_name = "ipopt";

    // casadi::Dict solver_opts;
    solver_opts["expand"] = true;
    solver_opts["print_time"] = 0;

    if (solver_name == "ipopt")
    {
        // solver_opts["ipopt.max_iter"] = 50;
        solver_opts["ipopt.max_cpu_time"] = 0.02;
        solver_opts["ipopt.print_level"] = 0;
        solver_opts["ipopt.acceptable_tol"] = 1e-3;
        solver_opts["ipopt.acceptable_obj_change_tol"] = 1e-3;
        solver_opts["ipopt.linear_solver"] = "ma27";
    }
    else if (solver_name == "qpoases")
    {
        solver_opts["printLevel"] = "none";
        // solver_opts["nWSR"] = 50;
        solver_opts["CPUtime"] = 0.02;
        solver_opts["sparse"] = true;
    }
    else if (solver_name == "osqp")
    {
        // solver_opts["verbose"] = true;
        solver_opts["warm_start_primal"] = true;
    }

    // solver = qpsol("qpsol", solver_name, qp, solver_opts);
    solver = nlpsol("nlpsol", solver_name, qp, solver_opts);

    // solver = nlpsol("nlpsol", "ipopt", "/home/yx/catkin_ws/src/detector/src/code_gen/Nmpc_nlp.so");

    // Nmpc code gen
    /*
    solver.generate_dependencies("Nmpc_nlp.c", solver_opts);
    string compile_command = "gcc -fPIC -shared -O3 Nmpc_nlp.c -o Nmpc_nlp.so";
    cout << compile_command << endl;
    int compile_flag = std::system(compile_command.c_str());
    casadi_assert(compile_flag == 0, "Compilation failed");
    std::cout << "Compilation successed!" << std::endl;
    solver = nlpsol("nlpsol", "ipopt", "Nmpc_nlp.so");
    */

    lbg.clear();
    ubg.clear();
    lbx.clear();
    ubx.clear();

    for (int i = 0; i < N + 1; i++)
    {
        for (int j = 0; j < n_state; j++)
        {
            lbg.emplace_back(0.0);
            ubg.emplace_back(0.0);
        }
    }

    for (int i = 0; i < N + 1; i++)
    {
        // P_x casadi::inf
        lbx.emplace_back(-casadi::inf);
        ubx.emplace_back(casadi::inf);
        // P_y
        lbx.emplace_back(-casadi::inf);
        ubx.emplace_back(casadi::inf);
        // P_z
        lbx.emplace_back(-casadi::inf);
        ubx.emplace_back(casadi::inf);
        // P_x_sum
        lbx.emplace_back(-casadi::inf);
        ubx.emplace_back(casadi::inf);
        // P_y_sum
        lbx.emplace_back(-casadi::inf);
        ubx.emplace_back(casadi::inf);
        // P_z_sum
        lbx.emplace_back(-casadi::inf);
        ubx.emplace_back(casadi::inf);
        // V_x
        lbx.emplace_back(-2.0);
        ubx.emplace_back(2.0);
        // V_y
        lbx.emplace_back(-2.0);
        ubx.emplace_back(2.0);
        // V_z
        lbx.emplace_back(-2.0);
        ubx.emplace_back(2.0);
        // q_w
        lbx.emplace_back(-casadi::inf);
        ubx.emplace_back(casadi::inf);
        // q_x
        lbx.emplace_back(-casadi::inf);
        ubx.emplace_back(casadi::inf);
        // q_y
        lbx.emplace_back(-casadi ::inf);
        ubx.emplace_back(casadi::inf);
        // q_z
        lbx.emplace_back(-casadi::inf);
        ubx.emplace_back(casadi::inf);
    }

    for (int i = 0; i < N; i++)
    {
        // T
        lbx.emplace_back(T_min);
        ubx.emplace_back(T_max);
        // omega_x
        lbx.emplace_back(-omega_max);
        ubx.emplace_back(omega_max);
        // omega_y
        lbx.emplace_back(-omega_max);
        ubx.emplace_back(omega_max);
        // omega_z
        lbx.emplace_back(-omega_max);
        ubx.emplace_back(omega_max);
    }
    return true;
}

bool MPC::Nmpc_solve(const Eigen::Matrix<double, 13, 1> &current_states, const Eigen::Matrix<double, 13, 1> &desired_states, const Eigen::Vector4d &q_G, const Eigen::Vector3d &omega_G, const Eigen::Vector3d &alpha_G, const Eigen::Vector3d &a_G)
{
    vector<double> init_values;
    vector<double> control_params;
    vector<double> X_init((N + 1) * n_state, 0.0);
    vector<double> U_init(N * n_input, 0.0);

    vector<double> X_0{current_states(0), current_states(1), current_states(2), current_states(3), current_states(4), current_states(5), current_states(6), current_states(7), current_states(8), current_states(9), current_states(10), current_states(11), current_states(12)};
    vector<double> X_ref{desired_states(0), desired_states(1), desired_states(2), desired_states(3), desired_states(4), desired_states(5), desired_states(6), desired_states(7), desired_states(8), desired_states(9), desired_states(10), desired_states(11), desired_states(12)};
    vector<double> params{q_G(0), q_G(1), q_G(2), q_G(3), omega_G(0), omega_G(1), omega_G(2), alpha_G(0), alpha_G(1), alpha_G(2), a_G(0), a_G(1), a_G(2)};

    // cout << "params: " << params << endl;

    for (int i = 0; i < n_state; i++)
    {
        X_init[i] = current_states(i);
    }
    U_init[0] = last_T;
    U_init[1] = last_omega_x;
    U_init[2] = last_omega_y;
    U_init[3] = last_omega_z;

    // U_init[0] = 9.8;
    // U_init[1] = 0.0;
    // U_init[2] = 0.0;
    // U_init[3] = 0.0;

    // cout << "X_init: " << X_init << endl;
    // cout << "U_init: " << U_init << endl;
    // cout << "X_0: " << X_0 << endl;
    // cout << "X_N: " << X_N << endl;

    init_values.insert(init_values.end(), X_init.begin(), X_init.end());
    init_values.insert(init_values.end(), U_init.begin(), U_init.end());

    control_params.insert(control_params.end(), X_0.begin(), X_0.end());
    control_params.insert(control_params.end(), X_ref.begin(), X_ref.end());
    control_params.insert(control_params.end(), params.begin(), params.end());

    // cout << "init_values: " << init_values << endl;
    // cout << "control_params: " << control_params << endl;

    DMDict arg = {{"lbx", lbx}, {"ubx", ubx}, {"lbg", lbg}, {"ubg", ubg}, {"p", control_params}, {"x0", init_values}};
    res = solver(arg);

    return true;
}

vector<double> MPC::Nmpc_getFirstU()
{
    vector<double> result_x(res.at("x"));
    vector<double> vec;
    // cout << "result_x: " << result_x << endl;
    vec.assign(result_x.begin() + (N + 1) * n_state, result_x.begin() + (N + 1) * n_state + n_input);

    last_T = vec[0];
    last_omega_x = vec[1];
    last_omega_y = vec[2];
    last_omega_z = vec[3];

    return vec;
}

vector<double> MPC::Nmpc_getU()
{
    vector<double> result_x(res.at("x"));
    vector<double> vec;

    vec.assign(result_x.begin() + (N + 1) * n_state, result_x.end());

    last_T = vec[0];
    last_omega_x = vec[1];
    last_omega_y = vec[2];
    last_omega_z = vec[3];

    return vec;
}

/*
Function MPC::test_PV_integration()
{
    SX P_x = SX::sym("P_x");
    SX P_y = SX::sym("P_y");
    SX P_z = SX::sym("P_z");
    SX V_x = SX::sym("V_x");
    SX V_y = SX::sym("V_y");
    SX V_z = SX::sym("V_z");
    SX q_w = SX::sym("q_w");
    SX q_x = SX::sym("q_x");
    SX q_y = SX::sym("q_y");
    SX q_z = SX::sym("q_z");
    SX state_vars = SX::vertcat({P_x, P_y, P_z, V_x, V_y, V_z, q_w, q_x, q_y, q_z});

    SX T = SX::sym("T");
    SX control_vars = SX::vertcat({T});

    SX q_G_w = SX::sym("q_G_w");
    SX q_G_x = SX::sym("q_G_x");
    SX q_G_y = SX::sym("q_G_y");
    SX q_G_z = SX::sym("q_G_z");
    SX omega_G_x = SX::sym("omega_G_x");
    SX omega_G_y = SX::sym("omega_G_y");
    SX omega_G_z = SX::sym("omega_G_z");
    SX alpha_G_x = SX::sym("alpha_G_x");
    SX alpha_G_y = SX::sym("alpha_G_y");
    SX alpha_G_z = SX::sym("alpha_G_z");
    SX a_G_x = SX::sym("a_G_x");
    SX a_G_y = SX::sym("a_G_y");
    SX a_G_z = SX::sym("a_G_z");
    SX params = SX::vertcat({q_G_w, q_G_x, q_G_y, q_G_z, omega_G_x, omega_G_y, omega_G_z, alpha_G_x, alpha_G_y, alpha_G_z, a_G_x, a_G_y, a_G_z});

    SX rhs = SX::vertcat({
        V_x,
        V_y,
        V_z,
        (omega_G_z * omega_G_z + omega_G_y * omega_G_y) * P_x + (-omega_G_x * omega_G_y + alpha_G_z) * P_y + (-omega_G_x * omega_G_z - alpha_G_y) * P_z + 2 * (omega_G_z * V_y - omega_G_y * V_z) + 2 * (q_x * q_z + q_w * q_y) * T - 2 * (q_G_x * q_G_z - q_G_w * q_G_y) * 9.8 - a_G_x,
        (-omega_G_x * omega_G_y - alpha_G_z) * P_x + (omega_G_x * omega_G_x + omega_G_z * omega_G_z) * P_y + (-omega_G_y * omega_G_z + alpha_G_x) * P_z + 2 * (-omega_G_z * V_x + omega_G_x * V_z) + 2 * (q_y * q_z - q_w * q_x) * T - 2 * (q_G_y * q_G_z + q_G_w * q_G_x) * 9.8 - a_G_y,
        (-omega_G_x * omega_G_z + alpha_G_y) * P_x + (-omega_G_y * omega_G_z - alpha_G_x) * P_y + (omega_G_x * omega_G_x + omega_G_y * omega_G_y) * P_z + 2 * (omega_G_y * V_x - omega_G_x * V_y) + (1 - 2 * q_x * q_x - 2 * q_y * q_y) * T - (1 - 2 * q_G_x * q_G_x - 2 * q_G_y * q_G_y) * 9.8 - a_G_z,
    });

    return Function("q_integration", {state_vars, control_vars, params}, {rhs});
}

Function MPC::test_q_integration()
{
    SX q_w = SX::sym("q_w");
    SX q_x = SX::sym("q_x");
    SX q_y = SX::sym("q_y");
    SX q_z = SX::sym("q_z");
    SX state_vars = SX::vertcat({q_w, q_x, q_y, q_z});

    SX omega_x = SX::sym("omega_x");
    SX omega_y = SX::sym("omega_y");
    SX omega_z = SX::sym("omega_z");
    SX control_vars = SX::vertcat({omega_x, omega_y, omega_z});

    SX omega_G_x = SX::sym("omega_G_x");
    SX omega_G_y = SX::sym("omega_G_y");
    SX omega_G_z = SX::sym("omega_G_z");
    SX params = SX::vertcat({omega_G_x, omega_G_y, omega_G_z});

    SX rhs = SX::vertcat({0.5 * (-omega_x * q_x - omega_y * q_y - omega_z * q_z + omega_G_x * q_x + omega_G_y * q_y + omega_G_z * q_z),
                          0.5 * (omega_x * q_w + omega_z * q_y - omega_y * q_z - omega_G_x * q_w + omega_G_z * q_y - omega_G_y * q_z),
                          0.5 * (omega_y * q_w - omega_z * q_x + omega_x * q_z - omega_G_y * q_w - omega_G_z * q_x + omega_G_x * q_z),
                          0.5 * (omega_z * q_w + omega_y * q_x - omega_x * q_y - omega_G_z * q_w + omega_G_y * q_x - omega_G_x * q_y)});

    return Function("q_integration", {state_vars, control_vars, params}, {rhs});
}
*/

Function MPC::setLmpcDynamicEquation()
{

    SX P_x = SX::sym("P_x");
    SX P_y = SX::sym("P_y");
    SX P_z = SX::sym("P_z");
    SX P_x_sum = SX::sym("P_x_sum");
    SX P_y_sum = SX::sym("P_y_sum");
    SX P_z_sum = SX::sym("P_z_sum");
    SX V_x = SX::sym("V_x");
    SX V_y = SX::sym("V_y");
    SX V_z = SX::sym("V_z");
    SX state_vars = SX::vertcat({P_x, P_y, P_z, P_x_sum, P_y_sum, P_z_sum, V_x, V_y, V_z});

    SX U_x = SX::sym("U_x");
    SX U_y = SX::sym("U_y");
    SX U_z = SX::sym("U_z");
    SX control_vars = SX::vertcat({U_x, U_y, U_z});

    SX rhs = SX::vertcat({V_x,
                          V_y,
                          V_z,
                          P_x,
                          P_y,
                          P_z,
                          U_x,
                          U_y,
                          U_z});

    return Function("dynamic_equation", {state_vars, control_vars}, {rhs});
}

bool MPC::setLmpcSolver()
{
    Slice all;

    SX X = SX::sym("X", n_state, N + 1);
    SX U = SX::sym("U", n_input, N);
    SX P = SX::sym("P", n_state + n_state);

    SX obj = 0;
    SX constraints;

    constraints = X(all, 0) - P(Slice(0, 9));
    constraints = SX::reshape(constraints, -1, 1);

    cout << "Q: " << Q << endl;
    cout << "Q_n: " << Q_n << endl;
    cout << "R: " << R << endl;

    Function dynamic_equation = setLmpcDynamicEquation();
    for (int i = 0; i < N; i++)
    {
        obj += SX::mtimes(SX::mtimes((X(all, i) - P(Slice(9, 18))).T(), Q), (X(all, i) - P(Slice(9, 18))));
        obj += SX::mtimes(SX::mtimes(U(all, i).T(), R), U(all, i));

        vector<SX> input(2);
        input[0] = X(all, i);
        input[1] = U(all, i);
        auto X_next = dynamic_equation(input).at(0) * dt + X(all, i);
        constraints = SX::vertcat({constraints, X_next - X(all, i + 1)});
    }

    obj += SX::mtimes(SX::mtimes((X(all, N) - P(Slice(9, 18))).T(), Q_n), (X(all, N) - P(Slice(9, 18))));

    SXDict qp = {{"x", SX::vertcat({SX::reshape(X, -1, 1), SX::reshape(U, -1, 1)})}, {"f", obj}, {"p", P}, {"g", constraints}};

    string solver_name = "osqp";
    // string solver_name = "qpoases";
    casadi::Dict solver_opts;

    solver_opts["expand"] = true;
    solver_opts["print_time"] = 0;

    if (solver_name == "ipopt")
    {
        solver_opts["ipopt.max_iter"] = 50;
        solver_opts["ipopt.max_cpu_time"] = 0.02;
        solver_opts["ipopt.print_level"] = 0;
        solver_opts["ipopt.acceptable_tol"] = 1e-3;
        solver_opts["ipopt.acceptable_obj_change_tol"] = 1e-3;
        solver_opts["ipopt.linear_solver"] = "ma27";
    }
    else if (solver_name == "qpoases")
    {
        solver_opts["printLevel"] = "none";
        // solver_opts["nWSR"] = 50;
        solver_opts["CPUtime"] = 0.02;
        solver_opts["sparse"] = true;
    }
    else if (solver_name == "osqp")
    {
        // solver_opts["verbose"] = true;
        solver_opts["warm_start_primal"] = true;
    }

    solver = qpsol("qpsol", solver_name, qp, solver_opts);

    lbg.clear();
    ubg.clear();
    lbx.clear();
    ubx.clear();

    for (int i = 0; i < N + 1; i++)
    {
        for (int j = 0; j < n_state; j++)
        {
            lbg.emplace_back(0.0);
            ubg.emplace_back(0.0);
        }
    }

    for (int i = 0; i < N + 1; i++)
    {
        for (int j = 0; j < n_state; j++)
        {
            lbx.emplace_back(-casadi::inf);
            ubx.emplace_back(casadi::inf);
        }
    }

    for (int i = 0; i < N; i++)
    {
        lbx.emplace_back(-5.0);
        ubx.emplace_back(5.0);
        lbx.emplace_back(-5.0);
        ubx.emplace_back(5.0);
        lbx.emplace_back(-2.0);
        ubx.emplace_back(2.0);
    }
    return true;
}

bool MPC::Lmpc_solve(const Eigen::Matrix<double, 9, 1> &current_states, const Eigen::Matrix<double, 9, 1> &desired_states)
{
    vector<double> init_values;
    vector<double> control_params;
    vector<double> X_init((N + 1) * n_state, 0.0);
    vector<double> U_init(N * n_input, 0.0);

    vector<double> X_0{current_states[0], current_states[1], current_states[2], current_states[3], current_states[4], current_states[5], current_states[6], current_states[7], current_states[8]};
    vector<double> X_N{desired_states[0], desired_states[1], desired_states[2], desired_states[3], desired_states[4], desired_states[5], desired_states[6], desired_states[7], desired_states[8]};

    for (int i = 0; i < n_state; i++)
    {
        X_init[i] = current_states[i];
    }
    U_init[0] = last_U_x;
    U_init[1] = last_U_y;
    U_init[2] = last_U_z;

    // cout << "X_init: " << X_init << endl;
    // cout << "U_init: " << U_init << endl;
    // cout << "X_0: " << X_0 << endl;
    // cout << "X_N: " << X_N << endl;

    init_values.insert(init_values.end(), X_init.begin(), X_init.end());
    init_values.insert(init_values.end(), U_init.begin(), U_init.end());

    control_params.insert(control_params.end(), X_0.begin(), X_0.end());
    control_params.insert(control_params.end(), X_N.begin(), X_N.end());

    // std::merge(X_init.begin(), X_init.end(), U_init.begin(), U_init.end(), std::back_inserter(init_values));
    // std::merge(X_0.begin(), X_0.end(), X_N.begin(), X_N.end(), std::back_inserter(control_params));
    // cout << "init_values: " << init_values << endl;
    // cout << "control_params: " << control_params << endl;

    DMDict arg = {{"lbx", lbx}, {"ubx", ubx}, {"lbg", lbg}, {"ubg", ubg}, {"p", control_params}, {"x0", init_values}};
    res = solver(arg);

    return true;
}

vector<double> MPC::Lmpc_getFirstU()
{
    vector<double> result_x(res.at("x"));
    // cout << "result_x: " << result_x << endl;
    vector<double> vec;
    vec.assign(result_x.begin() + (N + 1) * n_state, result_x.begin() + (N + 1) * n_state + n_input);

    last_U_x = vec[0];
    last_U_y = vec[1];
    last_U_z = vec[2];

    return vec;
}