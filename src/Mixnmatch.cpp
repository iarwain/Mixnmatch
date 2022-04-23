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

void GenerateData()
{
    orxConfig_PushSection("Generation");

    const orxSTRING zFormat = orxConfig_GetString("TextureFormat");

    if(*zFormat != orxCHAR_NULL)
    {
        // For all characters
        for(orxS32 i = 0, iCount = orxConfig_GetListCount("Characters"); i < iCount; i++)
        {
            const orxSTRING zCharacter = orxConfig_GetListString("Characters", i);

            // Get character name
            orxCHAR acCharacterBuffer[64];
            orxString_NPrint(acCharacterBuffer, sizeof(acCharacterBuffer), "%sCharacter", zCharacter);

            // Push character's config section
            orxConfig_PushSection(zCharacter);

            // For all layers
            for(orxS32 j = 0, jCount = orxConfig_GetKeyCount(); j < jCount; j++)
            {
                const orxSTRING zLayer = orxConfig_GetKey(j);

                // For all variations
                for(orxS32 k = 0, kCount = orxConfig_GetListCount(zLayer); k < kCount; k++)
                {
                    const orxSTRING zVariation = orxConfig_GetListString(zLayer, k);

                    // Get texture name
                    orxCHAR acTextureBuffer[256];
                    orxString_NPrint(acTextureBuffer, sizeof(acTextureBuffer), zFormat, zCharacter, zLayer, zVariation);

                    // Does it exist?
                    if(orxResource_Locate(orxTEXTURE_KZ_RESOURCE_GROUP, acTextureBuffer))
                    {
                        orxCHAR acAssetBuffer[64];

                        // Get asset name
                        orxString_NPrint(acAssetBuffer, sizeof(acAssetBuffer), "%s%s%s", zCharacter, zLayer, zVariation);

                        // Add it to the base character's section to allow for random selection
                        orxConfig_PushSection(acCharacterBuffer);
                        orxConfig_SetParent(acCharacterBuffer, "Character");
                        const orxSTRING zAsset = acAssetBuffer;
                        orxConfig_AppendListString(zLayer, &zAsset, 1);
                        orxConfig_PopSection();

                        // Generate asset's content
                        orxConfig_PushSection(acAssetBuffer);
                        orxConfig_SetParent(acAssetBuffer, zLayer);
                        orxConfig_SetString(orxGRAPHIC_KZ_CONFIG_TEXTURE_NAME, acTextureBuffer);
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

/** Update function, it has been registered to be called every tick of the core clock
 */
void Mixnmatch::Update(const orxCLOCK_INFO &_rstInfo)
{
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
