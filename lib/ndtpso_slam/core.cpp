#include "ndtpso_slam/core.h"
#include "ndtpso_slam/ndtframe.h"
#include <iostream>
#include <omp.h>

using std::cout;

struct Particle {
    Vector3d position, velocity, best_position;
    double best_cost;
    double cost;

    Particle(Array3d mean, Array3d deviation, NdtFrame* const ref_frame, NdtFrame* const new_frame)
    {
        position = mean + (Array3d::Random() * deviation); // Randomly place the particles according to mean and deviation
        //        std::cout << Array3d::Random() * deviation;
        //        std::cout << std::endl;
        //        std::cout << deviation;
        //        std::cout << std::endl;

        velocity << 0, 0, 0;

        cost = cost_function(position, ref_frame, new_frame, 100);

        best_position = position;
        best_cost = cost;
    }
};

Vector3d pso_optimization(Vector3d initial_guess, NdtFrame* const ref_frame, NdtFrame* const new_frame, unsigned int iters_num)
{
    //    def pso(mean, ref_frame, new_frame, iters=25):
    double w = 1., w_damping_coef = .4, c1 = 2., c2 = 2.;
    unsigned short num_of_particles = PSO_POPULATION_SIZE;
    Array3d deviation;
    deviation << .1, .1, M_PI / 10.;

    vector<Particle> particles;
    unsigned int global_best_index = 0;

    for (unsigned int i = 0; i < num_of_particles; ++i) {
        particles.push_back(Particle(initial_guess.array(), deviation, ref_frame, new_frame));

        if (particles[i].cost < particles[global_best_index].cost) {
            global_best_index = i;
        }
    }

    for (unsigned int i = 0; i < iters_num; ++i) {
        omp_set_num_threads(omp_get_max_threads());
#pragma omp parallel for schedule(static, 1)
        for (unsigned int j = 0; j < num_of_particles; ++j) {
            for (unsigned int k = 0; k < 3; ++k) {
                Array2d random_coef = Array2d::Random().abs();
                particles[j].velocity[k] = w * particles[j].velocity[k]
                    + c1 * random_coef[0] * (particles[j].best_position[k] - particles[j].position[k])
                    + c2 * random_coef[1] * (particles[global_best_index].position[k] - particles[j].position[k]);

                particles[j].position[k] = particles[j].position[k] + particles[j].velocity[k];
            }

            particles[j].cost = cost_function(particles[j].position, ref_frame, new_frame);

            if (particles[j].cost < particles[j].best_cost) {
                particles[j].best_cost = particles[j].cost;
                particles[j].best_position = particles[j].position;
            }
#pragma omp critical
            if (particles[j].cost < particles[global_best_index].cost) {
                global_best_index = j;
            }
        }

        w *= w_damping_coef;
    }

    cout << "Global Best Cost (PSO) ";
    cout << particles[global_best_index].best_cost;
    cout << std::endl;
    return particles[global_best_index].best_position;
}

Vector3d glir_pso_optimization(Vector3d initial_guess, NdtFrame* const ref_frame, NdtFrame* const new_frame, unsigned int iters_num)
{
    //    def pso(mean, ref_frame, new_frame, iters=25):
    double omega = 1., c1 = 2., c2 = 2.;
    unsigned short num_of_particles = PSO_POPULATION_SIZE;
    Array3d deviation;
    deviation << 1., 1., M_PI / 3.;

    vector<Particle> particles;
    unsigned int global_best_index = 0;

    for (unsigned int i = 0; i < num_of_particles; ++i) {
        particles.push_back(Particle(initial_guess.array(), deviation, ref_frame, new_frame));

        if (particles[i].cost < particles[global_best_index].cost) {
            global_best_index = i;
        }
    }

    for (unsigned int i = 0; i < iters_num; ++i) {
        double pbest_avr = .0;

        //        omp_set_num_threads(omp_get_max_threads());
        //#pragma omp parallel for
        for (unsigned int j = 0; j < num_of_particles; ++j) {
            c1 = c2 = 1.0 + particles[global_best_index].cost / particles[j].best_cost;
            for (unsigned int k = 0; k < 3; ++k) {
                Array2d random_coef = Array2d::Random().abs();
                particles[j].velocity[k] = omega * particles[j].velocity[k]
                    + c1 * random_coef[0] * (particles[j].best_position[k] - particles[j].position[k])
                    + c2 * random_coef[1] * (particles[global_best_index].position[k] - particles[j].position[k]);

                particles[j].position[k] = particles[j].position[k] + particles[j].velocity[k];
            }

            particles[j].cost = cost_function(particles[j].position, ref_frame, new_frame);
            pbest_avr += particles[j].cost;

            if (particles[j].cost < particles[j].best_cost) {
                particles[j].best_cost = particles[j].cost;
                particles[j].best_position = particles[j].position;
            }
            //#pragma omp barrer
            if (particles[j].cost < particles[global_best_index].cost) {
                global_best_index = j;
            }
        }
        omega = 1.1 - particles[global_best_index].cost / pbest_avr;
    }

    cout << "Global Best Cost (GLIR PSO) ";
    cout << particles[global_best_index].best_cost;
    cout << std::endl;
    return particles[global_best_index].best_position;
}
