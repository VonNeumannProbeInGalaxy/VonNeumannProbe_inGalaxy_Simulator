#pragma warning(disable : 4715)

#include "Camera.h"
#include "Engine/Core/Assert.h"

_NPGS_BEGIN

Camera::Camera(const glm::vec3& Position, const glm::vec3& WorldUp) :
	_Orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
	_Position(Position),
	_Front(kFront),
	_WorldUp(WorldUp),
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
		NpgsAssert(false, "Invalid vector type");
	}
}

void Camera::ProcessKeyboard(Movement Direction, double DeltaTime)
{
    float Velocity = static_cast<float>(_Speed * DeltaTime);

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
        _Position += _Up * Velocity;
        break;
    case Movement::kDown:
        _Position -= _Up * Velocity;
        break;
    case Movement::kRollLeft:
    {
        float RollAngle = glm::radians(10.0f * Velocity);
        glm::quat RollQuat = glm::angleAxis(RollAngle, _Front);
        // 预先乘以 RollQuat 以确保一致的旋转顺序
        _Orientation = glm::normalize(RollQuat * _Orientation);
        break;
    }
    case Movement::kRollRight:
    {
        float RollAngle = glm::radians(-10.0f * Velocity);
        glm::quat RollQuat = glm::angleAxis(RollAngle, _Front);
        // 预先乘以 RollQuat 以确保一致的旋转顺序
        _Orientation = glm::normalize(RollQuat * _Orientation);
        break;
    }
    }

    UpdateVectors();
}

void Camera::ProcessMouseMovement(double OffsetX, double OffsetY, bool)
{
    float PitchDelta = static_cast<float>(_Sensitivity * -OffsetY);
    float YawDelta   = static_cast<float>(_Sensitivity *  OffsetX);

    glm::quat PitchQuat = glm::angleAxis(glm::radians(PitchDelta), _Right);
    glm::quat YawQuat   = glm::angleAxis(glm::radians(YawDelta),   _Up);

    _Orientation = glm::normalize(YawQuat * PitchQuat * _Orientation);

    UpdateVectors();
}

void Camera::ProcessMouseScroll(double OffsetY)
{
	_Speed += OffsetY * 0.1;

	if (_Speed <= 0)
	{
		_Speed = 0;
	}
}

void Camera::UpdateVectors()
{
    _Orientation = glm::normalize(_Orientation);

    _Front = glm::normalize(_Orientation * glm::vec3(0.0f, 0.0f, -1.0f));
    _Right = glm::normalize(_Orientation * glm::vec3(1.0f, 0.0f,  0.0f));
    _Up    = glm::normalize(_Orientation * glm::vec3(0.0f, 1.0f,  0.0f));
}

_NPGS_END
