#include "Fluid.h"

// Sub-engines
#include "RenderingEngine.h"

// Objects
#include "GameObject.h"

// Components
#include "Transform.h"
#include "Camera.h"

// Structs
#include "Ray.h"

// Debugging
#include "Gizmos.h"

// Utilities
#include "GLFW_Header.h"
#include "Time.h"
#include "Input.h"

Fluid::Fluid(int _width, int _height, float _viscocity, float _cellDist) {
	width = _width;
	height = _height;
	viscocity = _viscocity;
	cellDist = _cellDist;
	cellCount = _width * _height;
}
Fluid::~Fluid(){}
bool Fluid::Startup() {
	frontCells.velocity = new glm::vec2[cellCount];
	frontCells.dyeColor = new glm::vec3[cellCount];
	frontCells.pressure = new float[cellCount];
	backCells.velocity = new glm::vec2[cellCount];
	backCells.dyeColor = new glm::vec3[cellCount];
	backCells.pressure = new float[cellCount];
	divergence = new float[cellCount];

	memset(frontCells.velocity, 0, sizeof(glm::vec2) * cellCount);
	memset(frontCells.dyeColor, 0, sizeof(glm::vec3) * cellCount);
	memset(frontCells.pressure, 0, sizeof(float)* cellCount);

	memset(backCells.velocity, 0, sizeof(glm::vec2) * cellCount);
	memset(backCells.dyeColor, 0, sizeof(glm::vec3) * cellCount);
	memset(backCells.pressure, 0, sizeof(float)* cellCount);

	memset(divergence, 0, sizeof(float)* cellCount);

	for (int i = 0; i < cellCount; ++i) {
		frontCells.dyeColor[i] = glm::vec3(100, 20, 50);
		float x = (float)(i % width);
		float y = (float)(i / width);
		frontCells.dyeColor[i] = glm::vec3(x, y, 0);
		frontCells.pressure[i] = 1;
	}

	return true;
}
void Fluid::Shutdown() {
	//Front Cells
	delete[] frontCells.velocity;
	delete[] frontCells.dyeColor;
	delete[] frontCells.pressure;
	//Back Cells
	delete[] backCells.velocity;
	delete[] backCells.dyeColor;
	delete[] backCells.pressure;
	//Divergence
	delete[] divergence;
}
bool Fluid::Update() {
	Advect(Time::deltaTime);
	SwapVelocities();
	SwapColors();

	for (int diffuseStep = 0; diffuseStep < 50; ++diffuseStep) {
		Diffuse(Time::deltaTime);
		SwapVelocities();
	}
	Divergence(Time::deltaTime);
	for (int pressureStep = 0; pressureStep < 60; ++pressureStep) {
		UpdatePressure(Time::deltaTime);
		SwapPressures();
	}

	ApplyPressure(Time::deltaTime);
	SwapVelocities();

	UpdateBoundary();

	int boxSize = 10;
	int halfBoxSize = boxSize / 2;

	for (int x = width / 2 - halfBoxSize; x < width / 2 + halfBoxSize; ++x) {
		for (int y = 5; y < 5 + boxSize; ++y) {
			int cellIndex = x + y * width;
			frontCells.velocity[cellIndex].y += 10 * Time::deltaTime;
		}
	}

	if (Input::GetMouseButton(GLFW_MOUSE_BUTTON_1)) {
		MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
		if (meshRenderer != nullptr) {
			Ray mouseRay = Camera::current->ScreenPointToRay(Input::GetMousePosition());
			RaycastHit hit;
			if (meshRenderer->bounds.RayCast(mouseRay, &hit, 1000.0f)) {
				vec3 bottomCorner = transform->position - meshRenderer->bounds.size;
				vec2 relativePos = vec2(hit.point.x - bottomCorner.x,
					hit.point.z - bottomCorner.z);

				vec2 scale = vec2((width / 2.0f) / meshRenderer->bounds.size.x,
					(height / 2.0f) / meshRenderer->bounds.size.z);

				vec2 point = relativePos * scale;

				int x = (int)point.x;
				int y = (int)point.y;

				int cellIndex = x + y * width;
				frontCells.dyeColor[cellIndex] = vec3(0, 255, 0);
			}
		}
	}

	return true;
}
void Fluid::Draw(RenderingEngine& _renderer) {
	//Allocate space for texture data
	unsigned char* texData = new unsigned char[(width * height) * 3];

	//Fill texture data with dye colors
	for (int i = 0; i < width * height; ++i) {
		texData[i * 3 + 0] = (unsigned char)frontCells.dyeColor[i].r;
		texData[i * 3 + 1] = (unsigned char)frontCells.dyeColor[i].g;
		texData[i * 3 + 2] = (unsigned char)frontCells.dyeColor[i].b;
	}

	Texture::RemoveTexture("fluidTexture");
	Texture newTexture = Texture(width, height, texData, "fluidTexture",
		GL_TEXTURE_2D, GL_NEAREST, GL_RGB, GL_RGB);

	//Render texture using OpenGL
	MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
	if (meshRenderer != nullptr) {
		meshRenderer->materials.front().SetTexture("diffuse", newTexture);
	}

	//Delete texture
	delete[] texData;
}
void Fluid::Advect(float _deltaTime) {
	//Loop over every cell
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			//Find the point to sample for this cell
			int cellIndex = x + y * width;
			vec2 vel = frontCells.velocity[cellIndex] * _deltaTime;
			vec2 samplePoint = glm::vec2((float)x - vel.x / cellDist,
				(float)y - vel.y / cellDist);

			samplePoint.x = glm::clamp(samplePoint.x, 0.0f, (float)width - 1.1f);
			samplePoint.y = glm::clamp(samplePoint.y, 0.0f, (float)height - 1.1f);

			//Compute bilerp for the point
			vec2 bl = glm::floor(samplePoint);
			vec2 br = bl + vec2(1, 0);
			vec2 tl = bl + vec2(0, 1);
			vec2 tr = bl + vec2(1, 1);

			//Bottom left Index
			int bli = (int)bl.x + width * (int)bl.y;
			int bri = (int)br.x + width * (int)br.y;
			int tli = (int)tl.x + width * (int)tl.y;
			int tri = (int)tr.x + width * (int)tr.y;

			vec2 sampleFract = samplePoint - bl;

			//Actually advecting
			//Color 
			vec3 dyeB = glm::mix(frontCells.dyeColor[bli],
				frontCells.dyeColor[bri],
				sampleFract.x);
			vec3 dyeT = glm::mix(frontCells.dyeColor[tli],
				frontCells.dyeColor[tri],
				sampleFract.x);

			vec3 newDye = glm::mix(dyeB, dyeT, sampleFract.y);
			backCells.dyeColor[cellIndex] = newDye;

			//Velocity
			vec2 velB = glm::mix(frontCells.velocity[bli],
				frontCells.velocity[bri],
				sampleFract.x);
			vec2 velT = glm::mix(frontCells.velocity[tli],
				frontCells.velocity[tri],
				sampleFract.x);

			vec2 newVel = glm::mix(velB, velT, sampleFract.y);
			backCells.velocity[cellIndex] = newVel;
		}
	}
}
void Fluid::Diffuse(float _deltaTime) {
	float invViscosityDeltaTime = 1.0f / (viscocity * _deltaTime);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int cellIndex = x + y * width;
			// Gather the 4 velocities around us
				
			// x plus 1
			// x minus 1
			int xp1 = glm::clamp(x + 1, 0, width - 1);
			int xm1 = glm::clamp(x - 1, 0, width - 1);
			int yp1 = glm::clamp(y + 1, 0, height - 1);
			int ym1 = glm::clamp(y - 1, 0, height - 1);

			int up = x + yp1 * width;
			int down = x + ym1 * width;
			int right = xp1 + y * width;
			int left = xm1 + y * width;

			vec2 velUp = frontCells.velocity[up];
			vec2 velDown = frontCells.velocity[down];
			vec2 velLeft = frontCells.velocity[left];
			vec2 velRight = frontCells.velocity[right];

			vec2 velCenter = frontCells.velocity[cellIndex];

			// Put in equation
			float denom = 1.0f / (4 + invViscosityDeltaTime);

			vec2 diffusedVel = (velUp + velRight + velDown +
				velLeft + velCenter * invViscosityDeltaTime) * denom;

			backCells.velocity[cellIndex] = diffusedVel;
		}
	}
}
void Fluid::Divergence(float _deltaTime) {
	float invCellDist = 1.0f / (2.0f * cellDist);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int cellIndex = x + y * width;

			// x minus 1
			int xp1 = glm::clamp(x + 1, 0, width - 1);
			int xm1 = glm::clamp(x - 1, 0, width - 1);
			int yp1 = glm::clamp(y + 1, 0, height - 1);
			int ym1 = glm::clamp(y - 1, 0, height - 1);

			int up = x + yp1 * width;
			int down = x + ym1 * width;
			int right = xp1 + y * width;
			int left = xm1 + y * width;

			float velUp = frontCells.velocity[up].y;
			float velDown = frontCells.velocity[down].y;
			float velLeft = frontCells.velocity[left].x;
			float velRight = frontCells.velocity[right].x;

			float divergence = ((velRight - velLeft) + (velUp - velDown)) * invCellDist;

			this->divergence[cellIndex] = divergence;
		}
	}
}
void Fluid::UpdatePressure(float _deltaTime) {
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int cellIndex = x + y * width;
			// x plus 1
			// x minus 1
			int xp1 = glm::clamp(x + 1, 0, width - 1);
			int xm1 = glm::clamp(x - 1, 0, width - 1);
			int yp1 = glm::clamp(y + 1, 0, height - 1);
			int ym1 = glm::clamp(y - 1, 0, height - 1);

			int up = x + yp1 * width;
			int down = x + ym1 * width;
			int right = xp1 + y * width;
			int left = xm1 + y * width;

			float pUp = frontCells.pressure[up];
			float pDown = frontCells.pressure[down];
			float pLeft = frontCells.pressure[left];
			float pRight = frontCells.pressure[right];

			float d = divergence[cellIndex];

			float newPressure = (pUp + pDown + pLeft + pRight -
				d * cellDist * cellDist) * 0.25f;

			backCells.pressure[cellIndex] = newPressure;
		}
	}
}
void Fluid::ApplyPressure(float _deltaTime) {
	float invCellDist = 1.0f / (2.0f * cellDist);

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			int cellIndex = x + y * width;

			// x plus 1
			// x minus 1
			int xp1 = glm::clamp(x + 1, 0, width - 1);
			int xm1 = glm::clamp(x - 1, 0, width - 1);
			int yp1 = glm::clamp(y + 1, 0, height - 1);
			int ym1 = glm::clamp(y - 1, 0, height - 1);

			int up = x + yp1 * width;
			int down = x + ym1 * width;
			int left = xm1 + y * width;
			int right = xp1 + y * width;

			float pUp = frontCells.pressure[up];
			float pDown = frontCells.pressure[down];
			float pLeft = frontCells.pressure[left];
			float pRight = frontCells.pressure[right];

			vec2 deltaV = -vec2(pRight - pLeft, pUp - pDown) * invCellDist;

			backCells.velocity[cellIndex] =
				frontCells.velocity[cellIndex] + deltaV;
		}
	}
}
void Fluid::UpdateBoundary() {
	for (int x = 0; x < width; ++x) {
		int first_row_index = x;
		int second_row_index = x + width;

		//p = pressure
		//v = velocity

		float* p = frontCells.pressure;
		glm::vec2* v = frontCells.velocity;

		p[first_row_index] = p[second_row_index];
		v[first_row_index].x = v[second_row_index].x;
		v[first_row_index].y = -v[second_row_index].y;

		int last_row_index = x + (height - 1) * width;
		int second_last_row_index = x + (height - 2) * width;

		p[last_row_index] = p[second_last_row_index];
		v[last_row_index].x = v[second_last_row_index].x;
		v[last_row_index].y = -v[second_last_row_index].y;
	}

	for (int y = 0; y < height; ++y) {
		int first_col_index = 0 + y * width;
		int second_col_index = 1 + y * width;

		//p = pressure
		//v = velocity
		float* p = frontCells.pressure;
		glm::vec2* v = frontCells.velocity;

		p[first_col_index] = p[second_col_index];
		v[first_col_index].x = -v[second_col_index].x;
		v[first_col_index].y = v[second_col_index].y;

		int last_col_index = (width - 1) + y * width;
		int second_last_col_index = (width - 2) + y * width;

		p[last_col_index] = p[second_last_col_index];
		v[last_col_index].x = -v[second_last_col_index].x;
		v[last_col_index].y = v[second_last_col_index].y;
	}
}
void Fluid::SwapVelocities() {
	glm::vec2* temp = frontCells.velocity;
	frontCells.velocity = backCells.velocity;
	backCells.velocity = temp;
}
void Fluid::SwapColors() {
	glm::vec3* temp = frontCells.dyeColor;
	frontCells.dyeColor = backCells.dyeColor;
	backCells.dyeColor = temp;
}
void Fluid::SwapPressures() {
	float* temp = frontCells.pressure;
	frontCells.pressure = backCells.pressure;
	backCells.pressure = temp;
}
