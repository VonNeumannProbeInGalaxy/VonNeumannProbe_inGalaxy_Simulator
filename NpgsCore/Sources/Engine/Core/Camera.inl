#pragma once

#include "Camera.h"

_NPGS_BEGIN

inline void Camera::ProcessMouseScroll(double OffsetY)
{
	_Speed += static_cast<float>(OffsetY * 0.1);

	if (_Speed <= 0)
	{
		_Speed = 0;
	}
}

inline float Camera::GetCameraZoom() const
{
	return _Zoom;
}

inline glm::mat4x4 Camera::GetViewMatrix() const
{
	return glm::mat4_cast(_Orientation) * glm::translate(glm::mat4(1.0f), -_Position);
}

inline void Camera::SetOrientation(const glm::quat& Orientation)
{
	_Orientation = Orientation;
}

inline const glm::quat& Camera::GetOrientation() const
{
	return _Orientation;
}

_NPGS_END
