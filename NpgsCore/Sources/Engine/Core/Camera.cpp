#pragma warning(disable : 4715)

#include "Camera.h"
#include <cassert>

Camera::Camera(const glm::vec3& Position, const glm::vec3& WorldUp, GLfloat Pitch, GLfloat Yaw) :
	_Position(Position),
	_Front(kFront),
	_WorldUp(WorldUp),
	_Pitch(Pitch),
	_Yaw(Yaw),
	_Sensitivity(kSensitivity),
	_Speed(kSpeed),
	_Zoom(kZoom)
{
	UpdateVectors();
}

const glm::vec3& Camera::GetCameraVector(VectorType Type) const
{
	switch (Type)
	{
	case VectorType::kPosition:
		return _Position;
	case VectorType::kFront:
		return _Front;
	case VectorType::kUp:
		return _Up;
	case VectorType::kRight:
		return _Right;
	default:
		assert(GL_FALSE);
	}
}

GLvoid Camera::ProcessKeyboard(Movement Direction, GLdouble DeltaTime)
{
	GLfloat Velocity = static_cast<GLfloat>(_Speed * DeltaTime);

	switch (Direction)
	{
	case Movement::kForward:
		_Position += _Front * Velocity;
		break;
	case Movement::kBack:
		_Position -= _Front * Velocity;
		break;
	case Movement::kLeft:
		_Position -= _Right * Velocity;
		break;
	case Movement::kRight:
		_Position += _Right * Velocity;
		break;
	case Movement::kUp:
		_Position += _Up    * Velocity;
		break;
	case Movement::kDown:
		_Position -= _Up    * Velocity;
		break;
	}

	UpdateVectors();
}

GLvoid Camera::ProcessMouseMovement(GLdouble OffsetX, GLdouble OffsetY, GLboolean ConstrainPitch)
{
	_Pitch += static_cast<GLfloat>(OffsetY * _Sensitivity);
	_Yaw   += static_cast<GLfloat>(OffsetX * _Sensitivity);

	if (ConstrainPitch)
	{
		if (_Pitch < -90.0f)
		{
			_Pitch = -89.9f;
		}

		if (_Pitch > 90.0f)
		{
			_Pitch = 89.9f;
		}
	}

	UpdateVectors();
}

GLvoid Camera::ProcessMouseScroll(GLdouble OffsetY)
{
	_Speed += OffsetY * 0.1;

	if (_Speed <= 0)
	{
		_Speed = 0;
	}
}

GLvoid Camera::UpdateVectors()
{
	glm::vec3 Front = glm::vec3(glm::cos(glm::radians(_Yaw)) * glm::cos(glm::radians(_Pitch)),
								glm::sin(glm::radians(_Pitch)),
								glm::sin(glm::radians(_Yaw)) * glm::cos(glm::radians(_Pitch)));

	_Front = glm::normalize(Front);
	_Right = glm::normalize(glm::cross(_Front, _WorldUp));
	_Up    = glm::normalize(glm::cross(_Right, _Front));
}
