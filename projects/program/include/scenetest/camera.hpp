#pragma once
#include <util2/vec2.hpp>
#include <SDL3/SDL.h>


namespace um = util2::math;


class AppContext;


class CameraFPS
{
public:
	CameraFPS(
		AppContext const* context,
		um::vec3f const&  position = { 0.0f, 0.0f, 3.0f },
		um::vec3f const&  up 	   = { 0.0f, 1.0f, 0.0f },

		const float pitch = 0.0f,
		const float yaw   = 0.0f,
		const float vel   = 2.5f,

		const float Near = 0.1f,
		const float Far  = 100.0f,
		const float fov_degrees = 45.0f
	);


	void update(f32 dt);


	void updateProjectionParameters(
		f32 fov  = 60.0f, 
		f32 near = 0.1f, 
		f32 far  = 100.0f
	);


	__force_inline void updateCameraSpeed(f32 cameraSpeed) {
		m_vel = cameraSpeed;
		return;
	}


	__force_inline [[nodiscard]] um::mat4f const& getView() 	  const { return m_CameraTransform;  }
	__force_inline [[nodiscard]] um::mat4f const& getProjection() const { return m_ProjectionMatrix; }
	__force_inline [[nodiscard]] um::vec3f const& getPosition()   const { return m_position; }
	__force_inline [[nodiscard]] um::vec3f getDirection()  		  const { return m_position + m_forward; }


private:
	void updateCameraAxes()
	{
		m_forward = um::vec3f{
			cosf(m_yaw) * cosf(m_pitch),
			sinf(m_pitch),
			sinf(m_yaw) * cosf(m_pitch)
		};
		m_forward.normalize();
		
		
		m_right = um::cross(m_forward, m_world_up);
		m_right.normalize();

		m_up = um::cross(m_right, m_forward);
		m_up.normalize();
		return;
	}


private:
	static constexpr f32 k_MouseSensitivity = 10e-4f;
	um::vec2f m_firstPos;
	
	f32 m_pitch;
	f32 m_yaw;
	f32 m_vel;

	f32 m_Near;
	f32 m_Far;
	f32 m_fov;

	um::vec3f m_forward {0.0f, 0.0f, -1.0f};
	um::vec3f m_up      {0.0f};
	um::vec3f m_right   {0.0f};
	
	um::vec3f m_position;
	um::vec3f m_world_up;
	um::mat4f m_CameraTransform;  // view matrix
	um::mat4f m_ProjectionMatrix; // projection matrix.
	AppContext const* m_context;
};