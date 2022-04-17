/**
 * @file Character.h
 * @date 21-Apr-2022
 */

#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "mixnmatch.h"

/** Character Class
 */
class Character : public ScrollObject
{
public:

                enum Orientation {
                                Right = 0,
                                DownRight,
                                Down,
                                DownLeft,
                                Left,
                                UpLeft,
                                Up,
                                UpRight
                };


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &_rstInfo);


private:
                Orientation     eLastOrientation;
};

#endif // __CHARACTER_H__
