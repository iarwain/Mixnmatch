/**
 * @file Mixnmatch.h
 * @date 22-Apr-2022
 */

#ifndef __Mixnmatch_H__
#define __Mixnmatch_H__

#define __NO_SCROLLED__
#include "Scroll.h"

/** Game Class
 */
class Mixnmatch : public Scroll<Mixnmatch>
{
public:


private:

                orxSTATUS       Bootstrap() const;

                void            Update(const orxCLOCK_INFO &_rstInfo);

                orxSTATUS       Init();
                orxSTATUS       Run();
                void            Exit();
                void            BindObjects();


private:
};

#endif // __Mixnmatch_H__
