#include <vector>
#include <algorithm>
#include <execution>
#include <iostream>
#include <array>
#include <random>
#include <cmath>
#include <string>
#include <ranges>
#include <fstream>
#include <iomanip>
#include <cassert>

using namespace std;

constexpr size_t N = 40;                 // The grid is of size N x N
constexpr size_t numParticles = N * N;   // Total number of particles
constexpr auto imageFreq = 200;          // 0 means no images
constexpr auto policy = execution::par;

// Numberical parameters in grid units (a grid cell has size 1x1,
// an iteration advances time by 1.
constexpr auto dt = 0.05f;               // Discrete time step
constexpr auto minDistance = 1.e-2f;     // Cutoff-distance for force computation
constexpr auto maxVel = 1.f;             // Cutoff value for velocity components
constexpr auto maxT = 150'000;           // Total number of time iterations
constexpr auto gravityFactor = -1.e-5f;  // The G * m1 * m2 force prefactor

using vec2 = array<float, 2>;


struct Particle {
    array<float,2> position;
    array<float,2> velocity;
    size_t identity;
};

// Computes a linear index for the grid cell to which a particle is attached.
// This is a simple round-down operation, because cells have side-length 1.
constexpr size_t indexOf(const array<float,2>& position) {
    // Because of round-off errors in the periodicity implementation, the
    // round-down to N-1 can erroneously produce N, this must be corrected for.
    size_t xPos = min(N - 1, (size_t)position[0]);
    size_t yPos = min(N - 1, (size_t)position[1]);
    return yPos + N * xPos;
}

// Compute a gravitational acceleration, which is proportional to 1/d^2
constexpr auto computeAcceleration(const vec2& pos1, const vec2& pos2) {
    auto vecd = vec2{pos2[0] - pos1[0], pos2[1] - pos1[1]};
    auto d = sqrt(pow(vecd[0], 2.f) + pow(vecd[1], 2.f));
    d = max(d, minDistance);
    // We implement here a cut-off distance of 1.
    auto inv_d3 = d < 1.f ? 1.f / pow(d, 3.f) : 0.f;
    auto acc = vec2{ gravityFactor * vecd[0] * inv_d3, gravityFactor * vecd[1] * inv_d3 };
    return acc;
}

// Integrate the position from the velocity using a Verlet scheme.
void updatePositions(auto& particles) {
    for_each(policy, begin(particles), end(particles), [](Particle& particle) {
        particle.position[0] += dt * particle.velocity[0];
        particle.position[1] += dt * particle.velocity[1];

        // All boundaries are periodic.
        if (particle.position[0] >= (float)N) {
            particle.position[0] -= N;
        }
        else if (particle.position[0] < 0.f) {
            particle.position[0] += N;
        }
        if (particle.position[1] >= (float)N) {
            particle.position[1] -= N;
        }
        else if (particle.position[1] < 0.f) {
            particle.position[1] += N;
        }
    } );
}

// To be executed after each change of particle positions. This function reattaches the particles
// to the appropriate grid cell. After the execution of this function, the particles are sorted
// according to their position on the grid, and each element of the grid contains the index of the
// first particle contained in the corresponding cell.
void computeGrid(auto& particles, auto& grid) {
    assert( grid.size() == N*N );
    // Sort the particles according to the linearized position in the grid.
    sort(policy, begin(particles), end(particles), [](const Particle& particle1, const Particle& particle2)
        {
            return indexOf(particle1.position) < indexOf(particle2.position);
        } );

    // 2. Update the grid vector.
    auto ids = views::iota(size_t{}, particles.size());
    for_each(policy, begin(ids), end(ids), [&particles, &grid](auto i) {
        // idx is the grid index of the current particle, and prevIdx the index of the previous
        // (in the order of the particle vector) particle. If idx != prevIdx, we need to register
        // the index i (possibly multiple times, to account for empty grid cells) in the grid vector.
        auto idx = indexOf(particles[i].position);
        auto prevIdx = i == 0 ? int64_t{-1} : (int64_t)indexOf(particles[i - 1].position);
        for (int64_t g = 0; g < (int64_t)(idx - prevIdx); ++g) {
#ifndef NDEBUG
            if (idx - g < 0 || idx - g >= grid.size()) {
                cout << "i = " << i << endl;
                cout << "idx = " << idx << ", prevIdx = " << prevIdx << endl;
                cout << "idx=" << idx << "; g=" << g << "; idx-g=" << (idx-g) << endl;
                assert( false );
            }
#endif
            grid[idx - g] = i;
        }
    } );
}

// For the correct computation of a distance between particles across
// periodic boundaries, correct a particle position according to the
// cell in which it was found.
constexpr vec2 makePeriodic(vec2 pos, int posX, int posY) {
    if (posX >= (int)N) {
        pos[0] += float{N};
    }
    else if (posX < 0) {
        pos[0] -= float{N};
    }
    if (posY >= (int)N) {
        pos[1] += float{N};
    }
    else if (posY < 0) {
        pos[1] -= float{N};
    }
    return pos;
}

// Compute the force between particles and update the particle velocities accordingly.
void applyAcceleration(auto& particles, const auto& grid) {
    for_each(policy, begin(particles), end(particles), [&particles, &grid](auto& particle) {
        // Compute the grid position of the current particle.
        auto iX = (int)particle.position[0];
        auto iY = (int)particle.position[1];
        // Due to the cut-off distance of 1, all interacting particles are either in the current
        // cell or in one of the eight neighbors. These 9 cells are traversed in the following nested loops.
        for (int nbX = -1; nbX <= 1; ++nbX) {
            for (int nbY = -1; nbY <= 1; ++nbY) {
                auto nbXPeriodic = (iX + nbX + N) % N;
                auto nbYPeriodic = (iY + nbY + N) % N;
                auto nb = (size_t)nbYPeriodic + N * (size_t)nbXPeriodic;

                // Loop over all particles contained in the considered cell.
                auto nbBegin = grid[nb];
                auto nbEnd = nb == grid.size() - 1 ? particles.size() : grid[nb + 1];
                for (auto nbI = nbBegin; nbI < nbEnd; ++nbI) {
                    if (particle.identity != particles[nbI].identity) {
                        auto nbPos = makePeriodic(particles[nbI].position, iX + nbX, iY + nbY);
                        auto a = computeAcceleration(particle.position, nbPos);
                        // Integrate the particle velocity.
                        particle.velocity[0] += dt * a[0];
                        particle.velocity[1] += dt * a[1];
                    }
                }
            }
        }
        // For numerical stability reasons, apply a cut-off to the particle velocity.
        particle.velocity[0] = min(particle.velocity[0], maxVel);
        particle.velocity[1] = min(particle.velocity[1], maxVel);

    } );
}

// Generate the initial particles at random positions with zero velocity.
vector<Particle> generateParticles(size_t numParticles) {
    auto generator = mt19937{random_device{}()};
    auto dis = uniform_real_distribution<float>{0.f, float{N}};
    auto particles = vector<Particle>(numParticles);
    auto ids = views::iota(size_t{}, particles.size());
    for_each(begin(ids), end(ids), [&particles,&generator,&dis](auto i) {
            particles[i] = Particle {
                .position = vec2{dis(generator), dis(generator)},
                .velocity = vec2{},
                .identity = i };
    } );
    return particles;
}

// Write the particle positions to a text file.
void writeParticlePositions(const vector<Particle>& particles, string fname) {
    ofstream ofile(fname.c_str());
    for (auto particle: particles) {
        ofile << setprecision(6) << setw(15) << particle.position[0]
              << setprecision(6) << setw(15) << particle.position[1]
              << "\n";
    }
}

int main() {
    auto grid = vector<size_t>(N * N);
    auto particles = generateParticles(numParticles);

    auto start_time = chrono::steady_clock::now();
    int im = 0;
    for (int t = 0; t < maxT; ++t) {
        computeGrid(particles, grid);
        applyAcceleration(particles, grid);
        updatePositions(particles);
        if (imageFreq > 0 && t % imageFreq == 0) {
            writeParticlePositions(particles, "pos_" + to_string(im++) + ".txt");
        }
    }
    auto end_time = chrono::steady_clock::now();
    auto interval = chrono::duration_cast<chrono::microseconds>(end_time - start_time).count();
    auto megaParticlePerSecond = (double)maxT * (double)numParticles / interval;
    cout << "Elapsed: " << interval * 1e-6 << "s" << endl;
    cout << "Efficiency: " << megaParticlePerSecond << " Mega-Particles-per-second" << endl;
}
