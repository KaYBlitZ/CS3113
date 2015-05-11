#include "BloodSystem.h"

using namespace std;

BloodEmitter::BloodEmitter() : remove(true) {}

BloodEmitter::BloodEmitter(Vector position) : position(position), remove(false) {
	for (unsigned int i = 0; i < Constants::BLOOD_NUM_PARTICLES; i++) {
		particles.push_back(
			BloodParticle(position,
			Vector((rand() % 5) * 0.1f, (rand() % 5) * 0.1f, (rand() % 5) * 0.1f, 0.0f),
			(rand() % 10 + 5) / 10.0f)
			);
	}
}

BloodEmitter::BloodEmitter(const BloodEmitter &rhs) : position(rhs.position), remove(rhs.remove) {
	for (unsigned int i = 0; i < Constants::BLOOD_NUM_PARTICLES; i++) {
		particles.push_back(
			BloodParticle(position,
			Vector((rand() % 5) * 0.2f, (rand() % 5) * 0.2f, (rand() % 5) * 0.2f, 0.0f),
			(rand() % 10 + 5) / 10.0f)
			);
	}
}

void BloodEmitter::fixedUpdate() {
	bool isDone = true;

	for (BloodParticle &particle : particles) {
		particle.velocity.y -= Constants::BLOOD_GRAVITY * Constants::FIXED_TIMESTEP;

		particle.position.x += particle.velocity.x * Constants::FIXED_TIMESTEP;
		particle.position.y += particle.velocity.y * Constants::FIXED_TIMESTEP;
		particle.position.z += particle.velocity.z * Constants::FIXED_TIMESTEP;

		particle.lifeTime += Constants::FIXED_TIMESTEP;

		float time = particle.lifeTime / particle.maxLifeTime;

		if (lerp(1.0f, 0.0f, time) > 0.0f) isDone = false;
	}

	remove = isDone;
}

void BloodEmitter::render() {
	vector<float> particleVertices;
	vector<float> particleColors;

	for (BloodParticle &particle : particles) {
		particleVertices.push_back(particle.position.x);
		particleVertices.push_back(particle.position.y);
		particleVertices.push_back(particle.position.z);

		particleColors.push_back(1.0f);
		particleColors.push_back(0.0f);
		particleColors.push_back(0.0f);
		particleColors.push_back(lerp(1.0f, 0.0f, particle.lifeTime / particle.maxLifeTime));
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);

	glPointSize(5.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, particleVertices.data());

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, particleColors.data());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_POINTS, 0, particleVertices.size() / 3);

	glDisable(GL_BLEND);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
}

float BloodEmitter::lerp(float v0, float v1, float t) {
	return (1.0f - t) * v0 + t * v1;
}

BloodParticle::BloodParticle(Vector position, Vector velocity, float maxLifeTime) : 
position(position), velocity(velocity), lifeTime(0.0f), maxLifeTime(maxLifeTime) {}