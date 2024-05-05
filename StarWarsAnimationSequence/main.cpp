#include "pch.h"
#include "cMesh.h"
#include "cJsonReader.h"
#include "cHiResTimer.h"
#include "cMoveToCommand.h"
#include "cOrientToCommand.h"
#include "iAnimationCommand.h"
#include "cCameraAnimation.h"
#include "LuaBrain/cLuaBrain.h"
#include "cAnimationCommandGroup.h"

#include <cControlGameEngine.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

//---------------------------Global Objects-----------------------------------------------

GLFWwindow* window;
cControlGameEngine gameEngine;
cJsonReader jsonReader;
sCameraDetailsFromFile camDetails;
sCubeMapDetailsFromFile cubeMapDetails;
cHiResTimer* timer = new cHiResTimer(60);
cLuaBrain luaBrain;

std::vector<std::unique_ptr<cAnimationCommandGroup>> groupAnimationsList;

std::vector<cCameraAnimation> camAnimationList;
std::vector<sModelDetailsFromFile> modelDetailsList;
std::vector<sLightDetailsFromFile> lightDetailsList;
std::vector<sPhysicsDetailsFromFile> physicsDetailsList;

const float POSITION_OFFSET = 1.0f;

unsigned int sceneCounter = 1;

bool bPauseAnimation = false;

glm::vec3 timeStampCamPos = glm::vec3(0.0f);
glm::vec3 timeStampCamTarget = glm::vec3(0.0f);

float timeStampCamYaw = 0.0f;
float timeStampCamPitch = 0.0f;

extern int Lua_MoveCommand(lua_State* L);
extern int Lua_OrientCommand(lua_State* L);
extern int Lua_CamMoveCommand(lua_State* L);
extern int Lua_CamPositionCommand(lua_State* L);
extern int Lua_CamLookAtCommand(lua_State* L);
extern int Lua_CurveCommand(lua_State* L);
extern int Lua_FollowObjectCommand(lua_State* L);
extern int Lua_CamLookAtAnimationCommand(lua_State* L);
extern int Lua_ChangeColorCommand(lua_State* L);
extern int Lua_SelfDestructCommand(lua_State* L);

int main()
{
    //-----------------------------------Initialize Window----------------------------------------------------------------

    int result = 0;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(800, 600, "Simple example", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //----------------------------Input key and Cursor initialize---------------------------------------------------------

    glfwSetKeyCallback(window, key_callback);

    glfwSetCursorPosCallback(window, mouse_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    //--------------------------------Initialize Game Engine--------------------------------------------------------------

    result = gameEngine.InitializeGameEngine();

    if (result != 0)
        return -1;

    //------------------Loading Models, Lights and initial camera position from Json file---------------------------------

    bool jsonresult = jsonReader.ReadScene("SceneDescription.json", modelDetailsList, cubeMapDetails, physicsDetailsList, lightDetailsList, camDetails);

    if (jsonresult)
    {
        std::cout << "File read successfully !" << std::endl;

        std::string modelName;
        int lightId;
        float angleDegrees;

        // Loading Models
        for (int index = 0; index < modelDetailsList.size(); index++)
        {
            modelName = modelDetailsList[index].modelName;

            //---------------------------Load Models with position----------------------------------------------------------------------

            gameEngine.LoadModelsInto3DSpace(modelDetailsList[index].modelFilePath, modelName, modelDetailsList[index].modelPosition.x + POSITION_OFFSET,
                modelDetailsList[index].modelPosition.y + POSITION_OFFSET, modelDetailsList[index].modelPosition.z + POSITION_OFFSET);

            //---------------------------------Rotate Models-----------------------------------------------------------------------------

            if (modelDetailsList[index].modelOrientation.x != 0.f)
            {
                angleDegrees = modelDetailsList[index].modelOrientation.x;
                gameEngine.RotateMeshModel(modelName, angleDegrees, 1.f, 0.f, 0.f);
            }
            if (modelDetailsList[index].modelOrientation.y != 0.f)
            {
                angleDegrees = modelDetailsList[index].modelOrientation.y;
                gameEngine.RotateMeshModel(modelName, angleDegrees, 0.f, 1.f, 0.f);
            }
            if (modelDetailsList[index].modelOrientation.z != 0.f)
            {
                angleDegrees = modelDetailsList[index].modelOrientation.z;
                gameEngine.RotateMeshModel(modelName, angleDegrees, 0.f, 0.f, 1.f);
            }

            //------------------------------------Scale Models-----------------------------------------------------------------------------

            gameEngine.ScaleModel(modelName, modelDetailsList[index].modelScaleValue);

            //-----------------------------------Model Wireframe---------------------------------------------------------------------------

            if (modelDetailsList[index].wireframeModeOn)
                gameEngine.TurnWireframeModeOn(modelName);

            //----------------------------------Model Mesh Light---------------------------------------------------------------------------

            if (modelDetailsList[index].meshLightsOn)
                gameEngine.TurnMeshLightsOn(modelName);

            //----------------------------------Cube Map Details-----------------------------------------------------------------------------

            if (modelDetailsList[index].isSkyBox)
            {
                if (!cubeMapDetails.cubeMapName.empty())
                {
                    result = gameEngine.AddCubeMap(modelName, cubeMapDetails.cubeMapName, cubeMapDetails.filePathPosX, cubeMapDetails.filePathNegX,
                        cubeMapDetails.filePathPosY, cubeMapDetails.filePathNegY, cubeMapDetails.filePathPosZ,
                        cubeMapDetails.filePathNegZ);
                    if (result != 0)
                        return -1;
                }
                else
                {
                    std::cout << "Cube Properties wasnt updated or added in the json file" << std::endl;

                    return -1;
                }
            }
            else
            {
                //------------------------------------Color Models-----------------------------------------------------------------------------

                if (modelDetailsList[index].manualColors)
                {
                    gameEngine.UseManualColors(modelName, true);
                    gameEngine.ChangeColor(modelName, modelDetailsList[index].modelColorRGB.x, modelDetailsList[index].modelColorRGB.y, modelDetailsList[index].modelColorRGB.z);
                }

                //------------------------------------Add Textures------------------------------------------------------------------------------

                if (modelDetailsList[index].useTextures)
                {
                    gameEngine.UseTextures(modelName, true);

                    for (int textureIndex = 0; textureIndex < modelDetailsList[index].textureNameList.size(); textureIndex++)
                    {
                        if (modelDetailsList[index].blendTexture[textureIndex])
                            gameEngine.AddTexturesToTheMix(modelName, modelDetailsList[index].textureFilePathList[textureIndex],
                                modelDetailsList[index].textureNameList[textureIndex], modelDetailsList[index].textureRatiosList[textureIndex]);
                        else
                            gameEngine.AddTexturesToOverlap(modelName, modelDetailsList[index].textureFilePathList[textureIndex],
                                modelDetailsList[index].textureNameList[textureIndex]);

                        if (modelDetailsList[index].useDiscardMaskTexture[textureIndex])
                            gameEngine.AddDiscardMaskTexture(modelName, modelDetailsList[index].textureNameList[textureIndex],
                                modelDetailsList[index].discardMaskTextureFilePath[textureIndex]);
                    }
                }
            }

            //-----------------------------------Adding Physics------------------------------------------------------------------------------

            if (modelDetailsList[index].physicsMeshType == "Sphere") // Sphere Physics
            {
                for (int physicsIndex = 0; physicsIndex < physicsDetailsList.size(); physicsIndex++)
                {
                    if (physicsDetailsList[physicsIndex].modelName == modelName)
                    {
                        gameEngine.AddSpherePhysicsToMesh(modelName, modelDetailsList[index].physicsMeshType, physicsDetailsList[physicsIndex].modelRadius);

                        if (physicsDetailsList[physicsIndex].randomVelocity)
                        {
                            float randomVelocity = gameEngine.getRandomFloat(0.7, 5.0);

                            gameEngine.ChangeModelPhysicsVelocity(modelName, glm::vec3(0.0f, -5.0f, 0.0f));
                        }
                        else
                            gameEngine.ChangeModelPhysicsVelocity(modelName, glm::vec3(physicsDetailsList[physicsIndex].modelVelocity.x, physicsDetailsList[physicsIndex].modelVelocity.y,
                                physicsDetailsList[physicsIndex].modelVelocity.z));

                        gameEngine.ChangeModelPhysicsAcceleration(modelName, glm::vec3(physicsDetailsList[physicsIndex].modelAcceleration.x, physicsDetailsList[physicsIndex].modelAcceleration.y,
                            physicsDetailsList[physicsIndex].modelAcceleration.z));

                        result = gameEngine.ChangeModelPhysicalMass(modelName, physicsDetailsList[physicsIndex].modelMass);

                        if (result != 0)
                            std::cout << "Mass provided for the sphere - '" << modelName << "' is not above 0. Hence default mass 1.0/10.0 is used for this sphere model." << std::endl;
                    }
                }
            }
            else if (modelDetailsList[index].physicsMeshType == "Plane" || modelDetailsList[index].physicsMeshType == "Box") // Plane Physics
            {
                gameEngine.AddPlanePhysicsToMesh(modelName, modelDetailsList[index].physicsMeshType);
            }
            else
                std::cout << "The Physics mesh type cannot be identified for the model name - " << modelDetailsList[index].modelName << std::endl;
        }

        //-----------------------------------Adding Lights--------------------------------------------------------------------------------

        for (int index = 0; index < lightDetailsList.size(); index++)
        {
            lightId = lightDetailsList[index].lightId;

            gameEngine.CreateLight(lightId, lightDetailsList[index].lightPosition.x + POSITION_OFFSET, lightDetailsList[index].lightPosition.y + POSITION_OFFSET, lightDetailsList[index].lightPosition.z + POSITION_OFFSET);
            gameEngine.ChangeLightType(lightId, lightDetailsList[index].lightType);
            gameEngine.ChangeLightIntensity(lightId, lightDetailsList[index].linearAttenuation, lightDetailsList[index].quadraticAttenuation);
            gameEngine.ChangeLightDirection(lightId, lightDetailsList[index].lightDirection.x, lightDetailsList[index].lightDirection.y, lightDetailsList[index].lightDirection.z);
            gameEngine.ChangeLightColour(lightId, lightDetailsList[index].lightColorRGB.r, lightDetailsList[index].lightColorRGB.g, lightDetailsList[index].lightColorRGB.b);
            gameEngine.ChangeLightAngle(lightId, lightDetailsList[index].innerAngle, lightDetailsList[index].outerAngle);

            if (lightDetailsList[index].lightOn)
                gameEngine.TurnOffLight(lightId, false);
            else
                gameEngine.TurnOffLight(lightId, true);
        }

        //--------------------------Loading Initial Camera Position-----------------------------------------------------------------------

        gameEngine.MoveCameraPosition(camDetails.initialCameraPosition.x + POSITION_OFFSET, camDetails.initialCameraPosition.y + POSITION_OFFSET, camDetails.initialCameraPosition.z + POSITION_OFFSET);
    }
    else
        return -1;
    
    //--------------------------------------Animation Scripts--------------------------------------------------------------

    //---------------------------------LIST OF ALL COMMANDS----------------------------------------------------
    // 
    // 1. CamMoveToCommand(CamPos.x, CamPos.y, CamPos.z, TimeTaken)
    // 
    // 2. CamInstantMoveCommand(CamPos.x, CamPos.y, CamPos.z)
    // 
    // 3. CamInstantLookAtCommand(CamLookAt.x, CamLookAt.y, CamLookAt.z)
    // 
    // 4. CamLookAtAnimationCommand(ObjectNameToLookAt, AnimationTime)
    // 
    // 5. MoveToCommand(MeshName, DestinationPos.x, DestinationPos.y, DestinationPos.z, TimeTaken, EaseInTime, EaseOutTime, GroupType, GroupName)
    // 
    // 6. OrientToCommand(MeshName, Degrees, RotationAxis.x, RotationAxis.y, RotationAxis.z, TimeTaken, GroupType, GroupName)
    // 
    // 7. CurvePathCommand(MeshName, ControlPt0.x, ControlPt0.y, ControlPt0.z, ControlPt1.x, ControlPt1.y, ControlPt1.z, 
    //                          DestinationPos.x, DestinationPos.y, DestinationPos.z, TimeTaken, GroupType, GroupName)
    // 
    // 8. FollowObjectCommand(MeshName, ObjectNameToFollow, MaxFollowDistance, PositionOffset.x, PositionOffset.y, PositionOffset.z, MaxSpeed, FollowTime, 
    //                                                                                                                          GroupType, GroupName)
    // 9. ChangeColorCommand(MeshName, Color.x, Color.y, Color.z, GroupType, GroupName)
    //
    // 10. SelfDestructCommand(MeshName, GroupType, GroupName)

    
    std::string luaScript = "CamInstantMoveCommand(700.0, 100.0, 0.0)\n \
                             CamLookAtAnimationCommand('Fighter', 5.0)\n \
                             MoveToCommand('Fighter', 0.0, 300.0, 0.0, 7.5, 2.0, 0.0, 'serial', 'HoverUp')\n \
                             ChangeColorCommand('Start_Cube', 0.85, 0.3, 0.2, 'serial', 'HoverUp')\n \
                             CamInstantMoveCommand(0.0, 100.0, 1000.0)\n \
                             CamLookAtAnimationCommand('Fighter', 2.5)\n \
                             CamMoveToCommand(-750.0, 100.0, 1000.0, 3.0)\n \
                             MoveToCommand('Fighter', -750.0, 300.0, 0.0, 3.0, 0.75, 0.0, 'parallel', 'startFlying')\n \
                             FollowObjectCommand('Orb_01', 'Fighter', 10.0, 0.0, 0.0, 50.0, 250.0, 3.0, 'parallel', 'startFlying')\n \
                             FollowObjectCommand('Orb_02', 'Fighter', 10.0, 0.0, 0.0, -50.0, 250.0, 3.0, 'parallel', 'startFlying')\n \
                             CamInstantMoveCommand(-500.0, 200.0, 0.0)\n \
                             CamLookAtAnimationCommand('Fighter', 4.0)\n \
                             CurvePathCommand('Fighter', -950.0, 300.0, 600.0, -1250.0, 300.0, 300.0, -1500.0, 300.0, 0.0, 4.0, 'parallel', 'takeALeftCurve')\n \
                             FollowObjectCommand('Orb_01', 'Fighter', 10.0, 0.0, 0.0, 50.0, 250.0, 4.0, 'parallel', 'takeALeftCurve')\n \
                             FollowObjectCommand('Orb_02', 'Fighter', 10.0, 0.0, 0.0, -50.0, 250.0, 4.0, 'parallel', 'takeALeftCurve')\n \
                             OrientToCommand('Fighter', -30.0, 1.0, 0.0, 0.0, 1.5, 'serial', 'takeALeftCurve')\n \
                             OrientToCommand('Fighter', -30.0, 1.0, 0.0, 0.0, 1.5, 'serial', 'takeALeftCurve')\n \
                             OrientToCommand('Fighter', 60.0, 1.0, 0.0, 0.0, 1.0, 'serial', 'takeALeftCurve')\n \
                             CamInstantMoveCommand(-2400.0, 500.0, 800.0)\n \
                             CamLookAtAnimationCommand('Fighter', 3.0)\n \
                             MoveToCommand('Fighter', -1700.0, 300.0, 0.0, 0.5, 0.0, 0.25, 'parallel', 'moveStraight')\n \
                             FollowObjectCommand('Orb_01', 'Fighter', 10.0, 0.0, 0.0, 50.0, 250.0, 0.5, 'parallel', 'moveStraight')\n \
                             FollowObjectCommand('Orb_02', 'Fighter', 10.0, 0.0, 0.0, -50.0, 250.0, 0.5, 'parallel', 'moveStraight')\n \
                             CurvePathCommand('Fighter', -2000.0, 300.0, -600.0, -2300.0, 300.0, 400.0, -2600.0, 300.0, 0.0, 2.5, 'parallel', 'takeARightCurve')\n \
                             FollowObjectCommand('Orb_01', 'Fighter', 5.0, 0.0, 0.0, 50.0, 250.0, 2.5, 'parallel', 'takeARightCurve')\n \
                             FollowObjectCommand('Orb_02', 'Fighter', 5.0, 0.0, 0.0, -50.0, 250.0, 2.5, 'parallel', 'takeARightCurve')\n \
                             OrientToCommand('Fighter', 30.0, 1.0, 0.0, 0.0, 0.84, 'serial', 'takeARightCurve')\n \
                             OrientToCommand('Fighter', 30.0, 1.0, 0.0, 0.0, 0.84, 'serial', 'takeARightCurve')\n \
                             OrientToCommand('Fighter', -60.0, 1.0, 0.0, 0.0, 0.83, 'serial', 'takeARightCurve')\n \
                             MoveToCommand('Fighter', -3600.0, 300.0, 0.0, 1.5, 0.0, 0.0, 'parallel', 'moveInStraightLine')\n \
                             FollowObjectCommand('Orb_01', 'Fighter', 10.0, 0.0, 0.0, 50.0, 250.0, 1.5, 'parallel', 'moveInStraightLine')\n \
                             FollowObjectCommand('Orb_02', 'Fighter', 10.0, 0.0, 0.0, -50.0, 250.0, 1.5, 'parallel', 'moveInStraightLine')\n \
                             CamInstantMoveCommand(-2000.0, 400.0, 0.0)\n \
                             CamLookAtAnimationCommand('Fighter', 1.0)\n \
                             CamMoveToCommand(-4000.0, 400.0, 0.0, 2.5)\n \
                             MoveToCommand('Fighter', -4500.0, 300.0, 0.0, 2.5, 0.0, 0.0, 'parallel', 'shakeIfOff')\n \
                             OrientToCommand('Fighter', 180.0, 1.0, 0.0, 0.0, 0.15, 'serial', 'shakeIfOff')\n \
                             OrientToCommand('Fighter', 180.0, 1.0, 0.0, 0.0, 0.15, 'serial', 'shakeIfOff')\n \
                             OrientToCommand('Fighter', 180.0, 1.0, 0.0, 0.0, 0.15, 'serial', 'shakeIfOff')\n \
                             OrientToCommand('Fighter', 180.0, 1.0, 0.0, 0.0, 0.15, 'serial', 'shakeIfOff')\n \
                             OrientToCommand('Fighter', 180.0, 1.0, 0.0, 0.0, 0.15, 'serial', 'shakeIfOff')\n \
                             OrientToCommand('Fighter', 160.0, 1.0, 0.0, 0.0, 0.15, 'serial', 'shakeIfOff')\n \
                             MoveToCommand('Orb_01', -3600.0, 300.0, 1000.0, 0.5, 0.0, 0.0, 'parallel', 'shakeIfOff')\n \
                             MoveToCommand('Orb_02', -3600.0, 300.0, -1000.0, 0.5, 0.0, 0.0, 'parallel', 'shakeIfOff')\n \
                             MoveToCommand('Fighter', -4800.0, 300.0, 0.0, 0.25, 0.0, 0.0, 'serial', 'moveForward')\n \
                             CamInstantMoveCommand(-5000.0, 300.0, -600.0)\n \
                             CamLookAtAnimationCommand('Fighter', 5.0)\n \
                             CurvePathCommand('Fighter', -5100.0, 700.0, 350.0, -5300.0, 600.0, 250.0, -5600.0, 500.0, 150.0, 2.0, 'parallel', 'LeftCurveBeforeLanding')\n \
                             OrientToCommand('Fighter', -30.0, 1.0, 0.0, 0.0, 1.0, 'serial', 'LeftCurveBeforeLanding')\n \
                             OrientToCommand('Fighter', -30.0, 1.0, 0.0, 0.0, 1.0, 'serial', 'LeftCurveBeforeLanding')\n \
                             CurvePathCommand('Fighter', -5900.0, 400.0, 150.0, -6200.0, 350.0, 75.0, -6000.0, 300.0, 0.0, 2.5, 'parallel', 'AboutToLand')\n \
                             OrientToCommand('Fighter', -90.0, 0.0, 1.0, 0.0, 0.75, 'serial', 'AboutToLand')\n \
                             OrientToCommand('Fighter', -90.0, 0.0, 1.0, 0.0, 0.75, 'serial', 'AboutToLand')\n \
                             OrientToCommand('Fighter', -59.0, 1.0, 0.0, 0.0, 0.75, 'serial', 'AboutToLand')\n \
                             MoveToCommand('Fighter', -6000.0, 0.0, 0.0, 7.5, 0.0, 2.0, 'serial', 'HoverDown')\n \
                             CamInstantMoveCommand(-5000.0, 200.0, 0.0)\n \
                             CamLookAtAnimationCommand('Fighter', 5.0)\n \
                             ChangeColorCommand('End_Cube', 0.1, 0.85, 0.2, 'serial', 'HoverDown')\n \
                             MoveToCommand('Fighter', -6000.0, 0.025, 0.0, 1.0, 0.0, 0.0, 'serial', 'HoverDown')\n \
                             SelfDestructCommand('Fighter', 'serial', 'FinalTouch')"; 

    luaBrain.RunScriptImmediately(luaScript);

    //-------------------------------------Begin Update Loop--------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        //----------------------Frame Rate Count---------------------------------------------

        gameEngine.deltaTime = timer->GetFrameTime();

        //----------------------Update Animation---------------------------------------------

        if (!bPauseAnimation)
        {
            if (!groupAnimationsList.empty())
            {
               if (groupAnimationsList[0]->IsParallelGroup())
                {
                    if (!groupAnimationsList[0]->groupParallelAnimationsList.empty())
                    {
                        for (auto commandIterator = groupAnimationsList[0]->groupParallelAnimationsList.begin(); commandIterator != groupAnimationsList[0]->groupParallelAnimationsList.end(); )
                        {
                            auto& animationCommand = *commandIterator;
                            bool isDone = animationCommand->UpdateAnimation(gameEngine);

                            if (isDone)
                                commandIterator = groupAnimationsList[0]->groupParallelAnimationsList.erase(commandIterator);
                            else
                                commandIterator++;
                        }
                    }
                    else if(groupAnimationsList[0]->groupSerialAnimationsList.empty())
                        groupAnimationsList.erase(groupAnimationsList.begin());
                }

               if(!groupAnimationsList.empty() && groupAnimationsList[0]->IsSerialGroup())
               {
                    if (!groupAnimationsList[0]->groupSerialAnimationsList.empty())
                    {
                        auto commandIterator = groupAnimationsList[0]->groupSerialAnimationsList.begin();
                        auto& animationCommand = *commandIterator;

                        bool isDone = animationCommand->UpdateAnimation(gameEngine);

                        if (isDone)
                            commandIterator = groupAnimationsList[0]->groupSerialAnimationsList.erase(commandIterator);
                    }
                    else if(groupAnimationsList[0]->groupParallelAnimationsList.empty())
                        groupAnimationsList.erase(groupAnimationsList.begin());
               }
            }

            if (!camAnimationList.empty())
            {
                bool isDone = camAnimationList[0].UpdateCameraAnimation(gameEngine);

                if (isDone)
                    camAnimationList.erase(camAnimationList.begin());
            }
        }

        //--------------------Run Engine-----------------------------------------------------

        gameEngine.RunGameEngine(window);
    }

    //---------------------------------------Close Window-----------------------------------------------------------------

    glfwDestroyWindow(window);

    glfwTerminate();

    exit(EXIT_SUCCESS);

    std::cout << "Window closed !" << std::endl;

    return 0;
}