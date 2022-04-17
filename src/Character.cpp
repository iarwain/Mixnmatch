/**
 * @file Character.cpp
 * @date 21-Apr-2022
 */

#include "Character.h"

void Character::OnCreate()
{
    // Enable our input set
    orxInput_EnableSet(orxConfig_GetCurrentSection(), orxTRUE);

    // Update status
    orxConfig_SetBool("IsCharacter", orxTRUE);

    // Starts facing down
    this->eLastOrientation = Down;
}

void Character::OnDelete()
{
}

void Character::Update(const orxCLOCK_INFO &_rstInfo)
{
}
