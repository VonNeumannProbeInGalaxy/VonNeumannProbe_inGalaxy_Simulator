#include "Camera.h"

inline GLfloat Camera::GetCameraZoom() const
{
	return _Zoom;
}

inline glm::mat4x4 Camera::GetViewMatrix() const
{
	return glm::lookAt(_Position, _Position + _Front, _WorldUp);
}
