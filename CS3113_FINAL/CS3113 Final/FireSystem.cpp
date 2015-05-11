#include "FireSystem.h"

using namespace std;
using namespace Constants;

FireEmitter::FireEmitter(Vector position, float width, float height, GLuint fire) : Billboard(position.x, position.y, position.z), width(width), height(height), fire(fire) {
	for (size_t i = 0; i < FIRE_NUM_PARTICLES; i++) {
		particles.push_back(FireParticle(Vector(), 
			Vector(rand() % 5 / 10.0f - 0.5f, rand() % 10 / 10.0f + 1.0f, rand() % 5 / 10.0f - 0.5f, 0.0f),
			rand() % 10 / 10.0f + 0.5f));
	}
}

void FireEmitter::fixedUpdate() {
	for (FireParticle &particle : particles) {
		particle.lifeTime += FIXED_TIMESTEP;

		particle.position.x += particle.velocity.x * FIXED_TIMESTEP;
		particle.position.y += particle.velocity.y * FIXED_TIMESTEP;
		particle.position.z += particle.velocity.z * FIXED_TIMESTEP;

		particle.alpha = lerp(1.0f, 0.0f, particle.lifeTime / particle.maxLifeTime);

		if (particle.alpha < 0) {
			particle.position = Vector();
			particle.velocity = Vector(rand() % 5 / 10.0f - 0.5f, rand() % 10 / 10.0f + 1.0f, rand() % 5 / 10.0f - 0.5f, 0.0f);
			particle.lifeTime = 0.0f;
			particle.maxLifeTime = rand() % 10 / 10.0f + 0.5f;
		}
	}
}

void FireEmitter::render() {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glMultMatrixf(matrix.ml);

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//glEnable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_GREATER, 0.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fire);

	vector<GLfloat> vertices;
	vector<GLfloat> colors;
	vector<GLfloat> texCoords;
	vector<GLuint> indices;

	float halfWidth = width / 2;
	float halfHeight = height / 2;

	for (size_t i = 0; i < FIRE_NUM_PARTICLES; i++) {
		FireParticle &particle = particles[i];
		Vector &position = particle.position;

		vertices.insert(vertices.end(), { position.x - halfWidth, position.y + halfHeight, position.x - halfWidth, position.y - halfHeight,
			position.x + halfWidth, position.y - halfHeight, position.x + halfWidth, position.y + halfHeight });

		colors.insert(colors.end(), { 1.0f, 1.0f, 1.0f, particle.alpha, 1.0f, 1.0f, 1.0f, particle.alpha, 1.0f, 1.0f, 1.0f, particle.alpha, 1.0f, 1.0f, 1.0f, particle.alpha });
		texCoords.insert(texCoords.end(), { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f });

		GLuint index = i * 4;
		indices.insert(indices.end(), { index, index + 1, index + 2, index, index + 2, index + 3 });
	}

	//GLubyte indices[] = { 0, 1, 2, 0, 2, 3 };

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertices.data());

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, colors.data());

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoords.data());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); // additive blending

	glDrawElements(GL_TRIANGLES, 6 * FIRE_NUM_PARTICLES, GL_UNSIGNED_INT, indices.data());

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	//glDisable(GL_ALPHA_TEST);
	//glDisable(GL_DEPTH_TEST);

	glPopMatrix();
}

float FireEmitter::lerp(float v0, float v1, float t) {
	return (1.0f - t) * v0 + t * v1;
}

FireParticle::FireParticle(Vector position, Vector velocity, float maxLifeTime) : position(position), velocity(velocity), lifeTime(0.0f), maxLifeTime(maxLifeTime), alpha(1.0f) {}