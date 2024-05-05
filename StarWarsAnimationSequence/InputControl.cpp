#include "pch.h"

#include "cControlGameEngine.h"

extern cControlGameEngine gameEngine;

extern bool bPauseAnimation;

extern glm::vec3 timeStampCamPos;
extern glm::vec3 timeStampCamTarget; 
extern float timeStampCamYaw; 
extern float timeStampCamPitch;

const float POSITION_OFFSET = 1.0f;

cMesh* controlMeshModel;

void resetCamAttributes()
{
    if (!gameEngine.mouseMoved)
        gameEngine.mouseMoved = true;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    float ROTATION_DEGREES = 5.0f;

    const float CAMERA_SPEED = 300.0f;

    const float MODEL_MOVE_SPEED = 1.0f;

    float CAMERA_MOVEMENT = CAMERA_SPEED * gameEngine.deltaTime;

    glm::vec3 CAMERA_TARGET = gameEngine.GetCurrentCameraTarget();
    glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 CAMERA_POSITION = gameEngine.GetCurrentCameraPosition();

    //---------------Controls for Mesh Models----------------------------------------------------------

    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
    {
        controlMeshModel = gameEngine.GetCurrentModelSelected();

        std::string currentModelName = controlMeshModel->friendlyName;

        glm::vec3 currentModelPosition = gameEngine.GetModelPosition(currentModelName);

        float currentModelScaleValue = gameEngine.GetModelScaleValue(currentModelName);

        if (key == GLFW_KEY_W && action) // Decrement model's Z position
        {
            gameEngine.MoveModel(currentModelName, currentModelPosition.x, currentModelPosition.y, currentModelPosition.z - MODEL_MOVE_SPEED);
        }
        if (key == GLFW_KEY_A && action) // Decrement model's X position
        {
            gameEngine.MoveModel(currentModelName, currentModelPosition.x - MODEL_MOVE_SPEED, currentModelPosition.y, currentModelPosition.z);
        }
        if (key == GLFW_KEY_S && action) // Increment model's Z position
        {
            gameEngine.MoveModel(currentModelName, currentModelPosition.x, currentModelPosition.y, currentModelPosition.z + MODEL_MOVE_SPEED);
        }
        if (key == GLFW_KEY_D && action) // Increment model's X position
        {
            gameEngine.MoveModel(currentModelName, currentModelPosition.x + MODEL_MOVE_SPEED, currentModelPosition.y, currentModelPosition.z);
        }
        if (key == GLFW_KEY_Q && action) // Increment model's Y position 
        {
            gameEngine.MoveModel(currentModelName, currentModelPosition.x, currentModelPosition.y + MODEL_MOVE_SPEED, currentModelPosition.z);
        }
        if (key == GLFW_KEY_E && action) // Decrement model's Y position
        {
            gameEngine.MoveModel(currentModelName, currentModelPosition.x, currentModelPosition.y - MODEL_MOVE_SPEED, currentModelPosition.z);
        }
        if (key == GLFW_KEY_I && action) // Rotate model's X axis - 5 degrees
        {
            gameEngine.RotateMeshModel(currentModelName, ROTATION_DEGREES, 1.f, 0.f, 0.f);
        }
        if (key == GLFW_KEY_O && action) // Rotate model's Y axis - 5 degrees
        {
            gameEngine.RotateMeshModel(currentModelName, ROTATION_DEGREES, 0.f, 1.f, 0.f);
        }
        if (key == GLFW_KEY_P && action) // Rotate model's Z axis - 5 degrees
        {
            gameEngine.RotateMeshModel(currentModelName, ROTATION_DEGREES, 0.f, 0.f, 1.f);
        }
        if (key == GLFW_KEY_C && action) // Decrement model's scale value
        {
            gameEngine.ScaleModel(currentModelName, currentModelScaleValue - 0.01f);
        }
        if (key == GLFW_KEY_V && action) // Increment model's scale value
        {
            gameEngine.ScaleModel(currentModelName, currentModelScaleValue + 0.01f);
        }
        if (key == GLFW_KEY_Z && action) // Shift to previous model
        {
            gameEngine.ShiftToPreviousMeshInList();
        }
        if (key == GLFW_KEY_X && action) // Shift to next model
        {
            gameEngine.ShiftToNextMeshInList();
        }
        if (key == GLFW_KEY_B && action) // Change to wireframe mode
        {
            gameEngine.TurnWireframeModeOn(currentModelName);
        }
    }

    //-------------------Controls for Camera-----------------------------------------------------------

    if (mods == 0)
    {
        //------------------------------Camera Showcases------------------------------------------------
        
           
        if (key == GLFW_KEY_SPACE && action)
        {
            if (bPauseAnimation)
            {
                if (gameEngine.enableMouseMovement)
                    gameEngine.enableMouseMovement = false;
                else
                    gameEngine.enableMouseMovement = true;

                resetCamAttributes();
            }
        }

        //------------------------------Move Camera--------------------------------------------------------

        if (key == GLFW_KEY_W && action)
        {
            if (bPauseAnimation)
                CAMERA_POSITION += CAMERA_MOVEMENT * CAMERA_TARGET;
        }
        if (key == GLFW_KEY_A && action)
        {
            if (bPauseAnimation)
                CAMERA_POSITION -= glm::normalize(glm::cross(CAMERA_TARGET, CAMERA_UP)) * CAMERA_MOVEMENT;
        }
        if (key == GLFW_KEY_S && action)
        {
            if (bPauseAnimation)
                CAMERA_POSITION -= CAMERA_MOVEMENT * CAMERA_TARGET;
        }
        if (key == GLFW_KEY_D && action)
        {
            if (bPauseAnimation)
                CAMERA_POSITION += glm::normalize(glm::cross(CAMERA_TARGET, CAMERA_UP)) * CAMERA_MOVEMENT;
        }
        if (key == GLFW_KEY_Q && action)
        {
            if (bPauseAnimation)
                CAMERA_POSITION += CAMERA_MOVEMENT * CAMERA_UP;
        }
        if (key == GLFW_KEY_E && action)
        {
            if (bPauseAnimation)
                CAMERA_POSITION -= CAMERA_MOVEMENT * CAMERA_UP;
        }

        //---------------------------------Pause/Resume Cam Animation----------------------------------------------

        if (key == GLFW_KEY_P && action)
        {
            if (!bPauseAnimation)
            {
                timeStampCamPos = gameEngine.GetCurrentCameraPosition();
                timeStampCamTarget = gameEngine.GetCurrentCameraTarget();
                timeStampCamYaw = gameEngine.yaw;
                timeStampCamPitch = gameEngine.pitch;

                bPauseAnimation = true;
            }
            else
            {
                gameEngine.enableMouseMovement = false;

                gameEngine.MoveCameraPosition(timeStampCamPos.x, timeStampCamPos.y, timeStampCamPos.z);
                gameEngine.MoveCameraTarget(timeStampCamTarget.x, timeStampCamTarget.y, timeStampCamTarget.z);
                gameEngine.yaw = timeStampCamYaw;
                gameEngine.pitch = timeStampCamPitch;

                bPauseAnimation = false;
            }
        }

        if(bPauseAnimation)
            gameEngine.MoveCameraPosition(CAMERA_POSITION.x, CAMERA_POSITION.y, CAMERA_POSITION.z);
    }

    return;
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (gameEngine.enableMouseMovement)
    {
        glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 CAMERA_POSITION = gameEngine.GetCurrentCameraPosition();

        int screenWidth, screenHeight;
        glfwGetWindowSize(window, &screenWidth, &screenHeight);

        float mouseSensitivity = 0.05f;

        float xOffset = 0.0f;
        float yOffset = 0.0f;

        if (xPos >= screenWidth - 1 || xPos <= 0) // Rotate Cam when mouse moves to the edge of the screen(left or right)
        {
            xOffset = 0.0f;

            if (xPos <= 0)
                xOffset = -10.0f;
            else if (xPos >= screenWidth - 1)
                xOffset = 10.0f;

            gameEngine.yaw += mouseSensitivity * xOffset;
        }

        if (yPos >= screenHeight - 1 || yPos <= 20) // Rotate Cam when mouse moves to the edge of the screen(up or down)
        {
            yOffset = 0.0f;

            if (yPos <= 20)
                yOffset = 10.0f;
            else if (yPos >= screenHeight - 1)
                yOffset = -10.0f;

            gameEngine.pitch += mouseSensitivity * yOffset;
        }

        if (gameEngine.mouseMoved)
        {
            gameEngine.previousX = xPos;
            gameEngine.previousY = yPos;
            gameEngine.mouseMoved = false;
        }

        xOffset = xPos - gameEngine.previousX;
        yOffset = gameEngine.previousY - yPos; //Reversed Y

        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;

        gameEngine.yaw += xOffset;
        gameEngine.pitch += yOffset;

        if (gameEngine.pitch > 90.0f)
            gameEngine.pitch = 90.0f;

        if (gameEngine.pitch < -90.0f)
            gameEngine.pitch = -90.0f;

        glm::vec3 front;

        front.x = cos(glm::radians(gameEngine.yaw)) * cos(glm::radians(gameEngine.pitch));
        front.y = sin(glm::radians(gameEngine.pitch));
        front.z = sin(glm::radians(gameEngine.yaw)) * cos(glm::radians(gameEngine.pitch));

        glm::vec3 cameraFront = glm::normalize(front);

        gameEngine.MoveCameraTarget(cameraFront.x, cameraFront.y, cameraFront.z);

        gameEngine.previousX = xPos;
        gameEngine.previousY = yPos;
    }
}