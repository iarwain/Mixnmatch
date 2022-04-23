/**
 * @file Character.cpp
 * @date 21-Apr-2022
 */

#include "Character.h"

void Character::OnCreate()
{
    // Enable our input set
    orxInput_EnableSet(orxConfig_GetCurrentSection(), orxTRUE);

    // Starts facing down
    this->eLastOrientation = Down;
}

void Character::OnDelete()
{
}

void Character::Update(const orxCLOCK_INFO &_rstInfo)
{
    // Push our config section
    PushConfigSection();

    // Select our input set
    const orxSTRING zInputSet = orxInput_GetCurrentSet();
    orxInput_SelectSet(orxConfig_GetCurrentSection());

    // Update movement
    const orxSTRING zMove = "Idle";
    orxVECTOR vMove = {orxInput_GetValue("MoveRight") - orxInput_GetValue("MoveLeft"), orxInput_GetValue("MoveDown") - orxInput_GetValue("MoveUp")};
    if(!orxVector_IsNull(&vMove))
    {
        zMove = orxInput_IsActive("Run") ? "Run" : "Walk";
        orxVector_FromCartesianToSpherical(&vMove, &vMove);
        vMove.fRho = orxConfig_GetFloat(zMove);
        this->eLastOrientation = (Orientation)(orxF2S(orxMath_Round(vMove.fTheta / orxMATH_KF_PI_BY_4)) & 7);
        orxVector_FromSphericalToCartesian(&vMove, &vMove);
    }
    SetSpeed(vMove);

    // Update anim
    orxCHAR acAnim[64];
    orxString_NPrint(acAnim, sizeof(acAnim), "%s%s", zMove, orxConfig_GetListString("Orientations", this->eLastOrientation));
    SetAnim(acAnim, orxFALSE, orxTRUE);

    // Restore previous input set
    orxInput_SelectSet(zInputSet);

    // Pop config section
    PopConfigSection();
}
