#pragma once
#include <SDL2/SDL.h>
#include "Constants.h"
#include <glm/glm.hpp>
#include <iostream>

void handleKeys() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			/* Look for a keypress */
		case SDL_KEYDOWN:
			/* Check the SDLKey values and move change the coords */
			switch (event.key.keysym.sym) {
			case SDLK_w:
				front = glm::normalize(lookAt - cameraPosition) * dt;
				cameraPosition = cameraPosition + front;
				lookAt = lookAt + front;
				break;
			case SDLK_s:
				front = glm::normalize(lookAt - cameraPosition) * dt;
				cameraPosition = cameraPosition - front;
				lookAt = lookAt - front;
				break;
			case SDLK_a:
				side = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0))) * dt;
				cameraPosition = cameraPosition - side;
				lookAt = lookAt - side;
				break;
			case SDLK_d:
				side = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0))) * dt;
				cameraPosition = cameraPosition + side;
				lookAt = lookAt + side;
				break;
			case SDLK_q:
				cameraPosition = cameraPosition + glm::vec3(0, dt, 0);
				lookAt = lookAt + glm::vec3(0, dt, 0);
				break;
			case SDLK_e:
				cameraPosition = cameraPosition - glm::vec3(0, dt, 0);
				lookAt = lookAt - glm::vec3(0, dt, 0);
				break;
			case SDLK_p:
				depthMapToggle = !depthMapToggle;
				if (depthMapToggle) {
					depthMapIndex += 1;
					depthMapIndex = depthMapIndex % NUM_IMAGES;
					std::cout << "DepthMap: " << depthMapIndex << std::endl;
				}
				break;
			case SDLK_i:
				lookAt = lookAt + glm::vec3(0, 0, dt);
				break;
			case SDLK_k:
				lookAt = lookAt + glm::vec3(0, 0, -dt);
				break;
			case SDLK_j:
				lookAt = lookAt + glm::vec3(dt, 0, 0);
				break;
			case SDLK_l:
				lookAt = lookAt + glm::vec3(-dt, 0, 0);
				break;
			case SDLK_o:
				lightPosition = lightPosition + glm::vec3(0.0, dt, 0.0);
				lightLookAt = lightLookAt + glm::vec3(0.0, dt, 0.0);
				break;
			case SDLK_u:
				lightPosition = lightPosition - glm::vec3(0.0, dt, 0.0);
				lightLookAt = lightLookAt - glm::vec3(0.0, dt, 0.0);
				break;
			case SDLK_ESCAPE:
				display.Terminate();
				std::cout << "escape\n";
				break;
			case SDLK_z:
				bbox -= dt;
				fov -= dt;
				break;
			case SDLK_x:
				bbox += dt;
				fov += dt;
				break;
			case SDLK_r:
				cameraPosition = cameraDefaultPosition;
				lookAt = lookAtDefault;
				//lightPosition = defaultLightPosition;
				//lightLookAt = lookAtDefault;
				fov = defaultFOV;
				break;
			case SDLK_c:
				printCameraCoord = true;
				runtime = !runtime;
				break;
			case SDLK_h:
				CountNumberOfPoints = !CountNumberOfPoints;
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			/* If the mouse is moving to the left */
			if (event.motion.xrel < 0) {
				front = glm::normalize(lookAt - cameraPosition);
				side = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
				lookAt = lookAt - side;
			}
			/* If the mouse is moving to the right */
			else if (event.motion.xrel > 0) {
				front = glm::normalize(lookAt - cameraPosition);
				side = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
				lookAt = lookAt + side;
			}
			break;
		default:
			break;
		}
	}
}
