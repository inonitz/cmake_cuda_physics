#include <scenetest/camera.hpp>
#include <algorithm>
#include <scenetest/context.hpp>


CameraFPS::CameraFPS(
		AppContext const* context,
		um::vec3f const&  position,
		um::vec3f const&  up,

		const float pitch,
		const float yaw,
		const float vel,

		const float Near,
		const float Far ,
		const float fov_degrees
	) :
		m_firstPos{0.0f, 0.0f},
		m_pitch{pitch},
		m_yaw{yaw},
		m_vel{vel},
		m_Near{Near},
		m_Far{Far},
		m_fov{fov_degrees},
		m_forward{0.0f},
		m_up	 {0.0f},
		m_right  {0.0f},
		m_position{position},
		m_world_up{up},
        m_context{context}
	{
		updateCameraAxes();
		um::identity(m_CameraTransform);
		um::identity(m_ProjectionMatrix);


		auto winsize = context->getSwapchainTextureSize();
		m_firstPos = um::vec2f{ winsize[0] / 2, winsize[1] / 2 };
		return;
	}



void CameraFPS::update(f32 dt)
{
    auto* m_ioman = m_context->getInputManager();


    if(m_ioman->isMouseMoving()) {
        auto lastMousePos = m_ioman->getMouseCursorPosition();
        m_pitch += (lastMousePos.y - m_firstPos.y) * k_MouseSensitivity;
        m_yaw   += (lastMousePos.x - m_firstPos.x) * k_MouseSensitivity;
        m_firstPos = { lastMousePos.x, lastMousePos.y };
    }
    if(m_ioman->isMouseScrollMoving())
        m_fov += m_ioman->getMouseScrollOffset().y * 50.0f * k_MouseSensitivity;


    m_pitch = std::clamp(m_pitch, -89.9f, 89.9f);
    m_yaw   = std::clamp(m_yaw  , -74.9f, 74.9f);
    m_fov   = std::clamp(m_fov  ,   1.0f, 45.0f);
    updateCameraAxes();

    um::vec3f tmp{ m_vel * dt };
    if(m_ioman->isKeyPressed(KeyboardKeyType::KEY_W)) {
        tmp *= m_forward; 
        m_position += tmp;
    } 
    else if (m_ioman->isKeyPressed(KeyboardKeyType::KEY_S)) {
        tmp *= m_forward; 
        m_position -= tmp;
    } 
    else if (m_ioman->isKeyPressed(KeyboardKeyType::KEY_D)) {
        tmp *= m_right; 
        m_position += tmp;
    } 
    else if (m_ioman->isKeyPressed(KeyboardKeyType::KEY_A)) {
        tmp *= m_right; 
        m_position -= tmp;
    } 
    // else if (m_ioman->awc2isKeyPressed(KeyboardKeyType::KEY_R)) {
    //     tmp *= m_up; 
    //     m_position += tmp;
    // } 
    // else if (m_ioman->awc2isKeyPressed(KeyboardKeyType::KEY_F)) {
    //     tmp *= m_up; 
    //     m_position -= tmp;
    // }


    tmp = m_position + m_forward;
    um::lookAt(m_position, tmp, m_up, m_CameraTransform);
    
    auto winsize	   = m_context->getSwapchainTextureSize();
    f32 aspectRatio    = m_context->getWindowAspectRatio();
    um::perspective(um::radians(m_fov), aspectRatio, m_Near, m_Far, m_ProjectionMatrix);
    return;
}


void CameraFPS::updateProjectionParameters(
    f32 fieldOfView,
    f32 nearPlane,
    f32 farPlane
) {
    auto winsize	= m_context->getSwapchainTextureSize();
    f32 aspectRatio = m_context->getWindowAspectRatio();
    m_Near = nearPlane;
    m_Far  = farPlane;
    m_fov  = fieldOfView;
    um::perspective(um::radians(m_fov), aspectRatio, m_Near, m_Far, m_ProjectionMatrix);
    return;
}