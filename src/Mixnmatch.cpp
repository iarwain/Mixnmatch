/**
 * @file Mixnmatch.cpp
 * @date 21-Apr-2022
 */

#define __SCROLL_IMPL__
#include "Mixnmatch.h"
#undef __SCROLL_IMPL__

#define orxBUNDLE_IMPL
#include "orxBundle.h"

#include "Character.h"

#ifdef __orxMSVC__

/* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement        = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

struct LayerContext
{
    const orxCHAR  *acCharacter;
    const orxSTRING zCharacter;
    const orxSTRING zFormat;
};

orxBOOL orxFASTCALL ProcessLayer(const orxSTRING _zKeyName, orxBOOL _bInherited, void *_pContext)
{
    const orxSTRING zLayer = _zKeyName;

    // Get context
    LayerContext *pstContext = (LayerContext *)_pContext;

    // For all layer variations
    for(orxS32 i = 0, iCount = orxConfig_GetListCount(zLayer); i < iCount; i++)
    {
        const orxSTRING zVariation = orxConfig_GetListString(zLayer, i);

        // Get texture name
        orxCHAR acTexture[256];
        orxString_NPrint(acTexture, sizeof(acTexture), pstContext->zFormat, pstContext->zCharacter, zLayer, zVariation);

        // Does it exist?
        if(orxResource_Locate(orxTEXTURE_KZ_RESOURCE_GROUP, acTexture))
        {
            orxCHAR acAsset[64];

            // Get asset name
            orxString_NPrint(acAsset, sizeof(acAsset), "%s%s%s", pstContext->zCharacter, zLayer, zVariation);

            // Add it to the character's section to default with random layer selection
            orxConfig_PushSection(pstContext->acCharacter);
            orxConfig_SetParent(pstContext->acCharacter, "Character");
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

    return orxTRUE;
}

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
            LayerContext stContext = {acCharacter, zCharacter, zFormat};
            orxConfig_ForAllKeys(ProcessLayer, orxTRUE, &stContext);

            // Pop config section
            orxConfig_PopSection();
        }
    }

    orxConfig_PopSection();
}

struct FrameContext
{
    const orxCHAR *acFrame;
    const orxCHAR *acParent;
};

orxBOOL orxFASTCALL ProcessFrame(const orxSTRING _zKeyName, orxBOOL _bInherited, void *_pContext)
{
    // Get context
    FrameContext *pstContext = (FrameContext *)_pContext;

    orxConfig_PushSection(pstContext->acFrame);
    orxConfig_SetString(_zKeyName, pstContext->acParent);
    // Execute generative lazy command hook
    if(*_zKeyName == '!')
    {
        orxConfig_GetString(_zKeyName);
        orxConfig_ClearValue(_zKeyName);
    }
    orxConfig_PopSection();

    return orxTRUE;
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
                                FrameContext stContext = {acFrame, acParent};
                                orxConfig_ForAllKeys(ProcessFrame, orxTRUE, &stContext);
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
    orxBundle_Exit();

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
    orxBundle_Init();

    // Add config storage to find the initial config file
    orxResource_AddStorage(orxCONFIG_KZ_RESOURCE_GROUP, orxBUNDLE_KZ_RESOURCE_STORAGE "Mixnmatch.obr", orxFALSE);
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
