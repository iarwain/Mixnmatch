/**
 * @file Mixnmatch.cpp
 * @date 21-Apr-2022
 */

#define __SCROLL_IMPL__
#include "Mixnmatch.h"
#undef __SCROLL_IMPL__

#include "Character.h"

#ifdef __orxMSVC__

/* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement        = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

void GenerateCharacters()
{
    orxConfig_PushSection("Characters");

    // Has texture format?
    const orxSTRING zFormat = orxConfig_GetString("TextureFormat");
    if(*zFormat != orxCHAR_NULL)
    {
        // For all characters
        for(orxS32 i = 0, iCount = orxConfig_GetListCount("Characters"); i < iCount; i++)
        {
            const orxSTRING zCharacter = orxConfig_GetListString("Characters", i);

            // Push character's config section
            orxConfig_PushSection(zCharacter);

            // Get character section
            orxCHAR acCharacter[64];
            orxString_NPrint(acCharacter, sizeof(acCharacter), "%sCharacter", zCharacter);

            // For all defined layers
            for(orxS32 j = 0, jCount = orxConfig_GetKeyCount(); j < jCount; j++)
            {
                const orxSTRING zLayer = orxConfig_GetKey(j);

                // For all layer variations
                for(orxS32 k = 0, kCount = orxConfig_GetListCount(zLayer); k < kCount; k++)
                {
                    const orxSTRING zVariation = orxConfig_GetListString(zLayer, k);

                    // Get texture name
                    orxCHAR acTexture[256];
                    orxString_NPrint(acTexture, sizeof(acTexture), zFormat, zCharacter, zLayer, zVariation);

                    // Does it exist?
                    if(orxResource_Locate(orxTEXTURE_KZ_RESOURCE_GROUP, acTexture))
                    {
                        orxCHAR acAsset[64];

                        // Get asset name
                        orxString_NPrint(acAsset, sizeof(acAsset), "%s%s%s", zCharacter, zLayer, zVariation);

                        // Add it to the character's section to default with random layer selection
                        orxConfig_PushSection(acCharacter);
                        orxConfig_SetParent(acCharacter, "Character");
                        const orxSTRING zAsset = acAsset;
                        orxConfig_AppendListString(zLayer, &zAsset, 1);
                        orxConfig_PopSection();

                        // Generate asset's content
                        orxConfig_PushSection(acAsset);
                        orxConfig_SetParent(acAsset, zLayer);
                        orxConfig_SetString(orxGRAPHIC_KZ_CONFIG_TEXTURE_NAME, acTexture);
                        orxConfig_PopSection();
                    }
                }
            }

            // Pop config section
            orxConfig_PopSection();
        }
    }

    orxConfig_PopSection();
}

void GenerateAnims()
{
    for(orxS32 i = 0, iCount = orxConfig_GetSectionCount(); i < iCount; i++)
    {
        const orxSTRING zSection = orxConfig_GetSection(i);

        orxConfig_PushSection(zSection);

        // Should generate?
        if(orxConfig_GetBool("Generate") && !orxConfig_IsInheritedValue("Generate"))
        {
            orxCHAR acStartAnim[64] = {}, acStartAnimLink[64];
            orxU32  u32FrameIndex = 1;

            // For all animations
            for(orxS32 j = 0, jCount = orxConfig_GetListCount("Anims"); j < jCount; j++)
            {
                const orxSTRING zAnim = orxConfig_GetListString("Anims", j);

                // For all orientations
                for(orxS32 k = 0, kCount = orxConfig_GetListCount("Orientations"); k < kCount; k++)
                {
                    const orxSTRING zOrientation = orxConfig_GetListString("Orientations", k);

                    // Init anim
                    orxCHAR acAnim[64];
                    orxString_NPrint(acAnim, sizeof(acAnim), ".%s%s", zAnim, zOrientation);
                    orxConfig_SetU32(acAnim + 1, 0);

                    // Generate all frames
                    orxU32 u32FrameCount = orxConfig_GetU32(zAnim);
                    for(orxU32 u32Index = 1; ; u32Index++)
                    {
                        orxCHAR acParentFrame[64];
                        orxString_NPrint(acParentFrame, sizeof(acParentFrame), "%s%u", zAnim, u32Index);
                        // Is physical frame or was it manually defined in config?
                        if((u32Index <= u32FrameCount) || orxConfig_HasSection(acParentFrame))
                        {
                            orxCHAR acFrame[64];
                            orxString_NPrint(acFrame, sizeof(acFrame), "%s%s%u", zAnim, zOrientation, u32Index);
                            // Physical frame?
                            if(u32Index <= u32FrameCount)
                            {
                                orxConfig_PushSection(acFrame);
                                orxConfig_SetU32("FrameIndex", u32FrameIndex++);
                                orxConfig_PopSection();
                            }
                            // Manually defined in config?
                            if(orxConfig_HasSection(acParentFrame))
                            {
                                orxCHAR acParent[64];
                                orxString_NPrint(acParent, sizeof(acParent), "@%s", acParentFrame);
                                orxConfig_PushSection(acParentFrame);
                                // For all manually defined properties
                                for(orxS32 l = 0, lCount = orxConfig_GetKeyCount(); l < lCount; l++)
                                {
                                    const orxSTRING zKey = orxConfig_GetKey(l);
                                    orxConfig_PushSection(acFrame);
                                    orxConfig_SetString(zKey, acParent);
                                    // Execute generative lazy command hook
                                    if(*zKey == '!')
                                    {
                                        orxConfig_GetString(zKey);
                                        orxConfig_ClearValue(zKey);
                                    }
                                    orxConfig_PopSection();
                                }
                                orxConfig_PopSection();
                            }
                        }
                        else
                        {
                            // Stop, all frames have been processed
                            break;
                        }
                    }

                    // Start anim?
                    if(*acStartAnim == orxCHAR_NULL)
                    {
                        // Set start anim & initial link
                        orxString_NPrint(acStartAnim, sizeof(acStartAnim), ".%s%s", zAnim, zOrientation);
                        orxConfig_SetString("StartAnim", acStartAnim + 1);
                        orxString_NPrint(acStartAnimLink, sizeof(acStartAnimLink), "%s%s->", zAnim, zOrientation);
                        orxConfig_SetString(acStartAnimLink, acStartAnim + 1);
                    }
                    else
                    {
                        // Set anim links
                        const orxSTRING zTargetAnim;
                        orxCHAR acAnimLink[64];
                        orxString_NPrint(acAnimLink, sizeof(acAnimLink), "%s%s->", zAnim, zOrientation);
                        orxConfig_SetString(acAnimLink, acAnim + 1);
                        zTargetAnim = acStartAnim;
                        orxConfig_AppendListString(acAnimLink, &zTargetAnim, 1);

                        // Update start anim links
                        zTargetAnim = acAnim;
                        orxConfig_AppendListString(acStartAnimLink, &zTargetAnim, 1);
                    }
                }
            }
        }

        orxConfig_PopSection();
    }
}

void GenerateData()
{
    GenerateCharacters();
    GenerateAnims();
}

/** Update function, it has been registered to be called every tick of the core clock
 */
void Mixnmatch::Update(const orxCLOCK_INFO &_rstInfo)
{
    // Should Reset?
    if(orxInput_HasBeenActivated("Reset"))
    {
        // Creates reset object
        CreateObject("Reset");
    }
    // Should quit?
    if(orxInput_IsActive("Quit"))
    {
        // Send close event
        orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
    }
}

/** Init function, it is called when all orx's modules have been initialized
 */
orxSTATUS Mixnmatch::Init()
{
    // Generate data
    GenerateData();

    // Create the scene
    CreateObject("Scene");

    // Done!
    return orxSTATUS_SUCCESS;
}

/** Run function, it should not contain any game logic
 */
orxSTATUS Mixnmatch::Run()
{
    // Return orxSTATUS_FAILURE to instruct orx to quit
    return orxSTATUS_SUCCESS;
}

/** Exit function, it is called before exiting from orx
 */
void Mixnmatch::Exit()
{
    // Let orx clean all our mess automatically. :)
}

/** BindObjects function, ScrollObject-derived classes are bound to config sections here
 */
void Mixnmatch::BindObjects()
{
    // Bind the Character class to the Character config section
    ScrollBindObject<Character>("Character");
}

/** Bootstrap function, it is called before config is initialized, allowing for early resource storage definitions
 */
orxSTATUS Mixnmatch::Bootstrap() const
{
    // Add config storage to find the initial config file
    orxResource_AddStorage(orxCONFIG_KZ_RESOURCE_GROUP, "../data/config", orxFALSE);

    // Return orxSTATUS_FAILURE to prevent orx from loading the default config file
    return orxSTATUS_SUCCESS;
}

/** Main function
 */
int main(int argc, char **argv)
{
    // Execute our game
    Mixnmatch::GetInstance().Execute(argc, argv);

    // Done!
    return EXIT_SUCCESS;
}
