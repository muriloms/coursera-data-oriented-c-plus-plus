#include<memory>
#include<vector>
#include<numeric>
#include<functional>
#include<algorithm>
#include<iostream>
#include <random>
#include <chrono>
#include <ranges>

using namespace std;

struct Vec3 {
    float x;
    float y;
    float z;
};


/// PARTICLES WITH POINTERS ///////////////////////////////////////

struct Particle {
    shared_ptr<Vec3> position;
    shared_ptr<Vec3> velocity;
    shared_ptr<Vec3> accel;
    float mass;
};

shared_ptr<Particle> makeParticle1( mt19937& gen ) {
   uniform_real_distribution<float> dis(0.0, 10.0);
    return make_shared<Particle>(
        make_shared<Vec3>(dis(gen), dis(gen), dis(gen)),
        make_shared<Vec3>(dis(gen), dis(gen), dis(gen)),
        make_shared<Vec3>(dis(gen), dis(gen), dis(gen)),
        dis(gen)
    );
}

vector<shared_ptr<Particle>> makeNParticles1( size_t n, mt19937& gen  ) {
    vector<shared_ptr<Particle>> ps(n);
    generate( ps.begin(), ps.end(), [&gen](){ return makeParticle1(gen); });
    return ps;
}

/* Computes the squared norm of a Vec3 */
inline float norm2( const shared_ptr<Vec3>& v ) {
    return powf(v->x,2) + powf(v->y,2) + powf(v->z,2);
}

/* Computes the kinetic energy of a Single Particule */
float kineticEnergy( const shared_ptr<Particle> p ) {
    return 0.5f * p->mass * norm2(p->velocity);
}

/* Computes the total Kinetic energy of many particles */
float totalKineticEnergy(  const vector<shared_ptr<Particle>>& particles ) {
    float sum = 0.0;
    for( size_t i = 0; i < particles.size(); i++ ) {
        sum += kineticEnergy( particles[i] );
    }
    return sum;
}

/* Finds the left-most particle among a population */
float leftMost1( const vector<shared_ptr<Particle>>& particles ) {
    float minX = particles[1]->position->x;
    for( size_t i = 0; i < particles.size(); i++ ) {
        minX = min( minX, particles[i]->position->x);
    }
    return minX;
}

/* Apply a constant force to a single particule, updating velocity,
 * and position.
 * Here `dt` stands for delta-t, the time increment.
 */
void applyForce1_(shared_ptr<Particle> p, const Vec3& F, float dt ) {
    auto m = p->mass;
    p->accel->x += F.x/m;
    p->accel->y += F.y/m;
    p->accel->z += F.z/m;
    p->velocity->x += p->accel->x*dt;
    p->velocity->y += p->accel->y*dt;
    p->velocity->z += p->accel->z*dt;
    p->position->x += p->velocity->x*dt;
    p->position->y += p->velocity->y*dt;
    p->position->z += p->velocity->z*dt;
}

/* Applies a constant force to a population of particles (see above.) */
void applyForce1( vector<shared_ptr<Particle>>& particles,  const Vec3& F, float dt ) {
    for( size_t i = 0; i < particles.size(); i++ ) {
        applyForce1_(particles[i], F, dt);
    }
}

//// WITHOUT VECTORS /////////////////////////////////////////

struct Particle2 {
    Vec3 position;
    Vec3 velocity;
    Vec3 accel;
    float mass;
};


vector<Particle2> makeNParticles2( size_t n, mt19937& gen  ) {
   uniform_real_distribution<float> dis(0.0, 10.0);
    vector<Particle2> ps;
    generate_n( back_inserter(ps), n, [&gen,&dis](){
        return Particle2 {
        Vec3 { dis(gen),dis(gen),dis(gen) },
        Vec3 { dis(gen),dis(gen),dis(gen) },
        Vec3 { dis(gen),dis(gen),dis(gen) },
        dis(gen)
        };
    });
    return ps;
}


inline float norm2_2( const Vec3& v ) {
    return powf(v.x,2) + powf(v.y,2) * powf(v.z,2);
}

float kineticEnergy2( const Particle2& p ) {
    return 0.5f *  p.mass * norm2_2(p.velocity);
}

float totalKineticEnergy2(  const vector<Particle2>& particles ) {
    return transform_reduce(particles.begin(),
                            particles.end(),
                            0.f, plus<float>{},
                            [](auto particle){
                                return kineticEnergy2(particle);
                            });
}

float leftMost2( const vector<Particle2>& particles ) {
    return transform_reduce(particles.begin(),
                            particles.end(),
                            0.f, [](float x, float y){ return min(x,y); },
                            [](auto particle){
                                return particle.position.x;
                            });
}

void applyForce2_(Particle2& p, const Vec3& F, float dt ) {
    auto m = p.mass;
    p.accel.x += F.x/m;
    p.accel.y += F.y/m;
    p.accel.z += F.z/m;
    p.velocity.x += p.accel.x*dt;
    p.velocity.y += p.accel.y*dt;
    p.velocity.z += p.accel.z*dt;
    p.position.x += p.velocity.x*dt;
    p.position.y += p.velocity.y*dt;
    p.position.z += p.velocity.z*dt;
}
void applyForce2(vector<Particle2>& particles, const Vec3& F, float dt ) {
    for_each( particles.begin(), particles.end(), [&F,dt]( auto& p ) {
        applyForce2_(p,F,dt);
    });
}


/////////////////////////////////////////////

struct Particles3 {
    vector<Vec3> position;
    vector<Vec3> velocity;
    vector<Vec3> accel;
    vector<float> mass;
};

vector<Vec3> makeVectorVec3( size_t n, mt19937& gen ) {
   uniform_real_distribution<float> dis(0.0, 10.0);
   vector<Vec3> vec;
    generate_n( back_inserter(vec), n, [&gen,&dis](){
        return  Vec3 { dis(gen),dis(gen),dis(gen) };
    });
    return vec;
}

vector<float> makeVectorFloat( size_t n, mt19937& gen ) {
   uniform_real_distribution<float> dis(0.0, 10.0);
   vector<float> vec;
    generate_n( back_inserter(vec), n, [&gen,&dis](){
        return  dis(gen);
    });
    return vec;
}

Particles3 makeNParticles3( size_t n, mt19937& gen  ) {
   return Particles3 {
     makeVectorVec3(n, gen),
     makeVectorVec3(n, gen),
     makeVectorVec3(n, gen),
     makeVectorFloat(n, gen)
   };
}


float totalKineticEnergy3(  const Particles3& particles ) {
    return transform_reduce(particles.velocity.begin(),
                            particles.velocity.end(),
                            particles.mass.begin(),
                            0.f, plus<float>{},
                            [](auto v, auto m){
                                return  0.5 *  m * norm2_2(v);
                            });

}

float leftMost3( const Particles3& particles ) {
    return transform_reduce(particles.position.begin(),
                            particles.position.end(),
                            particles.position[0].x, [](float x, float y){ return min(x,y); },
                            [](auto pos){
                                return pos.x;
                            });
}

void applyForce3(Particles3& ps, const Vec3& F, float dt ) {
    ranges::for_each( views::iota(size_t{},ps.position.size()), [&ps,&F,dt]( auto i ) {
        auto m = ps.mass[i];
        ps.accel[i].x += F.x/m;
        ps.accel[i].y += F.y/m;
        ps.accel[i].z += F.z/m;
        ps.velocity[i].x += ps.accel[i].x*dt;
        ps.velocity[i].y += ps.accel[i].y*dt;
        ps.velocity[i].z += ps.accel[i].z*dt;
        ps.position[i].x += ps.velocity[i].x*dt;
        ps.position[i].y += ps.velocity[i].y*dt;
        ps.position[i].z += ps.velocity[i].z*dt;
    });
}


////////////////////////////////////////////////////////////////////////////////

struct Particles4 {
    vector<float> posx;
    vector<float> posy;
    vector<float> posz;
    vector<float> velx;
    vector<float> vely;
    vector<float> velz;
    vector<float> accx;
    vector<float> accy;
    vector<float> accz;
    vector<float> mass;
};

Particles4 makeNParticles4( size_t n, mt19937& gen  ) {
   return Particles4 {
     makeVectorFloat(n, gen),
     makeVectorFloat(n, gen),
     makeVectorFloat(n, gen),
     makeVectorFloat(n, gen),
     makeVectorFloat(n, gen),
     makeVectorFloat(n, gen),
     makeVectorFloat(n, gen),
     makeVectorFloat(n, gen),
     makeVectorFloat(n, gen),
     makeVectorFloat(n, gen)
   };
}

float totalKineticEnergy4(  const Particles4 particles ) {
    const size_t n = particles.posx.size();
    auto es = views::transform(  views::iota(size_t{},n),
                          [&particles](auto i){
                              auto v2 = powf(particles.velx[i],2)
                                  + powf(particles.vely[i],2)
                                  + powf(particles.velz[i],2);
                              return 0.5 * particles.mass[i] * v2;
                          });
    float eTot = 0.0;
    ranges::for_each( es, [&eTot]( auto e ){ eTot += e; } );
    return eTot;
}

float leftMost4( const Particles4& particles ) {
    return reduce( particles.posx.begin(), particles.posx.end(),
                   particles.posx[0],   [](float x, float y){ return min(x,y); });

}

void applyForce4(Particles4& ps, const Vec3& F, float dt ) {
    ranges::for_each( views::iota(size_t{},ps.posx.size()), [&ps,&F,dt]( auto i ) {
        auto m = ps.mass[i];
        ps.accx[i] += F.x/m;
        ps.accy[i] += F.y/m;
        ps.accz[i] += F.z/m;
        ps.velx[i] += ps.accx[i]*dt;
        ps.vely[i] += ps.accy[i]*dt;
        ps.velz[i] += ps.accz[i]*dt;
        ps.posx[i] += ps.velx[i]*dt;
        ps.posy[i] += ps.vely[i]*dt;
        ps.posz[i] += ps.velz[i]*dt;
    });
}


////////////////////////////////////////////////////////////////////////////////

int main() {
    const size_t N { 100000000 };
    std::mt19937 gen{1};
    const Vec3 F{0.0,0.0,-9.81};
    const float dt {0.01};
    {
    auto ps1 = makeNParticles1(N, gen);
    cout << "PS1[4].velocity.x=" << (ps1[4]->velocity->x) << endl;
    auto t0 = chrono::high_resolution_clock::now();
    //auto e1 = totalKineticEnergy(ps1);
    //auto e1 = leftMost1(ps1);
    applyForce1(ps1,F,dt);
    auto e1 = 0;
    auto t1 = chrono::high_resolution_clock::now();
    long long dt = std::chrono::duration_cast<chrono::milliseconds>(t1-t0).count();
    cout << "E1: " << e1 << "  duration: " << dt << " ms" << endl;
    }
    {
    auto ps2 = makeNParticles2(N, gen);
    cout << "PS2[4].velocity.x=" << (ps2[4].velocity.x) << endl;
    auto t2 = chrono::high_resolution_clock::now();
    //auto e2 = totalKineticEnergy2(ps2);
    //auto e2 = leftMost2(ps2);
    applyForce2(ps2,F,dt);
    auto e2 = 0;
    auto t3 = chrono::high_resolution_clock::now();
    long long dt = std::chrono::duration_cast<chrono::milliseconds>(t3-t2).count();
    cout << "E2: " << e2 << "  duration: " << dt << " ms" << endl;
    }{
        auto ps3 = makeNParticles3(N, gen);
    cout << "PS3[4].velocity.x=" << (ps3.velocity[4].x) << endl;
    auto t4 = chrono::high_resolution_clock::now();
    //auto e3 = totalKineticEnergy3(ps3);
    //auto e3 = leftMost3(ps3);
    applyForce3(ps3,F,dt);
    auto e3 = 0;
    auto t5 = chrono::high_resolution_clock::now();
    long long dt = std::chrono::duration_cast<chrono::milliseconds>(t5-t4).count();
    cout << "E3: " << e3 << "  duration: " << dt << " ms" << endl;
    }{
        auto ps4 = makeNParticles4(N, gen);
    cout << "PS4[4].velocity.x=" << (ps4.velx[4]) << endl;
    auto t6 = chrono::high_resolution_clock::now();
    //auto e4 = totalKineticEnergy4(ps4);
    //auto e4 = leftMost4(ps4);
    applyForce4(ps4,F,dt);
    auto e4 = 0;
    auto t7 = chrono::high_resolution_clock::now();
    long long dt = std::chrono::duration_cast<chrono::milliseconds>(t7-t6).count();
    cout << "E4: " << e4 << "  duration: " << dt << " ms" << endl;
    }






}
